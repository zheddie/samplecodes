	.file	"i8gemm_mma.s"
	.toc
	.csect .text[PR]
	.align 2
	.globl i8gemm_mma_4x4
	.globl .i8gemm_mma_4x4
	.csect i8gemm_mma_4x4[DS],3
i8gemm_mma_4x4:
	.llong .i8gemm_mma_4x4, TOC[tc0], 0
	.csect .text[PR]
.i8gemm_mma_4x4:
	/* adjust lda, ldb, ldc for vector size 4 */
	slwi 7, 7, 2
	slwi 8, 8, 2
	slwi 9, 9, 2
	/* Reset acc0 & prime */
	xxsetaccz 0
	/* LOOP for K to 0 */
	K_LOOP:
	/* Load 4 elements of A, B */
	lxv 32, 0(3)
	lxv 33, 0(4)
	/* Multiply-Add-Accumulate */
	xvi8ger4pp 0, 32, 33
	/* Update Loop count & A,B */
	add 3, 3, 7
	add 4, 4, 8
	addic. 6, 6, -1
	bgt K_LOOP
	/* Unprime the accumulator 0 */
	xxmfacc 0
	/* Offsets of 4x4 C Matrix */
	slwi 3, 9, 1
	add 4, 5, 9
	add 6, 5, 3
	add 7, 4, 3
	/* Store the 4x4 C Matrix */
	stxv 0, 0(5)
	stxv 1, 0(4)
	stxv 2, 0(6)
	stxv 3, 0(7)
	blr
LT..i8gemm_mma_4x4:
	.long 0
	.byte 0,0,32,96,128,1,7,1
	.long 0
	.long LT..i8gemm_mma_4x4-.i8gemm_mma_4x4
	.short 16
	.byte "i8gemm_mma_4x4"
	.byte 31
	.align 2
_section_.text:
	.csect .data[RW],4
	.llong _section_.text
