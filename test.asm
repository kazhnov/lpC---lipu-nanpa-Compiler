    global _start

_start:
    ;; mystr db "asas"

    mov r8, 25
    push r8
    mov rdi, 3
    mov rax, 60
    syscall
    
