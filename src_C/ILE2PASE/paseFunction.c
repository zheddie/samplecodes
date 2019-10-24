/* i5/OS PASE function to be called from ILE
 *
 * Compile with something like:
 *  xlc -q64 -c -o paseFunction.o paseFunction.c
 *  ld -b64 -o shr64.o -bnoentry -bexpall -bM:SRE -lc paseFunction.o
 *  ar -X64 -r /home/joeuser/libpasefn.a shr64.o
 *
 * The ILE side of this example expects to find libpasefn.a in
 * /home/joeuser/libpasefn.a
 *
 * The compiler options -qalign=natural and -qldbl128 are
 * necessary only when interacting with i5/OS ILE programs
 * to force relative 16-byte alignment of type long double
 * (used inside type ILEpointer)
 */

#include <stdlib.h>
#include <stdio.h>

int paseFunction(void * inputPtr, void * outputPtr)
{
    /* An output string to return from i5/OS PASE to ILE *
     * this is the string "output" in EBCDIC             */
    const char outputValue[] = {0x96, 0xa4, 0xa3, 0x97, 0xa4, 0xa3, 0x00};


    printf("Entered paseFunction The input is >%s<\n",
           (char*)inputPtr);

    /* copy the output results to the outputPtr argument */
    memcpy(outputPtr, outputValue, sizeof(outputValue));


    return(52); /* return something more interesting than 0 */
}
char * zgetenv(void *iP,void *oP){
	printf("In pase,zgetenv(%s)\n",(char *)iP);
	char * sp = getenv("PATH");
	printf("In pase,zgetenv(PATH)=%s\n",sp);
	sp = getenv((char *)iP);
	printf("In pase,zgetenv(ZZ)beforeputenv=%s\n",sp);
	putenv("ZZ=HelloEnv");
	sp = getenv("ZZ");
	printf("In pase,zgetenv(ZZ)=%s\n",sp);
	sp = getenv("HOME");
	printf("In pase,zgetenv(HOME)=%s\n",sp);
	sp = getenv("QIBM_PASE_CCSID");
	printf("In pase,zgetenv(QIBM_PASE_CCSID)=%s\n",sp);
	sp = getenv(iP);
	printf("In pase,zgetenv(%s)=%s\n",(char *)iP,sp);
	strcpy(oP,sp);
	return(sp);
}

