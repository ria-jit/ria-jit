//
// Created by jhne on 10/3/20.
//

#include <gen/instr/patterns.h>
#include <gen/translate.h>
#include <util/typedefs.h>
#include <common.h>

void emit_pattern_0(const t_risc_instr *instr);
void emit_pattern_1(const t_risc_instr *instr);
void emit_pattern_2(const t_risc_instr *instr);


const pattern_element p_0_elem[] = {
        {LUI, DONT_CARE, DONT_CARE, DONT_CARE, 0, 0, 0, 0},
        {LD, rd_h1, DONT_CARE, DONT_CARE, 0, 0, 0, 0},
        {ADDI, rd_h1, DONT_CARE, rd_h1, 1, 0, 0, 0},
        {SD, rd_h2, rd_h1, DONT_CARE, 1, 0, 2, 1}
};

const pattern_element p_1_elem[] = {
        {LUI, DONT_CARE, DONT_CARE, DONT_CARE, 0, 0, 0, 0},
        {LW, rd_h1, DONT_CARE, DONT_CARE, 0, 0, 0, 0},
        {ADDIW, rd_h1, DONT_CARE, rd_h1, 1, 0, 0, 0},
        {SW, rd_h2, rd_h1, DONT_CARE, 1, 0, 2, 1}
};

const pattern_element p_2_elem[] = {
        {AUIPC, DONT_CARE, DONT_CARE, DONT_CARE, 0, 0, 0, 0},
        {ADDI, rd_h1, DONT_CARE, DONT_CARE, 0, 0, 0, 0},
        {LW, rd_h1, DONT_CARE, DONT_CARE, 1, 0, 0, 0},
        {ADDIW, rd_h1, DONT_CARE, DONT_CARE, 2, 0, 0, 0},
        {SW, rd_h1, rd_h2, DONT_CARE, 0, 3, 2, 2}
};

const pattern patterns[] = {
        {p_0_elem, 4, &emit_pattern_0},
        {p_1_elem, 4, &emit_pattern_1},
        {p_2_elem, 5, &emit_pattern_2}
};

void init_patterns() {

}

void emit_pattern_0(const t_risc_instr *instr) {
    printf("emit pattern 0: inc mem64 at %p\n", instr->addr);

    err |= fe_enc64(&current, FE_ADD64mi, FE_MEM_ADDR(instr->addr + instr->imm), instr[2].imm);
}

void emit_pattern_1(const t_risc_instr *instr) {
    printf("emit pattern 1: inc mem64 at %p\n", instr->addr);

    err |= fe_enc64(&current, FE_ADD32mi, FE_MEM_ADDR(instr->addr + instr->imm), instr[2].imm);
}

void emit_pattern_2(const t_risc_instr *instr) {
    printf("emit pattern 2: inc m64 at %p\n", instr->addr);

    err |= fe_enc64(&current, FE_ADD32mi, FE_MEM_ADDR(instr->addr + instr->imm + instr[2].imm), instr[3].imm);
}
