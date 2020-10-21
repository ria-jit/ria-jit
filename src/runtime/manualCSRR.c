//
// Created by noah on 20.10.20.
//

#include "manualCSRR.h"
#include <xmmintrin.h>

static inline uint32_t to_RISCV_flags(uint32_t flags) {
    //set riscv flags depending on x86 flags
    uint32_t riscv_flags = 0;
    if (flags & SSE_NX) riscv_flags |= rv_NX;
    if (flags & SSE_UF) riscv_flags |= rv_UF;
    if (flags & SSE_OF) riscv_flags |= rv_OF;
    if (flags & SSE_DZ) riscv_flags |= rv_DZ;
    if (flags & SSE_NV) riscv_flags |= rv_NV;
    return riscv_flags;
}

static inline uint32_t to_SSE_flags(uint32_t flags) {
    //set riscv flags depending on SSE flags
    uint32_t SSE_flags = 0;
    if (flags & rv_NX) SSE_flags |= SSE_NX;
    if (flags & rv_UF) SSE_flags |= SSE_UF;
    if (flags & rv_OF) SSE_flags |= SSE_OF;
    if (flags & rv_DZ) SSE_flags |= SSE_DZ;
    if (flags & rv_NV) SSE_flags |= SSE_NV;
    return SSE_flags;
}

__attribute__((force_align_arg_pointer))
void manualCSRR(t_risc_reg_val *registerValues, t_risc_imm imm, t_risc_reg src_1, t_risc_mnem mnem, t_risc_reg dest) {
    uint32_t csr = 0;
    uint32_t mxcsr = _mm_getcsr();
    //load csr
    switch (imm) {
        case FFLAGS: {
            //only load flags
            csr = to_RISCV_flags(mxcsr);
        }
            break;
        case FRM: {
            //only load rounding mode and shift
            uint32_t temp = mxcsr >> SSE_ROUND_SHIFT;
            temp &= SSE_ROUND_MASK;
            //convert to RISCV rounding mode
            csr = to_RISCV_RoundMode(temp);
        }
            break;
        case FCSR: {
            //load rounding mode and flags
            csr = to_RISCV_flags(mxcsr);
            uint32_t temp = mxcsr >> SSE_ROUND_SHIFT;
            temp &= SSE_ROUND_MASK;
            //convert to RISCV rounding mode
            csr |= to_RISCV_RoundMode(temp) << FE_COUNT_RISCV;
        }
            break;
        default:
            critical_not_yet_implemented("unsupported immediate in manualCSRR");
            break;
    }

    uint64_t oldRegSrc1Value = registerValues[src_1]; //avoid overwrite if dest = src_1

    if(dest != x0) {
        registerValues[dest] = csr;
    }

    switch (mnem) {
        case CSRRW:
            csr = oldRegSrc1Value;
            break;
        case CSRRWI:
            csr = src_1; //imm stored in src_1 in this case
            break;
        case CSRRS:
            //sets masked bits
            csr |= oldRegSrc1Value;
            break;
        case CSRRSI:
            //sets masked bits
            csr |= src_1; //imm stored in src_1 in this case
            break;
        case CSRRC:
            //clears masked bits
            csr &= ~oldRegSrc1Value;
            break;
        case CSRRCI:
            //clears masked bits
            csr &= ~src_1; //imm stored in src_1 in this case
            break;
        default:
            critical_not_yet_implemented("unsupported mnem in manualCSRR");
            break;
    }

    //don't write back if src1 is 0 for CSRRS and CSRRC see RISCV-Spec p.56
    if (!((mnem == CSRRS || mnem == CSRRSI || mnem == CSRRC || mnem == CSRRCI) && src_1 == x0)) {
        //write back csr
        switch (imm) {
            case FFLAGS: {
                //only write flags
                //clear flags
                mxcsr &= ~SSE_FLAGS_MASK;
                //set new flags
                mxcsr |= to_SSE_flags(csr);
            }
                break;
            case FRM: {
                //only write rounding mode
                //clear rounding mode
                mxcsr &= ~(SSE_ROUND_MASK << SSE_ROUND_SHIFT);
                //set new rounding mode
                mxcsr |= to_SSE_RoundMode(csr & 0x7) << SSE_ROUND_SHIFT; //only lowest 3bits should have round mode
            }
                break;
            case FCSR: {
                //write rounding mode and flags
                //clear flags and round mode
                mxcsr &= ~(SSE_FLAGS_MASK | (SSE_ROUND_MASK << SSE_ROUND_SHIFT));
                //set new flags
                mxcsr |= to_SSE_flags(csr);
                //shift csr to get round mode in first 3 bits
                csr >>= FE_COUNT_RISCV;
                //set new rounding mode
                mxcsr |= to_SSE_RoundMode(csr & 0x7) << SSE_ROUND_SHIFT; //only lowest 3bits should have round mode
            }
                break;
            default:
                critical_not_yet_implemented("unsupported immediate in manualCSRR");
                break;
        }
        _mm_setcsr(mxcsr);
    }
}
