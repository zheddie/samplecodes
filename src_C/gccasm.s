	.file	"gccasm.cc"
	.toc
	.csect .text[PR]
	.align 2
	.lglobl .wcsftime
	.csect wcsftime[DS],3
wcsftime:
	.llong .wcsftime, TOC[tc0], 0
	.csect .text[PR]
.wcsftime:
LFB..0:
	mflr 0
	std 0,16(1)
	std 31,-8(1)
	stdu 1,-128(1)
LCFI..0:
	mr 31,1
LCFI..1:
	std 3,176(31)
	std 4,184(31)
	std 5,192(31)
	std 6,200(31)
	ld 6,200(31)
	ld 5,192(31)
	ld 4,184(31)
	ld 3,176(31)
	bl .__iso_wcsftime
	nop
	mr 9,3
	mr 3,9
	addi 1,31,128
LCFI..2:
	ld 0,16(1)
	mtlr 0
	ld 31,-8(1)
	blr
LT..wcsftime:
	.long 0
	.byte 0,9,32,97,128,1,4,1
	.long 0
	.long LT..wcsftime-.wcsftime
	.short 8
	.byte "wcsftime"
	.byte 31
	.align 2
LFE..0:
	.align 2
	.lglobl .wcstok
	.csect wcstok[DS],3
wcstok:
	.llong .wcstok, TOC[tc0], 0
	.csect .text[PR]
.wcstok:
LFB..1:
	mflr 0
	std 0,16(1)
	std 31,-8(1)
	stdu 1,-128(1)
LCFI..3:
	mr 31,1
LCFI..4:
	std 3,176(31)
	std 4,184(31)
	std 5,192(31)
	ld 5,192(31)
	ld 4,184(31)
	ld 3,176(31)
	bl .__iso_wcstok
	nop
	mr 9,3
	mr 3,9
	addi 1,31,128
LCFI..5:
	ld 0,16(1)
	mtlr 0
	ld 31,-8(1)
	blr
LT..wcstok:
	.long 0
	.byte 0,9,32,97,128,1,3,1
	.long 0
	.long LT..wcstok-.wcstok
	.short 6
	.byte "wcstok"
	.byte 31
	.align 2
LFE..1:
	.csect _gccasm.rw_[RO],4
_ZStL19piecewise_construct:
	.space 1
	.csect .text[PR]
	.align 2
	.lglobl ._ZL20__gthread_key_deletei
	.csect _ZL20__gthread_key_deletei[DS],3
_ZL20__gthread_key_deletei:
	.llong ._ZL20__gthread_key_deletei, TOC[tc0], 0
	.csect .text[PR]
._ZL20__gthread_key_deletei:
LFB..419:
	std 31,-8(1)
	stdu 1,-64(1)
LCFI..6:
	mr 31,1
LCFI..7:
	mr 9,3
	stw 9,112(31)
	li 9,0
	mr 3,9
	addi 1,31,64
LCFI..8:
	ld 31,-8(1)
	blr
LT.._ZL20__gthread_key_deletei:
	.long 0
	.byte 0,9,32,96,128,1,1,1
	.long 0
	.long LT.._ZL20__gthread_key_deletei-._ZL20__gthread_key_deletei
	.short 26
	.byte "_ZL20__gthread_key_deletei"
	.byte 31
	.align 2
LFE..419:
	.align 2
	.lglobl .strtold
	.csect strtold[DS],3
strtold:
	.llong .strtold, TOC[tc0], 0
	.csect .text[PR]
.strtold:
LFB..817:
	mflr 0
	std 0,16(1)
	std 31,-8(1)
	stdu 1,-128(1)
LCFI..9:
	mr 31,1
LCFI..10:
	std 3,176(31)
	std 4,184(31)
	ld 4,184(31)
	ld 3,176(31)
	bl .strtod
	nop
	fmr 0,1
	fmr 1,0
	addi 1,31,128
