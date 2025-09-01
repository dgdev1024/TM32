; Test file for edge cases and error conditions
; Tests various edge cases in variable and constant evaluation

; Constants with different numeric formats
.const HEX_CONST = 0xFF00
.const BIN_CONST = 0b11110000
.const OCT_CONST = 0o777
.const DEC_CONST = 255

; Variables with edge values
.let zero_var = 0
.let max_uint32 = 0xFFFFFFFF
.let negative_var = -1
.let large_positive = 2147483647

; Division edge cases
.const DIV_BY_ONE = 100 / 1
.const MOD_BY_ONE = 100 % 1
.const ZERO_DIV_ZERO = 0 / 1  ; Valid: 0/1 = 0
.const ZERO_MOD_NONZERO = 0 % 5  ; Valid: 0%5 = 0

; Bit shift edge cases
.const SHIFT_ZERO = 5 << 0
.const SHIFT_ONE = 5 << 1
.const SHIFT_LARGE = 1 << 31
.const RSHIFT_TO_ZERO = 1 >> 31

; Operator precedence edge cases
.const UNARY_PRECEDENCE = -2 ** 3        ; Should be -(2**3) = -8
.const MIXED_UNARY = ~-1                 ; Should be ~(-1) = ~0xFFFFFFFF = 0
.const CHAINED_UNARY = !!1               ; Should be !(!1) = !(0) = 1

; Parentheses nesting
.const NESTED_1 = ((((5))))
.const NESTED_2 = (((1 + 2) * 3) + 4)
.const NESTED_3 = ((1 << 2) | (2 << 1)) & 0xFF

; Maximum expression complexity
.const ULTRA_COMPLEX = ((((1 + 2) * (3 + 4)) << 1) | ((5 & 6) ^ 7)) + (((8 >> 1) + 9) * 10)

; Self-referential attempts (should cause errors)
; .let bad_self = bad_self + 1          ; Error: circular reference
; .const BAD_CIRCULAR_A = BAD_CIRCULAR_B
; .const BAD_CIRCULAR_B = BAD_CIRCULAR_A ; Error: circular reference

; Variables with zero initialization
.let implicit_zero           ; Should default to 0
.let explicit_zero = 0

; Constants that should evaluate to same value via different paths
.const PATH_A = 2 * 3 * 4
.const PATH_B = 6 * 4
.const PATH_C = 24
.const VERIFICATION_EQUAL = (PATH_A == PATH_B) && (PATH_B == PATH_C)

.data
    edge_case_data:
        .dw HEX_CONST, BIN_CONST, OCT_CONST, DEC_CONST
        .dw DIV_BY_ONE, MOD_BY_ONE, ZERO_DIV_ZERO, ZERO_MOD_NONZERO
        .dw SHIFT_ZERO, SHIFT_ONE, SHIFT_LARGE, RSHIFT_TO_ZERO
        .dw UNARY_PRECEDENCE, MIXED_UNARY, CHAINED_UNARY
        .dw NESTED_1, NESTED_2, NESTED_3
        .dd ULTRA_COMPLEX
        .dw PATH_A, PATH_B, PATH_C

.text
    ld a, ULTRA_COMPLEX
    ld b, implicit_zero
    ld c, VERIFICATION_EQUAL
