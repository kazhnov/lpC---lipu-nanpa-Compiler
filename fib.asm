    global _start:
_start:
    mov rax, 10
    
    mov r8, 0
    mov r9, 1
    mov rcx, 0
loop:
    mov r10, r8
    add r10, r9
    mov r9, r8
    mov r8, r10
    inc rcx     
    cmp rcx, rax
    jl loop
    
    mov rax, 60
    mov rdi, r8
    syscall
    
