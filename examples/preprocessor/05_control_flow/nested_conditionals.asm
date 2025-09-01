; Test complex nested conditional assembly
; This file tests deeply nested if statements with basic conditions

.const TARGET_CPU = 32
.const OPTIMIZATION = 2
.const DEBUG_LEVEL = 1

; Test 1: Deep nesting with basic conditions
.if TARGET_CPU  ; Non-zero is true (should be included)
    ; 16+ bit CPU (should be included)
    ld a, 0x1000
    
    .if OPTIMIZATION
        ; Optimization enabled (should be included)
        
        .if 1  ; Simulate OPTIMIZATION >= 2 (true)
            ; High optimization (should be included)
            ; Inline critical functions
            mov b, a
            shl b, 1
            
            .if DEBUG_LEVEL
                ; Debug info with optimization (should be included)
                nop  ; Debug marker
            .endif
            
        .elif 0  ; Simulate OPTIMIZATION == 1 (false)
            ; Medium optimization (should NOT be included)
            call optimized_function
        .endif
        
    .else
        ; No optimization (should NOT be included)
        call unoptimized_function
    .endif
    
.else
    ; 8-bit CPU (should NOT be included)
    ld a, 0x10
.endif

; Test 2: Complex conditions using arithmetic (basic evaluation)
.let temp_val = TARGET_CPU  ; 32
.if temp_val  ; Non-zero is true (should be included)
    ; All conditions met (should be included)
    ld c, 0xDEAD
    ld d, 0xBEEF
.endif

; Test 3: Bitwise operations as basic values
.const FLAGS = 13  ; Binary: 1101
.let bit0 = 1      ; Simulate (FLAGS & 0b0001) != 0
.let bit1 = 1      ; Simulate (FLAGS & 0b0010) != 0  
.let bit2 = 0      ; Simulate (FLAGS & 0b0100) == 0
.let bit3 = 1      ; Simulate (FLAGS & 0b1000) != 0

.if bit0
    ; Bit 0 is set (should be included)
    set_flag_0
.endif

.if bit1
    ; Bit 1 is set (should be included)  
    set_flag_1
.endif

.if bit2
    ; Bit 2 is clear (should NOT be included)
    clear_flag_2
.endif

.if bit3
    ; Bit 3 is set (should be included)
    set_flag_3
.endif

halt
