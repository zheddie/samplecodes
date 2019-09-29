#include <stdio.h>
#include <stdlib.h>
int main(int argc, char * argv[]){
	if (argc <2){
		printf("Usage:\n\t%s <filenametoread>\n",argv[0]);
		exit(1);
	}
	
	FILE *f = fopen(argv[1],"r");
	unsigned char ch=0;
/*
	char rb[20];
	int l = fread(f,rb,20);
	printf("rb=%s\n",rb);
*/
	printf("-1:%x %c\n",-1,-1);
	printf("EOF:%x %c\n",EOF,EOF);
	/*while(chi!=EOF){*/
	ch = fgetc(f);
	while(!feof(f)){
		/*chi = (int)ch;
		*/
      		printf("%x (%c)\n", ch,ch);
		ch = fgetc(f);
	}
	fclose(f);
	return(0);
}
