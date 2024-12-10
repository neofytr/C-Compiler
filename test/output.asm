section .text
global main
main:
    push rbp
    mov rbp, rsp
    sub rsp, 4
    mov qword [rbp-4], 9
    mov rcx, 10
    sal qword [rbp-4], cl
    mov rax, [rbp-4]
    mov rsp, rbp
    pop rbp
    ret
