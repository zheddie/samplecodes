MYMODULE=mymodule4.c
PASE2ILE=PASEtoILE4.c
PGMNAME=pase2ile
mod: ${MYMODULE}
	system -e "CRTCMOD MODULE(GAVIN/MYMOD) SRCSTMF('/qopensys/home/zhanggan/samplecodes/src_C/ILE/${MYMODULE}')"
# 	CRTCMOD MODULE(MYLIB/MYMODULE)
#         SRCFILE(MYLIB/SRCPF)
#         TERASPACE(*YES *TSIFC) 
srvpgm: mod
	system -e "CRTSRVPGM2 SRVPGM(GAVIN/MYSRVPGM) MODULE(GAVIN/MYMOD) EXPORT(*ALL) STGMDL(*SNGLVL)"
# 	CRTSRVPGM SRVPGM(MYLIB/MYSRVPGM) 
#         MODULE(MYLIB/MOMODULE)
dsp:
	system -e "dspsrvpgm SRVPGM(GAVIN/MYSRVPGM)"
pasepgm:
	xlc -q32 -o ${PGMNAME} ${PASE2ILE}
	#xlc -q32 -o ${PGMNAME} -qldbl128 -qalign=natural ${PASE2ILE}
	#xlc -o PASEtoILE -qldbl128 -qalign=natural -bI:/afs/rich.xyz.com/usr1/shupe/PASE/as400_libc.exp ${PASE2ILE}
run:
	./${PGMNAME}
all: srvpgm pasepgm
#Reference:
#https://www.ibm.com/support/knowledgecenter/ssw_ibm_i_73/rzalf/rzalfcallileexamp.htm
