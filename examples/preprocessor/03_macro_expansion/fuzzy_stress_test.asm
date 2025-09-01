; Ultra-fuzzy stress test for preprocessor edge cases
; This file tests extreme scenarios that might break the preprocessor

; ============================================================================
; STRESS TEST: DEEP NESTING AND COMPLEXITY
; ============================================================================

; Test extremely long parameter lists (stress memory management)
.macro STRESS_LONG_PARAMS
    .asciz "@1@2@3@4@5@6@7@8@9@10@11@12@13@14@15@16@17@18@19@20@21@22@23@24@25@26@27@28@29@30"
.endmacro

; Test complex shift patterns in loops
.macro STRESS_SHIFT_LOOP
    .repeat @#
        .if @# > 0
            .asciz "Level @#: @1"
            .shift 1
        .endif
    .endrep
.endmacro

; Test extreme parameter substitution patterns
.macro STRESS_PARAM_PATTERNS
    ; Test parameter concatenation stress
    @1@2@3_label_@4@5:
    ; Test parameter in every possible context
    .@1 @2, @3[@4 + @5], (@6)
    .asciz "@1 @2 @3 @4 @5 @6 @7 @8 @9 @10"
    ; Test parameter reference edge cases
    .db @99, @100, @999  ; Out of bounds parameters
.endmacro

; Test maximum recursion depth simulation
.macro STRESS_DEEP_CALL_1
    STRESS_DEEP_CALL_2 @*
.endmacro

.macro STRESS_DEEP_CALL_2
    STRESS_DEEP_CALL_3 @*
.endmacro

.macro STRESS_DEEP_CALL_3
    .asciz "Deep call result: @*"
.endmacro

; ============================================================================
; STRESS TEST: MIXED PARAMETER TYPES AND EDGE VALUES
; ============================================================================

; Test with extreme numeric values
.macro STRESS_EXTREME_NUMBERS
    .dw @1    ; Should handle 0
    .dw @2    ; Should handle -1
    .dw @3    ; Should handle large positive
    .dw @4    ; Should handle large negative
    .dw @5    ; Should handle hex
    .dw @6    ; Should handle binary
.endmacro

; Test with special characters and symbols
.macro STRESS_SPECIAL_CHARS
    .asciz "@1"  ; Empty string
    .asciz "@2"  ; String with spaces
    .asciz "@3"  ; String with special chars
    .asciz "@4"  ; String with quotes
    .asciz "@5"  ; String with escapes
.endmacro

; Test parameter boundary conditions
.macro STRESS_BOUNDARIES
    ; Test zero parameters
    .if @# == 0
        .asciz "No parameters provided"
    .endif
    
    ; Test single parameter
    .if @# == 1
        .asciz "Single parameter: @1"
    .endif
    
    ; Test many parameters
    .if @# > 10
        .asciz "Many parameters: @*"
    .endif
.endmacro

; ============================================================================
; STRESS TEST: COMPLEX CONTROL FLOW WITH PARAMETERS
; ============================================================================

; Test nested conditionals with parameter manipulation
.macro STRESS_NESTED_CONDITIONS
    .if @# > 0
        .if @1 != ""
            .repeat 2
                .while @# > 1
                    .asciz "Processing @1 with @# total params"
                    .shift 1
                .endwhile
            .endrep
        .else
            .asciz "First parameter is empty"
        .endif
    .else
        .asciz "No parameters at all"
    .endif
.endmacro

; Test extreme shift scenarios within complex control flow
.macro STRESS_SHIFT_CONTROL
    ; Save original count
    .asciz "Starting with @# parameters: @*"
    
    ; Shift in a loop with conditions
    .repeat 5
        .if @# > 0
            .asciz "Round @# of shifting: @1"
            .shift 1
        .else
            .asciz "No more parameters to shift"
        .endif
    .endrep
    
    .asciz "Final state: @# parameters remaining"
.endmacro

; ============================================================================
; STRESS TEST: UNIQUE IDENTIFIER GENERATION
; ============================================================================

; Test unique identifier stress
.macro STRESS_UNIQUE_IDS
    @?_start:
    @?_loop:
    @?_end:
    @?_temp:
    @?_var1:
    @?_var2:
    @?_var3:
    jmp @?_start
    jmp @?_loop  
    jmp @?_end
.endmacro

; Test unique IDs with parameters
.macro STRESS_UNIQUE_WITH_PARAMS
    @?_@1_label:
    @?_@2_loop:
    .asciz "Labels: @?_@1_label, @?_@2_loop"
.endmacro

; ============================================================================
; STRESS TEST: MIXED DIRECTIVE AND PARAMETER USAGE
; ============================================================================

