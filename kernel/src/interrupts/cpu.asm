[bits 64]
global crash_me
crash_me:       
    mov eax, 0
    div eax
    ret

global enableInterrupts
enableInterrupts:
    sti
    ret

global disableInterrupts
disableInterrupts:
    cli
    ret