//
// Created by flo on 09.05.20.
//

#include "translate.h"
#include <iostream>
#include <asmjit/asmjit.h>
#include <sys/mman.h>

using namespace asmjit;

void generate_strlen();

void translate_addi(t_risc_instr instr);

void translate_lui(t_risc_instr instr);

void translate_slli(t_risc_instr instr);

void translate_addiw(t_risc_instr instr);


void translate_JAL(t_risc_instr instr);
void translate_JALR(t_risc_instr instr);
void translate_BEQ(t_risc_instr instr);
void translate_BNE(t_risc_instr instr);
void translate_BLT(t_risc_instr instr);
void translate_BGE(t_risc_instr instr);
void translate_BLTU(t_risc_instr instr);
void translate_BGEU(t_risc_instr instr);

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
    typedef int (*str_len_asm)(char *);
    int ret = ((str_len_asm) executable)(string);

    std::cout << "Length of string " << string << " is " << ret;
}

/**
 * Translate the passed instruction and add the output
 * to the current x86 block.
 * @param instr the RISC instruction to translate
 */
void translate_risc_instr(t_risc_instr instr) {
    //todo once the optype is finalized in t_risc_instr->optype, extract multiple dispatch layers here

    switch(instr.mnem) {
        case LUI:
            translate_lui(instr);
            break;
        case AUIPC:
            break;
        case JAL:
            translate_JAL(instr);
            break;
        case JALR:
            translate_JALR(instr);
            break;
        case BEQ:
            translate_BEQ(instr);
            break;
        case BNE:
            translate_BNE(instr);
            break;
        case BLT:
            translate_BLT(instr);
            break;
        case BGE:
            translate_BGE(instr);
            break;
        case BLTU:
            translate_BLTU(instr);
            break;
        case BGEU:
            translate_BGEU(instr);
            break;
        case LB:
            break;
        case LBU:
            break;
        case LH:
            break;
        case LW:
            break;
        case LHU:
            break;
        case SB:
            break;
        case SH:
            break;
        case SW:
            break;
        case ADDI:
            translate_addi(instr);
            break;
        case SLTI:
            break;
        case SLTIU:
            break;
        case XORI:
            break;
        case ORI:
            break;
        case ANDI:
            break;
        case SLLI:
            translate_slli(instr);
            break;
        case SRLI:
            break;
        case SRAI:
            break;
        case ADD:
            break;
        case SUB:
            break;
        case SLL:
            break;
        case SLT:
            break;
        case SLTU:
            break;
        case XOR:
            break;
        case SRL:
            break;
        case SRA:
            break;
        case OR:
            break;
        case AND:
            break;
        case FENCE:
            break;
        case FENCE_I:
            break;
        case LWU:
            break;
        case LD:
            break;
        case SD:
            break;
        case ADDIW:
            translate_addiw(instr);
            break;
        case SLLIW:
            break;
        case SRLIW:
            break;
        case SRAIW:
            break;
        case ADDW:
            break;
        case SUBW:
            break;
        case SLLW:
            break;
        case SRLW:
            break;
        case SRAW:
            break;
        case MUL:
            break;
        case MULH:
            break;
        case MULHSU:
            break;
        case MULHU:
            break;
        case DIV:
            break;
        case DIVU:
            break;
        case REM:
            break;
        case REMU:
            break;
        case MULW:
            break;
        case DIVW:
            break;
        case DIVUW:
            break;
        case REMW:
            break;
        case REMUW:
            break;
    }
}

/**
 * ADDIW adds the sign-extended 12-bit immediate to register rs1 and produces the
 * proper sign-extension of a 32-bit result in rd.
 * @param instr
 */
void translate_addiw(t_risc_instr instr) {
    std::cout << "Translate addiw...\n";
}

/**
 * SLLI is a logical left shift.
 * The operand to be shifted is in rs1, the shift amount is in the lower 6 bits of the I-immediate field.
 * @param instr
 */
void translate_slli(t_risc_instr instr) {
    std::cout << "Translate slli...\n";
}

