; Test file for variable reassignment
; Tests .let directive with reassignment using = and compound operators

; Initial variable declarations
.let counter = 10
.let accumulator = 0
.let multiplier = 5
.let divider = 100

; Basic reassignment using =
.let counter = 25              ; Reassign counter to 25
.let accumulator = counter     ; Copy counter value to accumulator

; Arithmetic assignment operators
.let counter += 15             ; counter = 25 + 15 = 40
.let accumulator -= 5          ; accumulator = 25 - 5 = 20
.let multiplier *= 3           ; multiplier = 5 * 3 = 15
.let divider /= 4              ; divider = 100 / 4 = 25
.let counter %= 7              ; counter = 40 % 7 = 5

; Compound assignment with expressions
.let accumulator += counter * 2    ; accumulator = 20 + (5 * 2) = 30
.let multiplier -= (divider / 5)   ; multiplier = 15 - (25 / 5) = 10
.let divider *= (counter + 1)      ; divider = 25 * (5 + 1) = 150

; Bitwise assignment operators
.let flags = 0xFF
.let mask = 0x0F

.let flags &= 0xF0             ; flags = 0xFF & 0xF0 = 0xF0
.let flags |= 0x08             ; flags = 0xF0 | 0x08 = 0xF8
.let flags ^= 0x18             ; flags = 0xF8 ^ 0x18 = 0xE0
.let mask <<= 4                ; mask = 0x0F << 4 = 0xF0
.let flags >>= 2               ; flags = 0xE0 >> 2 = 0x38

; Exponentiation assignment
.let power = 2
.let power **= 3               ; power = 2 ** 3 = 8

; Complex compound assignments
.let result = 1000
.let result += (counter * multiplier) / divider  ; result = 1000 + (5 * 10) / 150 = 1000 + 0 = 1000
.let result *= (flags & 0x30) >> 4               ; result = 1000 * ((0x38 & 0x30) >> 4) = 1000 * (0x30 >> 4) = 1000 * 3 = 3000

; Chain of assignments
.let chain_a = 100
.let chain_b = chain_a
.let chain_c = chain_b
.let chain_a += 50             ; chain_a = 150, but chain_b and chain_c remain 100

; Using constants in assignments
.const BASE_VALUE = 1000
.let variable_value = BASE_VALUE
.let variable_value += BASE_VALUE / 2    ; variable_value = 1000 + 500 = 1500

.data
    final_values:
        .dw counter, accumulator, multiplier, divider
        .dw flags, mask, power, result
        .dw chain_a, chain_b, chain_c, variable_value

.text
    ld a, counter
    ld b, accumulator
    ld c, result
