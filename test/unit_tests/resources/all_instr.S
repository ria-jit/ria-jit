.section .data
msg:
    .string "Hello World\n"

.section .text
.globl _main
_main:
	lui a1, 42
	auipc a1, 42
	jal a1,_main
	jalr a1,a2,42
	beq a1,s2,_main
	bne a1,a2,_main
	blt a1,a2,_main
	bge a1,a2,_main
	bltu a1,a2,_main
	bgeu a1,a2,_main
	lb a1,%lo(msg)(a2)
	lh a1,%lo(msg)(a2)
	lw a1,%lo(msg)(a2)
	lbu a1,%lo(msg)(a2)
	lhu a1,%lo(msg)(a2)
	sb a1, (a2)
	sh a1, (a2)
	sw a1, (a2)
	addi a1,a2,42
	slti a1,a2,42
	sltiu a1,a2,42
	xori a1,a2,42
	ori a1,a2,42
	andi a1,a2,42
	slli a1,a2,42
	srli a1,a2,42
	srai a1,a2,42
	add a1,a2,a3
	sub a1,a2,a3
	sll a1,a2,a3
	slt a1,a2,a3
	sltu a1,a2,a3
	xor a1,a2,a3
	srl a1,a2,a3
	sra a1,a2,a3
	or a1,a2,a3
	and a1,a2,a3
	fence
	ecall
	ebreak
	fence.i
	csrrw a1,fcsr,a2
    csrrs a1,fcsr,a2
    csrrc a1,fcsr,a2
    csrrwi a1,fcsr,8
    csrrsi a1,fcsr,8
    csrrci a1,fcsr,8
	lwu a1,%lo(msg)(a2)
	ld a1,%lo(msg)(a2)
	sd a1,(a2)
	addiw a1,a2,42
	slliw a1,a2,8
	srliw a1,a2,8
	sraiw a1,a2,8
	addw a1,a2,a3
	subw a1,a2,a3
	sllw a1,a2,a3
	srlw a1,a2,a3
	sraw a1,a2,a3
	mul a1,a2,a3
	mulh a1,a2,a3
	mulhsu a1,a2,a3
	mulhu a1,a2,a3
	div a1,a2,a3
	divu a1,a2,a3
	rem a1,a2,a3
	remu a1,a2,a3
	mulw a1,a2,a3
	divw a1,a2,a3
	divuw a1,a2,a3
	remw a1,a2,a3
	remuw a1,a2,a3
	lr.w  a1, (a2)
	sc.w a1,a2, (a3)
	amoswap.w a1,a2, (a3)
	amoadd.w a1,a2, (a3)
	amoxor.w a1,a2, (a3)
	amoor.w a1,a2, (a3)
	amomin.w a1,a2, (a3)
	amomax.w a1,a2, (a3)
	amominu.w a1,a2, (a3)
	amomaxu.w a1,a2, (a3)
	lr.d  a1, (a2)
	sc.d a1,a2, (a3)
	amoswap.d a1,a2, (a3)
	amoadd.d a1,a2, (a3)
	amoxor.d a1,a2, (a3)
	amoor.d a1,a2, (a3)
	amomin.d a1,a2, (a3)
	amomax.d a1,a2, (a3)
	amominu.d a1,a2, (a3)
	amomaxu.d a1,a2, (a3)