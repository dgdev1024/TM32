; Advanced functions demonstrating nested file inclusion
; This file itself includes another file, showing recursive inclusion

.include "../math_routines.asm"  ; Include from parent directory

; Advanced constants
ADVANCED_CONSTANT   equ 0x42      ; Special constant
BUFFER_SIZE         equ 0x100     ; Buffer size for operations

; Advanced function that uses both local and included functionality  
advanced_function:
    push b              ; Save B register
    push c              ; Save C register
    
    ; Use the multiply function from math_routines.asm
    ld b, ADVANCED_CONSTANT
    call nc, multiply_ab
    
    ; Add buffer size offset
    add a, BUFFER_SIZE
    
    pop c               ; Restore C register  
    pop b               ; Restore B register
    ret nc              ; Return with result in A

; Buffer manipulation function
clear_buffer:
    push a              ; Save registers
    push b
    push c
    
    ld a, WORK_AREA_START    ; Start address
    ld b, 0x00               ; Clear value
    ld c, BUFFER_SIZE        ; Counter
    
clear_loop:
    st [a], bl               ; Store clear value
    inc a                    ; Next address
    dec c                    ; Decrement counter
    jpb zc, clear_loop       ; Continue if not zero
    
    pop c               ; Restore registers
    pop b
    pop a
    ret nc
