section .text
global main
main:
    push rbp
    mov rbp, rsp
    sub rsp, 8
    mov qword [rbp-4], 7
    neg qword [rbp-4]
    mov qword [rbp-8], 12
    mov r11, [rbp-8]
    imul r11, [rbp-4]
    mov qword [rbp-8], r11
    mov rax, [rbp-8]
    mov rsp, rbp
    pop rbp
    ret
