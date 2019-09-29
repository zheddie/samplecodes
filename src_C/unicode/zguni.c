#include <stdio.h>
#include <string.h>
#define SS "\u0111,\u0144,\u0148,\u00F3,\u00F4,\u0151,\u00F6,\u00F7,\u0159,\u016F,\u00FA,\u0171,\u00FC"
#define TT "\xF0,\xF1,\xF2,\xF3,\xF4,\xF5,\xF6,\xF7,\xF8,\xF9,\xFA,\xFB,\xFC"

#define LEN 40
int main(){
	char s[LEN],t[LEN];
	int sl=0,tl=0;
	memset(s,0,LEN);
	memset(t,0,LEN);
	sl=strlen(SS);
	tl=strlen(TT);
	memcpy(s,SS,sl);
	memcpy(t,TT,tl);
	/*
	memcpy(s,"\u0111,\u0144,\u0148,\u00F3,\u00F4,\u0151,\u00F6,\u00F7,\u0159,\u016F,\u00FA,\u0171,\u00FC");
	memcpy(t,"\xF0,\xF1,\xF2,\xF3,\xF4,\xF5,\xF6,\xF7,\xF8,\xF9,\xFA,\xFB,\xFC");
	*/
	printf("s(%d)=%s\n",sl,s);
	printf("t(%d)=%s\n",tl,t);
	return(0);
}
