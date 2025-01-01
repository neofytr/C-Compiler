section .text
global main
main:
    push rbp
    mov rbp, rsp
    sub rsp, 8
    mov r11, 16
    cmp r11, 0
    je _false0
    mov r11, 4
    cmp r11, 0
    je _false0
    mov qword [rbp-4], 1
    jmp _end0
_false0:
    mov qword [rbp-4], 0
_end0:
    cmp qword [rbp-4], 0
    jne _false1
    mov r11, 0
    cmp r11, 0
    jne _false1
    mov qword [rbp-8], 0
    jmp _end1
_false1:
    mov qword [rbp-8], 1
_end1:
    mov rax, [rbp-8]
    mov rsp, rbp
    pop rbp
    ret
