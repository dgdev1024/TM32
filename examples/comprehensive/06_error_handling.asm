; Error Handling and Debug Directives
; Tests: .warn, .error, .assert, .file, .line

.text

.let DEBUG_MODE = 1
.let SAFETY_CHECKS = 1
.let MAX_BUFFER_SIZE = 512

main:
    ; Test warning directive
    .if DEBUG_MODE
        .warn "Debug mode is enabled - performance may be reduced"
    .endif

    ; Test assertions with conditions
    .assert MAX_BUFFER_SIZE > 0, "Buffer size must be positive"
    .assert MAX_BUFFER_SIZE <= 1024, "Buffer size too large"

    ; Test file and line directives for debugging
    .file "test_module.asm"
    .line 100
    LD A, MAX_BUFFER_SIZE

    .file "error_handler.asm"
    .line 50
    .if SAFETY_CHECKS
        ; Test assertion that should pass
        .let TEMP_VAL = MAX_BUFFER_SIZE / 2
        .assert TEMP_VAL > 0, "Calculated value should be positive"
        LD B, TEMP_VAL
    .endif

    ; Test conditional error (this should NOT trigger)
    .let VALID_CONFIG = 1
    .if !VALID_CONFIG
        .error "Invalid configuration detected"
    .endif

    ; Test multiple assertions
    .let ARRAY_SIZE = 64
    .let ELEMENT_SIZE = 4
    .let TOTAL_SIZE = ARRAY_SIZE * ELEMENT_SIZE

    .assert (TOTAL_SIZE % 4) == 0, "Total size must be 4-byte aligned"
    .assert TOTAL_SIZE < MAX_BUFFER_SIZE, "Array too large for buffer"

    ; Test warning with complex condition
    .if (TOTAL_SIZE > (MAX_BUFFER_SIZE / 2)) && DEBUG_MODE
        .warn "Array uses more than half the available buffer space"
    .endif

    ; Reset file/line for final instructions
    .file "05_error_handling.asm"
    .line 999
    
    LD C, TOTAL_SIZE
    LD D, 0x12345678

    HALT

; The following would cause an error if uncommented:
; .error "This is a test error message"
; .assert 1 == 0, "This assertion would fail"
