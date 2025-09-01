; Test file for simple macro definitions and expansions
; This file tests basic .define/.def functionality

.define BUFFER_SIZE "1024"
.define MAX_ITEMS "64"
.def MAGIC_NUMBER "0x12345678"

; Test simple substitution
.data
    buffer_size: .dw BUFFER_SIZE
    max_items: .dw MAX_ITEMS
    magic: .dd MAGIC_NUMBER

; Test macro invocation in instructions
.text
    ld a, BUFFER_SIZE
    ld b, MAX_ITEMS
    ld c, MAGIC_NUMBER

; Test multiple occurrences
.rodata
    size_table: .dw BUFFER_SIZE, MAX_ITEMS, BUFFER_SIZE
