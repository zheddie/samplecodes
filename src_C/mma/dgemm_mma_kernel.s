	.file	"dgemm_mma_kernel.s"
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
	/* adjust lda, ldb, ldc for vector size 8 */
	slwi 7, 7, 3
	slwi 8, 8, 3
	slwi 9, 9, 3
	/* Reset acc0 */
	xxsetaccz 0
	/* LOOP for K to 0 */
	K_LOOP:
	/* Load 4 elements of A, B */
	lxvp 32, 0(3)
	lxv 34, 0(4)
	/* Multiply-Add-Accumulate */
	xvf64gerpp 0, 32, 34
	/* Update Loop count & A,B */
	add 3, 3, 7
	add 4, 4, 8
	addic. 6, 6, -1
	bgt K_LOOP
	/* Unprime the accumulator 0 */
	xxmfacc 0
	/* Offsets of 4x2 C Matrix */
	slwi 3, 9, 1
	add 4, 5, 9
	add 6, 5, 3
	add 7, 4, 3
	/* Store the 4x2 C Matrix */
	stxv 0, 0(5)
	stxv 1, 0(4)
	stxv 2, 0(6)
	stxv 3, 0(7)
/*	stxv 3, 0(5)
	stxv 2, 0(4)
	stxv 1, 0(6)
	stxv 0, 0(7) */
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
