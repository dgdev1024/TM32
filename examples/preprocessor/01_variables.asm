;
; @file     examples/preprocessor/01_variables.asm
; @brief    Contains code which demonstrates the TM32 Assembler Tool's
;           Preprocessor Component's ability to create, store and manage
;           variables.
;

; Run this file with:
;   `tm32asm --preprocessor-only examples/preprocessor/01_variables.asm`

; Create a simple, un-initialized mutable variable.
; Un-initialized variables automatically initialize to zero.
.let var_a

; Create another variable, this time with an initialization.
.let var_b = 34

; Create an immutable constant.
.const CONST_A = 7

; Initialize another variable, then assign it another value.
.let var_c = 99
.let var_c = 110

; Initialize another variable with an expression, then use an arithmetic
; assignment operator to assign it another value.
.let var_d = 21 * 4
.let var_d /= (8 - 6)

.text
main:
    ld al, var_a
    ld ah, var_c
    add al, var_b
    sub al, CONST_A
    sub ah, var_d

done:
    jpb nc, done

;
; The preprocessor should resolve the above code to the following:
;
; ```
;   .text
;   main:
;       ld al, 0
;       ld ah, 110
;       add al, 34
;       sub al, 7
;       sub ah, 42
;
;   done:
;       jpb nc, done
; ```
;
; Notes:
;   - Variables and constants are visible only to the preprocessor and are
;       managed only during the preprocessor step of the assembly process.
;
