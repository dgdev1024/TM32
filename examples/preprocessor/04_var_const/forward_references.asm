; Test file for forward references
; Tests variables and constants referencing symbols defined later

; Forward reference to constants defined later
.let early_var = LATE_CONSTANT + 10
.const early_const = LATE_CONSTANT * 2

; Variables that reference each other
.let var_a = var_b + 5
.let var_b = var_c * 2
.let var_c = 100

; Constants with forward references
.const BUFFER_END = BUFFER_START + BUFFER_LENGTH
.const MIDPOINT = (BUFFER_START + BUFFER_END) / 2

; Later definitions
.const LATE_CONSTANT = 42
.const BUFFER_START = 0x2000
.const BUFFER_LENGTH = 512

; Verify the forward references worked
.const VERIFICATION_1 = early_var      ; Should be 52 (42 + 10)
.const VERIFICATION_2 = early_const    ; Should be 84 (42 * 2)
.const VERIFICATION_3 = var_a          ; Should be 205 (200 + 5)
.const VERIFICATION_4 = BUFFER_END     ; Should be 0x2200 (0x2000 + 512)
.const VERIFICATION_5 = MIDPOINT       ; Should be 0x2100

; Complex forward reference chain
.const CHAIN_A = CHAIN_B + 1
.const CHAIN_B = CHAIN_C + 2
.const CHAIN_C = CHAIN_D + 3
.const CHAIN_D = 10

; Should resolve to: CHAIN_D=10, CHAIN_C=13, CHAIN_B=15, CHAIN_A=16

.data
    verification_data:
        .dw VERIFICATION_1, VERIFICATION_2, VERIFICATION_3
        .dw VERIFICATION_4, VERIFICATION_5
        .dw CHAIN_A, CHAIN_B, CHAIN_C, CHAIN_D

.text
    ld a, early_var
    ld b, BUFFER_END
    ld c, CHAIN_A
