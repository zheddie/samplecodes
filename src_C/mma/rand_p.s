	.file	"rand_p.c"
	.toc
	.csect .text[PR]
	.align 2
	.lglobl .strtold
	.csect strtold[DS],3
strtold:
	.llong .strtold, TOC[tc0], 0
	.csect .text[PR]
.strtold:
	mflr 0
	std 0,16(1)
	std 31,-8(1)
	stdu 1,-128(1)
	mr 31,1
	std 3,176(31)
	std 4,184(31)
	ld 4,184(31)
	ld 3,176(31)
	bl .strtod
	nop
	fmr 0,1
	fmr 1,0
	addi 1,31,128
	ld 0,16(1)
	mtlr 0
	ld 31,-8(1)
	blr
LT..strtold:
	.long 0
	.byte 0,0,32,97,128,1,2,1
	.long 0
	.long LT..strtold-.strtold
	.short 7
	.byte "strtold"
	.byte 31
	.align 2
	.align 2
	.lglobl .select
	.csect select[DS],3
select:
	.llong .select, TOC[tc0], 0
	.csect .text[PR]
.select:
	mflr 0
	std 0,16(1)
	std 31,-8(1)
	stdu 1,-128(1)
	mr 31,1
	mr 9,3
	std 4,184(31)
	std 5,192(31)
	std 6,200(31)
	std 7,208(31)
	stw 9,176(31)
	lwa 9,176(31)
	ld 7,208(31)
	ld 6,200(31)
	ld 5,192(31)
	ld 4,184(31)
	mr 3,9
	bl .__fd_select
	nop
	mr 9,3
	mr 3,9
	addi 1,31,128
	ld 0,16(1)
	mtlr 0
	ld 31,-8(1)
	blr
LT..select:
	.long 0
	.byte 0,0,32,97,128,1,5,1
	.long 0
	.long LT..select-.select
	.short 6
	.byte "select"
	.byte 31
	.align 2
	.toc
LC..0:
	.tc FD_40dfffc0_0[TC],0x40dfffc000000000
	.csect .text[PR]
	.align 2
	.globl rand_p
	.globl .rand_p
	.csect rand_p[DS],3
rand_p:
	.llong .rand_p, TOC[tc0], 0
	.csect .text[PR]
.rand_p:
	mflr 0
	std 0,16(1)
	std 31,-8(1)
	stdu 1,-160(1)
	mr 31,1
	stfd 1,208(31)
	bl .rand
	nop
	mr 9,3
	extsw 9,9
	std 9,128(31)
	lfd 0,128(31)
	fcfid 12,0
	lfd 0,LC..0(2)
	fdiv 0,12,0
	stfd 0,112(31)
	lfd 12,112(31)
	lfd 0,208(31)
	fcmpu 0,12,0
	bnl 0,L..10
	li 9,1
	b L..8
L..10:
	li 9,-1
L..8:
	mr 3,9
	addi 1,31,160
	ld 0,16(1)
	mtlr 0
	ld 31,-8(1)
	blr
LT..rand_p:
	.long 0
	.byte 0,0,32,97,128,1,0,3
	.long -1073741824
	.long LT..rand_p-.rand_p
	.short 6
	.byte "rand_p"
	.byte 31
	.align 2
_section_.text:
	.csect .data[RW],4
	.llong _section_.text
