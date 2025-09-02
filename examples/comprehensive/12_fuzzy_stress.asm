; Fuzzy Testing and Stress Cases
; Tests: Random-like inputs, stress conditions, boundary exploitation

.text

; Random-like test values for fuzzy testing
.const FUZZ_1 = 0x12345678
.const FUZZ_2 = 0xDEADBEEF  
.const FUZZ_3 = 0x87654321
.const FUZZ_4 = 0xCAFEBABE
.const FUZZ_5 = 0x13579BDF
.const FUZZ_6 = 0x2468ACE0
.const FUZZ_7 = 0xFEDCBA98
.const FUZZ_8 = 0x01234567

; Stress test: Large symbol table
.let stress_var_000 = FUZZ_1
.let stress_var_001 = FUZZ_2
.let stress_var_002 = FUZZ_3
.let stress_var_003 = FUZZ_4
.let stress_var_004 = FUZZ_5
.let stress_var_005 = FUZZ_6
.let stress_var_006 = FUZZ_7
.let stress_var_007 = FUZZ_8
.let stress_var_008 = stress_var_000 + stress_var_001
.let stress_var_009 = stress_var_002 * stress_var_003
.let stress_var_010 = stress_var_004 - stress_var_005
.let stress_var_011 = stress_var_006 / stress_var_007
.let stress_var_012 = stress_var_008 & stress_var_009
.let stress_var_013 = stress_var_010 | stress_var_011
.let stress_var_014 = stress_var_012 ^ stress_var_013
.let stress_var_015 = ~stress_var_014

; Test 1: Extreme parameter combinations
.macro EXTREME_PARAM_TEST
    ; Test with very large numbers
    .let huge_num = @1 * @2 + @3
    .if huge_num > 0x7FFFFFFF
        .let huge_num = huge_num & 0x7FFFFFFF
    .endif
    
    ; Test with very small numbers
    .let tiny_num = (@1 % 256) + (@2 % 16) + (@3 % 4)
    
    ; Test with mixed operations
    .let mixed = ((huge_num >> 16) << 8) | (tiny_num & 0xFF)
    
    LD A, mixed
.endmacro

; Test 2: String comparison stress test
.macro STRING_STRESS
    .let reg_type = @1
    .let is_8bit = 0
    .let is_16bit = 0
    .let is_32bit = 0
    
    ; Test all 8-bit registers
    .if strcmp(reg_type, "AH") == 0; .let is_8bit = 1; .endif
    .if strcmp(reg_type, "AL") == 0; .let is_8bit = 1; .endif
    .if strcmp(reg_type, "BH") == 0; .let is_8bit = 1; .endif
    .if strcmp(reg_type, "BL") == 0; .let is_8bit = 1; .endif
    .if strcmp(reg_type, "CH") == 0; .let is_8bit = 1; .endif
    .if strcmp(reg_type, "CL") == 0; .let is_8bit = 1; .endif
    .if strcmp(reg_type, "DH") == 0; .let is_8bit = 1; .endif
    .if strcmp(reg_type, "DL") == 0; .let is_8bit = 1; .endif
    
    ; Test all 16-bit registers
    .if strcmp(reg_type, "AW") == 0; .let is_16bit = 1; .endif
    .if strcmp(reg_type, "BW") == 0; .let is_16bit = 1; .endif
    .if strcmp(reg_type, "CW") == 0; .let is_16bit = 1; .endif
    .if strcmp(reg_type, "DW") == 0; .let is_16bit = 1; .endif
    
    ; Test all 32-bit registers
    .if strcmp(reg_type, "A") == 0; .let is_32bit = 1; .endif
    .if strcmp(reg_type, "B") == 0; .let is_32bit = 1; .endif
    .if strcmp(reg_type, "C") == 0; .let is_32bit = 1; .endif
    .if strcmp(reg_type, "D") == 0; .let is_32bit = 1; .endif
    
    ; Generate appropriate load instruction
    .if is_8bit == 1
        MV @1, @2
    .elif is_16bit == 1
        MV @1, @2
    .elif is_32bit == 1
        MV @1, @2
    .else
        .error "Unknown register type"
    .endif
.endmacro

; Test 3: Deeply nested conditionals with fuzzy data
.macro NESTED_CHAOS
    .let val = @1
    .let chaos_factor = @2
    
    .if val > 0x80000000
        .if chaos_factor & 0x1
            .if (val >> 16) > (chaos_factor << 8)
                .if val % 3 == 0
                    LD A, 0x11111111
                .elif val % 3 == 1
                    LD A, 0x22222222
                .else
                    LD A, 0x33333333
                .endif
            .else
                .if (chaos_factor & 0xF0) == 0x50
                    LD B, val + chaos_factor
                .else
                    LD B, val - chaos_factor
                .endif
            .endif
        .else
            .if val < chaos_factor
                LD C, val * 2
            .else
                LD C, chaos_factor / 2
            .endif
        .endif
    .else
        .if chaos_factor > 0x1000000
            LD D, val ^ chaos_factor
        .else
            LD D, val & chaos_factor
        .endif
    .endif
.endmacro

; Test 4: Loop stress with variable conditions
.macro LOOP_STRESS
    .let limit = @1
    .let step = @2
    .let pattern = @3
    
    .assert limit <= 50, "Loop limit too high for stress test"
    
    .let i = 0
    .let accumulator = 0
    .while i < limit
        ; Apply different patterns based on iteration
        .if (i % 4) == 0
            .let accumulator = accumulator + pattern
        .elif (i % 4) == 1
            .let accumulator = accumulator - (pattern >> 1)
        .elif (i % 4) == 2
            .let accumulator = accumulator * 2
        .else
            .let accumulator = accumulator / 2
        .endif
        
        ; Complex condition for early exit
        .if (accumulator > 0x7FFFFFFF) || (accumulator < 0)
            .let accumulator = accumulator & 0x7FFFFFFF
        .endif
        
        .let i = i + step
        
        ; Safety valve
        .if i > 100
            .error "Loop safety exceeded"
        .endif
    .endwhile
    
    LD A, accumulator