LCFI..11:
	ld 0,16(1)
	mtlr 0
	ld 31,-8(1)
	blr
LT..strtold:
	.long 0
	.byte 0,9,32,97,128,1,2,1
	.long 0
	.long LT..strtold-.strtold
	.short 7
	.byte "strtold"
	.byte 31
	.align 2
LFE..817:
	.align 2
	.lglobl .select
	.csect select[DS],3
select:
	.llong .select, TOC[tc0], 0
	.csect .text[PR]
.select:
LFB..818:
	mflr 0
	std 0,16(1)
	std 31,-8(1)
	stdu 1,-128(1)
LCFI..12:
	mr 31,1
LCFI..13:
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
LCFI..14:
	ld 0,16(1)
	mtlr 0
	ld 31,-8(1)
	blr
LT..select:
	.long 0
	.byte 0,9,32,97,128,1,5,1
	.long 0
	.long LT..select-.select
	.short 6
	.byte "select"
	.byte 31
	.align 2
LFE..818:
	.lcomm _ZStL8__ioinit,1,_gccasm.bss_,2
	.csect _gccasm.rw_[RO],4
	.align 3
LC..0:
	.byte "111111111111"
	.byte 0
	.toc
LC..1:
	.tc LC..0[TC],LC..0
LC..2:
	.tc _ZSt4cout[TC],_ZSt4cout[UA]
LC..3:
	.tc _ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_[TC],_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_[DS]
	.csect .text[PR]
	.align 2
	.globl main
	.globl .main
	.csect main[DS],3
main:
	.llong .main, TOC[tc0], 0
	.csect .text[PR]
.main:
LFB..1441:
	mflr 0
	std 0,16(1)
	std 31,-8(1)
	stdu 1,-160(1)
LCFI..15:
	mr 31,1
LCFI..16:
	ld 9,LC..1(2)
	ld 8,0(9)
	lwz 10,8(9)
	lbz 9,12(9)
	std 8,120(31)
	stw 10,128(31)
	stb 9,132(31)
	li 9,9999
	stw 9,112(31)
	addi 9,31,120
	mr 4,9
	ld 3,LC..2(2)
	bl ._ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	nop
	mr 10,3
	lwz 9,112(31)
	addi 9,9,1
	extsw 9,9
	mr 4,9
	mr 3,10
	bl ._ZNSolsEi
	nop
	mr 9,3
	ld 4,LC..3(2)
	mr 3,9
	bl ._ZNSolsEPFRSoS_E
	nop
	li 9,0
	mr 3,9
	addi 1,31,160
LCFI..17:
	ld 0,16(1)
	mtlr 0
	ld 31,-8(1)
	blr
LT..main:
	.long 0
	.byte 0,9,32,97,128,1,0,1
	.long LT..main-.main
	.short 4
	.byte "main"
	.byte 31
	.align 2
LFE..1441:
	.toc
LC..4:
	.tc _ZStL8__ioinit[TC],_ZStL8__ioinit
LC..5:
	.tc __dso_handle[TC],__dso_handle[UA]
LC..6:
	.tc _ZNSt8ios_base4InitD1Ev[TC],_ZNSt8ios_base4InitD1Ev[DS]
	.csect .text[PR]
	.align 2
	.lglobl ._Z41__static_initialization_and_destruction_0ii
	.csect _Z41__static_initialization_and_destruction_0ii[DS],3
_Z41__static_initialization_and_destruction_0ii:
	.llong ._Z41__static_initialization_and_destruction_0ii, TOC[tc0], 0
	.csect .text[PR]
._Z41__static_initialization_and_destruction_0ii:
LFB..1885:
	mflr 0
	std 0,16(1)
	std 31,-8(1)
	stdu 1,-128(1)
LCFI..18:
	mr 31,1
