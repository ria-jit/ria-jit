.section .data
msg:
    .string "Hello World\n"

.section .text
.globl _main
_main:
	//F-extension
	flw f0, 0(x0)
	fsw f0, 0(x0)
	fmadd.s f0, f0, f0, f0
	fmsub.s f0, f0, f0, f0
	fnmsub.s f0, f0, f0, f0
	fnmadd.s f0, f0, f0, f0
	fadd.s f0, f0, f0
	fsub.s f0, f0, f0
	fmul.s f0, f0, f0
	fdiv.s f0, f0, f0
	fsqrt.s f0, f0
	fsgnj.s f0, f0, f0
	fsgnjn.s f0, f0, f0
	fsgnjx.s f0, f0, f0
	fmin.s f0, f0, f0
	fmax.s f0, f0, f0
	fcvt.w.s x0, f0
	fcvt.wu.s x0, f0
	fmv.x.w x0, f0
	feq.s x0, f0, f0
	flt.s x0, f0, f0
	fle.s x0, f0, f0
	fclass.s x0, f0
	fcvt.s.w f0, x0
	fcvt.s.wu f0, x0
	fmv.w.x f0, x0
	fcvt.l.s x0, f0
	fcvt.lu.s x0, f0
	fcvt.s.l f0, x0
	fcvt.s.lu f0, x0

	//D-extension
	fld f0, 0(x0)
	fsd f0, 0(x0)
	fmadd.d f0, f0, f0, f0
	fmsub.d f0, f0, f0, f0
	fnmsub.d f0, f0, f0, f0
	fnmadd.d f0, f0, f0, f0
	fadd.d f0, f0, f0
	fsub.d f0, f0, f0
	fmul.d f0, f0, f0
	fdiv.d f0, f0, f0
	fsqrt.d f0, f0
	fsgnj.d f0, f0, f0
	fsgnjn.d f0, f0, f0
	fsgnjx.d f0, f0, f0
	fmin.d f0, f0, f0
	fmax.d f0, f0, f0
	fcvt.s.d f0, f0
	fcvt.d.s f0, f0
	fcvt.w.d x0, f0
	fcvt.wu.d x0, f0
	fmv.x.d x0, f0
	feq.d x0, f0, f0
	flt.d x0, f0, f0
	fle.d x0, f0, f0
	fclass.d x0, f0
	fcvt.d.w f0, x0
	fcvt.d.wu f0, x0
	fmv.d.x f0, x0
	fcvt.l.d x0, f0
	fcvt.lu.d x0, f0
	fcvt.d.l f0, x0
	fcvt.d.lu f0, x0