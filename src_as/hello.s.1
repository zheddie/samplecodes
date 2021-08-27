	.file	"hello.c"
	.toc
	.csect .text[PR]
	.csect _hello.rop_[RO],4
	.align 3
LC..0:
	.byte "HelloC"
	.byte 0
	.toc
LC..1:
	.tc LC..0[TC],LC..0
	.csect .text[PR]
	.align 2
	.globl main
	.globl .main
	.csect main[DS],3
main:
	.llong .main, TOC[tc0], 0
	.csect .text[PR]
.main:
	mflr 0
	std 0,16(1)
	std 31,-8(1)
	stdu 1,-128(1)
	mr 31,1
	ld 3,LC..1(2)
	bl .puts
	nop
	li 9,0
	mr 3,9
	addi 1,31,128
	ld 0,16(1)
	mtlr 0
	ld 31,-8(1)
	blr
LT..main:
	.long 0
	.byte 0,0,32,97,128,1,0,1
	.long LT..main-.main
	.short 4
	.byte "main"
	.byte 31
	.align 2
_section_.text:
	.csect .data[RW],4
	.llong _section_.text