/**
 * LUI places the 20-bit U-immediate into bits 31-12 of register rd and places zero in the lowest 12 bits.
 * The 32-bit result is sign-extended to 64 bits.
 * @param instr
 */
void translate_lui(t_risc_instr instr) {
    std::cout << "Translate lui...\n";
}

/**
 * ADDI adds the sign-extended 12-bit immediate to register rs1.
 * Overflow is ignored and the result is the low (in our case) 64 bit of the result.
 * The result is stored in rd.
 * @param instr
 */
void translate_addi(t_risc_instr instr) {
    std::cout << "Translate addi...\n";
}



/**
 * The following instructions return to the binary translator after writing pc
 * */

void translate_JAL(t_risc_instr instr) {
    std::cout << "Translate JAL should not ever be needed" << std::endl;
}

void translate_JALR(t_risc_instr instr) {
    std::cout << "Translate JALR" << std::endl;
}

void translate_BEQ(t_risc_instr instr) {
    std::cout << "Translate BRANCH" << std::endl;
}

void translate_BNE(t_risc_instr instr) {
    std::cout << "Translate BRANCH" << std::endl;
}

void translate_BLT(t_risc_instr instr) {
    std::cout << "Translate BRANCH" << std::endl;
}

void translate_BGE(t_risc_instr instr) {
    std::cout << "Translate BRANCH" << std::endl;
}

void translate_BLTU(t_risc_instr instr) {
    std::cout << "Translate BRANCH" << std::endl;
}

void translate_BGEU(t_risc_instr instr) {
    std::cout << "Translate BRANCH" << std::endl;
}



//NEITHER FINISHED NOR TESTED

#include "util.h"
#include "parser.h"
//#include <vector>

//void translate_risc_instr(t_risc_instr risc_instr);
void translate_risc_JAL_onlylink(t_risc_instr risc_instr);
//void translate_risc_JALR(t_risc_instr risc_instr);
//void translate_risc_BRANCH(t_risc_instr risc_instr);


void translate_block(t_risc_addr risc_addr) {

    //why even save the structs?
    //std::vector<t_risc_instr> block_cache;

    int instructions_in_block = 0;
    t_risc_instr risc_instr = {};

    while(true) {

        //TODO here: somehow obtain the raw contents of the instruction at address risc_addr
        //int testMemoryContent = 0x38537; //value for testing
        //int32_t *rawInstr = &testMemoryContent;

        risc_instr.addr = risc_addr;

        //block_cache.push_back(risc_instr);

        //parse_instruction(&block_cache.back());
        parse_instruction(&risc_instr);

        instructions_in_block++;

        ///branch?
        if (risc_instr.optype == BRANCH) { ///BEQ, BNE, BLT, BGE, BLTU, BGEU
            translate_risc_instr(risc_instr);
            break;
        }

        ///unconditional jump? -> follow
        if (risc_instr.optype == JUMP) {   ///JAL, JALR
            if(risc_instr.mnem == JAL) {
                ///calculate address of jump destination
                risc_addr += risc_instr.imm;//(signed long) (parse_jump_immediate(block_cache)); //left shift???

                ///link
                translate_risc_JAL_onlylink(risc_instr);
            }
            else if(risc_instr.mnem == JALR) {
                ///destination address unknown at translate time
                translate_risc_instr(risc_instr);
                break;
            }
            else {
                ///should not get here
                std::cerr << "Oops: line " << __LINE__ << " in " __FILE__ << std::endl;
            }
        }

        ///no jump or branch -> continue fetching
        else {
            translate_risc_instr(risc_instr);

            ///next instruction address
            risc_addr += 4;
        }
    }

    std::cout << "Translated Block: " << instructions_in_block << " instructions" << std::endl;
}

///writes rd but doesn't actually jump
void translate_risc_JAL_onlylink(t_risc_instr risc_instr) {
    not_yet_implemented("single-instruction JAL onlylink translator not implemented yet");
}
