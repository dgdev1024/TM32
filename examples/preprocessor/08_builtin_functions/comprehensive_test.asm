; Comprehensive test of all built-in functions
; Tests combination of function types and complex expressions

.section .data

; Test combining functions in expressions
complex_value .const = 0x12345678
result1 .const = high(complex_value) + low(complex_value)  ; 0x1234 + 0x5678
result2 .const = bitwidth(result1) * 2                     ; Should be bitwidth * 2

; Test nested function calls
nested1 .const = high(low(complex_value))                  ; high(0x5678) = 0x0000
nested2 .const = bitwidth(high(complex_value))             ; bitwidth(0x1234)

; Test functions with arithmetic
calc1 .const = high(0xFFFF0000) | low(0x0000FFFF)         ; 0xFFFF | 0xFFFF = 0xFFFF
calc2 .const = bitwidth(255) + bitwidth(65535)            ; 8 + 16 = 24

; Test string functions in expressions  
str1 .const = "Test"
str_calc .const = strlen(str1) + strbyte(str1, 0)         ; 4 + 'T' = 4 + 84 = 88

; Test meta functions in expressions
test_symbol .const = 42
meta_calc .const = defined(test_symbol) + isconst(test_symbol) ; 1 + 1 = 2

; Test trigonometric in expressions
trig_calc .const = sin(0) + cos(0)                        ; 0 + 65536 = 65536

; Test fixed-point functions
fp_calc .const = integer(100) + fraction(100)             ; 100 + 0 = 100

; Complex expression combining multiple function types
complex_expr .const = (high(0xABCD1234) * low(0xABCD1234)) + 
                    (strlen("ABC") * bitwidth(255)) + 
                    defined(test_symbol)
                    ; (0xABCD * 0x1234) + (3 * 8) + 1

; Test function aliases
alias_test1 .const = int(42) + frac(42)                   ; 42 + 0 = 42
alias_test2 .const = isvar(test_symbol)                   ; Should be 0 (it's a const)

; Error boundary tests (should work)
boundary1 .const = bitwidth(0)                            ; Should be 1
boundary2 .const = strlen("")                             ; Should be 0
boundary3 .const = strbyte("A", 0)                        ; Should be 'A'

.section .text
start:
    nop
