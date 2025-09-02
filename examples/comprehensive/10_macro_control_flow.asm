; Complex Macro Control Flow Testing
; Tests: Nested conditionals in macros, loop constructs, error handling

.text

; Test 1: Conditional macro based on register type
.macro SMART_LOAD
    .if strcmp("@1", "A") == 0
        LD A, @2
        ; Add validation for A register
        .assert @2 <= 0xFFFFFFFF, "Value too large for A register"
    .elif strcmp("@1", "B") == 0
        LD B, @2
    .elif strcmp("@1", "C") == 0
        LD C, @2
    .elif strcmp("@1", "D") == 0
        LD D, @2
    .elif strcmp("@1", "AH") == 0 || strcmp("@1", "AL") == 0 || strcmp("@1", "BH") == 0 || strcmp("@1", "BL") == 0 || strcmp("@1", "CH") == 0 || strcmp("@1", "CL") == 0 || strcmp("@1", "DH") == 0 || strcmp("@1", "DL") == 0
        ; Handle 8-bit registers
        LD @1, (@2 & 0xFF)
    .elif strcmp("@1", "AW") == 0 || strcmp("@1", "BW") == 0 || strcmp("@1", "CW") == 0 || strcmp("@1", "DW") == 0
        ; Handle 16-bit registers
        LD @1, (@2 & 0xFFFF)
    .else
        .error "Unknown register type: @1"
    .endif
.endmacro

; Test 2: Macro with loop construct using variables
.macro INIT_ARRAY
    .let array_size = @#
    .let i = 0
    .while i < array_size
        .let value = @[i + 1]  ; Get positional parameter
        LD [0x10000000 + (i * 4)], value
        .let i = i + 1
    .endwhile
.endmacro

; Test 3: Nested macro calls with parameter shifting
.macro NESTED_SHIFTS
    .if @# > 0
        SMART_LOAD @1, @2
        .shift 2
        .if @# > 0
            NESTED_SHIFTS @*
        .endif
    .endif
.endmacro

; Test 4: Complex conditional with built-in functions
.macro MATH_OPERATION
    .let op_type = @1
    .let val1 = @2
    .let val2 = @3
    
    .if strcmp(op_type, "ADD") == 0
        LD A, val1
        ADD A, val2
    .elif strcmp(op_type, "SUB") == 0
        LD A, val1
        SUB A, val2
    .elif strcmp(op_type, "MUL") == 0
        ; Multiplication using shifts (power of 2 only)
        .if (val2 & (val2 - 1)) == 0  ; Check if power of 2
            .let shift_count = log2(val2)
            LD A, val1
            .let j = 0
            .while j < shift_count
                SLA A
                .let j = j + 1
            .endwhile
        .else
            .error "MUL only supports power of 2 multipliers"
        .endif
    .elif strcmp(op_type, "DIV") == 0
        ; Division using shifts (power of 2 only)
        .if (val2 & (val2 - 1)) == 0  ; Check if power of 2
            .let shift_count = log2(val2)
            LD A, val1
            .let k = 0
            .while k < shift_count
                SRA A
                .let k = k + 1
            .endwhile
        .else
            .error "DIV only supports power of 2 divisors"
        .endif
    .else
        .error "Unknown operation: @1"
    .endif
.endmacro

; Test 5: Macro with nested loops and conditionals
.macro MATRIX_INIT
    .let rows = @1
    .let cols = @2
    .let base_addr = @3
    
    .let r = 0
    .while r < rows
        .let c = 0
        .while c < cols
            .let addr = base_addr + (r * cols * 4) + (c * 4)
            .if (r + c) % 2 == 0
                LD [addr], 0xAAAAAAAA  ; Checkerboard pattern
            .else
                LD [addr], 0x55555555
            .endif
            .let c = c + 1
        .endwhile
        .let r = r + 1
    .endwhile
.endmacro

; Test 6: Error handling macro with validation
.macro SAFE_DIVISION
    .assert @# == 2, "SAFE_DIVISION requires exactly 2 arguments"
    .let dividend = @1
    .let divisor = @2
    
    .assert divisor != 0, "Division by zero not allowed"
    .assert divisor > 0, "Negative divisors not supported"
    .assert (divisor & (divisor - 1)) == 0, "Divisor must be power of 2"
    
    .let result = dividend / divisor
    LD A, result
.endmacro

; Test 7: Recursive-style macro (simulated with loop)
.macro FIBONACCI_SEQUENCE
    .let count = @1
    .let base_addr = @2
    
    .assert count >= 2, "Fibonacci sequence needs at least 2 numbers"
    
    ; Initialize first two numbers
    LD [base_addr], 1
    LD [base_addr + 4], 1
    
    .let n = 2
    .while n < count
        .let prev_addr = base_addr + ((n - 2) * 4)
        .let curr_addr = base_addr + ((n - 1) * 4)
        .let next_addr = base_addr + (n * 4)
        
        LD A, [prev_addr]
        ADD A, [curr_addr]
        ST [next_addr], A
        
        .let n = n + 1
    .endwhile
.endmacro

; Constants for testing
.const TEST_BASE_ADDR = 0x80000000
.const MATRIX_ROWS = 4
.const MATRIX_COLS = 4

main:
    ; Test smart loading with different register types
    SMART_LOAD A, 0x12345678
    SMART_LOAD BH, 0x42
    SMART_LOAD CW, 0x1234
    
    ; Test array initialization
    INIT_ARRAY 0x11111111, 0x22222222, 0x33333333, 0x44444444
    
    ; Test nested shifts
    NESTED_SHIFTS A, 0x1000, B, 0x2000, C, 0x3000
    
    ; Test math operations
    MATH_OPERATION "ADD", 100, 50
    MATH_OPERATION "SUB", 200, 75
    MATH_OPERATION "MUL", 10, 8    ; 8 is power of 2
    MATH_OPERATION "DIV", 64, 4    ; 4 is power of 2
    
    ; Test matrix initialization
    MATRIX_INIT MATRIX_ROWS, MATRIX_COLS, TEST_BASE_ADDR
    
    ; Test safe division
    SAFE_DIVISION 128, 16
    
    ; Test Fibonacci sequence
    FIBONACCI_SEQUENCE 10, TEST_BASE_ADDR + 0x1000
    
    RET NC
