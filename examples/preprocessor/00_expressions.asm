;
; @file     examples/preprocessor/00_expressions.asm
; @brief    Contains code which demonstrates the TM32 Assembler Tool's
;           Preprocessor Component's ability to evaluate complex expressions.
;

; Run this file with:
;   `tm32asm --preprocess-only examples/preprocessor/00_expressions.asm`

.text
main:
    ; Basic arithmetic operations
    ld al, 5 + 2                        ; Simple addition: 7
    ld bh, 10 * (7 + 3)                 ; Parentheses and precedence: 100
    ld cw, 0x15 * (8 / 4)               ; Hex literal with division: 42
    ld d, 1000 / (125 * (20 - 16))      ; Nested parentheses: 2

    ; Operator precedence testing
    ld al, 2 + 3 * 4                    ; Multiplication first: 14
    ld bh, (2 + 3) * 4                  ; Parentheses override: 20
    ld cw, 15 - 3 * 2 + 1               ; Mixed operations: 10
    ld dl, 100 / 5 / 2                  ; Left-to-right division: 10

    ; Exponentiation (highest precedence except unary)
    ld aw, 2 ** 3 + 1                   ; Exponentiation first: 9
    ld bw, (2 + 1) ** 3                 ; Parentheses override: 27
    ld cw, 2 ** 3 ** 2                  ; Right-associative: 512 (2**(3**2))

    ; Bitwise operations
    ld al, 0xFF & 0x0F                  ; Bitwise AND: 15
    ld bh, 0xF0 | 0x0F                  ; Bitwise OR: 255
    ld cl, 0xFF ^ 0xAA                  ; Bitwise XOR: 85
    ld dh, ~0xFF                        ; Bitwise NOT: -256 (or 0xFFFFFF00)
    ld aw, 0x1 << 8                     ; Left shift: 256
    ld bw, 0x100 >> 2                   ; Right shift: 64

    ; Complex bitwise expressions
    ld cw, (0xF0 & 0xFF) | (0x0F << 4)  ; Combined bitwise: 255
    ld dw, ~(0xAAAA ^ 0x5555)           ; NOT of XOR: -1 (0xFFFFFFFF)

    ; Modulus operations
    ld al, 17 % 5                       ; Basic modulus: 2
    ld bh, 100 % (3 * 7)                ; Modulus with expression: 16
    ld cl, (50 + 7) % 8                 ; Parentheses in modulus: 1

    ; Mixed arithmetic and bitwise
    ld dh, (10 + 5) * 2 & 0x1F          ; Arithmetic then bitwise: 30
    ld aw, 0x100 | (50 - 18)            ; Bitwise with arithmetic: 288
    ld bw, (0xFF << 1) + 1               ; Shift then add: 511

    ; Comparison operations (result in 0 or 1)
    ld cw, 5 == 5                       ; Equality true: 1
    ld dw, 3 != 3                       ; Inequality false: 0
    ld al, 10 > 5                       ; Greater than true: 1
    ld bh, 2 < 1                        ; Less than false: 0
    ld cl, 5 >= 5                       ; Greater equal true: 1
    ld dh, 3 <= 2                       ; Less equal false: 0

    ; Logical operations
    ld aw, (5 > 3) && (2 < 4)           ; Logical AND true: 1
    ld bw, (1 == 2) || (3 == 3)         ; Logical OR true: 1
    ld cw, !(5 < 3)                     ; Logical NOT true: 1

    ; Complex logical expressions
    ld dw, (10 > 5) && (3 * 2 == 6) && (8 / 2 != 3)  ; All true: 1
    ld al, (1 == 2) || (3 == 4) || (5 == 5)           ; One true: 1

    ; Ternary operator (condition ? true_value : false_value)
    ld bh, (5 > 3) ? 100 : 200          ; Condition true: 100
    ld cl, (1 == 2) ? 50 : 75           ; Condition false: 75
    ld dh, (10 % 2 == 0) ? 1 : 0        ; Even number check: 1

    ; Nested ternary operators
    ld aw, (5 > 3) ? ((2 < 4) ? 10 : 20) : 30   ; Nested true: 10
    ld bw, (1 > 5) ? 40 : ((3 == 3) ? 50 : 60)  ; Nested false->true: 50

    ; Character and string literals
    ld cw, 'A'                          ; Character literal: 65
    ld dw, '\n'                         ; Escape sequence: 10
    ld al, '\x41'                       ; Hex escape: 65

    ; Different number bases
    ld bh, 0b11010011                   ; Binary: 211
    ld cl, 0o377                        ; Octal: 255
    ld dh, 0x1A2B                       ; Hexadecimal: 6699

    ; Large numbers and edge cases
    ld aw, 0xFFFF                       ; Max 16-bit: 65535
    ld bw, 0x10000 - 1                  ; Same as above: 65535
    ld cw, 1000000 / 1000               ; Large division: 1000

    ; Expression evaluation order testing
    ld dw, 2 + 3 * 4 ** 2               ; 2 + 3 * 16 = 50
    ld al, (2 + 3) * 4 ** 2             ; 5 * 16 = 80
    ld bh, 2 ** 3 + 4 * 5               ; 8 + 20 = 28
    ld cl, 2 ** (3 + 4) * 5             ; 128 * 5 = 640

    ; Bitwise precedence testing
    ld cl, 1 | 2 & 4                    ; (2 & 4) | 1 = 1
    ld dh, (1 | 2) & 4                  ; 3 & 4 = 0
    ld aw, 8 ^ 4 | 2                    ; (8 ^ 4) | 2 = 14
    ld bw, 8 ^ (4 | 2)                  ; 8 ^ 6 = 14

    ; Comparison and logical precedence
    ld cw, 1 < 2 && 3 < 4               ; (1 < 2) && (3 < 4) = 1
    ld dw, 1 && 2 == 2                  ; 1 && (2 == 2) = 1
    ld al, 0 || 1 && 0                  ; 0 || (1 && 0) = 0

    ; Negative numbers and unary operators
    ld bh, -5 + 10                      ; Unary minus: 5
    ld cl, ~(-1)                        ; NOT of negative: 0
    ld dh, !0                           ; Logical NOT of zero: 1
    ld aw, !(5 - 5)                     ; Logical NOT of expression: 1

    ; Division and modulus edge cases
    ld bw, 7 / 3                        ; Integer division: 2
    ld cw, 7 % 3                        ; Remainder: 1
    ld dw, -7 / 3                       ; Negative division: -2 or -3 (implementation defined)
    ld al, -7 % 3                       ; Negative modulus: -1 or 2 (implementation defined)

    ; Overflow scenarios (depending on target size)
    ld bh, 255 + 1                      ; May overflow to 0 for 8-bit
    ld cl, 0 - 1                        ; May underflow to 255 for 8-bit
    ld dh, 16 ** 4                      ; Large exponentiation: 65536

    ; Complex multi-level expressions
    ld aw, ((10 + 5) * 2 - 3) / (7 % 4) ; ((15 * 2 - 3) / 3) = 9
    ld bw, (2 ** 3) * (4 + 1) - (10 / 2) ; (8 * 5 - 5) = 35
    ld cw, ~((0xFF & 0xF0) >> 4) + 1    ; ~(15) + 1 = -14
    ld dw, (5 > 3) ? (10 + 2) : (20 - 5) ; 12

    ; Deeply nested parentheses
    ld al, (((2 + 3) * (4 + 1)) - ((6 / 2) + (8 % 3))) ; (25 - 5) = 20
    ld bh, ((10 > 5) && (3 < 7)) ? ((2 ** 4) + 1) : 0  ; 17
    ld cl, (((0xFF >> 4) & 0x0F) << 2) | 0x03          ; ((15 & 15) << 2) | 3 = 63

    ; Zero division edge cases (should generate errors)
    ; ld aw, 10 / 0                     ; Division by zero - should error
    ; ld bw, 10 % 0                     ; Modulus by zero - should error

