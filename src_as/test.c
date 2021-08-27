/* /usr/include/procinfo.h does this:
   1.  #define _ALL_SOURCE
   2.  #include other headers that include <sys/types.h>,
       which includes <sys/inttypes.h>,
       which defines int64 as type signed long in a 64-bit compilation
   3.  #undef _ALL_SOURCE */
#include <procinfo.h>

/* "as400_type.h" defines int64 as type signed long long
   (because _ALL_SOURCE is not defined) */
#include "as400_types.h"

int main(int argc, char *argv[])
{
    return 0;
}

