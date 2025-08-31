;
; @file     examples/preprocessor/02_symbol_declaration/test_redefinition.asm
; @brief    Test symbol redefinition error handling
;

; This should cause an error because MAX_VALUE is defined twice
.define MAX_VALUE "255"
.define MAX_VALUE "1024"

.text
main:
    ld a, MAX_VALUE
    
done:
    jpb nc, done
