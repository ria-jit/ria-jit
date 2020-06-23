//
// Created by flo on 09.05.20.
//

#include "translate.hpp"
#include <asmjit/asmjit.h>
#include <sys/mman.h>
#include "register.h"
#include "translate_arithmetic.hpp"
#include "translate_controlflow.hpp"
#include "translate_csr.hpp"
#include "translate_loadstore.hpp"
#include "translate_m_ext.hpp"
#include "translate_other.hpp"

//#include <string>

#include "util.h"
#include "parser.h"


using namespace asmjit;

t_risc_addr lastUsedAddress = TRANSLATOR_BASE;

//instruction translation
void translate_risc_instr(const t_risc_instr &instr, const register_info &r_info);

/**
 * The AsmJit code holder for our generated x86 machine code.
 */
CodeHolder *code;

/**
 * The Assembly emitter that writes to the CodeBuffer in the CodeHolder.
 */
x86::Assembler *a;

/**
 * Initializes a new translatable block of code.
 * Call this before translating any instructions that belong together in the same execution run
 * (e.g., before translating every basic block).
 */
void init_block() {
    //delete the last block's CodeHolder and Assembler
    delete code;
    delete a;

    //initialize new CodeHolders and Assemblers for each block
    code = new CodeHolder();
    code->init(CodeInfo(ArchInfo::kIdHost));
    a = new x86::Assembler(code);
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
    code->flatten();
    code->resolveUnresolvedLinks();

    //get the worst case size estimate for memory allocation
    size_t size = code->codeSize();

    //allocate executable page for determined worst case code size, initialized to 0
    void *addr = reinterpret_cast<void *>(ALIGN_DOWN((lastUsedAddress - size), 4096lu));
    void *ptr = mmap(addr, ALIGN_UP(size, 4096lu), PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE |
                                                                                       MAP_FIXED_NOREPLACE, -1, 0);
    if (!ptr || ptr != addr) {
        printf("Bad. Memory allocation fault.\n");
        return nullptr;
    }
    lastUsedAddress -= ALIGN_UP(size, 4096lu);
    //relocate code to allocated memory area, size may have changed
    code->relocateToBase(reinterpret_cast<uint64_t>(ptr));
    size = code->codeSize();

    //copy the .text section to the allocated page
    CodeBuffer buffer = code->sectionById(0)->buffer();
    memcpy(ptr, buffer.data(), buffer.size());

    //prevents gcc from optimizing the assumed dead store away
    __builtin___clear_cache(static_cast<char *>(ptr), static_cast<char *>(ptr) + size - 1);

    return ptr;
}

/**
 * Translate the passed instruction and add the output
 * to the current x86 block.
 * @param instr the RISC instruction to translate
 */
