.macro PUSHALL 
    push %rax
    push %rbx
    push %rcx
    push %rdx
    push %rsi
    push %rdi
    push %rbp
    push %r8
    push %r9
    push %r10
    push %r11
    push %r12
    push %r13
    push %r14
    push %r15
.endm

.macro POPALL 
    pop %r15
    pop %r14
    pop %r13
    pop %r12
    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rbp
    pop %rdi
    pop %rsi
    pop %rdx
    pop %rcx
    pop %rbx
    pop %rax
.endm

# this is stolen from https://osdev.wiki/wiki/Cooperative_Multitasking
# rewritten just for it to NOT BLOODY WORK >:-(
.section .text
.global task_switch
task_switch:
    PUSHALL
    pushfq

    mov %cr3, %rax #Push CR3
    push %rax

    mov 88(%rsp), %rax #The first argument, where to save
    mov %rbx, 8(%rax)
    mov %rcx, 16(%rax)
    mov %rdx, 24(%rax)
    mov %rsi, 32(%rax)
    mov %rdi, 40(%rax)
    mov 72(%rsp), %rbx #RAX
    mov 80(%rsp), %rcx #RIP
    mov 20(%rsp), %rdx #RSP
    add $8, %rdx #Remove the return value ;)
    mov 32(%rsp), %rsi #RBP
    mov 8(%rsp), %rdi #RFLAGS
    mov %rbx, (%rax)
    mov %rdx, 24(%rax)
    mov %rsi, 56(%rax)
    mov %rcx, 64(%rax)
    mov %rdi, 72(%rax)
    pop %rbx #CR3
    mov %rbx, 80(%rax)
    push %rbx #Goodbye again ;)
    mov 96(%rsp), %rax #Now it is the new object
    mov 8(%rax), %rbx #RBX
    mov 16(%rax), %rcx #RCX
    mov 24(%rax), %rdx #RDX
    mov 32(%rax), %rsi #RSI
    mov 40(%rax), %rdi #RDI
    mov 56(%rax), %rbp #RBP
    push %rax
    mov 72(%rax), %rax #RFLAGS
    push %rax
    popfq
    pop %rax
    mov 48(%rax), %rsp #RSP
    push %rax
    mov 80(%rax), %rax #CR3
    mov %rax, %cr3
    pop %rax
    push %rax
    mov 64(%rax), %rax #RIP
    xchg (%rsp), %rax
    mov (%rax), %rax #EAX
    ret #This ends all!