
#include <frvdec/frvdec.h>
#include <util/log.h>
#include "parser.h"

int32_t set_error_message(t_risc_instr *p_instr_struct, int32_t error_code) {
    p_instr_struct->optype = INVALID_INSTRUCTION;
    p_instr_struct->mnem = INVALID_MNEM;
    p_instr_struct->reg_dest = error_code;
    p_instr_struct->imm = *(int32_t *) p_instr_struct->addr; //cast and dereference
    return error_code;
}

/**
 * @param p_instr_struct struct filled with the addr of the instruction to be translated
 */
int32_t parse_instruction(t_risc_instr *p_instr_struct) {
    log_asm_in("Parsing at %p\n", (void *) p_instr_struct->addr);

    FrvInst frv;
    int frvres = frv_decode(4, p_instr_struct->addr, FRV_RV64, &frv);
    if (frvres < 0)
        return set_error_message(p_instr_struct, frvres);

    p_instr_struct->size = frvres;
    p_instr_struct->reg_dest = frv.rd != FRV_REG_INV ? frv.rd : INVALID_REG;
    p_instr_struct->reg_src_1 = frv.rs1 != FRV_REG_INV ? frv.rs1 : INVALID_REG;
    p_instr_struct->reg_src_2 = frv.rs2 != FRV_REG_INV ? frv.rs2 : INVALID_REG;
    p_instr_struct->imm = frv.imm; // overwritten for rounding mode/rs3.

    int has_rs3_rm = 0;
    switch (frv.mnem) {
    case FRV_LUI: p_instr_struct->optype = UPPER_IMMEDIATE, p_instr_struct->mnem = LUI; break;
    case FRV_AUIPC: p_instr_struct->optype = IMMEDIATE, p_instr_struct->mnem = AUIPC; break;
    case FRV_JAL: p_instr_struct->optype = JUMP, p_instr_struct->mnem = JAL; break;
    case FRV_JALR: p_instr_struct->optype = JUMP, p_instr_struct->mnem = JALR; break;
    case FRV_FLW: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FLW; break;
    case FRV_FLD: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FLD; break;
    case FRV_FSW: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FSW; break;
    case FRV_FSD: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FSD; break;
    case FRV_FENCE: p_instr_struct->optype = SYSTEM, p_instr_struct->mnem = FENCE; break;
    case FRV_FENCEI: p_instr_struct->optype = SYSTEM, p_instr_struct->mnem = FENCE_I; break;
    case FRV_BEQ: p_instr_struct->optype = BRANCH, p_instr_struct->mnem = BEQ; break;
    case FRV_BNE: p_instr_struct->optype = BRANCH, p_instr_struct->mnem = BNE; break;
    case FRV_BLT: p_instr_struct->optype = BRANCH, p_instr_struct->mnem = BLT; break;
    case FRV_BGE: p_instr_struct->optype = BRANCH, p_instr_struct->mnem = BGE; break;
    case FRV_BLTU: p_instr_struct->optype = BRANCH, p_instr_struct->mnem = BLTU; break;
    case FRV_BGEU: p_instr_struct->optype = BRANCH, p_instr_struct->mnem = BGEU; break;
    case FRV_LB: p_instr_struct->optype = IMMEDIATE, p_instr_struct->mnem = LB; break;
    case FRV_LH: p_instr_struct->optype = IMMEDIATE, p_instr_struct->mnem = LH; break;
    case FRV_LW: p_instr_struct->optype = IMMEDIATE, p_instr_struct->mnem = LW; break;
    case FRV_LD: p_instr_struct->optype = IMMEDIATE, p_instr_struct->mnem = LD; break;
    case FRV_LBU: p_instr_struct->optype = IMMEDIATE, p_instr_struct->mnem = LBU; break;
    case FRV_LHU: p_instr_struct->optype = IMMEDIATE, p_instr_struct->mnem = LHU; break;
    case FRV_LWU: p_instr_struct->optype = IMMEDIATE, p_instr_struct->mnem = LWU; break;
    case FRV_SB: p_instr_struct->optype = STORE, p_instr_struct->mnem = SB; break;
    case FRV_SH: p_instr_struct->optype = STORE, p_instr_struct->mnem = SH; break;
    case FRV_SW: p_instr_struct->optype = STORE, p_instr_struct->mnem = SW; break;
    case FRV_SD: p_instr_struct->optype = STORE, p_instr_struct->mnem = SD; break;
    case FRV_MUL: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = MUL; break;
    case FRV_MULH: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = MULH; break;
    case FRV_MULHSU: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = MULHSU; break;
    case FRV_MULHU: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = MULHU; break;
    case FRV_DIV: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = DIV; break;
    case FRV_DIVU: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = DIVU; break;
    case FRV_REM: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = REM; break;
    case FRV_REMU: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = REMU; break;
    case FRV_MULW: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = MULW; break;
    case FRV_DIVW: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = DIVW; break;
    case FRV_DIVUW: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = DIVUW; break;
    case FRV_REMW: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = REMW; break;
    case FRV_REMUW: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = REMUW; break;
    case FRV_ADD: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = ADD; break;
    case FRV_SLL: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = SLL; break;
    case FRV_SLT: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = SLT; break;
    case FRV_SLTU: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = SLTU; break;
    case FRV_XOR: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = XOR; break;
    case FRV_SRL: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = SRL; break;
    case FRV_OR: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = OR; break;
    case FRV_AND: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = AND; break;
    case FRV_SUB: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = SUB; break;
    case FRV_SRA: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = SRA; break;
    case FRV_ADDW: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = ADDW; break;
    case FRV_SLLW: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = SLLW; break;
    case FRV_SRLW: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = SRLW; break;
    case FRV_SUBW: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = SUBW; break;
    case FRV_SRAW: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = SRAW; break;
    case FRV_ADDI: p_instr_struct->optype = IMMEDIATE, p_instr_struct->mnem = ADDI; break;
    case FRV_SLLI: p_instr_struct->optype = IMMEDIATE, p_instr_struct->mnem = SLLI; break;
    case FRV_SLTI: p_instr_struct->optype = IMMEDIATE, p_instr_struct->mnem = SLTI; break;
    case FRV_SLTIU: p_instr_struct->optype = IMMEDIATE, p_instr_struct->mnem = SLTIU; break;
    case FRV_XORI: p_instr_struct->optype = IMMEDIATE, p_instr_struct->mnem = XORI; break;
    case FRV_SRAI: p_instr_struct->optype = IMMEDIATE, p_instr_struct->mnem = SRAI; break;
    case FRV_SRLI: p_instr_struct->optype = IMMEDIATE, p_instr_struct->mnem = SRLI; break;
    case FRV_ORI: p_instr_struct->optype = IMMEDIATE, p_instr_struct->mnem = ORI; break;
    case FRV_ANDI: p_instr_struct->optype = IMMEDIATE, p_instr_struct->mnem = ANDI; break;
    case FRV_ADDIW: p_instr_struct->optype = IMMEDIATE, p_instr_struct->mnem = ADDIW; break;
    case FRV_SLLIW: p_instr_struct->optype = IMMEDIATE, p_instr_struct->mnem = SLLIW; break;
    case FRV_SRAIW: p_instr_struct->optype = IMMEDIATE, p_instr_struct->mnem = SRAIW; break;
    case FRV_SRLIW: p_instr_struct->optype = IMMEDIATE, p_instr_struct->mnem = SRLIW; break;
    case FRV_ECALL: p_instr_struct->optype = SYSTEM, p_instr_struct->mnem = frv.imm & 1 ? EBREAK : ECALL; break;
    case FRV_CSRRW: p_instr_struct->optype = SYSTEM, p_instr_struct->mnem = CSRRW; break;
    case FRV_CSRRS: p_instr_struct->optype = SYSTEM, p_instr_struct->mnem = CSRRS; break;
    case FRV_CSRRC: p_instr_struct->optype = SYSTEM, p_instr_struct->mnem = CSRRC; break;
    case FRV_CSRRWI: p_instr_struct->optype = SYSTEM, p_instr_struct->mnem = CSRRWI; break;
    case FRV_CSRRSI: p_instr_struct->optype = SYSTEM, p_instr_struct->mnem = CSRRSI; break;
    case FRV_CSRRCI: p_instr_struct->optype = SYSTEM, p_instr_struct->mnem = CSRRCI; break;
    case FRV_FMADDS: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FMADDS, has_rs3_rm = 1; break;
    case FRV_FMSUBS: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FMSUBS, has_rs3_rm = 1; break;
    case FRV_FNMSUBS: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FNMSUBS, has_rs3_rm = 1; break;
    case FRV_FNMADDS: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FNMADDS, has_rs3_rm = 1; break;
    case FRV_FMADDD: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FMADDD, has_rs3_rm = 1; break;
    case FRV_FMSUBD: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FMSUBD, has_rs3_rm = 1; break;
    case FRV_FNMSUBD: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FNMSUBD, has_rs3_rm = 1; break;
    case FRV_FNMADDD: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FNMADDD, has_rs3_rm = 1; break;
    case FRV_FMVXW: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FMVXW, has_rs3_rm = 1; break;
    case FRV_FMVWX: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FMVWX, has_rs3_rm = 1; break;
    case FRV_FCLASSS: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FCLASSS, has_rs3_rm = 1; break;
    case FRV_FADDS: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FADDS, has_rs3_rm = 1; break;
    case FRV_FSUBS: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FSUBS, has_rs3_rm = 1; break;
    case FRV_FMULS: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FMULS, has_rs3_rm = 1; break;
    case FRV_FDIVS: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FDIVS, has_rs3_rm = 1; break;
    case FRV_FSQRTS: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FSQRTS, has_rs3_rm = 1; break;
    case FRV_FSGNJS: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FSGNJS, has_rs3_rm = 1; break;
    case FRV_FSGNJNS: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FSGNJNS, has_rs3_rm = 1; break;
    case FRV_FSGNJXS: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FSGNJXS, has_rs3_rm = 1; break;
    case FRV_FMINS: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FMINS, has_rs3_rm = 1; break;
    case FRV_FMAXS: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FMAXS, has_rs3_rm = 1; break;
    case FRV_FLES: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FLES, has_rs3_rm = 1; break;
    case FRV_FLTS: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FLTS, has_rs3_rm = 1; break;
    case FRV_FEQS: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FEQS, has_rs3_rm = 1; break;
    case FRV_FCVTWS: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FCVTWS, has_rs3_rm = 1; break;
    case FRV_FCVTWUS: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FCVTWUS, has_rs3_rm = 1; break;
    case FRV_FCVTLS: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FCVTLS, has_rs3_rm = 1; break;
    case FRV_FCVTLUS: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FCVTLUS, has_rs3_rm = 1; break;
    case FRV_FCVTSW: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FCVTSW, has_rs3_rm = 1; break;
    case FRV_FCVTSWU: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FCVTSWU, has_rs3_rm = 1; break;
    case FRV_FCVTSL: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FCVTSL, has_rs3_rm = 1; break;
    case FRV_FCVTSLU: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FCVTSLU, has_rs3_rm = 1; break;
    case FRV_FMVXD: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FMVXD, has_rs3_rm = 1; break;
    case FRV_FMVDX: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FMVDX, has_rs3_rm = 1; break;
    case FRV_FCLASSD: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FCLASSD, has_rs3_rm = 1; break;
    case FRV_FADDD: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FADDD, has_rs3_rm = 1; break;
    case FRV_FSUBD: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FSUBD, has_rs3_rm = 1; break;
    case FRV_FMULD: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FMULD, has_rs3_rm = 1; break;
    case FRV_FDIVD: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FDIVD, has_rs3_rm = 1; break;
    case FRV_FSQRTD: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FSQRTD, has_rs3_rm = 1; break;
    case FRV_FSGNJD: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FSGNJD, has_rs3_rm = 1; break;
    case FRV_FSGNJND: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FSGNJND, has_rs3_rm = 1; break;
    case FRV_FSGNJXD: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FSGNJXD, has_rs3_rm = 1; break;
    case FRV_FMIND: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FMIND, has_rs3_rm = 1; break;
    case FRV_FMAXD: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FMAXD, has_rs3_rm = 1; break;
    case FRV_FLED: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FLED, has_rs3_rm = 1; break;
    case FRV_FLTD: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FLTD, has_rs3_rm = 1; break;
    case FRV_FEQD: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FEQD, has_rs3_rm = 1; break;
    case FRV_FCVTSD: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FCVTSD, has_rs3_rm = 1; break;
    case FRV_FCVTDS: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FCVTDS, has_rs3_rm = 1; break;
    case FRV_FCVTWD: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FCVTWD, has_rs3_rm = 1; break;
    case FRV_FCVTWUD: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FCVTWUD, has_rs3_rm = 1; break;
    case FRV_FCVTLD: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FCVTLD, has_rs3_rm = 1; break;
    case FRV_FCVTLUD: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FCVTLUD, has_rs3_rm = 1; break;
    case FRV_FCVTDW: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FCVTDW, has_rs3_rm = 1; break;
    case FRV_FCVTDWU: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FCVTDWU, has_rs3_rm = 1; break;
    case FRV_FCVTDL: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FCVTDL, has_rs3_rm = 1; break;
    case FRV_FCVTDLU: p_instr_struct->optype = FLOAT, p_instr_struct->mnem = FCVTDLU, has_rs3_rm = 1; break;

    case FRV_LRW: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = LRW; break;
    case FRV_SCW: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = SCW; break;
    case FRV_LRD: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = LRD; break;
    case FRV_SCD: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = SCD; break;
    case FRV_AMOADDW: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = AMOADDW; break;
    case FRV_AMOSWAPW: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = AMOSWAPW; break;
    case FRV_AMOXORW: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = AMOXORW; break;
    case FRV_AMOORW: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = AMOORW; break;
    case FRV_AMOANDW: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = AMOANDW; break;
    case FRV_AMOMINW: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = AMOMINW; break;
    case FRV_AMOMAXW: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = AMOMAXW; break;
    case FRV_AMOMINUW: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = AMOMINUW; break;
    case FRV_AMOMAXUW: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = AMOMAXUW; break;
    case FRV_AMOADDD: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = AMOADDD; break;
    case FRV_AMOSWAPD: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = AMOSWAPD; break;
    case FRV_AMOXORD: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = AMOXORD; break;
    case FRV_AMOORD: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = AMOORD; break;
    case FRV_AMOANDD: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = AMOANDD; break;
    case FRV_AMOMIND: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = AMOMIND; break;
    case FRV_AMOMAXD: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = AMOMAXD; break;
    case FRV_AMOMINUD: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = AMOMINUD; break;
    case FRV_AMOMAXUD: p_instr_struct->optype = REG_REG, p_instr_struct->mnem = AMOMAXUD; break;
    }

    if (has_rs3_rm) {
        p_instr_struct->reg_src_3 = frv.rs3 != FRV_REG_INV ? frv.rs3 : INVALID_REG;
        p_instr_struct->rounding_mode = frv.misc;
        if (p_instr_struct->rounding_mode == RMM) {
            // fallback RMM to RNE
            not_yet_implemented("unsupported rounding mode RMM at 0x%lx, fallback to RNE",
                                p_instr_struct->addr);
            p_instr_struct->rounding_mode = RNE;
        }
    }

    return 0;
}
