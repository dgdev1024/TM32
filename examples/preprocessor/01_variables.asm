;
; @file     examples/preprocessor/01_variables.asm
; @brief    Contains code which demonstrates the TM32 Assembler Tool's
;           Preprocessor Component's ability to create, store and manage
;           variables and constants with comprehensive test cases.
;

; Run this file with:
;   `tm32asm --preprocess-only examples/preprocessor/01_variables.asm`

; ============================================================================
; BASIC VARIABLE AND CONSTANT DECLARATIONS
; ============================================================================

; Create a simple, uninitialized mutable variable.
; Uninitialized variables automatically initialize to zero.
.let var_a

; Create another variable, this time with an initialization.
.let var_b = 34

; Create an immutable constant.
.const CONST_A = 7

; Initialize another variable, then assign it another value.
.let var_c = 99
.let var_c = 110

; Initialize another variable with an expression, then use a compound
; assignment operator to assign it another value.
.let var_d = 21 * 4
.let var_d /= (8 - 6)

; ============================================================================
; DIFFERENT NUMBER BASES AND LITERAL TYPES
; ============================================================================

; Binary literals
.let bin_val = 0b11010110
.const BIN_MASK = 0b00001111

; Octal literals
.let oct_val = 0o377
.const OCT_PERMISSIONS = 0o755

; Hexadecimal literals
.let hex_val = 0xFF00
.const HEX_COLOR = 0x3A2F1E

; Character literals
.let char_a = 'A'
.let char_newline = '\n'
.let char_hex = '\x41'
.const ESCAPE_CHAR = '\x1B'

; ============================================================================
; COMPLEX EXPRESSIONS WITH ALL OPERATOR TYPES
; ============================================================================

; Arithmetic expressions with precedence
.let arith_basic = 5 + 3 * 2
.let arith_complex = (10 + 5) * 3 - 8 / 2
.let arith_power = 2 ** 3 + 1
.let arith_modulo = 17 % 5

; Bitwise operations
.let bitwise_and = 0xFF & 0x0F
.let bitwise_or = 0xF0 | 0x0F
.let bitwise_xor = 0xFF ^ 0xAA
.let bitwise_not = ~0xFF
.let bitwise_shift_left = 1 << 4
.let bitwise_shift_right = 0xFF >> 2

; Logical operations
.let logical_and = (5 > 3) && (2 < 4)
.let logical_or = (1 == 0) || (3 != 4)
.let logical_not = !(5 < 3)

; Comparison operations
.let comp_equal = 5 == 5
.let comp_not_equal = 3 != 4
.let comp_less = 2 < 5
.let comp_greater = 7 > 3
.let comp_less_equal = 4 <= 4
.let comp_greater_equal = 6 >= 5

; Ternary conditional operator
.let ternary_simple = (5 > 3) ? 100 : 200
.let ternary_nested = (var_b > 30) ? ((var_c > 100) ? 1 : 2) : 3

; ============================================================================
; COMPOUND ASSIGNMENT OPERATORS
; ============================================================================

; Basic compound assignments
.let compound_add = 10
.let compound_add += 5

.let compound_sub = 20
.let compound_sub -= 3

.let compound_mul = 4
.let compound_mul *= 3

.let compound_div = 24
.let compound_div /= 4

.let compound_mod = 17
.let compound_mod %= 5

.let compound_pow = 2
.let compound_pow **= 3

; Bitwise compound assignments
.let compound_and = 0xFF
.let compound_and &= 0x0F

.let compound_or = 0xF0
.let compound_or |= 0x0F

.let compound_xor = 0xFF
.let compound_xor ^= 0xAA

.let compound_shl = 1
.let compound_shl <<= 4

.let compound_shr = 0xFF
.let compound_shr >>= 2

; ============================================================================
; VARIABLE INTERDEPENDENCIES AND FORWARD REFERENCES
; ============================================================================

