; Test file specifically for arithmetic assignment operators
; Focuses on all compound assignment operators with various scenarios

; Initialize test variables
.let x = 100
.let y = 20
.let z = 5

; Addition assignment
.let x += 25          ; x = 100 + 25 = 125
.let y += x           ; y = 20 + 125 = 145
.let z += y * 2       ; z = 5 + (145 * 2) = 5 + 290 = 295

; Subtraction assignment
.let x -= 25          ; x = 125 - 25 = 100
.let y -= z           ; y = 145 - 295 = -150
.let z -= (x / 4)     ; z = 295 - (100 / 4) = 295 - 25 = 270

; Multiplication assignment
.let x *= 3           ; x = 100 * 3 = 300
.let y *= -1          ; y = -150 * -1 = 150
.let z *= (x / 100)   ; z = 270 * (300 / 100) = 270 * 3 = 810

; Division assignment
.let x /= 6           ; x = 300 / 6 = 50
.let y /= 3           ; y = 150 / 3 = 50
.let z /= (x + y)     ; z = 810 / (50 + 50) = 810 / 100 = 8

; Modulo assignment
.let x %= 7           ; x = 50 % 7 = 1
.let y %= 13          ; y = 50 % 13 = 11
.let z %= 3           ; z = 8 % 3 = 2

; Exponentiation assignment
.let base = 2
.let exp = 3
.let base **= exp     ; base = 2 ** 3 = 8
.let exp **= 2        ; exp = 3 ** 2 = 9

; Bitwise assignment operators
.let bits = 0xFF
.let mask = 0x0F

; Bitwise AND assignment
.let bits &= 0xF0     ; bits = 0xFF & 0xF0 = 0xF0
.let mask &= bits     ; mask = 0x0F & 0xF0 = 0x00

; Bitwise OR assignment
.let bits |= 0x08     ; bits = 0xF0 | 0x08 = 0xF8
.let mask |= 0x55     ; mask = 0x00 | 0x55 = 0x55

; Bitwise XOR assignment
.let bits ^= 0x18     ; bits = 0xF8 ^ 0x18 = 0xE0
.let mask ^= 0xAA     ; mask = 0x55 ^ 0xAA = 0xFF

; Left shift assignment
.let shift_val = 3
.let shift_val <<= 2  ; shift_val = 3 << 2 = 12

; Right shift assignment
.let shift_val >>= 1  ; shift_val = 12 >> 1 = 6

; Complex expressions with assignment operators
.let complex = 10
.let complex += (x * y) - (z ** 2)     ; complex = 10 + (1 * 11) - (2 ** 2) = 10 + 11 - 4 = 17
.let complex *= ((bits >> 5) + 1)      ; complex = 17 * ((0xE0 >> 5) + 1) = 17 * (7 + 1) = 17 * 8 = 136
.let complex /= (base - z)             ; complex = 136 / (8 - 2) = 136 / 6 = 22
.let complex %= (exp - base)           ; complex = 22 % (9 - 8) = 22 % 1 = 0

; Edge cases
.let edge_case = 1
.let edge_case += edge_case            ; edge_case = 1 + 1 = 2
.let edge_case *= edge_case            ; edge_case = 2 * 2 = 4
.let edge_case **= edge_case / 2       ; edge_case = 4 ** (4 / 2) = 4 ** 2 = 16

; Using constants in compound assignments
.const MULTIPLIER = 5
.const DIVISOR = 2
.let test_var = 100
.let test_var *= MULTIPLIER            ; test_var = 100 * 5 = 500
.let test_var /= DIVISOR               ; test_var = 500 / 2 = 250

.data
    arithmetic_results:
        .dw x, y, z
        .dw base, exp
        .dw bits, mask
        .dw shift_val, complex
        .dw edge_case, test_var

.text
    ; Use the calculated values
    ld a, x
    ld b, y
    add a, b
    ld c, complex
