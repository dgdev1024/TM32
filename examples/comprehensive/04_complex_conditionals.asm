; Complex Conditional Assembly
; Tests: nested .if/.elif/.else, complex conditions

.text

.let BUILD_TYPE = 1  ; 0=Debug, 1=Release, 2=Test
.let OPTIMIZATION_LEVEL = 2
.let ENABLE_LOGGING = 1
.let TARGET_PLATFORM = 1  ; 0=Simulator, 1=Hardware, 2=FPGA

.const DEBUG_BUILD = 0
.const RELEASE_BUILD = 1
.const TEST_BUILD = 2

main:
    ; Test nested conditionals with complex logic
    .if BUILD_TYPE == DEBUG_BUILD
        .if ENABLE_LOGGING
            LD A, 0xDEBUG001  ; Debug with logging
            .if OPTIMIZATION_LEVEL > 0
                LD B, 0x0001  ; Some optimization
            .else
                LD B, 0x0000  ; No optimization
            .endif
        .else
            LD A, 0xDEBUG000  ; Debug without logging
        .endif
    .elif BUILD_TYPE == RELEASE_BUILD
        .if OPTIMIZATION_LEVEL >= 2
            LD A, 0xRELEA002  ; High optimization release
            .if TARGET_PLATFORM == 1
                LD B, 0xHARDWARE
            .elif TARGET_PLATFORM == 2
                LD B, 0xFPGA0000
            .else
                LD B, 0xSIMULATR
            .endif
        .else
            LD A, 0xRELEA001  ; Low optimization release
        .endif
    .elif BUILD_TYPE == TEST_BUILD
        LD A, 0xTEST0001
        .if ENABLE_LOGGING && (OPTIMIZATION_LEVEL < 2)
            LD B, 0xLOGTEST1
        .else
            LD B, 0xNOLOGTST
        .endif
    .else
        .error "Invalid BUILD_TYPE specified"
    .endif

    ; Test conditional with logical operators
    .if (BUILD_TYPE == RELEASE_BUILD) && (OPTIMIZATION_LEVEL > 1)
        LD C, 0xOPTIMIZE
    .elif (BUILD_TYPE == DEBUG_BUILD) || (BUILD_TYPE == TEST_BUILD)
        LD C, 0xDEBUGTST
    .else
        LD C, 0xDEFAULT0
    .endif

    ; Test deeply nested conditions
    .if BUILD_TYPE < 3
        .if TARGET_PLATFORM < 3
            .if OPTIMIZATION_LEVEL < 4
                .if ENABLE_LOGGING
                    LD D, 0x11111111
                .else
                    LD D, 0x22222222
                .endif
            .else
                LD D, 0x33333333
            .endif
        .else
            LD D, 0x44444444
        .endif
    .else
        LD D, 0x55555555
    .endif

    ; Test condition with arithmetic expressions
    .let CALCULATED_VALUE = (BUILD_TYPE * 10) + OPTIMIZATION_LEVEL
    .if CALCULATED_VALUE > 15
        LD AL, 0xFF
    .elif CALCULATED_VALUE > 10
        LD AL, 0x80
    .else
        LD AL, 0x00
    .endif

    HALT
