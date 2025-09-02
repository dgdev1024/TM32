; Test integer manipulation built-in functions
; Tests: high(), low(), bitwidth()

.section .data

; Test high() function
test_value .const = 0x12345678
high_part .const = high(test_value)    ; Should be 0x1234

; Test low() function  
low_part .const = low(test_value)      ; Should be 0x5678

; Test bitwidth() function
bitwidth_8 .const = bitwidth(255)      ; Should be 8
bitwidth_16 .const = bitwidth(65535)   ; Should be 16
bitwidth_32 .const = bitwidth(0xFFFFFFFF) ; Should be 32
bitwidth_1 .const = bitwidth(1)        ; Should be 1
bitwidth_0 .const = bitwidth(0)        ; Should be 1

; Test with negative values
neg_value .const = -1
bitwidth_neg .const = bitwidth(neg_value) ; Should account for sign bit

.section .text
start:
    nop
