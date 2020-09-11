//
// Created by flo on 09.05.20.
//

#include "translate.h"
#include <runtime/register.h>
#include <gen/dispatch.h>
#include <fadec/fadec-enc.h>
#include <common.h>
#include <linux/mman.h>
#include <util/util.h>
#include <util/log.h>
#include <util/typedefs.h>
#include <parser/parser.h>
#include <main/context.h>
#include <gen/instr/core/translate_controlflow.h>

t_risc_addr lastUsedAddress = TRANSLATOR_BASE;

//instruction translation
void translate_risc_instr(const t_risc_instr *instr, const context_info *c_info);

int parse_block(t_risc_addr risc_addr, t_risc_instr *parse_buf, int maxCount, const context_info *c_info);

t_cache_loc
translate_block_instructions(const t_risc_instr *block_cache, int instructions_in_block, const context_info *c_info);

/**
 * The pointer to the head of the current basic block.
 * Not externed, as this is only used inside of the current file.
 */
static uint8_t *block_head;

/**
 * The pointer to the current assembly instruction.
 */
uint8_t *current;

/**
 * Global flag for assembly errors.
 * Initialized to zero when initializing the blocks, and holds values != 0 for error conditions.
 * Best checked at least every block (i.e., in finalize_block() or equivalent).
 */
int err;

/**
 * Initializes a new translatable block of code.
 * Call this before translating any instructions that belong together in the same execution run
 * (e.g., before translating every basic block).
 */
void init_block() {
    void *addr = (void *) (lastUsedAddress - 4096lu);
    //allocate a memory page for the next basic block that will be translated
    void *buf = mmap(addr, 4096, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED_NOREPLACE,
                     -1, 0);
    lastUsedAddress -= 4096lu;
    //check for mmap fault and terminate
    if (buf != addr) {
        dprintf(2, "Memory allocation fault in assembly.\n");
        _exit(-1);
    }

    //set the block_head and current pointer and the failed status
    block_head = (uint8_t *) buf;
    current = block_head;
    err = 0;

    //insert nop at the beginning so debugger step-into works as expected
    *(current++) = 0x90;
}

/**
 * Finalize the translated block.
 * This emits the ret instruction in order to have the translated basic block return to the main loop.
 * @return the starting address of the function block, or the nullptr in case of error
 */
t_cache_loc finalize_block(int chainLinkOp) {

    ///write chainEnd to be chained by chainer
    if (flag_translate_opt && chainLinkOp == LINK_NULL) {
        err |= fe_enc64(&current, FE_MOV64mi, FE_MEM_ADDR((uint64_t) &chain_end), 0);
    }

    //emit the ret instruction as the final instruction in the block
    err |= fe_enc64(&current, FE_RET);

    //check failed flag
    if (err != 0) {
        //terminate if we encounter errors. this most likely is a bug in a RISC-V instruction's translation
        dprintf(2, "Assembly error after generating basic block.\n");
        _exit(-1);
    }

    //if that's fine, then we log and return
    if (flag_log_asm_out) {
        log_asm_out("Generated block code: ");

        log_print_mem((char *) block_head, current - block_head);
        printf("\n");
    }

    return (t_cache_loc) block_head;
}

/**
 * Translate the passed instruction and add the output
 * to the current x86 block.
 * @param instr the RISC instruction to translate
 */
void translate_risc_instr(const t_risc_instr *instr, const context_info *c_info) {
    //dispatch to translator functions
    dispatch_instr(instr, c_info);

    //log instruction
    log_asm_out(
            "Instruction %s at 0x%x (type %d) – (rs1: %s/%s) - (rs2: %s/%s) - (rd: %s/%s) - (imm: 0x%lx)\n",
            mnem_to_string(instr->mnem),
            instr->addr,
            instr->optype,
            reg_to_string(instr->reg_src_1),
            reg_to_alias(instr->reg_src_1),
            reg_to_string(instr->reg_src_2),
            reg_to_alias(instr->reg_src_2),
            reg_to_string(instr->reg_dest),
            reg_to_alias(instr->reg_dest),
            instr->imm
    );

    //make instruction boundaries visible in disassembly if required
    if (flag_log_asm_out) {
        err |= fe_enc64(&current, FE_NOP);
        err |= fe_enc64(&current, FE_NOP);
        err |= fe_enc64(&current, FE_NOP);
    }
}

