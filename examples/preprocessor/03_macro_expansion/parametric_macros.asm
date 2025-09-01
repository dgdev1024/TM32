; Test file for parametric macros with parameter substitution
; Tests .macro/.endmacro functionality with @1, @2, etc.

; Simple parametric macro
.macro LOAD_REG
    ld @1, @2
    nop
.endmacro

; Macro with multiple parameters
.macro SET_MEMORY
    ld a, @2
    st [@1], a
    ld a, @3
    st [@1 + 1], a
.endmacro

; Macro using special tokens
.macro DEBUG_INFO
    .asciz "Macro: @0"
    .asciz "Param count: @#"
    .asciz "First param: @1"
    .asciz "All params: @*"
    .asciz "All params, but the first: @-"
    .asciz "Unique Identifier: '@?'."
.endmacro

; Macro using @? for unique labels
.macro CALL_WITH_RETURN
    call @1
    jmp @?_skip
@?_error:
    ; Error handling code
    ret
@?_skip:
    nop
.endmacro

; Macro using positional parameter embedding to create unique block labels
.macro RESERVE_BLOCK
    .block_@1_start:
        .db @2
    .block_@1_end:
.endmacro

.text
    ; Test basic macro calls
    LOAD_REG a, 0x1000
    LOAD_REG b, 0x2000
    
    ; Test multi-parameter macro
    SET_MEMORY 0x3000, 0x12, 0x34
    
    ; Test debug macro
    DEBUG_INFO my_string, my_other_string, my_tertiary_string
    
    ; Test unique label macro
    CALL_WITH_RETURN some_function
    CALL_WITH_RETURN another_function

    ; Test block reservation macro
    RESERVE_BLOCK test, 0x42        ; Should expand to:
                                    ;   .block_test_start:
                                    ;       .db 0x42
                                    ;   .block_test_end:
                                    