section .text
global main
main:
    push rbp
    mov rbp, rsp
    sub rsp, 4
    mov qword [rbp-4], 12
    mov r11, [rbp-4]
    imul r11, 7
    mov qword [rbp-4], r11
    mov rax, [rbp-4]
    mov rsp, rbp
    pop rbp
    ret
