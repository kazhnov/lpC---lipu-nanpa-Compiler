global _start
_start:
    mov rax, 5
    push rax
    pop rax
    mov rax, rax
    push rax
    mov rax, rsp
    add rax, 0
    mov rax, [rax]
    push rax
    pop rax
    mov rax, rax
    push rax
    mov rax, 6
    push rax
    pop rax
    mov rax, rax
    push rax
    mov rax, rsp
    add rax, 8
    mov rax, [rax]
    push rax
    pop rax
    mov rax, rax
    push rax
    mov rax, rsp
    add rax, 0
    mov rax, [rax]
    push rax
    pop rdi
    mov rax, 60
    syscall
    mov rax, 60
    mov rdi, 0
    syscall
