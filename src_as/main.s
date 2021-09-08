.set r0,0; .set SP,1; .set RTOC,2; .set r3,3; .set r4,4
.set r5,5; .set r6,6; .set r7,7; .set r8,8; .set r9,9
.set r10,10; .set r11,11; .set r12,12; .set r13,13; .set r14,14
.set r15,15; .set r16,16; .set r17,17; .set r18,18; .set r19,19
.set r20,20; .set r21,21; .set r22,22; .set r23,23; .set r24,24
.set r25,25; .set r26,26; .set r27,27; .set r28,28; .set r29,29
.set r30,30; .set r31,31
.set fp0,0; .set fp1,1; .set fp2,2; .set fp3,3; .set fp4,4
.set fp5,5; .set fp6,6; .set fp7,7; .set fp8,8; .set fp9,9
.set fp10,10; .set fp11,11; .set fp12,12; .set fp13,13; .set fp14,14
.set fp15,15; .set fp16,16; .set fp17,17; .set fp18,18; .set fp19,19
.set fp20,20; .set fp21,21; .set fp22,22; .set fp23,23; .set fp24,24
.set fp25,25; .set fp26,26; .set fp27,27; .set fp28,28; .set fp29,29
.set fp30,30; .set fp31,31
.set v0,0; .set v1,1; .set v2,2; .set v3,3; .set v4,4
.set v5,5; .set v6,6; .set v7,7; .set v8,8; .set v9,9
.set v10,10; .set v11,11; .set v12,12; .set v13,13; .set v14,14
.set v15,15; .set v16,16; .set v17,17; .set v18,18; .set v19,19
.set v20,20; .set v21,21; .set v22,22; .set v23,23; .set v24,24
.set v25,25; .set v26,26; .set v27,27; .set v28,28; .set v29,29
.set v30,30; .set v31,31
.set x0,0; .set x1,1; .set x2,2; .set x3,3; .set x4,4
.set x5,5; .set x6,6; .set x7,7; .set x8,8; .set x9,9
.set x10,10; .set x11,11; .set x12,12; .set x13,13; .set x14,14
.set x15,15; .set x16,16; .set x17,17; .set x18,18; .set x19,19
.set x20,20; .set x21,21; .set x22,22; .set x23,23; .set x24,24
.set x25,25; .set x26,26; .set x27,27; .set x28,28; .set x29,29
.set x30,30; .set x31,31; .set x32,32; .set x33,33; .set x34,34
.set x35,35; .set x36,36; .set x37,37; .set x38,38; .set x39,39
.set x40,40; .set x41,41; .set x42,42; .set x43,43; .set x44,44
.set x45,45; .set x46,46; .set x47,47; .set x48,48; .set x49,49
.set x50,50; .set x51,51; .set x52,52; .set x53,53; .set x54,54
.set x55,55; .set x56,56; .set x57,57; .set x58,58; .set x59,59
.set x60,60; .set x61,61; .set x62,62; .set x63,63
.set q0,0; .set q1,1; .set q2,2; .set q3,3; .set q4,4
.set q5,5; .set q6,6; .set q7,7; .set q8,8; .set q9,9
.set q10,10; .set q11,11; .set q12,12; .set q13,13; .set q14,14
.set q15,15; .set q16,16; .set q17,17; .set q18,18; .set q19,19
.set q20,20; .set q21,21; .set q22,22; .set q23,23; .set q24,24
.set q25,25; .set q26,26; .set q27,27; .set q28,28; .set q29,29
.set q30,30; .set q31,31
.set MQ,0; .set XER,1; .set FROM_RTCU,4; .set FROM_RTCL,5; .set FROM_DEC,6
.set LR,8; .set CTR,9; .set TID,17; .set DSISR,18; .set DAR,19; .set TO_RTCU,20
.set TO_RTCL,21; .set TO_DEC,22; .set SDR_0,24; .set SDR_1,25; .set SRR_0,26
.set SRR_1,27
.set BO_dCTR_NZERO_AND_NOT,0; .set BO_dCTR_NZERO_AND_NOT_1,1
.set BO_dCTR_ZERO_AND_NOT,2; .set BO_dCTR_ZERO_AND_NOT_1,3
.set BO_IF_NOT,4; .set BO_IF_NOT_1,5; .set BO_IF_NOT_2,6
.set BO_IF_NOT_3,7; .set BO_dCTR_NZERO_AND,8; .set BO_dCTR_NZERO_AND_1,9
.set BO_dCTR_ZERO_AND,10; .set BO_dCTR_ZERO_AND_1,11; .set BO_IF,12
.set BO_IF_1,13; .set BO_IF_2,14; .set BO_IF_3,15; .set BO_dCTR_NZERO,16
.set BO_dCTR_NZERO_1,17; .set BO_dCTR_ZERO,18; .set BO_dCTR_ZERO_1,19
.set BO_ALWAYS,20; .set BO_ALWAYS_1,21; .set BO_ALWAYS_2,22
.set BO_ALWAYS_3,23; .set BO_dCTR_NZERO_8,24; .set BO_dCTR_NZERO_9,25
.set BO_dCTR_ZERO_8,26; .set BO_dCTR_ZERO_9,27; .set BO_ALWAYS_8,28
.set BO_ALWAYS_9,29; .set BO_ALWAYS_10,30; .set BO_ALWAYS_11,31
.set CR0_LT,0; .set CR0_GT,1; .set CR0_EQ,2; .set CR0_SO,3
.set CR1_FX,4; .set CR1_FEX,5; .set CR1_VX,6; .set CR1_OX,7
.set CR2_LT,8; .set CR2_GT,9; .set CR2_EQ,10; .set CR2_SO,11
.set CR3_LT,12; .set CR3_GT,13; .set CR3_EQ,14; .set CR3_SO,15
.set CR4_LT,16; .set CR4_GT,17; .set CR4_EQ,18; .set CR4_SO,19
.set CR5_LT,20; .set CR5_GT,21; .set CR5_EQ,22; .set CR5_SO,23
.set CR6_LT,24; .set CR6_GT,25; .set CR6_EQ,26; .set CR6_SO,27
.set CR7_LT,28; .set CR7_GT,29; .set CR7_EQ,30; .set CR7_SO,31
.set TO_LT,16; .set TO_GT,8; .set TO_EQ,4; .set TO_LLT,2; .set TO_LGT,1

	.rename	H.4.NO_SYMBOL{PR},""
	.rename	H.12.main{TC},"main"

	.lglobl	H.4.NO_SYMBOL{PR}       
	.globl	.main                   
	.globl	main{DS}                
	.extern	.func1{PR}              


