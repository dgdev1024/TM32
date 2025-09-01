; Test file for edge cases and error conditions in macro expansion
; Tests various error scenarios and edge cases for comprehensive fuzzy testing

; ============================================================================
; CIRCULAR DEPENDENCY TESTS
; ============================================================================

; Test circular dependency (should cause error)
.define CIRCULAR_A "CIRCULAR_B"
.define CIRCULAR_B "CIRCULAR_A"

; Test indirect circular dependency
.define INDIRECT_A "INDIRECT_B"
.define INDIRECT_B "INDIRECT_C"
.define INDIRECT_C "INDIRECT_A"

; Test self-referential define
.define SELF_REF "SELF_REF plus more"

; ============================================================================
; PARAMETER COUNT EDGE CASES
; ============================================================================

; Test macro with no parameters but called with parameters
.macro NO_PARAMS
    nop
    .asciz "No params macro: @#"
.endmacro

; Test macro expecting many parameters
.macro MANY_PARAMS
    .asciz "@1, @2, @3, @4, @5, @6, @7, @8, @9, @10"
.endmacro

; Test macro with maximum parameter usage
.macro MAX_PARAMS
    .asciz "All: @*"
    .asciz "Count: @#"
    .asciz "Except first: @-"
    .asciz "Name: @0"
    .asciz "Unique: @?"
.endmacro

; ============================================================================
; PARAMETER SUBSTITUTION EDGE CASES
; ============================================================================

; Test accessing non-existent parameters
.macro OUT_OF_BOUNDS
    .asciz "P1: @1"
    .asciz "P99: @99"  ; Should be empty or error
    .asciz "P0: @0"    ; Should be macro name
.endmacro

; Test parameter substitution in various contexts
.macro PARAM_CONTEXTS
    label_@1:              ; Parameter in label
    .@1                   ; Parameter as directive
    mov @1, @2            ; Parameter as operands
    .asciz "@1@2@3"       ; Parameters concatenated
    .db @1, @2, @3        ; Parameters in data
    .if @1 == @2          ; Parameters in expressions
        nop
    .endif
.endmacro

; Test empty parameters
.macro EMPTY_PARAMS
    .asciz "First: '@1'"
    .asciz "Second: '@2'"
    .asciz "All: '@*'"
.endmacro

; ============================================================================
; SHIFT DIRECTIVE EDGE CASES
; ============================================================================

; Test multiple shifts
.macro MULTIPLE_SHIFTS
    .asciz "Start: @*"
    .shift 1
    .asciz "After 1: @*"
    .shift 2
    .asciz "After 2 more: @*"
    .shift 100  ; Shift more than available
    .asciz "After big shift: @*"
.endmacro

; Test shift with parameter count checks
.macro SHIFT_WITH_CHECKS
    .while @# > 0
        .asciz "Processing: @1"
        .shift 1
    .endwhile
    .asciz "Done processing"
.endmacro

; Test shift in nested contexts
.macro NESTED_SHIFT
    .repeat 3
        .if @# > 0
            .asciz "@1"
            .shift 1
        .endif
    .endrep
.endmacro

; ============================================================================
; MACRO NESTING AND RECURSION
; ============================================================================

; Test macro calling another macro
.macro CALLER
    .asciz "Calling CALLEE"
    CALLEE @1, @2
.endmacro

.macro CALLEE
    .asciz "In CALLEE: @1, @2"
.endmacro

; Test potential recursion (should be prevented)
.macro RECURSIVE
    .asciz "Recursive call with @1"
    .if @1 > 0
        ; RECURSIVE @1 - 1  ; This would cause infinite recursion
    .endif
.endmacro

; Test nested macro definitions (should be error)
.macro OUTER
    .asciz "Outer macro"
    ; .macro INNER  ; Nested macro definition - should cause error
    ;     nop
    ; .endmacro
.endmacro

; ============================================================================
; SPECIAL CHARACTER AND SYNTAX TESTS
; ============================================================================

; Test macro with special characters in name
.macro SPECIAL_123_MACRO
    .asciz "Special macro: @1"
