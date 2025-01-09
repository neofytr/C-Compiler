section .text
global _start
_start:
    push rbp
    mov rbp, rsp
    sub rsp, 16
    mov dword [rbp-4], 0
    mov dword [rbp-8], 9
    mov r10d, [rbp-8]
    mov dword [rbp-12], r10d
    mov r11d, [rbp-12]
    imul r11d, 2
    mov dword [rbp-12], r11d
    mov r10d, [rbp-4]
    mov dword [rbp-16], r10d
    mov r10d, [rbp-16]
    add r10d, [rbp-12]
    mov dword [rbp-16], r10d
    mov r10d, [rbp-16]
    mov dword [rbp-4], r10d
    mov eax, [rbp-4]
    mov rsp, rbp
    pop rbp
    
    mov edi, eax
    mov eax, 60
    syscall