# .text section
	.file	"main.c"                
	.machine	"ppc64"


	.csect	H.4.NO_SYMBOL{PR}, 7    
.main:                                  # 0x0000000000000000 (H.4.NO_SYMBOL)
	mfspr      r0,LR
	stdu       SP,-128(SP)
	std        r0,144(SP)
	addi       r3,r0,2021
	bl         .func1{PR}
	ori        r0,r0,0x0000
	stw        r3,112(SP)
	lwa        r3,112(SP)
	ld         r0,144(SP)
	mtspr      LR,r0
	addi       SP,SP,128
	bclr       BO_ALWAYS,CR0_LT
	.long	0x00000000
# traceback table
	.byte	0x00			# VERSION=0
	.byte	0x00			# LANG=TB_C
	.byte	0x20			# IS_GL=0,IS_EPROL=0,HAS_TBOFF=1
					# INT_PROC=0,HAS_CTL=0,TOCLESS=0
					# FP_PRESENT=0,LOG_ABORT=0
	.byte	0x41			# INT_HNDL=0,NAME_PRESENT=1
					# USES_ALLOCA=0,CL_DIS_INV=WALK_ONCOND
					# SAVES_CR=0,SAVES_LR=1
	.byte	0x80			# STORES_BC=1,FPR_SAVED=0
	.byte	0x00			# GPR_SAVED=0
	.byte	0x00			# FIXEDPARMS=0
	.byte	0x01			# FLOATPARMS=0,PARMSONSTK=1
	.long	0x00000030		# TB_OFFSET
	.short	4			# NAME_LEN
	.byte	"main"                  # NAME

	.byte	0			# padding
	.byte	0			# padding
# End of traceback table
	.long	0x00000000              # "\0\0\0\0"
	.long	0x00000000              # "\0\0\0\0"
	.long	0x00000000              # "\0\0\0\0"
	.long	0x00000000              # "\0\0\0\0"
	.long	0x00000000              # "\0\0\0\0"
	.long	0x00000000              # "\0\0\0\0"
	.long	0x00000000              # "\0\0\0\0"
	.long	0x00000000              # "\0\0\0\0"
	.long	0x00000000              # "\0\0\0\0"
	.long	0x00000000              # "\0\0\0\0"
	.long	0x00000000              # "\0\0\0\0"
	.long	0x00000000              # "\0\0\0\0"
	.long	0x00000000              # "\0\0\0\0"
	.long	0x00000000              # "\0\0\0\0"
# End	csect	H.4.NO_SYMBOL{PR}

# .data section


	.toc	                        # 0x0000000000000080 
T.12.main:
	.tc	H.12.main{TC},main{DS}  


	.csect	main{DS}, 3             
	.llong	.main                   # "\0\0\0\0\0\0\0\0"
	.llong	TOC{TC0}                # "\0\0\0\0\0\0\0\200"
	.long	0x00000000              # "\0\0\0\0"
	.long	0x00000000              # "\0\0\0\0"
# End	csect	main{DS}



# .bss section


# dwarf sections

# end dwarf sections
