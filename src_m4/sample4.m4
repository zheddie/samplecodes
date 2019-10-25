divert(-1)
define(DEF_PAIR_OF,
`typedef struct pair_of_$1 {
        $1 first;
        $1 second;
} pair_of_$1')
divert(0)dnl

DEF_PAIR_OF(int);
DEF_PAIR_OF(double);
DEF_PAIR_OF(MyStruct);