.endmacro

; Test macro with underscores and numbers
.macro _UNDER_SCORE_123_
    .asciz "Underscore macro: @1"
.endmacro

; Test @@ literal at symbol (if supported)
.macro LITERAL_AT
    .asciz "Literal at: @@"  ; Should produce single @
    .asciz "Parameter: @1"
.endmacro

; Test parameter tokens in strings
.macro STRING_PARAMS
    .asciz "Param @1 in string"
    .asciz "@1@2@3concatenated"
    .asciz "Mixed @1 and @@"
.endmacro

; Test parameter tokens with special formatting
.macro FORMAT_PARAMS
    .asciz "@1"
    .asciz "@01"    ; Zero-padded parameter?
    .asciz "@10"    ; Parameter 10
    .asciz "@1@"    ; Parameter followed by @
.endmacro

; ============================================================================
; MIXED TOKEN TYPE PARAMETERS
; ============================================================================

; Test parameters with different token types
.macro MIXED_TOKENS
    .asciz "String: @1"
    ld a, @2        ; Numeric parameter
    jmp @3          ; Label parameter
    .db @4          ; Data parameter
.endmacro

; Test parameters with operators and expressions
.macro EXPRESSION_PARAMS
    ld a, @1 + @2
    ld b, (@1)
    ld c, [@1 + 1]
    .if @1 > @2
        nop
    .endif
.endmacro

; ============================================================================
; WHITESPACE AND FORMATTING EDGE CASES
; ============================================================================

; Test macro with weird whitespace
.macro   WEIRD_SPACES   
    .asciz   "@1"   
    nop     ; Comment with spaces
.endmacro

; Test parameters with spaces and commas
.macro SPACE_PARAMS
    .asciz "@*"  ; Should handle spaced parameters
.endmacro

; ============================================================================
; EMPTY AND MINIMAL CASES
; ============================================================================

; Test empty macro
.macro EMPTY_MACRO
.endmacro

; Test macro with only comments
.macro COMMENT_ONLY
    ; Just a comment
    ; Another comment
.endmacro

; Test macro with single token
.macro SINGLE_TOKEN
    nop
.endmacro

; ============================================================================
; CONTROL FLOW IN MACROS
; ============================================================================

; Test conditional compilation in macros
.macro CONDITIONAL
    .if @# > 0
        .asciz "Has parameters: @*"
    .else
        .asciz "No parameters"
    .endif
.endmacro

; Test loops in macros
.macro LOOP_MACRO
    .repeat @#
        .asciz "@1"
        .shift 1
    .endrep
.endmacro

; Test nested control flow
.macro NESTED_CONTROL
    .if @# > 2
        .repeat 2
            .asciz "@1"
            .shift 1
        .endrep
    .endif
.endmacro

; ============================================================================
; REDEFINITION AND OVERWRITING
; ============================================================================

; Test macro redefinition (comment out second definition to avoid error)
.macro REDEFINE_TEST
    .asciz "First definition"
.endmacro

; Redefine the same macro (commented out to avoid preprocessor error)
; .macro REDEFINE_TEST
;     .asciz "Second definition"
; .endmacro

; Test define vs macro with same name (commented out to avoid conflict)
.define SAME_NAME_DEFINE "define version"
.macro SAME_NAME_MACRO
    .asciz "macro version"
.endmacro

; ============================================================================
; EXTREME CASES
; ============================================================================

; Test very long parameter lists
.macro LONG_PARAMS
    .asciz "@1@2@3@4@5@6@7@8@9@10@11@12@13@14@15@16@17@18@19@20"
.endmacro

; Test deeply nested parameter substitution
.macro DEEP_NESTING
    .if @# > 0
        .if @1 != ""
            .repeat 1
                .while @# > 0
                    .asciz "@1"
                    .shift 1
                .endwhile
            .endrep
        .endif
    .endif
.endmacro

; Test macro with maximum unique identifiers
.macro UNIQUE_TEST
    @?_label1:
    @?_label2:
    @?_label3:
    jmp @?_label1
.endmacro

