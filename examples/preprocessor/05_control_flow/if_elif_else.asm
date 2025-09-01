; Test .if/.elif/.else/.endif conditional assembly
; This file tests multiple branch conditional compilation

.const PLATFORM = 2
.const FEATURE_A = 1
.const FEATURE_B = 0

; Test 1: if/elif/else chain with numeric conditions
; Note: Comparison operators (==, !=, etc.) are not yet implemented
; Using direct numeric evaluation instead

.if 0  ; Simulate PLATFORM == 1 (false)
    ; Windows platform code (should NOT be included)
    ld a, 0x01
    call win32_init
.elif PLATFORM  ; Simulate PLATFORM == 2 (true, since PLATFORM = 2)
    ; Linux platform code (should be included)
    ld a, 0x02
    call linux_init
.elif 0  ; Simulate PLATFORM == 3 (false)
    ; macOS platform code (should NOT be included)
    ld a, 0x03
    call macos_init
.else
    ; Unknown platform code (should NOT be included)
    ld a, 0x00
    call generic_init
.endif

; Test 2: Nested conditionals
.if FEATURE_A
    ; Feature A is enabled (should be included)
    ld b, 0x10
    
    .if FEATURE_B
        ; Feature B also enabled (should NOT be included)
        or b, 0x20
        call feature_b_init
    .else
        ; Feature B not enabled (should be included)
        and b, 0xDF
        call feature_b_disable
    .endif
    
    call feature_a_init
.endif

; Test 3: Expression-based conditions with variables
.let score = 85
.if score  ; Non-zero is true (should be included)
    ; Using score directly as condition
    ld c, 'B'  ; Good score
.endif

; Final instruction
halt
