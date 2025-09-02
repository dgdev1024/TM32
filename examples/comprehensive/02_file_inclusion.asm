; File Inclusion Test
; Tests: .include directive with nested includes

.text

; Include a common definitions file
.include "common_defs.inc"

; Include a file with utility macros
.include "utils.inc"

main:
    ; Use constants from included files
    LD A, SYSTEM_CLOCK
    LD B, MEMORY_BASE
    LD C, INTERRUPT_MASK
    
    ; Use macro from included file
    INIT_REGISTERS
    
    ; Use utility function
    CALCULATE_CHECKSUM
    
    HALT