LCFI..19:
	mr 10,3
	mr 9,4
	stw 10,176(31)
	stw 9,184(31)
	lwz 9,176(31)
	cmpwi 7,9,1
	bne 7,L..15
	lwz 10,184(31)
	li 9,0
	ori 9,9,0xffff
	cmpw 7,10,9
	bne 7,L..15
	ld 3,LC..4(2)
	bl ._ZNSt8ios_base4InitC1Ev
	nop
	ld 5,LC..5(2)
	ld 4,LC..4(2)
	ld 3,LC..6(2)
	bl .__cxa_atexit
	nop
L..15:
	nop
	addi 1,31,128
LCFI..20:
	ld 0,16(1)
	mtlr 0
	ld 31,-8(1)
	blr
LT.._Z41__static_initialization_and_destruction_0ii:
	.long 0
	.byte 0,9,32,97,128,1,2,1
	.long 0
	.long LT.._Z41__static_initialization_and_destruction_0ii-._Z41__static_initialization_and_destruction_0ii
	.short 47
	.byte "_Z41__static_initialization_and_destruction_0ii"
	.byte 31
	.align 2
LFE..1885:
	.align 2
	.lglobl ._GLOBAL__sub_I_main
	.csect _GLOBAL__sub_I_main[DS],3
_GLOBAL__sub_I_main:
	.llong ._GLOBAL__sub_I_main, TOC[tc0], 0
	.csect .text[PR]
._GLOBAL__sub_I_main:
LFB..1886:
	mflr 0
	std 0,16(1)
	std 31,-8(1)
	stdu 1,-128(1)
LCFI..21:
	mr 31,1
LCFI..22:
	li 4,-1
	rldicl 4,4,0,48
	li 3,1
	bl ._Z41__static_initialization_and_destruction_0ii
	addi 1,31,128
LCFI..23:
	ld 0,16(1)
	mtlr 0
	ld 31,-8(1)
	blr
LT.._GLOBAL__sub_I_main:
	.long 0
	.byte 0,9,32,97,128,1,0,1
	.long LT.._GLOBAL__sub_I_main-._GLOBAL__sub_I_main
	.short 19
	.byte "_GLOBAL__sub_I_main"
	.byte 31
	.align 2
LFE..1886:
	.align 2
	.globl _GLOBAL__I_65535_0_main
	.globl ._GLOBAL__I_65535_0_main
	.csect _GLOBAL__I_65535_0_main[DS],3
_GLOBAL__I_65535_0_main:
	.llong ._GLOBAL__I_65535_0_main, TOC[tc0], 0
	.csect .text[PR]
._GLOBAL__I_65535_0_main:
LFB..1887:
	mflr 0
	std 0,16(1)
	std 31,-8(1)
	stdu 1,-128(1)
LCFI..24:
	mr 31,1
LCFI..25:
	bl ._GLOBAL__sub_I_main
	addi 1,31,128
LCFI..26:
	ld 0,16(1)
	mtlr 0
	ld 31,-8(1)
	blr
LT.._GLOBAL__I_65535_0_main:
	.long 0
	.byte 0,9,32,97,128,1,0,1
	.long LT.._GLOBAL__I_65535_0_main-._GLOBAL__I_65535_0_main
	.short 23
	.byte "_GLOBAL__I_65535_0_main"
	.byte 31
	.align 2
LFE..1887:
	.csect _gccasm.ro_[RO],4
	.align 3
	.globl _GLOBAL__F_main
_GLOBAL__F_main:
Lframe..1:
	.vbyte	4,LECIE..1-LSCIE..1
LSCIE..1:
	.vbyte	4,0
	.byte	0x3
	.byte "zR"
	.byte 0
	.byte	0x1
	.byte	0x78
	.byte	0x41
	.byte	0x1
	.byte	0x1c
	.byte	0xc
	.byte	0x1
	.byte	0
	.align 3
LECIE..1:
LSFDE..1:
	.vbyte	4,LEFDE..1-LASFDE..1
