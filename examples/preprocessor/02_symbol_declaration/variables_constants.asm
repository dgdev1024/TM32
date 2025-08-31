;
; @file     examples/preprocessor/02_symbol_declaration/variables_constants.asm
; @brief    Test variable and constant declarations (.let and .const directives)
;

; Run this file with:
;   `tm32asm --preprocess-only --verbose examples/preprocessor/02_symbol_declaration/variables_constants.asm`

; Variable declarations (mutable)
.let counter = 0
.let buffer_size = 256
.let debug_enabled

; Constant declarations (immutable)
.const PI = 3.14159
.const MAX_ITERATIONS = 1000
.const VERSION = "1.0.0"

.text
main:
    ; These variables and constants will be evaluated in the variable evaluation pass
    ld a, counter
    ld b, buffer_size
    
done:
    jpb nc, done

;
; The symbol declaration pass should record these symbols:
; - counter (variable): will be evaluated to 0
; - buffer_size (variable): will be evaluated to 256
; - debug_enabled (variable): will be initialized to 0 (default)
; - PI (constant): will be evaluated to 3.14159
; - MAX_ITERATIONS (constant): will be evaluated to 1000
; - VERSION (constant): will be evaluated to "1.0.0"
;