void translate_risc_instr(const t_risc_instr &instr, const register_info &r_info) {
    //todo once the optype is finalized in t_risc_instr->optype, extract multiple dispatch layers here

    switch (instr.mnem) {
        case LUI:
            translate_lui(instr, r_info);
            break;
        case JAL:
            translate_JAL(instr, r_info);
            break;
        case JALR:
            translate_JALR(instr, r_info);
            break;
        case BEQ:
            translate_BEQ(instr, r_info);
            break;
        case BNE:
            translate_BNE(instr, r_info);
            break;
        case BLT:
            translate_BLT(instr, r_info);
            break;
        case BGE:
            translate_BGE(instr, r_info);
            break;
        case BLTU:
            translate_BLTU(instr, r_info);
            break;
        case BGEU:
            translate_BGEU(instr, r_info);
            break;
        case ADDI:
            translate_addi(instr, r_info);
            break;
        case SLLI:
            translate_slli(instr, r_info);
            break;
        case ADDIW:
            translate_addiw(instr, r_info);
            break;
        case AUIPC:
            translate_AUIPC(instr, r_info);
            break;
        case SLTI:
            translate_SLTI(instr, r_info);
            break;
        case SLTIU:
            translate_SLTIU(instr, r_info);
            break;
        case XORI:
            translate_XORI(instr, r_info);
            break;
        case ORI:
            translate_ORI(instr, r_info);
            break;
        case ANDI:
            translate_ANDI(instr, r_info);
            break;
        case SRLI:
            translate_SRLI(instr, r_info);
            break;
        case SRAI:
            translate_SRAI(instr, r_info);
            break;
        case ADD:
            translate_ADD(instr, r_info);
            break;
        case SUB:
            translate_SUB(instr, r_info);
            break;
        case SLL:
            translate_SLL(instr, r_info);
            break;
        case SLT:
            translate_SLT(instr, r_info);
            break;
        case SLTU:
            translate_SLTU(instr, r_info);
            break;
        case XOR:
            translate_XOR(instr, r_info);
            break;
        case SRL:
            translate_SRL(instr, r_info);
            break;
        case SRA:
            translate_SRA(instr, r_info);
            break;
        case OR:
            translate_OR(instr, r_info);
            break;
        case AND:
            translate_AND(instr, r_info);
            break;
        case SLLIW:
            translate_SLLIW(instr, r_info);
            break;
        case SRLIW:
            translate_SRLIW(instr, r_info);
            break;
        case SRAIW:
            translate_SRAIW(instr, r_info);
            break;
        case ADDW:
            translate_ADDW(instr, r_info);
            break;
        case SUBW:
            translate_SUBW(instr, r_info);
            break;
        case SLLW:
            translate_SLLW(instr, r_info);
            break;
        case SRLW:
            translate_SRLW(instr, r_info);
            break;
        case SRAW:
            translate_SRAW(instr, r_info);
            break;
        case CSRRW:
            translate_CSRRW(instr, r_info);
            break;
        case CSRRS:
            translate_CSRRS(instr, r_info);
            break;
        case CSRRC:
            translate_CSRRC(instr, r_info);
            break;
        case CSRRWI:
            translate_CSRRWI(instr, r_info);
            break;
        case CSRRSI:
            translate_CSRRSI(instr, r_info);
            break;
        case CSRRCI:
            translate_CSRRCI(instr, r_info);
            break;
        case LB:
            translate_LB(instr, r_info);
            break;
        case LH:
            translate_LH(instr, r_info);
            break;
        case LW:
            translate_LW(instr, r_info);
            break;
        case LBU:
            translate_LBU(instr, r_info);
            break;
        case LHU:
            translate_LHU(instr, r_info);
            break;
        case SB:
            translate_SB(instr, r_info);
            break;
        case SH:
            translate_SH(instr, r_info);
            break;
        case SW:
            translate_SW(instr, r_info);
            break;
        case LWU:
            translate_LWU(instr, r_info);
            break;
        case LD:
            translate_LD(instr, r_info);
            break;
        case SD:
            translate_SD(instr, r_info);
            break;
        case MUL:
            translate_MUL(instr, r_info);
            break;
        case MULH:
            translate_MULH(instr, r_info);
            break;
        case MULHSU:
            translate_MULHSU(instr, r_info);
            break;
        case MULHU:
            translate_MULHU(instr, r_info);
            break;
        case DIV:
            translate_DIV(instr, r_info);
            break;
        case DIVU:
            translate_DIVU(instr, r_info);
            break;
        case REM:
            translate_REM(instr, r_info);
            break;
        case REMU:
            translate_REMU(instr, r_info);
            break;
        case MULW:
            translate_MULW(instr, r_info);
            break;
        case DIVW:
            translate_DIVW(instr, r_info);
            break;
        case DIVUW:
            translate_DIVUW(instr, r_info);
            break;
        case REMW:
            translate_REMW(instr, r_info);
            break;
        case REMUW:
            translate_REMUW(instr, r_info);
            break;
        case FENCE:
            translate_FENCE(instr, r_info);
            break;
        case ECALL:
            translate_ECALL(instr, r_info);
            break;
        case EBREAK:
            translate_EBREAK(instr, r_info);
            break;
        case FENCE_I:
            translate_FENCE_I(instr, r_info);
            break;
    }

    //log instruction
    log_verbose(
            "Instruction %d at 0x%x (type %d) - rs1: %d rs2: %d rd: %d imm: %d\n",
            instr.mnem,
            instr.addr,
            instr.optype,
            instr.reg_src_1,
            instr.reg_src_2,
            instr.reg_dest,
            instr.imm
    );

    //temporary, to make instruction boundaries visible in disassembly
    a->push(x86::eax);
    a->pop(x86::eax);
}

