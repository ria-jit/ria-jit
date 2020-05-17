//
// Created by flo on 09.05.20.
//

#include "translate.hpp"
#include <iostream>
#include <asmjit/asmjit.h>
#include <sys/mman.h>
#include "register.h"

using namespace asmjit;

void generate_strlen();

x86::Gp get_mapped_reg(t_risc_reg reg);

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
 * Get the register mapping for the RISC-V register reg in the current context.
 * @param reg the RISC-V register in use
 * @return the x86 register we map it to in this context
 */
x86::Gp get_mapped_reg(t_risc_reg reg) {
    /*
     * We have an array which lists the usage numbers for the RISC-V regs. 
     * Potentially sort that and evaluate to figure out the mapping to x86?
     */
    not_yet_implemented("get_mapped_reg()");
    return x86::rax;
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
        case ECALL:
            break;
        case EBREAK:
            break;
        case CSRRW:
            break;
        case CSRRS:
            break;
        case CSRRC:
            break;
        case CSRRWI:
            break;
        case CSRRSI:
            break;
        case CSRRCI:
            break;
    }
}

/**
 * ADDIW adds the sign-extended 12-bit immediate to register rs1 and produces the
 * proper sign-extension of a 32-bit result in rd.
 * @param instr
 */
void translate_addiw(t_risc_instr instr) {
    // mov rd, rs1
    // add rd, instr.imm

    std::cout << "Translate addiw...\n";

    //sign-extend the immediate to 32-bit
    uint64_t imm = instr.imm & 0x00000FFF;
    if (0x00000800 & imm) {
        imm += 0xFFFFF000;
    }

    //add 32-bit, sign-extend to 64-bit and write back
    auto reg_base = reinterpret_cast<uint64_t>(get_reg_data());
    a->mov(x86::edx, x86::ptr(reg_base, 8 * instr.reg_src_1));
    a->add(x86::edx, imm);
    a->movsx(x86::rax, x86::edx);
    a->mov(x86::ptr(reg_base, 8 * instr.reg_dest), x86::rax);
}

/**
 * SLLI is a logical left shift.
 * The operand to be shifted is in rs1, the shift amount is in the lower 6 bits of the I-immediate field.
 * @param instr
 */
void translate_slli(t_risc_instr instr) {
    //mov rd, rs1
    //shl rd, (instr.imm & 0x3F)
    std::cout << "Translate slli...\n";
    auto reg_base = reinterpret_cast<uint64_t>(get_reg_data());
    a->mov(x86::rax, x86::ptr(reg_base, 8 * instr.reg_src_1));
    a->shl(x86::rax, instr.imm & 0b111111);
    a->mov(x86::ptr(reg_base, 8 * instr.reg_dest), x86::rax);
}

/**
 * LUI places the 20-bit U-immediate into bits 31-12 of register rd and places zero in the lowest 12 bits.
 * The 32-bit result is sign-extended to 64 bits.
 * @param instr
 */
