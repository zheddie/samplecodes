/* PASE for i function to be called from ILE
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
 * necessary only when interacting with IBM i ILE programs
 * to force relative 16-byte alignment of type long double
 * (used inside type ILEpointer)
 */

#include <stdlib.h>
#include <stdio.h>
int paseFunction(void * inputPtr, void * outputPtr)
{
    /* An output string to return from PASE for i to ILE *
     * this is the string "output" in EBCDIC             */
    const char outputValue[] = {0x96, 0xa4, 0xa3, 0x97, 0xa4, 0xa3, 0x00};


    printf("Entered paseFunction The input is >%s<\n",
           (char*)inputPtr);

    /* copy the output results to the outputPtr argument */
    memcpy(outputPtr, outputValue, sizeof(outputValue));


    return(52); /* return something more interesting than 0 */
}