//NEITHER FINISHED NOR TESTED



void translate_risc_JAL_onlylink(t_risc_instr risc_instr);

void load_risc_registers(register_info r_info);

void save_risc_registers(register_info r_info);

void set_pc_next_inst(const t_risc_instr &instr, uint64_t r_addr);

t_cache_loc translate_block(t_risc_addr risc_addr) {

    t_risc_instr risc_instr = {};

    /// get memory for structs
    //what was the problem with 2 standard libraries again?
    //I could use a custom class or arrays instead,
    //but that would be less elegant.
    //std::vector<t_risc_instr> block_cache;
#define BLOCK_CACHE_SIZE 64
    t_risc_instr *block_cache = (t_risc_instr *) mmap(NULL, BLOCK_CACHE_SIZE * sizeof(t_risc_instr),
                                                      PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    ///count register usage
    uint32_t reg_count[N_REG];

    int instructions_in_block = 0;

    bool block_full = false;

    ///parse structs
    for (int parse_pos = 0; parse_pos <= BLOCK_CACHE_SIZE - 2; parse_pos++) { //-2 rather than -1 bc of final AUIPC

        risc_instr.addr = risc_addr;

        //block_cache.push_back(risc_instr);
        block_cache[parse_pos] = risc_instr;

        //printf("parse at: %p", (void*)block_cache[parse_pos].addr);

        //parse_instruction(&block_cache.back(), reg_count);
        parse_instruction(&block_cache[parse_pos], reg_count);

        //switch (block_cache.back().optype) {
        switch (block_cache[parse_pos].optype) {

            ///branch? or syscall?
            case SYSTEM : //fallthrough Potential program end stop parsing
            case BRANCH : {    ///BEQ, BNE, BLT, BGE, BLTU, BGEU, syscalls
                ///destination address unknown at translate time, stop parsing
                instructions_in_block++;
                goto PARSE_DONE;
            }
                break;

                ///unconditional jump? -> follow
            case JUMP : {    ///JAL, JALR
                switch (block_cache[parse_pos].mnem) {
                    case JAL : {
                        ///link
                        ///replace [JAL rd, offset] with [AUIPC rd, 4]
                        ///(4 because next risc_addr)
                        //this AUIPC would not be possible on actual Risc V,
                        //because the lower 12 bits would always be zero.
                        //we can do this here, because the immediate parsing
                        //is done before this step: in parse_instruction()
                        //where AUIPC is parsed as IMMEDIADE instead of UPPER_IMMEDIATE

                        t_risc_imm temp = block_cache[parse_pos].imm;

                        block_cache[parse_pos] = t_risc_instr{
                                risc_addr,
                                AUIPC,
                                IMMEDIATE,
                                x0,
                                x0,
                                block_cache[parse_pos].reg_dest,
                                4
                        };

                        instructions_in_block++;

                        ///calculate address of jump destination
                        risc_addr += temp;//(signed long) (parse_jump_immediate(block_cache)); //left shift???

                    }
                        break;

                    case JALR : {
                        ///destination address unknown at translate time, stop parsing
                        instructions_in_block++;
                        goto PARSE_DONE;
                    }

                    default: {
                        ///should not get here
                        printf("Oops: line %d in %s\n", __LINE__, __FILE__);
                    }
                }
            }
                break;

                ///no jump or branch -> continue fetching
            default: {
                ///next instruction address
                risc_addr += 4;
                instructions_in_block++;
            }
        }

    }

    ///loop ended at BLOCK_CACHE_SIZE -> set pc for next instruction
    block_full = true;
    instructions_in_block++;


    ///loop ended at BRANCH: skip setting pc
    PARSE_DONE:

    ///REGISTER ALLOCATION:

    /*
    *The idea is to allocate the most used Risc V registers to real x86_64 registers
    *but we can not use all the available x86_64 registers because we need some of them for
    *the translated instructions that expand to multiple x86_64 instructions which probably
    *have to store some temporary values.
    *
    *at the end of the translated basic block all the Risc V registers allocated to real x86_64
    *registers will be copied back to their respective in-memory register representation fields.
    *this is necessary because the binary translator also uses the x86_64 registers in between basic blocks
    *and also because the allocation will probably be different for the next basic block.
    */


    //Again, I dont remember the exact problematic with using the standard library anymore.
    //The unordered_map /*could be*/ is replaced by (multiple) arrays,
    //and we could of course implement the sorting algorithm or whatever we'll use later on ourselves.

    ///rank registers by usage

    int indicesRanked[N_REG];
    for (int i = 0; i < N_REG; i++) {
        indicesRanked[i] = i;
    }
    ///insertion sort:
    {
        int key, j;
        for (int i = 1; i < N_REG; i++) {
            key = indicesRanked[i];
            j = i - 1;

            ///move move elements with index < i && element > i one to the left
            while (j >= 0 && reg_count[indicesRanked[j]] < reg_count[key]) {
                indicesRanked[j + 1] = indicesRanked[j];
                j--;
            }

            ///insert former element i to correct position
            indicesRanked[j + 1] = key;
        }
    }

    ///create allocation MAPping
    asmjit::x86::Gp register_map[N_REG];
    bool mapped[N_REG];



    //insert register pairs here, example:
#define USED_X86_REGS 8
    asmjit::x86::Gp x86_64_registers[] = {asmjit::x86::r8, asmjit::x86::r9,
                                          asmjit::x86::r10, asmjit::x86::r11,
                                          asmjit::x86::r12, asmjit::x86::r13,
                                          asmjit::x86::r14, asmjit::x86::r15};

    {
        int currMreg = 0;
        for (int i = 0; i < N_REG; i++) {
            /*if (indicesRanked[i] != t_risc_reg::x0 && indicesRanked[i] != t_risc_reg::pc && reg_count[indicesRanked[i]] > 2 && currMreg < USED_X86_REGS) {
                register_map[indicesRanked[i]] = x86_64_registers[i];
                mapped[indicesRanked[i]] = true;
                currMreg++;
            } else {
                //I'm not sure if it's zero initialized…
                mapped[indicesRanked[i]] = false;
            }*/

            /*
             * todo ignore the register mapping for now to deal with the other instruction execution issues
             * This forces all instructions to be translated in their"memory" form and makes debugging them easier.
             */

            mapped[indicesRanked[i]] = false;
        }
    }
    //notice: risc reg x0 will need special treatment

    ///create info struct
    register_info r_info = {
            register_map,
            mapped,
            reinterpret_cast<uint64_t>(get_reg_data())
    };


    ///initialize new block
    init_block(); //x86::Assembler a(&code)

    ///save the x86_64 registers
    //???

    ///load registers
    //load_risc_registers(r_info);

    /// translate structs
    for (int i = 0; i < instructions_in_block; i++) {
        translate_risc_instr(block_cache[i], r_info);
    }

    if(block_full) {
        set_pc_next_inst(block_cache[BLOCK_CACHE_SIZE - 2], reinterpret_cast<uint64_t>(get_reg_data()));
    }

    ///save registers
    //save_risc_registers(r_info);

    ///load the saved x86_64 registers
    //???
    log_verbose("Translated block: %d instructions\n", instructions_in_block);

    ///finalize block and return cached location
    return finalize_block();
}

///set the pc to next addr after inst
void set_pc_next_inst(const t_risc_instr &instr, uint64_t r_addr) {
    ///set pc
    a->mov(x86::rax, instr.addr + 4);
    a->mov(x86::ptr(r_addr + 8 * pc), x86::rax);
}

///writes rd but doesn't actually jump
void translate_risc_JAL_onlylink(t_risc_instr risc_instr) {
    not_yet_implemented("single-instruction JAL onlylink translator not implemented yet");
}

///loads the Risc V registers into their allocated x86_64 registers
void load_risc_registers(register_info r_info) {
    for (int i = t_risc_reg::x0; i <= t_risc_reg::pc; i++) {
        if (r_info.mapped[i]) {
            a->mov(r_info.map[i], x86::ptr(r_info.base + 8 * i, 0)); //x86::ptr(r_info.base+ 8 * i)
        }
    }
}

///saves the Risc V registers into their respective memory fields
void save_risc_registers(register_info r_info) {
    for (int i = t_risc_reg::x0; i <= t_risc_reg::pc; i++) {
        if (r_info.mapped[i]) {
            a->mov(x86::ptr(r_info.base + 8 * i), r_info.map[i]);
        }
    }
}
