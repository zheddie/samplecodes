# exit2.s
# To compile it, type the following in shell
# as exit2.s -o exit2.o
# ld exit2.o -e _main -lc -o exit

.section __TEXT,__text
.globl _main
_main:
  pushq %rbp
  movq %rsp, %rbp
  movl $5, %edi        # exit(5);
  callq _exit
