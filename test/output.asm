section .text
global main
main:
    push rbp
    mov rbp, rsp
    sub rsp, 64
    mov dword [rbp-4], 0
    mov dword [rbp-8], 1
    mov r10d, [rbp-8]
    mov dword [rbp-12], r10d
    mov r11d, [rbp-12]
    imul r11d, [rbp-8]
    mov dword [rbp-12], r11d
    mov r10d, [rbp-4]
    mov dword [rbp-16], r10d
    mov r10d, [rbp-16]
    add r10d, [rbp-12]
    mov dword [rbp-16], r10d
    mov r10d, [rbp-16]
    mov dword [rbp-4], r10d
    mov r10d, [rbp-8]
    mov dword [rbp-20], r10d
    add dword [rbp-20], 1
    mov r10d, [rbp-20]
    mov dword [rbp-8], r10d
    mov r10d, [rbp-8]
    mov dword [rbp-24], r10d
    mov r11d, [rbp-24]
    imul r11d, [rbp-8]
    mov dword [rbp-24], r11d
    mov r10d, [rbp-4]
    mov dword [rbp-28], r10d
    mov r10d, [rbp-28]
    add r10d, [rbp-24]
    mov dword [rbp-28], r10d
    mov r10d, [rbp-28]
    mov dword [rbp-4], r10d
    mov r10d, [rbp-8]
    mov dword [rbp-32], r10d
    add dword [rbp-32], 1
    mov r10d, [rbp-32]
    mov dword [rbp-8], r10d
    mov r10d, [rbp-8]
    mov dword [rbp-36], r10d
    mov r11d, [rbp-36]
    imul r11d, [rbp-8]
    mov dword [rbp-36], r11d
    mov r10d, [rbp-4]
    mov dword [rbp-40], r10d
    mov r10d, [rbp-40]
    add r10d, [rbp-36]
    mov dword [rbp-40], r10d
    mov r10d, [rbp-40]
    mov dword [rbp-4], r10d
    mov r10d, [rbp-8]
    mov dword [rbp-44], r10d
    add dword [rbp-44], 1
    mov r10d, [rbp-44]
    mov dword [rbp-8], r10d
    mov r10d, [rbp-8]
    mov dword [rbp-48], r10d
    mov r11d, [rbp-48]
    imul r11d, [rbp-8]
    mov dword [rbp-48], r11d
    mov r10d, [rbp-4]
    mov dword [rbp-52], r10d
    mov r10d, [rbp-52]
    add r10d, [rbp-48]
    mov dword [rbp-52], r10d
    mov r10d, [rbp-52]
    mov dword [rbp-4], r10d
    mov r10d, [rbp-8]
    mov dword [rbp-56], r10d
    add dword [rbp-56], 1
    mov r10d, [rbp-56]
    mov dword [rbp-8], r10d
    mov r10d, [rbp-8]
    mov dword [rbp-60], r10d
    mov r11d, [rbp-60]
    imul r11d, [rbp-8]
    mov dword [rbp-60], r11d
    mov r10d, [rbp-4]
    mov dword [rbp-64], r10d
    mov r10d, [rbp-64]
    add r10d, [rbp-60]
    mov dword [rbp-64], r10d
    mov r10d, [rbp-64]
    mov dword [rbp-4], r10d
    mov eax, [rbp-4]
    mov rsp, rbp
    pop rbp
    ret
