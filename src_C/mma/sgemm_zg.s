	.file	"sgemm_zg.c"
	.toc
	.csect .text[PR]
	.csect _sgemmzg.rop_[RO],4
	.align 3
LC..0:
	.byte "hello"
	.byte 0
	.toc
LC..1:
	.tc LC..0[TC],LC..0
	.csect .text[PR]
	.align 2
	.globl sgemm_kernel_4x4
	.globl .sgemm_kernel_4x4
	.csect sgemm_kernel_4x4[DS],3
sgemm_kernel_4x4:
	.llong .sgemm_kernel_4x4, TOC[tc0], 0
	.csect .text[PR]
.sgemm_kernel_4x4:
	mflr 0
	std 0,16(1)
	std 31,-8(1)
	stdu 1,-128(1)
	mr 31,1
	std 3,176(31)
	std 4,184(31)
	std 5,192(31)
	mr 10,9
	mr 9,6
	stw 9,200(31)
	mr 9,7
	stw 9,208(31)
	mr 9,8
	stw 9,216(31)
	mr 9,10
	stw 9,224(31)
	ld 3,LC..1(2)
	bl .printf
	nop
	nop
	addi 1,31,128
	ld 0,16(1)
	mtlr 0
	ld 31,-8(1)
	blr
LT..sgemm_kernel_4x4:
	.long 0
	.byte 0,0,32,97,128,1,7,1
	.long 0
	.long LT..sgemm_kernel_4x4-.sgemm_kernel_4x4
	.short 16
	.byte "sgemm_kernel_4x4"
	.byte 31
	.align 2
_section_.text:
	.csect .data[RW],4
	.llong _section_.text
