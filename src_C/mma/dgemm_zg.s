	.file	"dgemm_zg.c"
	.toc
	.csect .text[PR]
	.align 2
	.globl dgemm_kernel_4x2
	.globl .dgemm_kernel_4x2
	.csect dgemm_kernel_4x2[DS],3
dgemm_kernel_4x2:
	.llong .dgemm_kernel_4x2, TOC[tc0], 0
	.csect .text[PR]
.dgemm_kernel_4x2:
	std 31,-8(1)
	stdu 1,-64(1)
	mr 31,1
	std 3,112(31)
	std 4,120(31)
	std 5,128(31)
	mr 10,9
	mr 9,6
	stw 9,136(31)
	mr 9,7
	stw 9,144(31)
	mr 9,8
	stw 9,152(31)
	mr 9,10
	stw 9,160(31)
	nop
	addi 1,31,64
	ld 31,-8(1)
	blr
LT..dgemm_kernel_4x2:
	.long 0
	.byte 0,0,32,96,128,1,7,1
	.long 0
	.long LT..dgemm_kernel_4x2-.dgemm_kernel_4x2
	.short 16
	.byte "dgemm_kernel_4x2"
	.byte 31
	.align 2
_section_.text:
	.csect .data[RW],4
	.llong _section_.text
