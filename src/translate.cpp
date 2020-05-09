//
// Created by flo on 09.05.20.
//

#include "translate.h"
#include <iostream>
#include <asmjit/asmjit.h>
#include <sys/mman.h>

using namespace asmjit;

void generate_strlen();

/**
 * The AsmJit code holder for our generated x86 machine code.
 */
static CodeHolder code;

/**
 * The Assembly emitter that writes to the CodeBuffer in the CodeHolder.
 */
static x86::Assembler *a;

/**
 * Quick example to test code generation using AsmJit.
 */
void test_generation() {
    std::cout << "Testing code generation with a String length example...\n";
    generate_strlen();
}

/**
 * Initializes a new translatable block of code.
 * Call this before translating any instructions that belong together in the same execution run
 * (e.g., before translating every basic block).
 */
void init_block() {
    code.init(CodeInfo(ArchInfo::kIdHost));
    a = new x86::Assembler(&code);
}

/**
 * Finalize the translated block.
 * This emits the ret instruction in order to have the translated basic block return to the main loop.
 * It will, after performing cleanup, prepare the code for relocation and allocate an executable page for it.
 * @return the starting address of the function block, or the nullptr in case of error
 */
t_cache_loc finalize_block() {
    //emit ret instruction as the final instruction in the block
    a->ret();

    //flatten and resolve open links to prepare for relocation
    code.flatten();
    code.resolveUnresolvedLinks();

    //get the worst case size estimate for memory allocation
    size_t size = code.codeSize();

    //allocate executable page for determined worst case code size, initialized to 0
    void *ptr = mmap(nullptr, size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (!ptr) {
        std::cout << "Bad. Memory allocation fault.\n";
        return nullptr;
    }

    //relocate code to allocated memory area, size may have changed
    code.relocateToBase(reinterpret_cast<uint64_t>(ptr));
    size = code.codeSize();

    //copy the .text section to the allocated page
    CodeBuffer buffer = code.sectionById(0)->buffer();
    memcpy(ptr, buffer.data(), buffer.size());

    //prevents gcc from optimizing the assumed dead store away
    __builtin___clear_cache(static_cast<char *>(ptr), static_cast<char *>(ptr) + size - 1);

    return ptr;
}

/**
 * Generates a basic strlen loop in machine code and executes it
 * by calling the generated code in memory.
 */
void generate_strlen() {
    //initialize new block
    init_block();
    
    //calling convention: first argument in rdi, ret value in rax
    x86::Gp str = x86::rdi;
    x86::Gp len = x86::rax;

    Label loop_cond = a->newLabel();
    Label loop_end = a->newLabel();
    
    a->xor_(len, len);
    a->bind(loop_cond);
    a->cmp(x86::byte_ptr(str), 0);
    a->jz(loop_end);
    a->inc(len);
    a->inc(str);
    a->jmp(loop_cond);
    a->bind(loop_end);

    //finalize block and get cached location
    t_cache_loc executable = finalize_block();
    
    //call the assembled function on a string
    std::string hello_world = "Hello, World!"; //len = 13
    char *string = const_cast<char *>(hello_world.c_str());
    typedef int (*str_len_asm)(char*);
    int ret = ((str_len_asm) executable)(string);

    std::cout << "Length of string " << string << " is " << ret;
}