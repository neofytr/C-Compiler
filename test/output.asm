section .text
global main
main:
    push rbp
    mov rbp, rsp
    sub rsp, 20
    mov qword [rbp-4], 10
    mov r11, [rbp-4]
    imul r11, 6
    mov qword [rbp-4], r11
    mov r10, [rbp-4]
    mov qword [rbp-8], r10
    mov qword [rbp-12], 10
    mov qword [rbp-16], 10
    mov qword [rbp-20], 8
    mov rax, 10
    mov rsp, rbp
    pop rbp
    ret
