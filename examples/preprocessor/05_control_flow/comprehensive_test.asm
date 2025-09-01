; Test comprehensive control flow with basic numeric conditions
; This tests all the implemented control flow features without comparison operators

.const DEBUG = 1
.const RELEASE = 0
.const FEATURE_ENABLED = 1
.let optimization_level = 2

; Test 1: Simple .if/.endif with true condition
.if DEBUG
    ; This should be included
    ld a, 0x10
    add a, 5
.endif

; Test 2: Simple .if/.endif with false condition
.if RELEASE
    ; This should NOT be included
    ld a, 0x20
    add a, 10
.endif

; Test 3: .if/.else with true condition
.if FEATURE_ENABLED
    ; This should be included
    ld b, 0x30
.else
    ; This should NOT be included
    ld b, 0x40
.endif

; Test 4: .if/.else with false condition
.if RELEASE
    ; This should NOT be included
    ld c, 0x50
.else
    ; This should be included
    ld c, 0x60
.endif

; Test 5: .if/.elif/.else chain - first condition true
.if DEBUG
    ; This should be included
    ld d, 0x70
.elif FEATURE_ENABLED
    ; This should NOT be included (previous condition was true)
    ld d, 0x80
.else
    ; This should NOT be included
    ld d, 0x90
.endif

; Test 6: .if/.elif/.else chain - second condition true
.if RELEASE
    ; This should NOT be included
    ld e, 0xA0
.elif FEATURE_ENABLED
    ; This should be included
    ld e, 0xB0
.else
    ; This should NOT be included
    ld e, 0xC0
.endif

; Test 7: .if/.elif/.else chain - else condition
.if RELEASE
    ; This should NOT be included
    ld f, 0xD0
.elif RELEASE
    ; This should NOT be included
    ld f, 0xE0
.else
    ; This should be included
    ld f, 0xF0
.endif

; Test 8: Nested conditionals
.if DEBUG
    ; Outer condition true - this should be included
    ld g, 0x11
    
    .if FEATURE_ENABLED
        ; Inner condition also true - this should be included
        ld h, 0x22
    .else
        ; This should NOT be included
        ld h, 0x33
    .endif
    
    ld i, 0x44  ; This should be included
.else
    ; This whole block should NOT be included
    ld g, 0x55
    ld h, 0x66
    ld i, 0x77
.endif

; Test 9: Variable as condition
.if optimization_level
    ; This should be included (optimization_level = 2, which is truthy)
    ld j, 0x88
.endif

; Always included instruction
halt
