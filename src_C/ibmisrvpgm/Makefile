funcs1: funcs2
	system "CRTCMOD MODULE(GAVIN/FUNCS1) SRCSTMF('/QOpenSys/home/zhanggan/samplecodes/src_C/ibmisrvpgm/funcs1.c')"
	system "CRTSRVPGM SRVPGM(GAVIN/FUNCS1) MODULE(*SRVPGM) EXPORT(*SRCFILE) SRCSTMF('/QOpenSys/home/zhanggan/samplecodes/src_C/ibmisrvpgm/funcs1.exp') BNDSRVPGM((GAVIN/FUNCS2))"
funcs2:
	system "CRTCMOD MODULE(GAVIN/FUNCS2) SRCSTMF('/QOpenSys/home/zhanggan/samplecodes/src_C/ibmisrvpgm/funcs2.c')"
	system "CRTSRVPGM SRVPGM(GAVIN/FUNCS2) MODULE(*SRVPGM) EXPORT(*SRCFILE) SRCSTMF('/QOpenSys/home/zhanggan/samplecodes/src_C/ibmisrvpgm/funcs2.exp')"
caller: funcs1
	system "CRTCMOD MODULE(GAVIN/caller) SRCSTMF('/QOpenSys/home/zhanggan/samplecodes/src_C/ibmisrvpgm/main.c')"
	system "CRTPGM PGM(GAVIN/CALLER) MODULE(*PGM) BNDSRVPGM((GAVIN/FUNCS1))"
all: funcs1 funcs2 caller
clean:
	rm /QSYS.LIB/GAVIN.LIB/FUNCS*
	rm /QSYS.LIB/GAVIN.LIB/CALLER*
