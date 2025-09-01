; Test file for variable evaluation
; Tests .let directive with default values and assignments

; Variables with default values (should be 0)
.let counter
.let index
.let flags

; Variables with explicit initial values
.let buffer_pos = 0
.let max_iterations = 100
.let current_state = 1

; Variables using constants and expressions
.const BASE_ADDR = 0x1000
.let current_addr = BASE_ADDR
.let next_addr = BASE_ADDR + 64
.let end_addr = BASE_ADDR + 1024

; Variables with complex expressions
.let calculated_value = (100 + 50) * 2
.let bit_pattern = (1 << 3) | (1 << 5)
.let masked_value = 0xABCD & 0x00FF

; Variables that reference other variables
.let offset = buffer_pos + 10
.let range = max_iterations - counter

.data
    position_table: .dw current_addr, next_addr, end_addr
    
.text
    ld a, counter
    ld b, buffer_pos
    ld c, calculated_value
