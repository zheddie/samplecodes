.data

HelloWorldString:
    .ascii "Hello World\n"

.text

.globl _start

_start:
    # Load all the arguments for write ()

    li   0, 4  # syscall number of 4 (write)
    li   3, 1  # filenumber 1 (stdout)
    lis  4, HelloWorldString@ha   # load upper 16 bits of addr
    addi 4, 4, HelloWorldString@l # add lower 16 bits of addr
    li   5, 12 # length of string
    sc

    # exit the program
    li 0,1
    li 3,0
    sc
