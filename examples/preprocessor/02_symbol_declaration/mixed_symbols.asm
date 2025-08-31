;
; @file     examples/preprocessor/02_symbol_declaration/mixed_symbols.asm
; @brief    Test mixed symbol declarations (all types in one file)
;

; Run this file with:
;   `tm32asm --preprocess-only --verbose examples/preprocessor/02_symbol_declaration/mixed_symbols.asm`

; Simple text substitution macros
.define STACK_BASE "0xFFFE0000"
.def STACK_SIZE "8192"

; Variables (mutable)
.let current_address = 0x3000
.let loop_counter

; Constants (immutable)
.const MAX_MEMORY = 0xFFFFFFFF
.const PROGRAM_NAME = "Test Program"

; Parametric macro
.macro INITIALIZE_STACK
    ; Set up stack pointer
    ld sp, STACK_BASE + STACK_SIZE
.endmacro

.text
main:
    INITIALIZE_STACK
    ld a, current_address
    ld b, loop_counter
    
done:
    jpb nc, done

;
; The symbol declaration pass should record these symbols:
; - STACK_BASE (simple macro): "0xFFFE0000"
; - STACK_SIZE (simple macro): "8192"
; - current_address (variable): will be evaluated to 0x3000
; - loop_counter (variable): will be initialized to 0 (default)
; - MAX_MEMORY (constant): will be evaluated to 0xFFFFFFFF
; - PROGRAM_NAME (constant): will be evaluated to "Test Program"
; - INITIALIZE_STACK (parametric macro): will be processed in macro expansion pass
;
