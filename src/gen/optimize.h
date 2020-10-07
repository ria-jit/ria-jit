//
// Created by Flo Schmidt on 12.09.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_OPTIMIZE_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_OPTIMIZE_H

#include <util/typedefs.h>
#include <main/context.h>

void optimize_instr(t_risc_instr *block_cache, size_t index, size_t len);

void optimize_patterns(t_risc_instr *block_cache, int len);

void translate_pattern_emit(t_risc_instr *instr, const register_info *r_info);

typedef enum {
    DONT_CARE = 33,

    // -> same as rs[x] at pattern position h1
    rs1_h1,
    rs2_h1,
    rd_h1,
    rs1_h2,
    rs2_h2,
    rd_h2,
} t_risc_reg_meta;

typedef struct {
    short mnem;
    short rs1;
    short rs2;
    short rd;
    int h1;
    int h2;
    short imm;
    int imm_value;  //0: DONT_CARE  1: imm_value  2: imm patt. pos imm_value

} pattern_element;

typedef struct {
    const pattern_element * const elements;
    int len;
    //equivalent x86 code ...
    void (* emitter)(const t_risc_instr *, const register_info *);
} pattern;

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_OPTIMIZE_H