; ============================================================================
; PARAMETER EDGE CASES WITH SPECIAL VALUES
; ============================================================================

; Test with numeric parameters
.macro NUMERIC_PARAMS
    ld a, @1
    ld b, @2
    .db @3, @4
.endmacro

; Test with quoted and unquoted strings
.macro QUOTE_PARAMS
    .asciz @1      ; Unquoted string parameter
    .asciz "@2"    ; Double quoted
    .asciz '@3'    ; Single quoted (if supported)
.endmacro

; Test with special numeric formats
.macro SPECIAL_NUMBERS
    .dw @1         ; Decimal
    .dw @2         ; Hex
    .dw @3         ; Binary (if supported)
    .dw @4         ; Octal (if supported)
.endmacro

; ============================================================================
; TEST INVOCATIONS
; ============================================================================

.text
    ; Basic functionality tests
    NO_PARAMS
    NO_PARAMS "extra", "parameters"  ; Should work, extra ignored
    
    EMPTY_MACRO
    COMMENT_ONLY
    SINGLE_TOKEN
    
    ; Special character tests
    SPECIAL_123_MACRO "test"
    _UNDER_SCORE_123_ "underscore_test"
    LITERAL_AT "param"
    
    ; Parameter tests
    OUT_OF_BOUNDS "only_param"
    EMPTY_PARAMS "", "second", ""
    MANY_PARAMS "1", "2", "3"  ; Fewer than expected
    MANY_PARAMS "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11"  ; More than expected
    
    ; Shift tests
    MULTIPLE_SHIFTS "a", "b", "c", "d", "e"
    SHIFT_WITH_CHECKS "x", "y", "z"
    NESTED_SHIFT "first", "second", "third", "fourth"
    
    ; Macro calling tests
    CALLER "param1", "param2"
    
    ; Mixed token type tests
    MIXED_TOKENS "string", 42, label_name, 0xFF
    EXPRESSION_PARAMS 10, 20
    
    ; Whitespace tests
    WEIRD_SPACES   "param"
    SPACE_PARAMS "param 1", "param 2", "param 3"
    
    ; Control flow tests
    CONDITIONAL
    CONDITIONAL "has_param"
    LOOP_MACRO "a", "b", "c"
    NESTED_CONTROL "x", "y", "z", "extra"
    
    ; Redefinition test (should use first definition)
    REDEFINE_TEST
    SAME_NAME_MACRO
    
    ; Extreme cases
    LONG_PARAMS "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21"
    DEEP_NESTING "deep1", "deep2", "deep3"
    UNIQUE_TEST
    
    ; Numeric and special format tests
    NUMERIC_PARAMS 123, 0xABCD, 0b1010, 0o777
    QUOTE_PARAMS unquoted, "double_quoted", 'single_quoted'
    SPECIAL_NUMBERS 42, 0xFF00, 0b11110000, 0o755
    
    ; Parameter context tests
    PARAM_CONTEXTS register_a, register_b, register_c
    STRING_PARAMS "test_string"
    FORMAT_PARAMS "format_test"
    
    ; Maximum parameter tests
    MAX_PARAMS "p1", "p2", "p3", "p4", "p5"
    
    ; Edge case: macro with no body
    .macro NO_BODY
    .endmacro
    NO_BODY
    
    ; Edge case: empty parameter lists
    EMPTY_PARAMS
    
    ; ========================================================================
    ; COMMENTED OUT ERROR CONDITIONS (uncomment to test error handling)
    ; ========================================================================
    
    ; Circular dependency tests:
    ; CIRCULAR_A
    ; INDIRECT_A  
    ; SELF_REF
    
    ; Undefined macro:
    ; UNDEFINED_MACRO
    
    ; Shift outside macro:
    ; .shift 1
    
    ; Invalid parameter numbers:
    ; .macro INVALID_PARAMS
    ;     .asciz "@-1"   ; Negative parameter
    ;     .asciz "@ABC"  ; Non-numeric parameter
    ; .endmacro
    
    ; Macro name conflicts:
    ; .macro .data  ; Using directive name as macro name
    ;     nop
    ; .endmacro
