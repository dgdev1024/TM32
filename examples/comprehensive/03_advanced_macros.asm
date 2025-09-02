; Advanced Macro Testing
; Tests: Complex macro definitions, nested calls, parameter manipulation

.text

; Basic macro without parameter list - correct TM32 syntax
.macro SET_REGISTER
    LD @1, @2
.endmacro

; Macro with conditional logic based on parameter count
.macro CONDITIONAL_LOAD
    .if @# == 1
        LD A, @1
    .elif @# == 2
        LD @1, @2
    .else
        .error "CONDITIONAL_LOAD: Invalid number of parameters"
    .endif
.endmacro

; Macro demonstrating parameter shifting
.macro MULTI_LOAD
    .if @# > 0
        LD @1, @2
        .shift 2
        .if @# > 0
            MULTI_LOAD @*
        .endif
    .endif
.endmacro

; Macro with local variable generation using @?
.macro CREATE_LOOP
    .let counter@? = @1
    .let limit@? = @2
loop@?:
    INC counter@?
    CMP counter@?, limit@?
    JMP CC, loop@?
.endmacro

; Complex macro with arithmetic and validation
.macro SAFE_ARRAY_ACCESS
    .assert @# == 3, "SAFE_ARRAY_ACCESS requires 3 parameters: base_addr, index, element_size"
    .let base = @1
    .let index = @2
    .let size = @3
    .assert size > 0, "Element size must be positive"
    .let offset = index * size
    LD A, [base + offset]
.endmacro

; Macro with complex parameter manipulation
.macro SETUP_ABC
    .assert @# >= 3, "SETUP_ABC requires at least 3 parameters"
    SET_REGISTER A, @1
    SET_REGISTER B, @2
    SET_REGISTER C, @3
    ; Handle optional fourth parameter
    .if @# > 3
        SET_REGISTER D, @4
    .endif
.endmacro

; Macro with loop construct using variables
.macro FILL_MEMORY
    .let base_addr = @1
    .let count = @2
    .let value = @3
    .let i = 0
    .while i < count
        ST [base_addr + (i * 4)], value
        .let i = i + 1
    .endwhile
.endmacro

; Macro demonstrating all parameter special tokens
.macro PARAMETER_DEMO
    ; @0 = macro name
    ; @# = number of arguments
    ; @* = all arguments
    ; @- = all arguments except first
    ; @? = unique identifier for this macro invocation
    ; @@ = literal @ character
    
    .assert @# >= 1, "PARAMETER_DEMO requires at least 1 argument"
    
    ; Use first parameter
    LD A, @1
    
    ; If more than one parameter, use the rest
    .if @# > 1
        ; Forward remaining parameters to another macro
        MULTI_LOAD @-
    .endif
    
    ; Create unique label
    done@?:
        NOP
.endmacro

; Constants for testing
.const TEST_BASE_ADDR = 0x80000000

main:
    ; Test simple macro
    SET_REGISTER A, 0x12345678
    SET_REGISTER B, 0x87654321
    
    ; Test conditional loading
    CONDITIONAL_LOAD 0x11111111
    CONDITIONAL_LOAD B, 0x22222222
    
    ; Test multi-load with parameter shifting
    MULTI_LOAD A, 0x1000, B, 0x2000, C, 0x3000
    
    ; Test loop creation
    CREATE_LOOP 0, 10
    CREATE_LOOP 5, 15  ; Should create different variables
    
    ; Test safe array access
    SAFE_ARRAY_ACCESS TEST_BASE_ADDR, 5, 4
    
    ; Test setup with different parameter counts
    SETUP_ABC 0x1111, 0x2222, 0x3333
    SETUP_ABC 0x4444, 0x5555, 0x6666, 0x7777
    
    ; Test memory filling
    FILL_MEMORY TEST_BASE_ADDR, 10, 0xDEADBEEF
    
    ; Test parameter demonstration
    PARAMETER_DEMO 0x1234
    PARAMETER_DEMO 0x5678, A, 0x9ABC, B, 0xDEF0
    
    HALT
