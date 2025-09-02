; Test fixed-point manipulation built-in functions
; Tests: integer(), int(), fraction(), frac(), round(), ceil(), floor()

.section .data

; Test integer() and int() alias
test_val .const = 42
int_part .const = integer(test_val)    ; Should be 42
int_alias .const = int(test_val)       ; Should be 42 (alias test)

; Test fraction() and frac() alias  
frac_part .const = fraction(test_val)  ; Should be 0 (integers have no fractional part)
frac_alias .const = frac(test_val)     ; Should be 0 (alias test)

; Test rounding functions with integers (should return unchanged)
round_test .const = round(test_val)    ; Should be 42
ceil_test .const = ceil(test_val)      ; Should be 42
floor_test .const = floor(test_val)    ; Should be 42

; Test with negative values
neg_val .const = -25
int_neg .const = integer(neg_val)      ; Should be -25
frac_neg .const = fraction(neg_val)    ; Should be 0
round_neg .const = round(neg_val)      ; Should be -25
ceil_neg .const = ceil(neg_val)        ; Should be -25
floor_neg .const = floor(neg_val)      ; Should be -25

.section .text
start:
    nop