; Variables that depend on other variables
.let base_value = 100
.let derived_value = base_value * 2
.let double_derived = derived_value + base_value

; Variables using constants
.let const_based = CONST_A * 10
.const DERIVED_CONST = CONST_A + var_b

; ============================================================================
; EDGE CASES AND BOUNDARY CONDITIONS
; ============================================================================

; Maximum and minimum values
.let max_32bit = 0xFFFFFFFF
.let min_signed = -2147483648
.let zero_variants = 0x0000

; Very long expressions
.let complex_expr = ((((5 + 3) * 2) << 1) & 0xFF) | (((10 - 2) ** 2) >> 3)

; Nested parentheses
.let deeply_nested = ((((((1 + 2) * 3) - 4) / 2) % 3) ** 2)

; Mixed types in expressions
.let mixed_types = 'A' + 10 + 0x10 + 0b1010

; String and character operations
.let char_arithmetic = 'Z' - 'A'
.let ascii_range = 'z' - 'a' + 1

; Boolean arithmetic
.let bool_math = (5 > 3) + (2 < 1)

; Division by constants
.let division_test = 100 / CONST_A

; Negative numbers and unary operators
.let positive_val = 42
.let negative_val = -42
.let double_negative = --positive_val

; Bitwise NOT with different sizes
.let not_byte = ~0xFF
.let not_word = ~0xFFFF
.let not_dword = ~0xFFFFFFFF

; ============================================================================
; VARIABLE REASSIGNMENT PATTERNS
; ============================================================================

; Multiple reassignments
.let counter = 0
.let counter += 1
.let counter *= 2
.let counter -= 1

; Self-referential assignments
.let self_ref = 10
.let self_ref = self_ref * 2 + 1

; Conditional reassignments using ternary
.let conditional_var = 5
.let conditional_var = (conditional_var > 3) ? conditional_var * 2 : conditional_var + 1

; ============================================================================
; USAGE IN ASSEMBLY CODE
; ============================================================================

