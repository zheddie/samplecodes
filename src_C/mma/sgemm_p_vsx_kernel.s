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
    /* adjust lda, ldb, ldc for vector size 4 */
    slwi 7, 7, 2
    slwi 8, 8, 2
    slwi 9, 9, 2
    /* Reset VSX registers */
    xxlxor 0, 0, 0
    xxlxor 1, 1, 1
    xxlxor 2, 2, 2
    xxlxor 3, 3, 3
    /* LOOP for K to 0 */
    K_LOOP:
    /* Load 4 elements of A, B */
    lxv 32, 0(3)
    lxv 33, 0(4)
    /* Copy each A[i] 4 times */
    xxspltw 34, 32, 3
    xxspltw 35, 32, 2
    xxspltw 36, 32, 1
    xxspltw 37, 32, 0
    /* Multiply-Add-Accumulate */
    xvmaddasp 0, 34, 33
    xvmaddasp 1, 35, 33
    xvmaddasp 2, 36, 33
    xvmaddasp 3, 37, 33
    /* Update Loop count & A,B */
    add 3, 3, 7
    add 4, 4, 8
    addic. 6, 6, -1
    bgt K_LOOP
    /* Offsets of 4x4 C Matrix */
    slwi 3, 9, 1
    add 4, 5, 9
    add 6, 5, 3
    add 7, 4, 3
    /* Store the 4x4 c Matrix */
    /*stxv 0, 0(5)
    stxv 1, 0(4)
    stxv 2, 0(6)
    stxv 3, 0(7)
*/
    stxv 3, 0(5)
    stxv 2, 0(4)
    stxv 1, 0(6)
    stxv 0, 0(7)
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
