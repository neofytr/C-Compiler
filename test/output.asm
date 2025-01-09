section .text
global main
main:
    push rbp
    mov rbp, rsp
    sub rsp, 8
    mov qword [rbp-4], 10
    mov r11, [rbp-4]
    imul r11, 6
    mov qword [rbp-4], r11
    mov r10, [rbp-4]
    mov qword [rbp-8], r10
    mov rax, 10
    mov rsp, rbp
    pop rbp
    ret
