; Test file for simple constant evaluation
; Tests basic .const directive with expressions

; Basic numeric constants
.const BUFFER_SIZE = 256
.const MAX_ITEMS = 64
.const TOTAL_MEMORY = BUFFER_SIZE * MAX_ITEMS

; Arithmetic expressions
.const HALF_BUFFER = BUFFER_SIZE / 2
.const QUARTER_BUFFER = BUFFER_SIZE / 4
.const DOUBLE_MAX = MAX_ITEMS * 2

; Bitwise operations
.const MASK_LOW = 0xFF
.const MASK_HIGH = 0xFF00
.const FULL_MASK = MASK_LOW | MASK_HIGH

; Complex expressions
.const ALIGNED_SIZE = (BUFFER_SIZE + 15) & ~15
.const STATUS_BITS = (1 << 0) | (1 << 1) | (1 << 7)

; Expressions with parentheses
.const COMPLEX_CALC = ((BUFFER_SIZE + MAX_ITEMS) * 2) / 3

.data
    buffer_declaration: .ds BUFFER_SIZE
    item_count: .dw MAX_ITEMS
    total_bytes: .dd TOTAL_MEMORY

.text
    ld a, BUFFER_SIZE
    ld b, MAX_ITEMS
    ld c, TOTAL_MEMORY
