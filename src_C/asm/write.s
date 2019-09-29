.data
hello:
     .string "hello world\n"
.text
.globl main
main:
     movl     $4,%eax     
     movl     $1,%ebx
     movl     $hello,%ecx
     movl     $12,%edx
     int     $0x80

     movl     $1,%eax
     xorl     %ebx,%ebx
     int     $0x80
     ret