.text
main:
    ; Basic variable usage
    ld al, var_a
    ld ah, var_c
    add al, var_b
    sub al, CONST_A
    sub ah, var_d
    
    ; Different number base usage
    ld a, hex_val
    and a, BIN_MASK
    or a, oct_val
    
    ; Character literal usage
    ld bl, char_a
    ld bh, ESCAPE_CHAR
    
    ; Expression results usage
    ld c, arith_complex
    ld d, bitwise_and
    
    ; Compound assignment results
    ld al, compound_add
    ld ah, compound_mul
    
    ; Complex expression results
    ld a, complex_expr
    ld b, deeply_nested
    
    ; Derived variable usage - showing variable interdependencies
    ld a, base_value        ; Load the base value (100)
    ld b, derived_value     ; Load derived value (base_value * 2 = 200)
    ld c, double_derived    ; Load double derived (derived_value + base_value = 300)
    add a, b                ; a = base_value + derived_value = 100 + 200 = 300
    sub c, a                ; c = double_derived - (base_value + derived_value) = 300 - 300 = 0
    
    ; Constant-based calculations
    ld a, const_based       ; Load CONST_A * 10 = 70
    ld b, DERIVED_CONST     ; Load CONST_A + var_b = 7 + 34 = 41
    add a, b                ; a = const_based + DERIVED_CONST = 70 + 41 = 111
    
    ; Arithmetic expression results in calculations
    ld a, arith_basic       ; Load 5 + 3 * 2 = 11
    ld b, arith_modulo      ; Load 17 % 5 = 2
    add a, b                ; a = arith_basic + arith_modulo = 11 + 2 = 13
    
    ; Bitwise operation chains
    ld a, bitwise_shift_left    ; Load 1 << 4 = 16
    ld b, bitwise_shift_right   ; Load 0xFF >> 2 = 63
    and a, b                    ; a = 16 & 63 = 16
    or a, bitwise_xor           ; a = 16 | 85 = 85 | 16 = 101
    
    ; Logical and comparison results
    ld al, logical_and      ; Load (5 > 3) && (2 < 4) = 1
    ld ah, logical_or       ; Load (1 == 0) || (3 != 4) = 1
    ld bl, comp_equal       ; Load 5 == 5 = 1
    ld bh, comp_greater     ; Load 7 > 3 = 1
    add al, ah              ; al = 1 + 1 = 2
    add bl, bh              ; bl = 1 + 1 = 2
    
    ; Ternary operator results
    ld a, ternary_simple    ; Load (5 > 3) ? 100 : 200 = 100
    ld b, ternary_nested    ; Load complex ternary result = 1
    sub a, b                ; a = 100 - 1 = 99
    
    ; Character arithmetic in action
    ld a, char_arithmetic   ; Load 'Z' - 'A' = 25
    ld b, ascii_range       ; Load 'z' - 'a' + 1 = 26
    add a, b                ; a = 25 + 26 = 51
    add a, char_a           ; a = 51 + 65 = 116 (ASCII 't')
    
    ; Mixed type calculations
    ld a, mixed_types       ; Load 'A' + 10 + 0x10 + 0b1010 = 101
    sub a, char_a           ; a = 101 - 65 = 36
    add a, char_newline     ; a = 36 + 10 = 46
    
    ; Reassignment pattern results
    ld a, counter           ; Load final counter value = 1
    ld b, self_ref          ; Load self-referential result = 21
    ld c, conditional_var   ; Load conditional result = 10
    add a, b                ; a = 1 + 21 = 22
    add a, c                ; a = 22 + 10 = 32
    
    ; Division and modulo with variables
    ld a, division_test     ; Load 100 / CONST_A = 14
    ld b, arith_modulo      ; Load 17 % 5 = 2
    add a, b                ; a = 14 + 2 = 16
    
    ; Unary operation results
    ld a, positive_val      ; Load 42
    ld b, negative_val      ; Load -42
    ld c, double_negative   ; Load --42 = 42
    add a, c                ; a = 42 + 42 = 84
    sub a, negative_val     ; a = 84 - (-42) = 84 + 42 = 126
    
    ; Boolean math demonstration
    ld a, bool_math         ; Load (5 > 3) + (2 < 1) = 1
    ld b, logical_not       ; Load !(5 < 3) = 1
    add a, b                ; a = 1 + 1 = 2
    
    ; Edge case results
    ld c, max_32bit         ; Load maximum 32-bit value
    ld d, min_signed        ; Load minimum signed value

array_processing:
    ; Simulate array processing with derived values
    ld a, base_value        ; Base address simulation
    add a, derived_value    ; Add offset (base_value * 2)
    ld b, BIN_MASK          ; Load mask value
    and a, b                ; Apply mask for bounds checking

bounds_checking:
    ; Demonstrate bounds checking with constants
    ld a, oct_val           ; Load octal value 255
    ld b, hex_val           ; Load hex value 65280
    cmp a, OCT_PERMISSIONS  ; Compare with permissions value
    jpb nc, in_bounds       ; Jump if within bounds
    ld a, CONST_A           ; Load fallback value

in_bounds:
    ; Complex address calculations
    ld a, HEX_COLOR         ; Load color value
    add a, compound_shl     ; Add shifted value
    and a, not_word         ; Apply word mask

done:
    jpb nc, done

