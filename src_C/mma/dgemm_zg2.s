	.file	"dgemm_zg2.c"
	.toc
	.csect .text[PR]
	.align 2
	.globl dgemm_kernel_zg
	.globl .dgemm_kernel_zg
	.csect dgemm_kernel_zg[DS],3
dgemm_kernel_zg:
	.llong .dgemm_kernel_zg, TOC[tc0], 0
	.csect .text[PR]
.dgemm_kernel_zg:
	std 31,-8(1)
	stdu 1,-64(1)
	mr 31,1
	mr 9,3
	stw 9,112(31)
	nop
	addi 1,31,64
	ld 31,-8(1)
	blr
LT..dgemm_kernel_zg:
	.long 0
	.byte 0,0,32,96,128,1,1,1
	.long 0
	.long LT..dgemm_kernel_zg-.dgemm_kernel_zg
	.short 15
	.byte "dgemm_kernel_zg"
	.byte 31
	.align 2
_section_.text:
	.csect .data[RW],4
	.llong _section_.text
