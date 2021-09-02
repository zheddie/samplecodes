	.file	"func1.c"
	.toc
	.csect .text[PR]
	.align 2
	.globl func1
	.globl .func1
	.csect func1[DS],3
func1:
	.llong .func1, TOC[tc0], 0
	.csect .text[PR]
.func1:
	std 31,-8(1)
	stdu 1,-64(1)
	mr 31,1
	mr 9,3
	stw 9,112(31)
	lwa 9,112(31)
	mr 3,9
	addi 1,31,64
	ld 31,-8(1)
	blr
LT..func1:
	.long 0
	.byte 0,0,32,96,128,1,1,1
	.long 0
	.long LT..func1-.func1
	.short 5
	.byte "func1"
	.byte 31
	.align 2
_section_.text:
	.csect .data[RW],4
	.llong _section_.text