LASFDE..1:
	.vbyte	4,LASFDE..1-Lframe..1
	.vbyte	8,LFB..0-$
	.vbyte	8,LFE..0-LFB..0
	.byte	0
	.byte	0x4
	.vbyte	4,LCFI..0-LFB..0
	.byte	0xe
	.byte	0x80,0x1
	.byte	0x11
	.byte	0x41
	.byte	0x7e
	.byte	0x9f
	.byte	0x1
	.byte	0x4
	.vbyte	4,LCFI..1-LCFI..0
	.byte	0xd
	.byte	0x1f
	.byte	0x4
	.vbyte	4,LCFI..2-LCFI..1
	.byte	0xc
	.byte	0x1
	.byte	0
	.csect .text[PR]
	.ref Lframe..1
	.csect _gccasm.ro_[RO],4
	.align 3
LEFDE..1:
LSFDE..3:
	.vbyte	4,LEFDE..3-LASFDE..3
LASFDE..3:
	.vbyte	4,LASFDE..3-Lframe..1
	.vbyte	8,LFB..1-$
	.vbyte	8,LFE..1-LFB..1
	.byte	0
	.byte	0x4
	.vbyte	4,LCFI..3-LFB..1
	.byte	0xe
	.byte	0x80,0x1
	.byte	0x11
	.byte	0x41
	.byte	0x7e
	.byte	0x9f
	.byte	0x1
	.byte	0x4
	.vbyte	4,LCFI..4-LCFI..3
	.byte	0xd
	.byte	0x1f
	.byte	0x4
	.vbyte	4,LCFI..5-LCFI..4
	.byte	0xc
	.byte	0x1
	.byte	0
	.csect .text[PR]
	.ref Lframe..1
	.csect _gccasm.ro_[RO],4
	.align 3
LEFDE..3:
LSFDE..5:
	.vbyte	4,LEFDE..5-LASFDE..5
LASFDE..5:
	.vbyte	4,LASFDE..5-Lframe..1
	.vbyte	8,LFB..818-$
	.vbyte	8,LFE..818-LFB..818
	.byte	0
	.byte	0x4
	.vbyte	4,LCFI..12-LFB..818
	.byte	0xe
	.byte	0x80,0x1
	.byte	0x11
	.byte	0x41
	.byte	0x7e
	.byte	0x9f
	.byte	0x1
	.byte	0x4
	.vbyte	4,LCFI..13-LCFI..12
	.byte	0xd
	.byte	0x1f
	.byte	0x4
	.vbyte	4,LCFI..14-LCFI..13
	.byte	0xc
	.byte	0x1
	.byte	0
	.csect .text[PR]
	.ref Lframe..1
	.csect _gccasm.ro_[RO],4
	.align 3
LEFDE..5:
LSFDE..7:
	.vbyte	4,LEFDE..7-LASFDE..7
LASFDE..7:
	.vbyte	4,LASFDE..7-Lframe..1
	.vbyte	8,LFB..1441-$
	.vbyte	8,LFE..1441-LFB..1441
	.byte	0
	.byte	0x4
	.vbyte	4,LCFI..15-LFB..1441
	.byte	0xe
	.byte	0xa0,0x1
	.byte	0x11
	.byte	0x41
	.byte	0x7e
	.byte	0x9f
	.byte	0x1
	.byte	0x4
	.vbyte	4,LCFI..16-LCFI..15
	.byte	0xd
	.byte	0x1f
	.byte	0x4
	.vbyte	4,LCFI..17-LCFI..16
	.byte	0xc
	.byte	0x1
	.byte	0
	.csect .text[PR]
	.ref Lframe..1
	.csect _gccasm.ro_[RO],4
	.align 3
LEFDE..7:
	.vbyte	4,0
	.csect .text[PR]
_section_.text:
	.csect .data[RW],4
	.llong _section_.text
