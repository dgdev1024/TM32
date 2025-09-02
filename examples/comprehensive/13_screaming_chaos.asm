; "Screaming at the Code" - Aggressive Testing
; Tests: Intentional stress, boundary violations, error recovery

.text

; Aggressive constants for boundary testing
.const SCREAM_MAX = 0xFFFFFFFF
.const SCREAM_MIN = 0x00000000
.const SCREAM_HALF = 0x7FFFFFFF
.const SCREAM_NEG = 0x80000000
.const SCREAM_PATTERN_1 = 0xAAAAAAAA
.const SCREAM_PATTERN_2 = 0x55555555

; Test 1: AGGRESSIVE PARAMETER OVERLOAD
.macro SCREAM_PARAMS
    ; THROW EVERYTHING AT IT!
    .if @# >= 1; LD A, @1; .endif
    .if @# >= 2; LD B, @2; .endif
    .if @# >= 3; LD C, @3; .endif
    .if @# >= 4; LD D, @4; .endif
    .if @# >= 5; LD AW, @5; .endif
    .if @# >= 6; LD BW, @6; .endif
    .if @# >= 7; LD CW, @7; .endif
    .if @# >= 8; LD DW, @8; .endif
    .if @# >= 9; LD AH, @9; .endif
    .if @# >= 10; LD AL, @10; .endif
    .if @# >= 11; LD BH, @11; .endif
    .if @# >= 12; LD BL, @12; .endif
    .if @# >= 13; LD CH, @13; .endif
    .if @# >= 14; LD CL, @14; .endif
    .if @# >= 15; LD DH, @15; .endif
    .if @# >= 16; LD DL, @16; .endif
    
    ; EVEN MORE CHAOS!
    .let total_chaos = 0
    .let param_count = @#
    .while param_count > 0
        .let total_chaos = total_chaos + 1
        .let param_count = param_count - 1
    .endwhile
    
    .if total_chaos > 10
        NOP  ; Survived the chaos!
    .endif
.endmacro

; Test 2: MATHEMATICAL MAYHEM
.macro MATH_MAYHEM
    .let x = @1
    .let y = @2
    
    ; EXTREME CALCULATIONS!
    .let insane_calc = ((x * y) + (x / (y + 1))) - ((x % (y + 1)) * 2)
    
    ; TRIGONOMETRIC TORTURE!
    .let angle1 = x % 360
    .let angle2 = y % 360
    .let sin_x = sin(angle1)
    .let cos_x = cos(angle1)
    .let sin_y = sin(angle2)
    .let cos_y = cos(angle2)
    
    ; COMBINE EVERYTHING!
    .let trig_chaos = atan2(sin_x + sin_y, cos_x + cos_y)
    
    ; LOGARITHMIC LUNACY!
    .let log_madness = log2(abs(insane_calc) + 1) + log2(abs(trig_chaos) + 1)
    
    ; SQUARE ROOT SAVAGERY!
    .let sqrt_chaos = sqrt(abs(log_madness))
    
    ; FINAL FURY!
    .let final_fury = floor(sqrt_chaos) + ceil(sqrt_chaos) + round(sqrt_chaos)
    
    LD A, final_fury
.endmacro

; Test 3: CONDITIONAL CARNAGE
.macro CONDITIONAL_CARNAGE
    .let val = @1
    .let chaos = @2
    
    ; NESTED NIGHTMARE!
    .if val > SCREAM_HALF
        .if chaos & 0x1
            .if val & 0x2
                .if chaos & 0x4
                    .if val & 0x8
                        .if chaos & 0x10
                            .if val & 0x20
                                .if chaos & 0x40
                                    .if val & 0x80
                                        LD A, 0xDEADBEEF
                                    .else
                                        LD A, 0xCAFEBABE
                                    .endif
                                .else
                                    LD A, 0xFEEDFACE
                                .endif
                            .else
                                LD A, 0xDEADFEED
                            .endif
                        .else
                            LD A, 0xBEEFCAFE
                        .endif
                    .else
                        LD A, 0xFACEFEED
                    .endif
                .else
                    LD A, 0x12345678
                .endif
            .else
                LD A, 0x87654321
            .endif
        .else
            LD A, 0x13579BDF
        .endif
    .else
        ; ALTERNATIVE CHAOS PATH!
        .if (val ^ chaos) > 0x40000000
            LD B, val + chaos
        .elif (val | chaos) < 0x10000000
            LD B, val - chaos
        .else
            LD B, val * 2
        .endif
    .endif
.endmacro

