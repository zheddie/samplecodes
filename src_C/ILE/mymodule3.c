#include <stdio.h>
#include <math.h>
#include <recio.h>
#include <iconv.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
 
typedef struct {
    char        filler[5];
} aggregate;
 
// #pragma mapinc("datafile","SHUPE/PASEDATA(*all)","both",,,"")
// #include "datafile"
// #pragma argument(ileProcedure, nowiden) /* not necessary */
<<<<<<< HEAD
 
=======
  

int ileProcedure2(int         arg1,
                 char        *arg2,
                 long      arg3,
                 char        arg4[2],
                 aggregate   arg5,
                 short       arg6)
{
	printf("in ILE, before double\n");
	sleep(20);
	double x=4.0/2.0;
	printf("in ILE, after double\n");
	sleep(20);

	printf("zg.inILEProcedure,x=%f\n",x);
	return 0;
}

int dividzero(int s)
{
	printf("in ILE, before double\n");
	sleep(20);
	double x=4.0;
	x = x/0.0;
	printf("in ILE , ater double\n");
	sleep(20);
	printf("zg.inILEProcedure,x=%f\n",x);
	return 0;
}

>>>>>>> bde6ccb09f6a1ce4062c72e3ed06ec0e3230d790
/*
 * The arguments and function result for this ILE procedure
 * must be equivalent to the values presented to _ILECALL
 * function in the i5/OS program
 */
int ileProcedure(int         arg1,
                 char        *arg2,
                 double      arg3,
                 char        arg4[2],
                 aggregate   arg5,
                 short       arg6)
{
    char         fromcode[33];
    char         tocode[33];
    iconv_t      cd;     /* conversion descriptor */
    char         *src;
    char         *tgt;
    size_t       srcLen;
    size_t       tgtLen;
    int          result;
 
    /*
     * Open a conversion descriptor to convert CCSID 37
     * (EBCDIC) to CCSID 819 (ASCII), that is used for
     * any character data returned to the caller
     */
    memset(fromcode, 0, sizeof(fromcode));
    strcpy(fromcode, "IBMCCSID000370000000");
    memset(tocode, 0, sizeof(tocode));
    strcpy(tocode, "IBMCCSID00819");
    cd = iconv_open(tocode, fromcode);
    if (cd.return_value == -1)
    {
        printf("iconv_open failed\n");
        return -1;
    }
     /*
      * If arg1 equals one, return constant text (converted
      * to ASCII) in the buffer addressed by arg2. For any
      * other arg1 value, open a file and read some text,
      * then return that text (converted to ASCII) in the
      * buffer addressed by arg2
      */
    if (arg1 == 1)
    {
        src = "Sample 1 output text";
        srcLen = strlen(src) + 1;
        tgt = arg2; /* iconv output to arg2 buffer */
        tgtLen = srcLen;
        iconv(cd, &src, &srcLen, &tgt, &tgtLen);
 
        result = 1;
    }
    else
    {
        FILE *fp;
        fp = fopen("SHUPE/PASEDATA", "r");
        if (!fp) /* if file open error */
        {
             printf("fopen(\"SHUPE/PASEDATA\", \"r\") failed, "
                    "errno = %i\n", errno);
             result = 2;
        }
        else
        {
             char buf[25];
             char *string;
             errno = 0;
             string = fgets(buf, sizeof(buf), fp);
             if (!string)
             {
                  printf("fgets() EOF or error, errno = %i\n", errno);
                  buf[0] = 0; /* null-terminate empty buffer */
             }
             src = buf;
             srcLen = strlen(buf) + 1;
             tgt = arg2; /* iconv output to arg2 buffer */
             tgtLen = srcLen;
             iconv(cd, &src, &srcLen, &tgt, &tgtLen);
 
             fclose(fp);
        }
        result = 1;
    }
      /*
       * Close the conversion descriptor, and return the
       * result value determined above
       */
      iconv_close(cd);
      return result;
}