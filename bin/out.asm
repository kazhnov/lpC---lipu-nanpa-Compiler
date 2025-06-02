global _start
_start:
    mov r8, 0
    push r8
    pop r8
    push r8
    mov r8, 1
    push r8
    pop r8
    push r8
    mov r8, 0
    push r8
    pop r8
    push r8
    mov r8, 9
    push r8
    pop r8
    push r8
.loopin0:
    mov r8, rsp
    add r8, 24
    mov r8, [r8]
    push r8
    mov r8, 100
    push r8
    pop r9
    pop r8
    push 1
    cmp r8, r9
    jl $+6
    pop r8
    push 0
    pop rcx
    cmp rcx, 0
    je .loopout0
    mov r8, rsp
    add r8, 0
    mov r8, [r8]
    push r8
    mov r8, 1
    push r8
    pop r9
    pop r8
    add r8, r9
    push r8
    pop r8
    mov r9, rsp
    add r9, 0
    mov [r9], r8
    push r8
    pop r8
    mov r8, rsp
    add r8, 24
    mov r8, [r8]
    push r8
    pop r8
    mov r9, rsp
    add r9, 8
    mov [r9], r8
    push r8
    pop r8
    mov r8, rsp
    add r8, 24
    mov r8, [r8]
    push r8
    mov r8, rsp
    add r8, 24
    mov r8, [r8]
    push r8
    pop r9
    pop r8
    add r8, r9
    push r8
    pop r8
    mov r9, rsp
    add r9, 24
    mov [r9], r8
    push r8
    pop r8
