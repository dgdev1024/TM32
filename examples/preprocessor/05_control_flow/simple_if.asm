; Test simple .if/.endif conditional assembly
; This file tests basic conditional compilation with various conditions

.const DEBUG = 1
.const RELEASE = 0
.const VERSION = 15

; Test 1: Simple true condition
.if DEBUG
    ; This code should be included
    ld a, 100
    add a, 25
.endif

; Test 2: Simple false condition  
.if RELEASE
    ; This code should NOT be included
    ld a, 200
    add a, 50
.endif

; Test 3: Expression evaluation in condition (basic)
.if VERSION  ; Non-zero is true
    ; This code should be included (15 is true)
    ld b, VERSION
    inc b
.endif

; Test 4: Complex expressions (simulated with basic conditions)
.let complex_condition = 1  ; Simulate (DEBUG && VERSION >= 15)
.if complex_condition
    ; This code should be included
    ld c, DEBUG + VERSION
    mul c, 2
.endif

; Test 5: False condition simulation
.let false_condition = 0   ; Simulate (RELEASE || VERSION < 10)
.if false_condition
    ; This code should NOT be included
    ld d, 999
.endif

; Test final instruction (should always be included)
halt
