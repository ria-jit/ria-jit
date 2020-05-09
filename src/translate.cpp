//
// Created by flo on 09.05.20.
//

#include "translate.h"
#include <iostream>
#include <asmjit/asmjit.h>
#include <sys/mman.h>

using namespace asmjit;

void test_simple_assembly();
void test_strlen_example();
void test_strlen_relocated();

/**
 * Just a quick example to test code generation using AsmJit.
 * Externed to be called from C code.
 */
void test_generation() {
    std::cout << "Testing code generation with a few examples...\n";

    test_simple_assembly();
    test_strlen_relocated();
}

void test_strlen_relocated() {
    CodeHolder codeHolder;

    //init code holder for host info
    codeHolder.init(CodeInfo(ArchInfo::kIdHost));
    x86::Assembler a(&codeHolder);

    //create the labels for our strlen call
    Label loop_cond = a.newLabel();
    Label loop_break = a.newLabel();

    typedef int (*asm_strlen)(char*);

    //unix calling convention, so the first argument is in rdi, return value in rax
    x86::Gp str = x86::rdi;
    x86::Gp len = x86::rax;

    a.xor_(len, len);

    a.bind(loop_cond);
    a.cmp(x86::byte_ptr(str), 0);
    a.jz(loop_break);
    a.inc(len);
    a.inc(str);
    a.jmp(loop_cond);

    a.bind(loop_break);
    a.ret();

    //prepare for relocation
    codeHolder.flatten();
    codeHolder.resolveUnresolvedLinks();

    //get worst case code size from code holder
    size_t codeSize = codeHolder.codeSize();

    //allocate for worst case code size
    void *ptr = mmap(nullptr, codeSize, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANON, -1, 0);
    if (!ptr) {
        std::cout << "Bad. Memory allocation failed.\n";
        return;
    }

    //relocate code in object
    codeHolder.relocateToBase(reinterpret_cast<uint64_t>(ptr));
    codeSize = codeHolder.codeSize();
    
    CodeBuffer buffer = codeHolder.sectionById(0)->buffer();
    memcpy(ptr, buffer.data(), buffer.size());
    __builtin___clear_cache(static_cast<char *>(ptr), static_cast<char *>(ptr) + codeSize - 1);

    std::string hello = "Hello, World!";
    char *string = const_cast<char *>(hello.c_str());
    std::cout << "String " << hello << " has length " << ((asm_strlen) ptr)(string);

    munmap(ptr, codeSize);
}

void test_simple_assembly() {
    JitRuntime runtime;
    CodeHolder codeHolder;

    codeHolder.init(runtime.codeInfo());
    x86::Assembler assembler(&codeHolder);

    std::cout << "Emitting code mov eax, 0xBEEF\n";
    assembler.mov(x86::eax, 0xBEEF);
    assembler.ret();

    //main function to call the generated code experiment
    typedef int (*Exec)();
    Exec output;
    Error error = runtime.add(&output, &codeHolder);
    if (error) {
        std::cout << "Bad. Error occurred during code generation.\n";
        return;
    }

    int retValue = output();
    std::cout << "Function returned " << std::hex << retValue << std::dec << ", expected was 0xBEEF :)\n";

    runtime.release(output);
}