;
; The preprocessor should resolve the above code to the following:
;
; ```
; .text
; main:
;     ; Basic variable usage
;     ld al, 0         ; var_a (uninitialized = 0)
;     ld ah, 110       ; var_c (reassigned from 99 to 110)
;     add al, 34       ; var_b
;     sub al, 7        ; CONST_A
;     sub ah, 42       ; var_d (21 * 4 = 84, then 84 / 2 = 42)
;     
;     ; Different number base usage
;     ld a, 65280      ; hex_val (0xFF00)
;     and a, 15        ; BIN_MASK (0b00001111 = 15)
;     or a, 255        ; oct_val (0o377 = 255)
;     
;     ; Character literal usage
;     ld bl, 65        ; char_a ('A' = 65)
;     ld bh, 27        ; ESCAPE_CHAR ('\x1B' = 27)
;     
;     ; Expression results usage
;     ld c, 41         ; arith_complex ((10 + 5) * 3 - 8 / 2 = 45 - 4 = 41)
;     ld d, 15         ; bitwise_and (0xFF & 0x0F = 15)
;     
;     ; Compound assignment results
;     ld al, 15        ; compound_add (10 + 5 = 15)
;     ld ah, 12        ; compound_mul (4 * 3 = 12)
;     
;     ; Complex expression results
;     ld a, 40         ; complex_expr (((((5 + 3) * 2) << 1) & 0xFF) | (((10 - 2) ** 2) >> 3))
;                      ; = (((8 * 2) << 1) & 0xFF) | ((8 ** 2) >> 3)
;                      ; = ((16 << 1) & 0xFF) | (64 >> 3)
;                      ; = (32 & 0xFF) | 8 = 32 | 8 = 40
;     ld b, 4          ; deeply_nested (((((((1 + 2) * 3) - 4) / 2) % 3) ** 2))
;     
;     ; Derived variable usage - showing variable interdependencies
;     ld a, 100        ; base_value
;     ld b, 200        ; derived_value (base_value * 2)
;     ld c, 300        ; double_derived (derived_value + base_value)
;     add a, b         ; a = 100 + 200 = 300
;     sub c, a         ; c = 300 - 300 = 0
;     
;     ; Constant-based calculations
;     ld a, 70         ; const_based (CONST_A * 10 = 7 * 10)
;     ld b, 41         ; DERIVED_CONST (CONST_A + var_b = 7 + 34)
;     add a, b         ; a = 70 + 41 = 111
;     
;     ; Arithmetic expression results in calculations
;     ld a, 11         ; arith_basic (5 + 3 * 2)
;     ld b, 2          ; arith_modulo (17 % 5)
;     mul a, b         ; a = 11 * 2 = 22
;     
;     ; Bitwise operation chains
;     ld a, 16         ; bitwise_shift_left (1 << 4)
;     ld b, 63         ; bitwise_shift_right (0xFF >> 2)
;     and a, b         ; a = 16 & 63 = 16
;     or a, 85         ; a = 16 | bitwise_xor (0xFF ^ 0xAA = 85)
;     
;     ; Logical and comparison results
;     ld al, 1         ; logical_and ((5 > 3) && (2 < 4))
;     ld ah, 1         ; logical_or ((1 == 0) || (3 != 4))
;     ld bl, 1         ; comp_equal (5 == 5)
;     ld bh, 1         ; comp_greater (7 > 3)
;     add al, ah       ; al = 1 + 1 = 2
;     add bl, bh       ; bl = 1 + 1 = 2
;     
;     ; Ternary operator results
;     ld a, 100        ; ternary_simple ((5 > 3) ? 100 : 200)
;     ld b, 1          ; ternary_nested (complex ternary result)
;     sub a, b         ; a = 100 - 1 = 99
;     
;     ; Character arithmetic in action
;     ld a, 25         ; char_arithmetic ('Z' - 'A')
;     ld b, 26         ; ascii_range ('z' - 'a' + 1)
;     add a, b         ; a = 25 + 26 = 51
;     add a, 65        ; a = 51 + char_a = 116
;     
;     ; Mixed type calculations
;     ld a, 101        ; mixed_types ('A' + 10 + 0x10 + 0b1010)
;     sub a, 65        ; a = 101 - char_a = 36
;     add a, 10        ; a = 36 + char_newline = 46
;     
;     ; Reassignment pattern results
;     ld a, 1          ; counter (final value after multiple operations)
;     ld b, 21         ; self_ref (10 * 2 + 1)
;     ld c, 10         ; conditional_var (conditional result)
;     add a, b         ; a = 1 + 21 = 22
;     add a, c         ; a = 22 + 10 = 32
;     
;     ; Division and modulo with variables
;     ld a, 14         ; division_test (100 / CONST_A)
;     ld b, 2          ; arith_modulo (17 % 5)
;     add a, b         ; a = 14 + 2 = 16
;     
;     ; Unary operation results
;     ld a, 42         ; positive_val
;     ld b, -42        ; negative_val
;     ld c, 42         ; double_negative (--42)
;     add a, c         ; a = 42 + 42 = 84
;     sub a, -42       ; a = 84 - (-42) = 126
;     
;     ; Boolean math demonstration
;     ld a, 1          ; bool_math ((5 > 3) + (2 < 1))
;     ld b, 1          ; logical_not (!(5 < 3))
;     add a, b         ; a = 1 + 1 = 2
;     
;     ; Edge case results
;     ld c, 4294967295 ; max_32bit (0xFFFFFFFF)
;     ld d, -2147483648 ; min_signed
; 
; array_processing:
;     ; Simulate array processing with derived values
;     ld a, 100        ; base_value
;     add a, 200       ; Add derived_value offset
;     ld b, 15         ; BIN_MASK
;     and a, b         ; Apply mask for bounds checking
; 
; bounds_checking:
;     ; Demonstrate bounds checking with constants
;     ld a, 255        ; oct_val (0o377)
;     ld b, 65280      ; hex_val (0xFF00)
;     cmp a, 493       ; Compare with OCT_PERMISSIONS (0o755)
;     jpb nc, in_bounds ; Jump if within bounds
;     ld a, 7          ; CONST_A fallback
; 
; in_bounds:
;     ; Complex address calculations
;     ld a, 3813150    ; HEX_COLOR (0x3A2F1E)
;     add a, 16        ; Add compound_shl (1 <<= 4)
;     and a, -65536    ; Apply not_word mask (~0xFFFF)
; 
; done:
;     jpb nc, done
; ```
;
; Variable Value Summary:
; =====================
; var_a = 0 (uninitialized)
; var_b = 34
; var_c = 110 (reassigned from 99)
; var_d = 42 (84 / 2)
; CONST_A = 7
; bin_val = 214 (0b11010110)
; BIN_MASK = 15 (0b00001111)
; oct_val = 255 (0o377)
; OCT_PERMISSIONS = 493 (0o755)
; hex_val = 65280 (0xFF00)
; HEX_COLOR = 3813150 (0x3A2F1E)
; char_a = 65 ('A')
; char_newline = 10 ('\n')
; char_hex = 65 ('\x41')
; ESCAPE_CHAR = 27 ('\x1B')
; arith_basic = 11 (5 + 3 * 2)
; arith_complex = 41 ((10 + 5) * 3 - 8 / 2)
; arith_power = 9 (2 ** 3 + 1)
; arith_modulo = 2 (17 % 5)
; bitwise_and = 15 (0xFF & 0x0F)
; bitwise_or = 255 (0xF0 | 0x0F)
; bitwise_xor = 85 (0xFF ^ 0xAA)
; bitwise_not = -256 (~0xFF, depending on bit width)
; bitwise_shift_left = 16 (1 << 4)
; bitwise_shift_right = 63 (0xFF >> 2)
; logical_and = 1 ((5 > 3) && (2 < 4) = true && true = 1)
; logical_or = 1 ((1 == 0) || (3 != 4) = false || true = 1)
; logical_not = 1 (!(5 < 3) = !false = 1)
; comp_equal = 1 (5 == 5 = true)
; comp_not_equal = 1 (3 != 4 = true)
; comp_less = 1 (2 < 5 = true)
; comp_greater = 1 (7 > 3 = true)
; comp_less_equal = 1 (4 <= 4 = true)
; comp_greater_equal = 1 (6 >= 5 = true)
; ternary_simple = 100 ((5 > 3) ? 100 : 200 = true ? 100 : 200 = 100)
; ternary_nested = 1 ((var_b > 30) ? ((var_c > 100) ? 1 : 2) : 3 = true ? (true ? 1 : 2) : 3 = 1)
; compound_add = 15 (10 += 5)
; compound_sub = 17 (20 -= 3)
; compound_mul = 12 (4 *= 3)
; compound_div = 6 (24 /= 4)
; compound_mod = 2 (17 %= 5)
; compound_pow = 8 (2 **= 3)
; compound_and = 15 (0xFF &= 0x0F)
; compound_or = 255 (0xF0 |= 0x0F)
; compound_xor = 85 (0xFF ^= 0xAA)
; compound_shl = 16 (1 <<= 4)
; compound_shr = 63 (0xFF >>= 2)
; base_value = 100
; derived_value = 200 (base_value * 2)
; double_derived = 300 (derived_value + base_value)
; const_based = 70 (CONST_A * 10)
; DERIVED_CONST = 41 (CONST_A + var_b)
; max_32bit = 4294967295 (0xFFFFFFFF)
; min_signed = -2147483648
; zero_variants = 0 (0x0000)
; complex_expr = 40 (((((5 + 3) * 2) << 1) & 0xFF) | (((10 - 2) ** 2) >> 3) = (((8 * 2) << 1) & 0xFF) | ((8 ** 2) >> 3) = ((16 << 1) & 0xFF) | (64 >> 3) = (32 & 0xFF) | 8 = 32 | 8 = 40)
; deeply_nested = 4 (((((((1 + 2) * 3) - 4) / 2) % 3) ** 2) = ((((3 * 3) - 4) / 2) % 3) ** 2 = (((9 - 4) / 2) % 3) ** 2 = ((5 / 2) % 3) ** 2 = (2 % 3) ** 2 = 2 ** 2 = 4)
; mixed_types = 101 ('A' + 10 + 0x10 + 0b1010 = 65 + 10 + 16 + 10 = 101)
; char_arithmetic = 25 ('Z' - 'A' = 90 - 65 = 25)
; ascii_range = 26 ('z' - 'a' + 1 = 122 - 97 + 1 = 26)
; bool_math = 1 ((5 > 3) + (2 < 1) = 1 + 0 = 1)
; division_test = 14 (100 / CONST_A = 100 / 7 = 14.28... truncated to 14)
; positive_val = 42
; negative_val = -42
; double_negative = 42 (--positive_val = --42 = 42)
; not_byte = -256 (~0xFF)
; not_word = -65536 (~0xFFFF)
; not_dword = -4294967296 (~0xFFFFFFFF)
; counter = 1 (0 + 1 * 2 - 1 = 1)
; self_ref = 21 (10 * 2 + 1)
; conditional_var = 10 ((5 > 3) ? 5 * 2 : 5 + 1 = true ? 10 : 6 = 10)
;
; Edge Cases and Error Conditions:
; ===============================
; - Circular dependencies (circular_a/circular_b) should cause errors
; - Division by zero should cause errors
; - Undefined symbol references should cause errors
; - Constant reassignment should cause errors
; - Invalid function calls should cause errors
; - Type mismatches in unsupported contexts should cause warnings/errors
;
; Notes:
; ======
; - Variables and constants are visible only to the preprocessor and are
;   managed only during the preprocessor step of the assembly process.
; - All arithmetic is performed using the expression evaluator's precision
; - Boolean values are represented as 1 (true) or 0 (false)
; - All compound assignments modify the variable in place
; - Operator precedence follows the 12-level hierarchy defined in the specification
;
