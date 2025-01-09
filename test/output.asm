section .text
global _start
_start:
    push rbp
    mov rbp, rsp
    sub rsp, 12
    mov dword [rbp-4], 10
    mov r10d, [rbp-4]
    mov dword [rbp-8], r10d
    add dword [rbp-8], 5
    mov r10d, [rbp-8]
    mov dword [rbp-12], r10d
    mov eax, [rbp-12]
    mov rsp, rbp
    pop rbp
    
    mov edi, eax
    mov eax, 60
    syscall
