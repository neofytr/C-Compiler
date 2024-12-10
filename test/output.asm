section .text
global main
main:
    push rbp
    mov rbp, rsp
    sub rsp, 4
    mov rax, 9
    cqo
    mov r10, 10
    idiv r10
    mov qword [rbp-4], rax
    mov rax, [rbp-4]
    mov rsp, rbp
    pop rbp
    ret
