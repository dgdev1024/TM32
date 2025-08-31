;
; @file     examples/preprocessor/02_symbol_declaration/macros.asm
; @brief    Test parametric macro declarations (.macro directive)
;

; Run this file with:
;   `tm32asm --preprocess-only --verbose examples/preprocessor/02_symbol_declaration/macros.asm`

; Parametric macro definitions
.macro LOAD_IMMEDIATE
    ; Parameters: @1 = register, @2 = value
    ld @1, @2
.endmacro

.macro SAVE_REGISTERS
    ; No parameters - save common registers
    push a
    push b
    push c
.endmacro

.macro RESTORE_REGISTERS
    ; No parameters - restore common registers
    pop c
    pop b
    pop a
.endmacro

.text
main:
    SAVE_REGISTERS
    LOAD_IMMEDIATE a, 255
    LOAD_IMMEDIATE b, 0
    RESTORE_REGISTERS
    
done:
    jpb nc, done

;
; The symbol declaration pass should record these symbols:
; - LOAD_IMMEDIATE (parametric macro): parameters and body will be processed in macro expansion pass
; - SAVE_REGISTERS (parametric macro): no parameters, body will be processed in macro expansion pass  
; - RESTORE_REGISTERS (parametric macro): no parameters, body will be processed in macro expansion pass
;
