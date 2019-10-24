       #include <langinfo.h>
       #include <locale.h>
       #include <stdio.h>
       #include <stdlib.h>
       #include <locale.h>

       int
       main(int argc, char *argv[])
       {
           setlocale(LC_ALL,"");
           printf("%s\n", nl_langinfo(CODESET));
           return 0;
       }
