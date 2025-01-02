section .text
global main
main:
    push rbp
    mov rbp, rsp
    sub rsp, 8
    mov r11, 10
    cmp r11, 3
    mov qword [rbp-4], 0
    setge byte [rbp-4]
    cmp qword [rbp-4], 0
    je .L_false0
    mov r11, 5
    cmp r11, 0
    je .L_false0
    mov qword [rbp-8], 1
    jmp .L_end0
.L_false0:
    mov qword [rbp-8], 0
.L_end0:
    mov rax, [rbp-8]
    mov rsp, rbp
    pop rbp
    ret
