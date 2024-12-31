section .text
global _start
_start:
    push rbp
    mov rbp, rsp
    sub rsp, 8
    mov qword [rbp-4], 3
    mov r11, [rbp-4]
    imul r11, 4
    mov qword [rbp-4], r11
    mov r10, [rbp-4]
    mov qword [rbp-8], r10
    add qword [rbp-8], 10
    mov rax, [rbp-8]
    mov rsp, rbp
    pop rbp
    
    mov rdi, rax
    mov rax, 60
    syscall
