; Comprehensive Macro Fundamentals Testing
; Tests: Basic .macro without parameter lists, positional parameters, special tokens

.text

; Test 1: Basic macro without parameters
.macro NOP_SEQUENCE
    NOP
    NOP
    NOP
.endmacro

; Test 2: Simple macro with positional parameters
.macro LOAD_IMMEDIATE
    LD @1, @2
.endmacro

; Test 3: Macro with multiple parameters
.macro SETUP_REGISTERS
    LD @1, @2
    LD @3, @4
    LD @5, @6
.endmacro

; Test 4: Macro using special parameter tokens
.macro DEBUG_MACRO
    ; @0 = macro name
    ; @# = number of arguments
    ; @* = all arguments
    ; @- = all arguments except first
    .assert @# >= 1, "DEBUG_MACRO requires at least 1 argument"
    LD A, @1
    .if @# > 1
        LD B, @2
    .endif
    .if @# > 2
        LD C, @3
    .endif
.endmacro

; Test 5: Macro with identifier mangling using @?
.macro LOCAL_COUNTER
    .let counter@? = @1
    .let limit@? = @2
loop@?:
    INC counter@?
    CMP counter@?, limit@?
    JMP CC, loop@?
.endmacro

; Test 6: Macro demonstrating @@ (literal @ character)
.macro AT_SYMBOL_DEMO
    ; This should output a literal @ character: @@
    LD @1, 0x40  ; ASCII code for @@ character
.endmacro

; Test 7: Macro using @* (all arguments)
.macro FORWARD_ALL
    SETUP_REGISTERS @*
.endmacro

; Test 8: Macro using @- (all except first)
.macro SKIP_FIRST
    LD @1, 0
    .if @# > 1
        SETUP_REGISTERS A, @-
    .endif
.endmacro

; Constants for testing
.const TEST_VALUE1 = 0x12345678
.const TEST_VALUE2 = 0x87654321
.const TEST_VALUE3 = 0xDEADBEEF
.const COUNTER_START = 0
.const COUNTER_LIMIT = 10

main:
    ; Test basic macro without parameters
    NOP_SEQUENCE
    
    ; Test simple macro with parameters
    LOAD_IMMEDIATE A, TEST_VALUE1
    LOAD_IMMEDIATE B, TEST_VALUE2
    
    ; Test macro with multiple parameters
    SETUP_REGISTERS A, TEST_VALUE1, B, TEST_VALUE2, C, TEST_VALUE3
    
    ; Test debug macro with different argument counts
    DEBUG_MACRO 0x11111111
    DEBUG_MACRO 0x22222222, 0x33333333
    DEBUG_MACRO 0x44444444, 0x55555555, 0x66666666
    
    ; Test local counter macro (should create unique variables)
    LOCAL_COUNTER COUNTER_START, COUNTER_LIMIT
    LOCAL_COUNTER 5, 15  ; Should create different variables
    
    ; Test @ symbol demo
    AT_SYMBOL_DEMO AL
    
    ; Test forwarding all arguments
    FORWARD_ALL A, TEST_VALUE1, B, TEST_VALUE2, C, TEST_VALUE3
    
    ; Test skipping first argument
    SKIP_FIRST 0x99999999, TEST_VALUE1, B, TEST_VALUE2
    
    RET NC
