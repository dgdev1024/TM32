; Main program demonstrating file inclusion feature
; This file includes common definitions and subroutines

.include "common_defs.asm"
.include "math_routines.asm"

main:
    ; Load initial values
    ld a, START_VALUE
    ld b, MULTIPLIER
    
    ; Call math subroutine from included file
    call nc, multiply_ab
    
    ; Store result at memory location
    st [RESULT_ADDRESS], a
    
done:
    jpb nc, done    ; Infinite loop
    
; Expected output:
; - All symbols from common_defs.asm should be available
; - All subroutines from math_routines.asm should be available
; - The .include directives should be replaced with the actual file contents
