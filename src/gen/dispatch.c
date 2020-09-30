//
// Created by Flo Schmidt on 02.09.20.
//

#include "dispatch.h"
#include <gen/instr/core/translate_arithmetic.h>
#include <gen/instr/core/translate_controlflow.h>
#include <gen/instr/core/translate_csr.h>
#include <gen/instr/core/translate_loadstore.h>
#include <gen/instr/ext/translate_m_ext.h>
#include <gen/instr/core/translate_other.h>
#include <gen/instr/ext/translate_a_ext.h>
#include <gen/instr/ext/translate_f_ext.h>
#include <gen/instr/ext/translate_d_ext.h>
#include <gen/instr/pseudo/translate_pseudo.h>
#include <gen/optimize.h>

void dispatch_instr(t_risc_instr *instr, const context_info *c_info) {
    register_info *r_info = c_info->r_info;
    switch (instr->mnem) {
        case PATTERN_EMIT:
            translate_pattern_emit(instr, r_info);
            break;
        case INVALID_MNEM:
            translate_INVALID(instr, r_info);
            break;
        case LUI:
            translate_LUI(instr, r_info);
            break;
        case JAL:
            translate_JAL(instr, r_info, c_info);
            break;
        case JALR:
            translate_JALR(instr, r_info, c_info);
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
            translate_ADDI(instr, r_info);
            break;
        case SLLI:
            translate_SLLI(instr, r_info);
            break;
        case ADDIW:
            translate_ADDIW(instr, r_info);
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
            translate_ECALL(instr, r_info, c_info);
            break;
        case EBREAK:
            translate_EBREAK(instr, r_info);
            break;
        case FENCE_I:
            translate_FENCE_I(instr, r_info);
            break;
        case LRW:
            translate_LRW(instr, r_info);
            break;
        case SCW:
            translate_SCW(instr, r_info);
            break;
        case AMOSWAPW:
            translate_AMOSWAPW(instr, r_info);
            break;
        case AMOADDW:
            translate_AMOADDW(instr, r_info);
            break;
        case AMOXORW:
            translate_AMOXORW(instr, r_info);
            break;
        case AMOANDW:
            translate_AMOANDW(instr, r_info);
            break;
        case AMOORW:
            translate_AMOORW(instr, r_info);
            break;
        case AMOMINW:
            translate_AMOMINW(instr, r_info);
            break;
        case AMOMAXW:
            translate_AMOMAXW(instr, r_info);
            break;
        case AMOMINUW:
            translate_AMOMINUW(instr, r_info);
            break;
        case AMOMAXUW:
            translate_AMOMAXUW(instr, r_info);
            break;
        case LRD:
            translate_LRD(instr, r_info);
            break;
        case SCD:
            translate_SCD(instr, r_info);
            break;
        case AMOSWAPD:
            translate_AMOSWAPD(instr, r_info);
            break;
        case AMOADDD:
            translate_AMOADDD(instr, r_info);
            break;
        case AMOXORD:
            translate_AMOXORD(instr, r_info);
            break;
        case AMOANDD:
            translate_AMOANDD(instr, r_info);
            break;
        case AMOORD:
            translate_AMOORD(instr, r_info);
            break;
        case AMOMIND:
            translate_AMOMIND(instr, r_info);
            break;
        case AMOMAXD:
            translate_AMOMAXD(instr, r_info);
            break;
        case AMOMINUD:
            translate_AMOMINUD(instr, r_info);
            break;
        case AMOMAXUD:
            translate_AMOMAXUD(instr, r_info);
            break;
        case FLW:
            translate_FLW(instr, r_info);
            break;
        case FSW:
            translate_FSW(instr, r_info);
            break;
        case FMADDS:
            translate_FMADDS(instr, r_info);
            break;
        case FMSUBS:
            translate_FMSUBS(instr, r_info);
            break;
        case FNMSUBS:
            translate_FNMSUBS(instr, r_info);
            break;
        case FNMADDS:
            translate_FNMADDS(instr, r_info);
            break;
        case FADDS:
            translate_FADDS(instr, r_info);
            break;
        case FSUBS:
            translate_FSUBS(instr, r_info);
            break;
        case FMULS:
            translate_FMULS(instr, r_info);
            break;
        case FDIVS:
            translate_FDIVS(instr, r_info);
            break;
        case FSQRTS:
            translate_FSQRTS(instr, r_info);
            break;
        case FSGNJS:
            translate_FSGNJS(instr, r_info);
            break;
        case FSGNJNS:
            translate_FSGNJNS(instr, r_info);
            break;
        case FSGNJXS:
            translate_FSGNJXS(instr, r_info);
            break;
        case FMINS:
            translate_FMINS(instr, r_info);
            break;
        case FMAXS:
            translate_FMAXS(instr, r_info);
            break;
        case FCVTWS:
            translate_FCVTWS(instr, r_info);
            break;
        case FCVTWUS:
            translate_FCVTWUS(instr, r_info);
            break;
        case FMVXW:
            translate_FMVXW(instr, r_info);
            break;
        case FEQS:
            translate_FEQS(instr, r_info);
            break;
        case FLTS:
            translate_FLTS(instr, r_info);
            break;
        case FLES:
            translate_FLES(instr, r_info);
            break;
        case FCLASSS:
            translate_FCLASSS(instr, r_info);
            break;
        case FCVTSW:
            translate_FCVTSW(instr, r_info);
            break;
        case FCVTSWU:
            translate_FCVTSWU(instr, r_info);
            break;
        case FMVWX:
            translate_FMVWX(instr, r_info);
            break;
        case FCVTLS:
            translate_FCVTLS(instr, r_info);
            break;
        case FCVTLUS:
            translate_FCVTLUS(instr, r_info);
            break;
        case FCVTSL:
            translate_FCVTSL(instr, r_info);
            break;
        case FCVTSLU:
            translate_FCVTSLU(instr, r_info);
            break;
        case FLD:
            translate_FLD(instr, r_info);
            break;
        case FSD:
            translate_FSD(instr, r_info);
            break;
        case FMADDD:
            translate_FMADDD(instr, r_info);
            break;
        case FMSUBD:
            translate_FMSUBD(instr, r_info);
            break;
        case FNMSUBD:
            translate_FNMSUBD(instr, r_info);
            break;
        case FNMADDD:
            translate_FNMADDD(instr, r_info);
            break;
        case FADDD:
            translate_FADDD(instr, r_info);
            break;
        case FSUBD:
            translate_FSUBD(instr, r_info);
            break;
        case FMULD:
            translate_FMULD(instr, r_info);
            break;
        case FDIVD:
            translate_FDIVD(instr, r_info);
            break;
        case FSQRTD:
            translate_FSQRTD(instr, r_info);
            break;
        case FSGNJD:
            translate_FSGNJD(instr, r_info);
            break;
        case FSGNJND:
            translate_FSGNJND(instr, r_info);
            break;
        case FSGNJXD:
            translate_FSGNJXD(instr, r_info);
            break;
        case FMIND:
            translate_FMIND(instr, r_info);
            break;
        case FMAXD:
            translate_FMAXD(instr, r_info);
            break;
        case FCVTSD:
            translate_FCVTSD(instr, r_info);
            break;
        case FCVTDS:
            translate_FCVTDS(instr, r_info);
            break;
        case FEQD:
            translate_FEQD(instr, r_info);
            break;
        case FLTD:
            translate_FLTD(instr, r_info);
            break;
        case FLED:
            translate_FLED(instr, r_info);
            break;
        case FCLASSD:
            translate_FCLASSD(instr, r_info);
            break;
        case FCVTWD:
            translate_FCVTWD(instr, r_info);
            break;
        case FCVTWUD:
            translate_FCVTWUD(instr, r_info);
            break;
        case FCVTDW:
            translate_FCVTDW(instr, r_info);
            break;
        case FCVTDWU:
            translate_FCVTDWU(instr, r_info);
            break;
        case FCVTLD:
            translate_FCVTLD(instr, r_info);
            break;
        case FCVTLUD:
            translate_FCVTLUD(instr, r_info);
            break;
        case FMVXD:
            translate_FMVXD(instr, r_info);
            break;
        case FCVTDL:
            translate_FCVTDL(instr, r_info);
            break;
        case FCVTDLU:
            translate_FCVTDLU(instr, r_info);
            break;
        case FMVDX:
            translate_FMVDX(instr, r_info);
            break;
        case PC_NEXT_INST:
            translate_PC_NEXT_INST((t_risc_addr) instr->op_field.op.imm, r_info);
            break;
        case SILENT_NOP:
            translate_SILENT_NOP(instr, r_info);
            break;
        default:
            critical_not_yet_implemented("UNKNOWN mnemonic");
    }
}