; Test parameters in various directive contexts
.macro STRESS_DIRECTIVE_PARAMS
    .org @1
    .align @2
    .skip @3
    .byte @4, @5, @6
    .word @7, @8
    .long @9, @10
    .ascii "@11"
    .asciz "@12"
.endmacro

; Test conditional assembly with complex parameter expressions
.macro STRESS_CONDITIONAL_ASSEMBLY
    .if @1 > @2
        .asciz "Parameter 1 (@1) is greater than parameter 2 (@2)"
        .if @3 != ""
            .asciz "Parameter 3 is: @3"
        .endif
    .elseif @1 < @2
        .asciz "Parameter 1 (@1) is less than parameter 2 (@2)"
    .else
        .asciz "Parameters 1 and 2 are equal: @1 == @2"
    .endif
.endmacro

; ============================================================================
; STRESS TEST INVOCATIONS
; ============================================================================

.text
    ; Test extremely long parameter lists
    STRESS_LONG_PARAMS "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "extra1", "extra2"
    
    ; Test shift stress
    STRESS_SHIFT_LOOP "a", "b", "c", "d", "e", "f", "g"
    
    ; Test complex parameter patterns
    STRESS_PARAM_PATTERNS "prefix", "middle", "suffix", "idx1", "idx2", "offset", "reg1", "reg2", "reg3", "reg4"
    
    ; Test deep call chains
    STRESS_DEEP_CALL_1 "deep_param1", "deep_param2", "deep_param3"
    
    ; Test extreme numeric values
    STRESS_EXTREME_NUMBERS 0, -1, 65535, -32768, 0xFF00, 0b11110000
    
    ; Test special characters
    STRESS_SPECIAL_CHARS "", "string with spaces", "special!@#$%^&*()", "string\"with\"quotes", "string\nwith\tescapes"
    
    ; Test boundary conditions
    STRESS_BOUNDARIES
    STRESS_BOUNDARIES "single"
    STRESS_BOUNDARIES "p1", "p2", "p3", "p4", "p5", "p6", "p7", "p8", "p9", "p10", "p11", "p12"
    
    ; Test nested conditions
    STRESS_NESTED_CONDITIONS
    STRESS_NESTED_CONDITIONS ""
    STRESS_NESTED_CONDITIONS "param1", "param2", "param3"
    
    ; Test shift with control flow
    STRESS_SHIFT_CONTROL "first", "second", "third", "fourth", "fifth", "sixth"
    
    ; Test unique identifier generation
    STRESS_UNIQUE_IDS
    STRESS_UNIQUE_WITH_PARAMS "test", "label"
    
    ; Test directive parameters
    STRESS_DIRECTIVE_PARAMS 0x1000, 4, 10, 0x01, 0x02, 0x03, 0x1234, 0x5678, 0x12345678, 0x87654321, "ascii_string", "asciz_string"
    
    ; Test conditional assembly
    STRESS_CONDITIONAL_ASSEMBLY 5, 3, "extra_param"
    STRESS_CONDITIONAL_ASSEMBLY 2, 7, ""
    STRESS_CONDITIONAL_ASSEMBLY 10, 10, "equal_case"
    
    ; Edge case: macro with maximum parameter references
    .macro MAX_REF_TEST
        .asciz "@1@2@3@4@5@6@7@8@9@10@11@12@13@14@15@16@17@18@19@20@21@22@23@24@25@26@27@28@29@30@31@32@33@34@35@36@37@38@39@40@41@42@43@44@45@46@47@48@49@50"
    .endmacro
    
    MAX_REF_TEST "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0"
    
    ; Edge case: empty parameter list with complex processing
    .macro EMPTY_COMPLEX
        .asciz "Parameter count: @#"
        .asciz "All parameters: '@*'"
        .asciz "Except first: '@-'"
        .if @# > 0
            .asciz "Should not appear"
        .else
            .asciz "No parameters confirmed"
        .endif
    .endmacro
    
    EMPTY_COMPLEX

; ============================================================================
; COMMENTED OUT EXTREME ERROR CASES (uncomment to test error handling)
; ============================================================================

; Potential infinite recursion test:
; .macro INFINITE_A
;     INFINITE_B @*
; .endmacro
; .macro INFINITE_B  
;     INFINITE_A @*
; .endmacro
; INFINITE_A "test"

; Massive parameter overflow:
; .macro OVERFLOW_TEST
;     .asciz "@999999999"
; .endmacro
; OVERFLOW_TEST

; Nested macro definitions (should be error):
; .macro OUTER_NESTED
;     .macro INNER_NESTED
;         .asciz "This should be an error"
;     .endmacro
; .endmacro