done:
    jpb nc, done

;
; The preprocessor should resolve the above code to the following:
;
; ```
; .text
; main:
;     ; Basic arithmetic operations
;     ld al, 7                         ; Simple addition
;     ld bh, 100                       ; Parentheses and precedence
;     ld cw, 42                        ; Hex literal with division
;     ld d, 2                          ; Nested parentheses
;
;     ; Operator precedence testing
;     ld al, 14                        ; Multiplication first
;     ld bh, 20                        ; Parentheses override
;     ld cw, 10                        ; Mixed operations
;     ld dl, 10                        ; Left-to-right division
;
;     ; Exponentiation
;     ld aw, 9                         ; Exponentiation first
;     ld bw, 27                        ; Parentheses override
;     ld cw, 512                       ; Right-associative
;
;     ; Bitwise operations
;     ld al, 15                        ; Bitwise AND
;     ld bh, 255                       ; Bitwise OR
;     ld cl, 85                        ; Bitwise XOR
;     ld dh, -256                      ; Bitwise NOT (0xFFFFFF00)
;     ld aw, 256                       ; Left shift
;     ld bw, 64                        ; Right shift
;
;     ; Complex bitwise expressions
;     ld cw, 255                       ; Combined bitwise
;     ld dw, -1                        ; NOT of XOR (0xFFFFFFFF)
;
;     ; Modulus operations
;     ld al, 2                         ; Basic modulus
;     ld bh, 16                        ; Modulus with expression
;     ld cl, 1                         ; Parentheses in modulus
;
;     ; Mixed arithmetic and bitwise
;     ld dh, 30                        ; Arithmetic then bitwise
;     ld aw, 288                       ; Bitwise with arithmetic
;     ld bw, 511                       ; Shift then add
;
;     ; Comparison operations
;     ld cw, 1                         ; Equality true
;     ld dw, 0                         ; Inequality false
;     ld al, 1                         ; Greater than true
;     ld bh, 0                         ; Less than false
;     ld cl, 1                         ; Greater equal true
;     ld dh, 0                         ; Less equal false
;
;     ; Logical operations
;     ld aw, 1                         ; Logical AND true
;     ld bw, 1                         ; Logical OR true
;     ld cw, 1                         ; Logical NOT true
;
;     ; Complex logical expressions
;     ld dw, 1                         ; All conditions true
;     ld al, 1                         ; One condition true
;
;     ; Ternary operator
;     ld bh, 100                       ; Condition true
;     ld cl, 75                        ; Condition false
;     ld dh, 1                         ; Even number check
;
;     ; Nested ternary operators
;     ld aw, 10                        ; Nested true
;     ld bw, 50                        ; Nested false->true
;
;     ; Character and string literals
;     ld cw, 65                        ; Character literal 'A'
;     ld dw, 10                        ; Escape sequence '\n'
;     ld al, 65                        ; Hex escape '\x41'
;
;     ; Different number bases
;     ld bh, 211                       ; Binary 0b11010011
;     ld cl, 255                       ; Octal 0o377
;     ld dh, 6699                      ; Hexadecimal 0x1A2B
;
;     ; Large numbers and edge cases
;     ld aw, 65535                     ; Max 16-bit
;     ld bw, 65535                     ; Same calculation
;     ld cw, 1000                      ; Large division
;
;     ; Expression evaluation order testing
;     ld dw, 50                        ; 2 + 3 * 4 ** 2
;     ld al, 80                        ; (2 + 3) * 4 ** 2
;     ld bh, 28                        ; 2 ** 3 + 4 * 5
;     ld cl, 640                       ; 2 ** (3 + 4) * 5
;
;     ; Bitwise precedence testing
;     ld cl, 1                         ; 1 | 2 & 4
;     ld dh, 0                         ; (1 | 2) & 4
;     ld aw, 14                        ; 8 ^ 4 | 2
;     ld bw, 14                        ; 8 ^ (4 | 2)
;
;     ; Comparison and logical precedence
;     ld cw, 1                         ; 1 < 2 && 3 < 4
;     ld dw, 1                         ; 1 && 2 == 2
;     ld al, 0                         ; 0 || 1 && 0
;
;     ; Negative numbers and unary operators
;     ld bh, 5                         ; -5 + 10
;     ld cl, 0                         ; ~(-1)
;     ld dh, 1                         ; !0
;     ld aw, 1                         ; !(5 - 5)
;
;     ; Division and modulus edge cases
;     ld bw, 2                         ; 7 / 3
;     ld cw, 1                         ; 7 % 3
;     ld dw, -3                        ; -7 / 3 (implementation defined)
;     ld al, 2                         ; -7 % 3 (implementation defined)
;
;     ; Overflow scenarios
;     ld bh, 256                       ; 255 + 1 (may overflow for 8-bit targets)
;     ld cl, -1                        ; 0 - 1 (may underflow for 8-bit targets)
;     ld dh, 65536                     ; 16 ** 4
;
;     ; Complex multi-level expressions
;     ld aw, 9                         ; ((10 + 5) * 2 - 3) / (7 % 4)
;     ld bw, 35                        ; (2 ** 3) * (4 + 1) - (10 / 2)
;     ld cw, -14                       ; ~((0xFF & 0xF0) >> 4) + 1
;     ld dw, 12                        ; (5 > 3) ? (10 + 2) : (20 - 5)
;
;     ; Deeply nested parentheses
;     ld al, 20                        ; (((2 + 3) * (4 + 1)) - ((6 / 2) + (8 % 3)))
;     ld bh, 17                        ; ((10 > 5) && (3 < 7)) ? ((2 ** 4) + 1) : 0
;     ld cl, 63                        ; (((0xFF >> 4) & 0x0F) << 2) | 0x03
;
; done:
;     jpb nc, done
; ```
;
; Notes on edge cases and implementation details:
; - Negative division and modulus results are implementation-defined
; - Overflow behavior depends on the target data size (8-bit, 16-bit, 32-bit)
; - Bitwise NOT (~) on negative numbers produces two's complement results
; - Division by zero should generate assembler errors
; - Character literals are converted to their ASCII values
; - All expressions use only literal values, no variables or constants
; - Complex nested expressions test parser precedence and associativity rules
;