t_cache_loc translate_block(t_risc_addr risc_addr, const context_info *c_info) {
    log_asm_out("Start translating block at (riscv)%p...\n", risc_addr);

    /// get memory for structs
#define BLOCK_CACHE_SIZE 64
    int maxCount = BLOCK_CACHE_SIZE;
    if (flag_single_step) {
        maxCount = 2;
    }
    t_risc_instr *block_cache = (t_risc_instr *) mmap(NULL, maxCount * sizeof(t_risc_instr),
                                                      PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    if (BAD_ADDR(block_cache)) {
        dprintf(2, "Failed to allocate cache for parsing instructions");
        _exit(FAIL_HEAP_ALLOC);
    }

    int instructions_in_block = parse_block(risc_addr, block_cache, maxCount, c_info);

    ///Start of actual translation
    t_cache_loc block = translate_block_instructions(block_cache, instructions_in_block, c_info);

    log_asm_out("Translated block at (riscv)%p: %d instructions\n", risc_addr, instructions_in_block);

    munmap(block_cache, maxCount * sizeof(t_risc_instr));
    return block;
}

/**
 * Translates the parsed instructions into a new memory page.
 *
 * @param block_cache the array of parsed RISC-V instructions
 * @param instructions_in_block the number of instructions in block_cache
 * @param c_info the context info for this block
 * @return the cached location of the generated block.
 */
t_cache_loc
translate_block_instructions(const t_risc_instr *block_cache, int instructions_in_block, const context_info *c_info) {

    ///initialize new block
    init_block();


    /// translate structs
    for (int i = 0; i < instructions_in_block; i++) {
        translate_risc_instr(&block_cache[i], c_info);
    }


    t_cache_loc block;
    ///finalize block and return cached location
    if (block_cache[instructions_in_block - 1].mnem == JALR || block_cache[instructions_in_block - 1].mnem == ECALL) {
        block = finalize_block(LINK_NULL);
    } else {
        block = finalize_block(DONT_LINK);
    }
    return block;
}

/**
 * Parse the instructions in the block starting at the given address.
 * @param risc_addr the RISC-V address the block starts at.
 * @param parse_buf the buffer to place the parsed instructions into.
 * @param maxCount the maximum number of parsed instructions to be parsed. parse_buf is at least this big.
 * @param c_info the context info for this block.
 * @return
 */
int parse_block(t_risc_addr risc_addr, t_risc_instr *parse_buf, int maxCount, const context_info *c_info) {

    int instructions_in_block = 0;

    ///parse structs
    for (int parse_pos = 0; parse_pos <= maxCount - 2; parse_pos++) { //-2 rather than -1 bc of final AUIPC

        parse_buf[parse_pos] = (t_risc_instr) {.addr=risc_addr};

        //printf("parse at: %p", (void*)block_cache[parse_pos].addr);

        parse_instruction(&parse_buf[parse_pos]);

        switch (parse_buf[parse_pos].optype) {

            case INVALID_INSTRUCTION : {
                instructions_in_block++;
                goto PARSE_DONE;
            }

            ///branch? or syscall?
            case SYSTEM : //fallthrough Potential program end stop parsing
            {
                switch (parse_buf[parse_pos].mnem) {
                    case ECALL:
                        ///Potential program end stop parsing
                        instructions_in_block++;
                        goto PARSE_DONE;
                    case FENCE:
                    case FENCE_I:
                        ///ignore get next instruction address
                        risc_addr += 4;
                        parse_pos--; //decrement for next loop cycle
                        break;
                    case EBREAK : {
                        parse_buf[parse_pos].imm = 0;
                        parse_buf[parse_pos].mnem = INVALID_MNEM;
                        parse_buf[parse_pos].optype = INVALID_INSTRUCTION;

                        instructions_in_block++;
                        goto PARSE_DONE;
                    }
                    default:
                        ///should not get here
                        printf("Oops: line %d in %s\n", __LINE__, __FILE__);
                        _exit(1);
                        break;
                }
            }
                break;
            case BRANCH : {    ///BEQ, BNE, BLT, BGE, BLTU, BGEU, syscalls
                ///destination address unknown at translate time, stop parsing

                /* if'ed out for now because it's not really improving performance
                 * -> not finished
                 * also using '1' as "translation-started-flag" is a bit sketchy
                 *
                 * recursive translation (of cond. jumps) may become entirely obsolete when chaining in main works
                 * */

#define DISABLE_RECURSIVE_CONDJMP_TRANSLATION
#ifndef DISABLE_RECURSIVE_CONDJMP_TRANSLATION
                if (!flag_translate_opt) {
#endif
                instructions_in_block++;
                goto PARSE_DONE;
#ifndef DISABLE_RECURSIVE_CONDJMP_TRANSLATION
                }

                instructions_in_block++;

                t_risc_addr
                        target_cm = risc_addr + ((int64_t) (parse_buf[parse_pos].imm));              //ConditionMet
                t_risc_addr target_cnm = risc_addr + 4; //ConditionNotMet

                t_cache_loc cache_loc_cm = lookup_cache_entry(target_cm);
                t_cache_loc cache_loc_cnm = lookup_cache_entry(target_cnm);


                if (cache_loc_cm == UNSEEN_CODE) {
                    log_asm_out("Reursion b from (riscv)%p to (riscv)%p\n", risc_addr, target_cm);
                    set_cache_entry(target_cm, (t_cache_loc) 1); //translation-started-flag
                    cache_loc_cm = translate_block(target_cm, c_info);
                    set_cache_entry(target_cm, cache_loc_cm);
                }

                if (cache_loc_cnm == UNSEEN_CODE) {
                    log_asm_out("Reursion b from (riscv)%p to (riscv)%p\n", risc_addr, target_cnm);
                    set_cache_entry(target_cnm, (t_cache_loc) 1); //translation-started-flag
                    cache_loc_cnm = translate_block(target_cnm, c_info);
                    set_cache_entry(target_cnm, cache_loc_cnm);
                }


                goto PARSE_DONE;
#endif

            }

                ///unconditional jump? -> follow
            case JUMP : {    ///JAL, JALR
                switch (parse_buf[parse_pos].mnem) {
                    case JAL : {
                        if (!flag_translate_opt) {
                            ///could follow, but cache
                            instructions_in_block++;
                            goto PARSE_DONE;
                        }

                        if (parse_buf[parse_pos].reg_dest != x0) {
                            ///could follow, but cache
                            instructions_in_block++;

                            ///1: recursively translate target
                            {
                                t_risc_addr target = risc_addr + parse_buf[parse_pos].imm;

                                t_cache_loc cache_loc = lookup_cache_entry(target);

                                if (cache_loc == UNSEEN_CODE) {
                                    log_asm_out("Recursion in JAL from (riscv)%p to target (riscv)%p\n", risc_addr, target);
                                    set_cache_entry(target, (t_cache_loc) 1); //break cyles
                                    cache_loc = translate_block(target, c_info);
                                    set_cache_entry(target, cache_loc);
                                }
                            }


                            ///2: recursively translate return addr (+4)
                            //dead ends could arise here
                            {
                                t_risc_addr ret_target = risc_addr + 4;
                                t_cache_loc cache_loc = lookup_cache_entry(ret_target);

                                if (cache_loc == UNSEEN_CODE) {
                                    log_asm_out("Recursion in JAL from (riscv)%p to ret_target(+4) (riscv)%p\n", risc_addr, ret_target);
                                    set_cache_entry(ret_target, (t_cache_loc) 1); //break cycles
                                    cache_loc = translate_block(ret_target,c_info);
                                    set_cache_entry(ret_target, cache_loc);
                                }
                            }



                            goto PARSE_DONE;
                        }

                        ///link
                        ///replace [JAL rd, offset] with [AUIPC rd, 4]
                        ///(4 because next risc_addr)
                        //this AUIPC would not be possible on actual Risc V,
                        //because the lower 12 bits would always be zero.
                        //we can do this here, because the immediate parsing
                        //is done before this step: in parse_instruction()
                        //where AUIPC is parsed as IMMEDIATE instead of UPPER_IMMEDIATE

                        t_risc_imm temp = parse_buf[parse_pos].imm;

                        parse_buf[parse_pos] = (t_risc_instr) {
                                risc_addr,
                                AUIPC,
                                IMMEDIATE,
                                x0,
                                x0,
                                parse_buf[parse_pos].reg_dest,
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
                        _exit(1);
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
    ///insert Pseudo instruction (has the address for the next PC in immediate field) for this
    parse_buf[maxCount - 1] = (t_risc_instr) {.imm=risc_addr, .mnem=PC_NEXT_INST, .optype=PSEUDO};
    instructions_in_block++;

    ///loop ended at BRANCH: skip setting pc
    PARSE_DONE:

    return instructions_in_block;
}

///set the pc to next addr after inst
/**
 * Translate the PC_NEXT_INST pseudo instruction that is inserted when a block would overflow our buffer.
 * Emits instructions that sets the PC RISC_V register in our gp_reg_file to the given address.
 * @param addr the address the risc PC reg should have at the end of the current block (the start address of the next
 *        block)
 * @param reg_base the base_address of the gp_reg_file.
 */
void translate_PC_NEXT_INST(const t_risc_addr addr, uint64_t reg_base) {
    ///set pc
    err |= fe_enc64(&current, FE_MOV64ri, FE_AX, addr);
    err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(reg_base + 8 * pc), FE_AX);
}

/**
 * inserts direct jumps after first cache lookup in main
 * */
void chain(t_cache_loc target) {
    if (!flag_translate_opt) return;
    int chain_err = 0;
    if (chain_end != NULL) {
        log_general("chaining: ...\n");
        chain_err |= fe_enc64(&chain_end, FE_JMP, (intptr_t) target);
    }

    ///check failed flag
    if (chain_err != 0) {
        ///terminate if we encounter errors. this most likely is a bug in a RISC-V instruction's translation
        dprintf(2, "Assembly error in chain, exiting...\n");
        _exit(-1);
    }
}
