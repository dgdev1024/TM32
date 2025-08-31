;
; @file     examples/preprocessor/02_symbol_declaration/edge_cases.asm
; @brief    Test edge cases for symbol declaration
;

; Test empty variable declaration
.let empty_var

; Test constant without quotes
.const PI = 3.14159

; Test macro without parameters
.macro SIMPLE_MACRO
    nop
.endmacro

; Test define with complex string
.define COMPLEX_STRING "String with \"quotes\" and \\backslashes"

.text
main:
    ld a, empty_var
    ld b, PI
    SIMPLE_MACRO
    
done:
    jpb nc, done
