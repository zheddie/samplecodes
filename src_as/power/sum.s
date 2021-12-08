.data
.align 3
first_value:
	.quad 7
second_value:
	.quad 12
.section ".opd", "aw"
_start:
	.quad ._start,.TOC.@tocbase,0

.text
._start:
	lis 7,first_value@highest
	ori 7,7,first_value@higher
        rldicr 7,7,32,31
	oris 7,7,first_value@h
	ori 7,7, first_value@l

	ld 4,0(7)

	lis 7,second_value@highest
	ori 7,7,second_value@higher
        rldicr 7,7,32,31
	oris 7,7,second_value@h
	ori 7,7, second_value@l

	ld 5,0(7)
	add 6,4,5
	li 0,1
	mr 3,6
	sc
