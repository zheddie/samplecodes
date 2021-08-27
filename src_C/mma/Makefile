rb01: mma_rb_01.c
	gcc -o rb01 mma_rb_01.c
sgemm: sgemm_4x4.cc sgemm_vsx_kernel.s
	g++ -mcpu=power10 -O2 sgemm_4x4.cc sgemm_vsx_kernel.s -o sgemm_vsx