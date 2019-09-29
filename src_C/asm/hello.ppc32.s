.file "hello.ppc32.s"
.csect __start[PR]
.globl __start[PR]
    # Load all the arguments for write ()

    li   0, 4  # syscall number of 4 (write)
    li   3, 1  # filenumber 1 (stdout)
    lis  4, HelloWorldString@h   # load upper 16 bits of addr
    addi 4, 4, HelloWorldString@l # add lower 16 bits of addr
    li   5, 12 # length of string
    sc

    # exit the program
    li 0,1
    li 3,0
    sc
.toc
.csect HelloWorldString[TD]
	.byte 0x68,0x65,0x6c,0x6c
	.byte  0x6f,0x2c,0x77,0x6f
	.byte  0x72,0x6c,0x64,0xa,0x0

