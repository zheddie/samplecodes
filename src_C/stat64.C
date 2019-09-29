#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
int
statMode(const char *path, int *mode)
{
	printf("zg.statMode().CP0,path=%s=\n",path);
    struct stat64 sb;
    if (stat64(path, &sb) == 0) {

        *mode = sb.st_mode;
        printf("zg.statMode().CP1,path=%s=,mode=%d\n",path,*mode);
        return 1;
    }
    return 0;
}



int main(int args, char * argv[]){
	int mode  = 0;
    struct stat64 sb;
    char * pccsid=getenv("QIBM_PASE_CCSID");
    printf("zg.stat64.pccsid=%X,*QIBM_PASE_CCSID=%s\n",pccsid,pccsid);
    char * plang=getenv("PASE_LANG");
    printf("zg.stat64.plang=%X,*PASE_LANG=%s\n",plang,plang);
    char * pl=getenv("LANG");
    printf("zg.stat64.pl=%X,*LANG=%s\n",pl,pl);
   
//#pragma convert(819)
    getchar();
    int rt = stat64("/QDLS", &sb);
//#pragma convert(0)
    if(rt == 0){
        printf("zg.QDLS.mode=%d\n",sb.st_mode);
    }else{
        printf("zg.QDLS.rt=%d\n",rt);
    }
    mode = 0;
	statMode(argv[1],&mode);
	//int fmt = mode & S_IFMT;
    printf("zg.mode=%X\n",mode);
	return(0);
}
