; Edge Cases and Stress Test
; Tests: boundary conditions, extreme nesting, complex expressions

.text

; Test extremely nested conditionals (pushing limits)
.let DEPTH_TEST = 5

.if DEPTH_TEST > 0
    .if DEPTH_TEST > 1
        .if DEPTH_TEST > 2
            .if DEPTH_TEST > 3
                .if DEPTH_TEST > 4
                    .if DEPTH_TEST > 5
                        LD A, 0xDEEP0006
                    .else
                        LD A, 0xDEEP0005
                    .endif
                .else
                    LD A, 0xDEEP0004
                .endif
            .else
                LD A, 0xDEEP0003
            .endif
        .else
            LD A, 0xDEEP0002
        .endif
    .else
        LD A, 0xDEEP0001
    .endif
.else
    LD A, 0xDEEP0000
.endif

; Test complex arithmetic in preprocessor expressions
.let BASE = 100
.let MULTIPLIER = 3
.let OFFSET = 25
.let COMPLEX_CALC = ((BASE * MULTIPLIER) + OFFSET) / 2  ; Should be 162
.let BITWISE_TEST = (0xFF00 & 0xF0F0) | (0x0F0F & 0x5555)  ; Should be 0xF005

LD B, COMPLEX_CALC
LD C, BITWISE_TEST

; Test macro with extreme parameter count
.macro MANY_PARAMS p1, p2, p3, p4, p5, p6, p7, p8
    LD A, @1
    LD B, @2
    ADD A, @3
    SUB B, @4
    .shift 4
    AND A, @1  ; Now @5
    OR B, @2   ; Now @6
    XOR A, @3  ; Now @7
    NOT @4     ; Now @8 (but NOT doesn't work on immediates in real assembly)
.endmacro

; Test boundary values
.const MAX_INT32 = 0x7FFFFFFF
.const MIN_INT32 = 0x80000000
.const ZERO = 0x00000000
.const ALL_ONES = 0xFFFFFFFF

MANY_PARAMS 1, 2, 3, 4, 5, 6, 7, 8

; Test very long repeat loop (stress test)
.let STRESS_COUNTER = 0
.repeat 100, big_iter
    .if (big_iter % 10) == 0
        LD AL, big_iter
        .let STRESS_COUNTER = STRESS_COUNTER + 1
    .endif
.endrepeat

; Test deeply nested macro calls
.macro LEVEL1 val
    .macro LEVEL2 val2
        .macro LEVEL3 val3
            LD D, (@val + @val2 + @val3)
        .endmacro
        LEVEL3 @val2
    .endmacro
    LEVEL2 @val
.endmacro

LEVEL1 42

; Test edge case: empty macro
.macro EMPTY_MACRO
.endmacro

EMPTY_MACRO

; Test edge case: macro with only comments
.macro COMMENT_ONLY
    ; This macro contains only comments
    ; It should expand to nothing
.endmacro

COMMENT_ONLY

; Test variable redefinition edge cases
.let REDEFINED_VAR = 1
.let REDEFINED_VAR = 2
.let REDEFINED_VAR = REDEFINED_VAR + 3  ; Should be 5

LD AH, REDEFINED_VAR

; Test expression with multiple operators and precedence
.let PRECEDENCE_TEST = 2 + 3 * 4 - 1  ; Should be 13 (3*4=12, 2+12=14, 14-1=13)
.let PAREN_TEST = (2 + 3) * (4 - 1)   ; Should be 15 (5 * 3)

LD BH, PRECEDENCE_TEST
LD BL, PAREN_TEST

; Test for loop with zero iterations (edge case)
.for zero_loop = 5 to 3
    LD CH, zero_loop  ; This should never execute
.endfor

; Test for loop with single iteration
.for single_loop = 7 to 7
    LD CL, single_loop  ; Should execute once with value 7
.endfor

; Test while loop that never executes
.let FALSE_CONDITION = 0
.while FALSE_CONDITION
    LD DH, 0xFF  ; This should never execute
.endwhile

; Test complex conditional with many operators
.let A_TEST = 10
.let B_TEST = 20
.let C_TEST = 15

.if ((A_TEST < B_TEST) && (C_TEST > A_TEST)) || (B_TEST == 20)
    LD DL, 0xAA  ; This should execute
.else
    LD DL, 0xBB
.endif

HALT