void translate_lui(t_risc_instr instr) {
    //mov rd, extended
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
#include <vector>
#include <numeric>
#include <algorithm>
#include <unordered_map>

void translate_risc_JAL_onlylink(t_risc_instr risc_instr);
void load_risc_registers(std::unordered_map<t_risc_reg, asmjit::x86::Gp> map);
void save_risc_registers(std::unordered_map<t_risc_reg, asmjit::x86::Gp> map);


void translate_block(t_risc_addr risc_addr) {

    t_risc_instr risc_instr = {};

    // get memory for structs
    std::vector<t_risc_instr> block_cache;
    // parse until branch found - count register
    //uint32_t reg_count[N_REG];
    std::vector<uint32_t> reg_count(N_REG);

    //parse structs
    while(true) {

        risc_instr.addr = risc_addr;

        block_cache.push_back(risc_instr);

        parse_instruction(&block_cache.back(), reg_count.data());
        //parse_instruction(&risc_instr,reg_count);


        /*
        ///branch?
        if (block_cache.back().optype == BRANCH) { ///BEQ, BNE, BLT, BGE, BLTU, BGEU
            ///destination address unknown at translate time
            break;
        }

        ///unconditional jump? -> follow
        if (block_cache.back().optype == JUMP) {   ///JAL, JALR
            if(block_cache.back().mnem == JAL) {
                ///link
                ///replace [JAL rd, offset] with [AUIPC rd, 4]
                ///(4 because next risc_addr)
                //this AUIPC would not be possible on actual Risc V,
                //because the lower 12 bits would always be zero.
                //we can do this here, because the immediate parsing
                //is done before this step: in parse_instruction()
                //where AUIPC is parsed as IMMEDIADE instead of UPPER_IMMEDIATE
                block_cache.back() = t_risc_instr {
                    risc_addr,
                    AUIPC,
                    IMMEDIATE,
                    x0,
                    x0,
                    block_cache.back().reg_dest,
                    4
                };

                ///calculate address of jump destination
                risc_addr += block_cache.back().imm;//(signed long) (parse_jump_immediate(block_cache)); //left shift???

            }
            else if(block_cache.back().mnem == JALR) {
                ///destination address unknown at translate time
                break;
            }
            else {
                ///should not get here
                std::cerr << "Oops: line " << __LINE__ << " in " __FILE__ << std::endl;
            }
        }
        ///no jump or branch -> continue fetching
        else {
            ///next instruction address
            risc_addr += 4;
        }
        */


        switch(block_cache.back().optype) {

            ///branch?
            case BRANCH : {    ///BEQ, BNE, BLT, BGE, BLTU, BGEU
                ///destination address unknown at translate time, stop parsing
                goto PARSE_DONE;
            } break;

            ///unconditional jump? -> follow
            case JUMP : {    ///JAL, JALR
                switch(block_cache.back().mnem) {
                    case JAL : {
                        ///link
                        ///replace [JAL rd, offset] with [AUIPC rd, 4]
                        ///(4 because next risc_addr)
                        //this AUIPC would not be possible on actual Risc V,
                        //because the lower 12 bits would always be zero.
                        //we can do this here, because the immediate parsing
                        //is done before this step: in parse_instruction()
                        //where AUIPC is parsed as IMMEDIADE instead of UPPER_IMMEDIATE
                        block_cache.back() = t_risc_instr {
                                risc_addr,
                                AUIPC,
                                IMMEDIATE,
                                x0,
                                x0,
                                block_cache.back().reg_dest,
                                4
                        };

                        ///calculate address of jump destination
                        risc_addr += block_cache.back().imm;//(signed long) (parse_jump_immediate(block_cache)); //left shift???

                    } break;

                    case JALR : {
                        ///destination address unknown at translate time, stop parsing
                        goto PARSE_DONE;
                    }

                    default: {
                        ///should not get here
                        std::cerr << "Oops: line " << __LINE__ << " in " __FILE__ << std::endl;
                    }
                }
            } break;

            ///no jump or branch -> continue fetching
            default: {
                ///next instruction address
                risc_addr += 4;
            }
        }

    }

    /*
    while(true) {

        risc_instr.addr = risc_addr;

        block_cache.push_back(risc_instr);

        parse_instruction(&block_cache.back(), reg_count);
        //parse_instruction(&risc_instr,reg_count);

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
    */

    PARSE_DONE:

    ///REGISTER ALLOCATION:

    /*
    *The idea is to allocate the most used Risc V registers to the real x86_64 registers
    *but we cant use all the available x86_64 registers because we need some of them for
    *the translated instructions that expand to multiple x86_64 instructions which probably
    *have to store some temporary values.
    *
    *at the end of the translated basic block all the Risc V registers allocated to real x86_64
    *registers will be copied back to their respective in-memory register representation fields.
    *this is necessary because the binary translator also uses the x86_64 registers in between basic blocks
    *and also because the allocation will probably be different for the next basic block.
    */

    ///rank registers by usage
    std::vector<int> indices(N_REG);
    std::iota(indices.begin(), indices.end(), 0);
    std::stable_sort(indices.begin(),
            indices.end(),
            [&](int a, int b){return reg_count[a] < reg_count[b];}
            );

    ///create allocation MAPping
    std::unordered_map<t_risc_reg, asmjit::x86::Gp> register_map;
    //alternatively: asmjit::x86::Gp register_map[N_REG];
    //and bool allocated[N_REG];



    //insert register pairs here, example:
    std::vector<asmjit::x86::Gp> x86_64_registers = {asmjit::x86::r8, asmjit::x86::r9,
                                                     asmjit::x86::r10, asmjit::x86::r11,
                                                     asmjit::x86::r12, asmjit::x86::r13,
                                                     asmjit::x86::r14, asmjit::x86::r15};

    for(int i = 0; i < x86_64_registers.size(); i++) {
        if(indices[i] != t_risc_reg::x0) {
            register_map.insert(
                    std::pair<t_risc_reg, asmjit::x86::Gp>(
                            static_cast<t_risc_reg>(indices[i]), x86_64_registers[i]
                    )
            );
        }
    }
    //notice: risc reg x0 will need special treatment



    ///load registers
    load_risc_registers(register_map);

    /// translate structs
    for(int i = 0; i < block_cache.size(); i++) {
        translate_risc_instr(block_cache[i]);
    }

    ///save registers
    save_risc_registers(register_map);

    std::cout << "Translated Block: " << block_cache.size() << " instructions" << std::endl;
}

///writes rd but doesn't actually jump
void translate_risc_JAL_onlylink(t_risc_instr risc_instr) {
    not_yet_implemented("single-instruction JAL onlylink translator not implemented yet");
}

///loads the Risc V registers into their allocated x86_64 registers
void load_risc_registers(std::unordered_map<t_risc_reg, asmjit::x86::Gp> map) {
    for(int i = t_risc_reg::x1 ; i <= t_risc_reg::pc; i++) {
        if(map.find(static_cast<t_risc_reg>(i)) != map.end()) {
            //load into x86_64 register from risc_register_memory
        }
    }
}

///saves the Risc V registers into their respective memory fields
void save_risc_registers(std::unordered_map<t_risc_reg, asmjit::x86::Gp> map) {
    for(int i = t_risc_reg::x1 ; i <= t_risc_reg::pc; i++) {
        if(map.find(static_cast<t_risc_reg>(i)) != map.end()) {
            //load into risc_register_memory from x86_64 register
        }
    }
}
