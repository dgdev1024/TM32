;
; @file     examples/preprocessor/02_symbol_declaration/simple_defines.asm
; @brief    Test simple text substitution macros (.define/.def directive)
;

; Run this file with:
;   `tm32asm --preprocess-only --verbose examples/preprocessor/02_symbol_declaration/simple_defines.asm`

; Simple text substitution macros
.define MAX_VALUE "255"
.def MIN_VALUE "0"
.define GREETING "Hello, World!"

.text
main:
    ; These should be expanded in the macro expansion pass
    ld a, MAX_VALUE
    ld b, MIN_VALUE
    
done:
    jpb nc, done

;
; The symbol declaration pass should record these symbols:
; - MAX_VALUE (simple macro): "255"
; - MIN_VALUE (simple macro): "0" 
; - GREETING (simple macro): "Hello, World!"
;