; Test 4: LOOP LUNACY
.macro LOOP_LUNACY
    .let iterations = @1
    .assert iterations <= 50, "LOOP_LUNACY: Too many iterations!"
    
    .let i = 0
    .let accumulator = 1
    .let multiplier = 1
    
    .while i < iterations
        ; CHAOTIC ACCUMULATION!
        .if (i % 5) == 0
            .let accumulator = accumulator * multiplier
            .let multiplier = multiplier + 1
        .elif (i % 5) == 1
            .let accumulator = accumulator + (i * i)
        .elif (i % 5) == 2
            .let accumulator = accumulator - (i / 2)
        .elif (i % 5) == 3
            .let accumulator = accumulator ^ (i << 1)
        .else
            .let accumulator = accumulator & (0xFFFFFFFF >> i)
        .endif
        
        ; OVERFLOW PROTECTION (kind of...)
        .if accumulator > 0x7FFFFFFF
            .let accumulator = accumulator % 0x7FFFFFFF
        .endif
        
        .let i = i + 1
        
        ; SAFETY VALVE (we're not completely insane)
        .if i > 100
            .error "LOOP_LUNACY: Safety limit exceeded!"
        .endif
    .endwhile
    
    LD A, accumulator
.endmacro

; Test 5: STRING SLAUGHTER
.macro STRING_SLAUGHTER
    .let reg_name = @1
    .let operation = @2
    
    ; ALL THE REGISTER CHECKS! NO MERCY!
    .let reg_size = 0
    
    ; 8-bit register carnage
    .if strcmp(reg_name, "AH") == 0; .let reg_size = 8; .endif
    .if strcmp(reg_name, "AL") == 0; .let reg_size = 8; .endif
    .if strcmp(reg_name, "BH") == 0; .let reg_size = 8; .endif
    .if strcmp(reg_name, "BL") == 0; .let reg_size = 8; .endif
    .if strcmp(reg_name, "CH") == 0; .let reg_size = 8; .endif
    .if strcmp(reg_name, "CL") == 0; .let reg_size = 8; .endif
    .if strcmp(reg_name, "DH") == 0; .let reg_size = 8; .endif
    .if strcmp(reg_name, "DL") == 0; .let reg_size = 8; .endif
    
    ; 16-bit register rampage
    .if strcmp(reg_name, "AW") == 0; .let reg_size = 16; .endif
    .if strcmp(reg_name, "BW") == 0; .let reg_size = 16; .endif
    .if strcmp(reg_name, "CW") == 0; .let reg_size = 16; .endif
    .if strcmp(reg_name, "DW") == 0; .let reg_size = 16; .endif
    
    ; 32-bit register mayhem
    .if strcmp(reg_name, "A") == 0; .let reg_size = 32; .endif
    .if strcmp(reg_name, "B") == 0; .let reg_size = 32; .endif
    .if strcmp(reg_name, "C") == 0; .let reg_size = 32; .endif
    .if strcmp(reg_name, "D") == 0; .let reg_size = 32; .endif
    
    ; OPERATION BASED ON SIZE!
    .if reg_size == 8
        .if operation > 0xFF
            .let operation = operation & 0xFF
        .endif
    .elif reg_size == 16
        .if operation > 0xFFFF
            .let operation = operation & 0xFFFF
        .endif
    .endif
    
    ; EXECUTE THE MADNESS!
    MV @1, operation
.endmacro

; Test 6: SYMBOL STORM
.macro SYMBOL_STORM
    .let base = @1
    
    ; CREATE A HURRICANE OF SYMBOLS!
    .let storm_symbol_00@? = base + 0x00000000
    .let storm_symbol_01@? = base + 0x11111111
    .let storm_symbol_02@? = base + 0x22222222
    .let storm_symbol_03@? = base + 0x33333333
    .let storm_symbol_04@? = base + 0x44444444
    .let storm_symbol_05@? = base + 0x55555555
    .let storm_symbol_06@? = base + 0x66666666
    .let storm_symbol_07@? = base + 0x77777777
    .let storm_symbol_08@? = base + 0x88888888
    .let storm_symbol_09@? = base + 0x99999999
    .let storm_symbol_10@? = base + 0xAAAAAAAA
    .let storm_symbol_11@? = base + 0xBBBBBBBB
    .let storm_symbol_12@? = base + 0xCCCCCCCC
    .let storm_symbol_13@? = base + 0xDDDDDDDD
    .let storm_symbol_14@? = base + 0xEEEEEEEE
    .let storm_symbol_15@? = base + 0xFFFFFFFF
    
    ; CROSS-POLLINATE THE CHAOS!
    .let chaos_result@? = storm_symbol_00@? ^ storm_symbol_15@?
    .let chaos_result@? = chaos_result@? + storm_symbol_07@?
    .let chaos_result@? = chaos_result@? - storm_symbol_08@?
    
    LD A, chaos_result@?
.endmacro

; Test 7: BOUNDARY BLASTER
.macro BOUNDARY_BLASTER
    .let test_val = @1
    
    ; TEST EVERY POSSIBLE BOUNDARY!
    .if test_val == SCREAM_MIN
        LD A, 0x00000001  ; Minimum + 1
    .elif test_val == SCREAM_MAX
        LD A, 0xFFFFFFFE  ; Maximum - 1
    .elif test_val == SCREAM_HALF
        LD A, 0x80000000  ; Flip the sign bit
    .elif test_val == SCREAM_NEG
        LD A, 0x7FFFFFFF  ; Maximum positive
    .else
        ; CALCULATE BOUNDARY DISTANCE!
        .let dist_from_min = test_val - SCREAM_MIN
        .let dist_from_max = SCREAM_MAX - test_val
        .let dist_from_half = abs(test_val - SCREAM_HALF)
        
        .let min_distance = min(min(dist_from_min, dist_from_max), dist_from_half)
        LD A, min_distance
    .endif
.endmacro

; Test 8: EXPRESSION EXPLOSION
.macro EXPRESSION_EXPLOSION
    .let a = @1
    .let b = @2
    .let c = @3
    
    ; THE MOST COMPLEX EXPRESSION EVER CONCEIVED!
    .let step1 = ((a + b) * c) - ((a - b) / (c + 1))
    .let step2 = sqrt(abs(step1)) + log2(abs(step1) + 1)
    .let step3 = sin(step1 % 360) + cos(step2 % 360)
    .let step4 = pow2(abs(step3) % 20) + atan2(step2, step3)
    .let step5 = floor(step4) + ceil(step4) + round(step4)
    .let step6 = max(min(step5, step1), abs(step2 - step3))
    .let step7 = (step6 << 2) | (step5 >> 3) | (step4 & 0xFF)
    .let step8 = step7 ^ SCREAM_PATTERN_1 ^ SCREAM_PATTERN_2
    .let final_explosion = abs(step8) % 0xFFFFFF
    
    LD A, final_explosion
.endmacro

main:
    ; UNLEASH THE CHAOS!
    
    ; PARAMETER OVERLOAD!
    SCREAM_PARAMS 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20
    
    ; MATHEMATICAL MAYHEM!
    MATH_MAYHEM SCREAM_MAX, SCREAM_PATTERN_1
    MATH_MAYHEM SCREAM_NEG, SCREAM_PATTERN_2
    MATH_MAYHEM 0x12345678, 0x87654321
    
    ; CONDITIONAL CARNAGE!
    CONDITIONAL_CARNAGE SCREAM_MAX, SCREAM_PATTERN_1
    CONDITIONAL_CARNAGE SCREAM_NEG, SCREAM_PATTERN_2
    CONDITIONAL_CARNAGE 0xDEADBEEF, 0xCAFEBABE
    
    ; LOOP LUNACY!
    LOOP_LUNACY 10
    LOOP_LUNACY 25
    LOOP_LUNACY 40
    
    ; STRING SLAUGHTER!
    STRING_SLAUGHTER "AH", 0xFF
    STRING_SLAUGHTER "BW", 0xFFFF
    STRING_SLAUGHTER "C", SCREAM_MAX
    STRING_SLAUGHTER "DL", 0x42
    
    ; SYMBOL STORM!
    SYMBOL_STORM SCREAM_PATTERN_1
    SYMBOL_STORM SCREAM_PATTERN_2
    SYMBOL_STORM 0x13579BDF
    
    ; BOUNDARY BLASTER!
    BOUNDARY_BLASTER SCREAM_MIN
    BOUNDARY_BLASTER SCREAM_MAX
    BOUNDARY_BLASTER SCREAM_HALF
    BOUNDARY_BLASTER SCREAM_NEG
    BOUNDARY_BLASTER 0x12345678
    
    ; EXPRESSION EXPLOSION!
    EXPRESSION_EXPLOSION SCREAM_MAX, SCREAM_PATTERN_1, SCREAM_PATTERN_2
    EXPRESSION_EXPLOSION 0xDEADBEEF, 0xCAFEBABE, 0xFEEDFACE
    EXPRESSION_EXPLOSION 1000000, 500000, 250000
    
    ; IF WE GET HERE, WE SURVIVED THE CHAOS!
    LD A, 0xDEADBEEF
    LD B, 0xCAFEBABE
    LD C, 0xFEEDFACE
    LD D, 0x5CREAMEDDD  ; "SCREAMED" in hex-speak
    
    HALT