.endmacro

; Test 5: Mathematical function stress test
.macro MATH_STRESS
    .let base = @1
    .let exp = @2
    
    ; Test various math functions with fuzzy inputs
    .let sqrt_result = sqrt(base)
    .let log2_result = log2(base + 1)  ; Avoid log2(0)
    .let pow_result = pow2(exp % 31)   ; Limit exponent to prevent overflow
    
    ; Test trigonometric functions
    .let angle = base % 360
    .let sin_result = sin(angle)
    .let cos_result = cos(angle)
    .let atan_result = atan2(sin_result, cos_result)
    
    ; Test with boundary values
    .let min_val = min(sqrt_result, log2_result)
    .let max_val = max(sqrt_result, log2_result)
    .let abs_val = abs(sin_result - cos_result)
    
    ; Floor and ceiling tests
    .let floor_val = floor(abs_val)
    .let ceil_val = ceil(abs_val)
    .let round_val = round(abs_val)
    
    LD A, min_val
    LD B, max_val
    LD C, floor_val
    LD D, ceil_val
.endmacro

; Test 6: Symbol table stress with pattern generation
.macro SYMBOL_CHAOS
    .let base_id = @1
    
    ; Generate many symbols with patterns
    .let pattern_a@? = base_id * 0x01010101
    .let pattern_b@? = base_id * 0x10101010
    .let pattern_c@? = base_id ^ 0xAAAAAAAA
    .let pattern_d@? = base_id ^ 0x55555555
    .let pattern_e@? = (base_id << 1) | 1
    .let pattern_f@? = (base_id >> 1) & 0x7FFFFFFF
    
    ; Cross-reference patterns
    .let cross_ref_1@? = pattern_a@? + pattern_b@?
    .let cross_ref_2@? = pattern_c@? - pattern_d@?
    .let cross_ref_3@? = pattern_e@? * pattern_f@?
    .let cross_ref_4@? = cross_ref_1@? / (cross_ref_2@? + 1)
    
    LD A, cross_ref_4@?
.endmacro

; Test 7: Extreme expression complexity
.macro EXPRESSION_CHAOS
    .let a = @1
    .let b = @2
    .let c = @3
    
    ; Build extremely complex expression
    .let temp1 = ((a + b) * c) - ((a - b) / (c + 1))
    .let temp2 = (a << 2) | (b >> 3) | (c & 0xFF00)
    .let temp3 = sqrt(abs(temp1)) + log2(abs(temp2) + 1)
    .let temp4 = sin(temp1 % 360) + cos(temp2 % 360)
    .let temp5 = max(min(temp3, temp4), abs(temp1 - temp2))
    
    ; More chaos
    .let chaos = ((temp5 * pow2(5)) + round(temp4)) ^ (temp3 << 1)
    .let final = floor(sqrt(abs(chaos))) % 0xFFFF
    
    LD A, final
.endmacro

; Test 8: Parameter shifting stress
.macro SHIFT_STRESS
    ; Process many parameters with shifting
    .if @# > 0
        LD A, @1
        .shift
        .if @# > 0
            LD B, @1
            .shift
            .if @# > 0
                LD C, @1
                .shift
                .if @# > 0
                    LD D, @1
                    .shift
                    .if @# > 0
                        ; Continue processing remaining parameters
                        .let remaining = @#
                        .let sum = 0
                        .while @# > 0
                            .let sum = sum + @1
                            .shift
                        .endwhile
                        LD AW, sum
                    .endif
                .endif
            .endif
        .endif
    .endif
.endmacro

main:
    ; Extreme parameter testing
    EXTREME_PARAM_TEST FUZZ_1, FUZZ_2, FUZZ_3
    EXTREME_PARAM_TEST 0xFFFFFFFF, 0x12345678, 0x87654321
    
    ; String comparison stress
    STRING_STRESS "AH", 0x12
    STRING_STRESS "BW", 0x1234
    STRING_STRESS "C", 0x12345678
    
    ; Nested conditional chaos
    NESTED_CHAOS FUZZ_4, FUZZ_5
    NESTED_CHAOS 0x90000000, 0x5500AAFF
    
    ; Loop stress testing
    LOOP_STRESS 10, 1, FUZZ_6
    LOOP_STRESS 20, 2, FUZZ_7
    LOOP_STRESS 30, 3, FUZZ_8
    
    ; Mathematical stress testing
    MATH_STRESS 1000000, 15
    MATH_STRESS FUZZ_1, 20
    MATH_STRESS FUZZ_2, 10
    
    ; Symbol table chaos
    SYMBOL_CHAOS FUZZ_3
    SYMBOL_CHAOS FUZZ_4
    SYMBOL_CHAOS FUZZ_5
    
    ; Expression complexity chaos
    EXPRESSION_CHAOS FUZZ_6, FUZZ_7, FUZZ_8
    EXPRESSION_CHAOS 0x11111111, 0x22222222, 0x33333333
    
    ; Parameter shifting stress
    SHIFT_STRESS 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
    SHIFT_STRESS FUZZ_1, FUZZ_2, FUZZ_3, FUZZ_4, FUZZ_5
    
    HALT
