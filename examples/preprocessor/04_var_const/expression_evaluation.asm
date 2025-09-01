; Test file for complex expressions
; Tests all operators and precedence rules

; Arithmetic operators
.const ADD_RESULT = 10 + 5
.const SUB_RESULT = 20 - 8
.const MUL_RESULT = 6 * 7
.const DIV_RESULT = 100 / 4
.const MOD_RESULT = 17 % 5
.const EXP_RESULT = 2 ** 3

; Bitwise operators
.const AND_RESULT = 0xF0 & 0x0F
.const OR_RESULT = 0xF0 | 0x0F
.const XOR_RESULT = 0xAA ^ 0x55
.const NOT_RESULT = ~0xFF00
.const LSHIFT_RESULT = 1 << 8
.const RSHIFT_RESULT = 0x8000 >> 4

; Comparison operators (for future use with .if)
.const IS_EQUAL = 5 == 5
.const NOT_EQUAL = 5 != 3
.const LESS_THAN = 3 < 5
.const GREATER_THAN = 8 > 5
.const LESS_EQUAL = 5 <= 5
.const GREATER_EQUAL = 10 >= 8

; Logical operators (for future use with .if)
.const LOGICAL_AND = 1 && 1
.const LOGICAL_OR = 0 || 1
.const LOGICAL_NOT = !0

; Operator precedence tests
.const PRECEDENCE_1 = 2 + 3 * 4          ; Should be 14 (not 20)
.const PRECEDENCE_2 = (2 + 3) * 4        ; Should be 20
.const PRECEDENCE_3 = 16 / 2 / 2         ; Should be 4 (left-to-right)
.const PRECEDENCE_4 = 2 ** 3 ** 2        ; Should be 512 (right-to-left)
.const PRECEDENCE_5 = 1 + 2 << 2         ; Should be 12 (1+2=3, 3<<2=12)
.const PRECEDENCE_6 = 8 >> 1 + 1         ; Should be 2 (1+1=2, 8>>2=2)

; Complex nested expressions
.const COMPLEX_1 = ((10 + 5) * 2 - 8) / 3
.const COMPLEX_2 = (0xFF & 0xF0) | (0x0A << 1)
.const COMPLEX_3 = ~((1 << 4) - 1) & 0xFFFF

; Mixed arithmetic and bitwise
.const MIXED_1 = (100 + 50) & 0xFF
.const MIXED_2 = (1 << (3 + 2)) | 7
.const MIXED_3 = ((16 * 4) >> 2) + 1

.data
    arithmetic_table:
        .dw ADD_RESULT, SUB_RESULT, MUL_RESULT, DIV_RESULT
        .dw MOD_RESULT, EXP_RESULT
    
    bitwise_table:
        .dw AND_RESULT, OR_RESULT, XOR_RESULT, NOT_RESULT
        .dw LSHIFT_RESULT, RSHIFT_RESULT
    
    precedence_table:
        .dw PRECEDENCE_1, PRECEDENCE_2, PRECEDENCE_3
        .dw PRECEDENCE_4, PRECEDENCE_5, PRECEDENCE_6
    
    complex_table:
        .dw COMPLEX_1, COMPLEX_2, COMPLEX_3
        .dw MIXED_1, MIXED_2, MIXED_3

.text
    ld a, PRECEDENCE_1
    ld b, COMPLEX_1
    ld c, MIXED_1
