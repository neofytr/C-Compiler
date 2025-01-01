section .text
global _start
_start:
    push rbp
    mov rbp, rsp
    sub rsp, 4
    mov r11, 1
    cmp r11, 0
    je _false0
    mov r11, 2
    cmp r11, 0
    je _false0
    mov qword [rbp-4], 1
    jmp _end0
_false0:
    mov qword [rbp-4], 0
_end0:
    mov rax, [rbp-4]
    mov rsp, rbp
    pop rbp

    mov rdi, rax
    mov rax, 60
    syscall
