# from: https://smist08.wordpress.com/2019/09/07/risc-v-assembly-language-hello-world/
# a0-a2 - parameters to linux function services
# a7 - linux function number
#

.global _start      # Provide program starting address to linker

_start:
    addi x10, x0, 10
    addi x11, x0, 1

    loop_head:
    beq x10, x0, loop_end

    sub x10, x10, x11

    j loop_head
    loop_end:
# Setup the parameters to exit the program
# and then call Linux to do it.

    addi    a0, x0, 0   # Use 0 return code
    addi    a7, x0, 93  # Service command code 93 terminates
    ecall               # Call linux to terminate the program
