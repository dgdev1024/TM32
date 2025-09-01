; Test edge cases and various conditions for conditional assembly
; This file tests various edge cases that should be handled properly

.const ZERO = 0
.const ONE = 1
.const NEGATIVE = -5

; Test 1: Zero and non-zero evaluations
.if ZERO
    ; Should NOT be included (0 is false)
    error_should_not_happen
.endif

.if ONE
    ; Should be included (non-zero is true)
    ld a, ONE
.endif

; Test 2: Negative numbers as conditions
.if NEGATIVE
    ; Should be included (non-zero is true, even if negative)
    ld b, NEGATIVE
.endif

; Test 3: Arithmetic expressions (basic evaluation)
.let zero_result = 0   ; Simulate (5 - 5)
.let nonzero_result = 6 ; Simulate (3 * 2)

.if zero_result
    ; Should NOT be included (0 is false)
    error_zero_arithmetic
.endif

.if nonzero_result
    ; Should be included (6 is true)
    ld c, 6
.endif

; Test 4: Empty else blocks
.if ZERO
    ; Should NOT be included
    nop
.else
    ; Should be included - empty else is valid
.endif

.if ONE
    ; Should be included
    ld d, 42
.else
    ; Should NOT be included - empty else is valid
.endif

; Test 5: Basic comparisons using simulation
.let equal_test = 1      ; Simulate ONE == ONE (true)
.let not_equal_test = 1  ; Simulate ONE != ZERO (true)
.let less_test = 0       ; Simulate ONE < ZERO (false)
.let greater_equal_test = 1  ; Simulate NEGATIVE >= -10 (true)

.if equal_test
    ; Should be included (1 == 1 is true)
    ld e, 0x11
.endif

.if not_equal_test
    ; Should be included (1 != 0 is true) 
    ld f, 0x22
.endif

.if less_test
    ; Should NOT be included (1 < 0 is false)
    error_comparison
.endif

.if greater_equal_test
    ; Should be included (-5 >= -10 is true)
    ld g, 0x33
.endif

; Test 6: Logical operations simulation
.let not_zero = 1     ; Simulate !ZERO (true)
.let not_one = 0      ; Simulate !ONE (false)
.let and_test = 0     ; Simulate ONE && ZERO (false)
.let or_test = 1      ; Simulate ONE || ZERO (true)

.if not_zero
    ; Should be included (!0 is true)
    ld h, 0x44
.endif

.if not_one
    ; Should NOT be included (!1 is false)
    error_logical_not
.endif

.if and_test
    ; Should NOT be included (1 && 0 is false)
    error_logical_and
.endif

.if or_test
    ; Should be included (1 || 0 is true)
    ld i, 0x55
.endif

halt
