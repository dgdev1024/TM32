; Fuzzy Testing and "Screaming at the Code"
; Tests: unusual but valid combinations, stress scenarios

.text

; Define a ridiculous number of constants and variables
.const CONST1 = 1
.const CONST2 = 2
.const CONST3 = 3
.const CONST4 = 4
.const CONST5 = 5
.let VAR1 = 10
.let VAR2 = 20
.let VAR3 = 30
.let VAR4 = 40
.let VAR5 = 50

; Test macro within macro within macro (meta-programming madness)
.macro GENERATOR_MACRO base_name, count
    .for gen_i = 1 to @count
        .macro @base_name@gen_i val
            LD A, @val
            LD B, (@val + @gen_i)
        .endmacro
    .endfor
.endmacro

GENERATOR_MACRO TEST_GEN, 3

; Use the generated macros
TEST_GEN1 100
TEST_GEN2 200
TEST_GEN3 300

; Test insane nesting of everything possible
.repeat 3, outer_repeat
    .for inner_for = 1 to 2
        .while (outer_repeat + inner_for) < 5
            .if (outer_repeat % 2) == 0
                .repeat inner_for, deep_repeat
                    .macro INSANE_MACRO
                        LD C, (outer_repeat * 1000 + inner_for * 100 + deep_repeat)
                    .endmacro
                    INSANE_MACRO
                .endrepeat
            .else
                .for crazy_for = outer_repeat to (outer_repeat + 1)
                    LD D, (crazy_for * 10000 + inner_for * 1000)
                .endfor
            .endif
            .let TEMP_VAR = outer_repeat + inner_for
            .let TEMP_VAR = TEMP_VAR + 1
        .endwhile
    .endfor
.endrepeat

; Test text substitution in weird places
.define WEIRD_REG A
.define WEIRD_OP LD
.define WEIRD_VAL 0x12345678

WEIRD_OP WEIRD_REG, WEIRD_VAL

; Test macro that generates other macros
.macro MACRO_FACTORY name, operation
    .macro @name reg, val
        @operation @reg, @val
        @operation @reg, (@val + 1)
    .endmacro
.endmacro

MACRO_FACTORY DOUBLE_LOAD, LD
DOUBLE_LOAD B, 0x1000

; Test extremely complex expression evaluation
.let MADNESS = ((((1 + 2) * 3) + 4) * 5) + ((6 * 7) + (8 + 9))
.let MORE_MADNESS = (MADNESS & 0xFF00) | ((MADNESS >> 8) & 0x00FF)

LD AL, MADNESS
LD AH, MORE_MADNESS

; Test conditional compilation with computed constants
.const MAGIC_NUMBER = (0xDEAD << 16) | 0xBEEF
.if MAGIC_NUMBER == 0xDEADBEEF
    .define MAGIC_VALID TRUE
.else
    .define MAGIC_VALID FALSE
.endif

.if MAGIC_VALID
    LD BL, 0xFF
.else
    LD BL, 0x00
.endif

; Test abuse of parameter shifting
.macro SHIFT_ABUSE p1, p2, p3, p4, p5
    LD BH, @1
    .shift 1
    LD CH, @1  ; Now p2
    .shift 1
    LD CL, @1  ; Now p3
    .shift 1
    LD DH, @1  ; Now p4
    .shift 1
    LD DL, @1  ; Now p5
.endmacro

SHIFT_ABUSE 1, 2, 3, 4, 5

; Test recursive-like macro definitions (not truly recursive, but pattern-like)
.macro PATTERN_MACRO level
    .if @level > 0
        LD A, @level
        .let NEXT_LEVEL = @level - 1
        .if NEXT_LEVEL > 0
            .macro PATTERN_INNER
                LD B, NEXT_LEVEL
            .endmacro
            PATTERN_INNER
        .endif
    .endif
.endmacro

PATTERN_MACRO 3

; Test mixing all directive types in one construct
.for mix_i = 1 to 2
    .repeat mix_i, mix_j
        .if (mix_i + mix_j) > 2
            .macro MIX_MACRO
                .let MIX_VAR = mix_i * mix_j
                LD A, MIX_VAR
                .if MIX_VAR > 1
                    .warn "Mix variable is greater than 1"
                .endif
            .endmacro
            MIX_MACRO
        .else
            .while mix_j < 3
                LD B, mix_j
                .let mix_j = mix_j + 1
            .endwhile
        .endif
    .endrepeat
.endfor

; Test boundary pushing with large numbers
.const HUGE_NUMBER = 0x7FFFFFFF
.const TINY_NUMBER = 0x00000001
.let CALCULATED_HUGE = HUGE_NUMBER - 1000000
.let CALCULATED_TINY = TINY_NUMBER + 1000000

LD C, CALCULATED_HUGE
LD D, CALCULATED_TINY

; Test text substitution creating other directives (mind-bending)
.define LET_DIRECTIVE .let
.define CONST_DIRECTIVE .const

LET_DIRECTIVE DYNAMIC_VAR = 42
CONST_DIRECTIVE DYNAMIC_CONST = 84

; Test final sanity check
.assert DYNAMIC_VAR == 42, "Dynamic variable creation failed"
.assert DYNAMIC_CONST == 84, "Dynamic constant creation failed"

HALT

; Comments explaining the madness:
; This file intentionally pushes the preprocessor to its limits
; It tests deeply nested constructs, complex macro generation,
; unusual but syntactically valid combinations, and edge cases
; that might expose bugs in the implementation
