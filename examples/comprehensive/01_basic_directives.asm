; Basic Preprocessor Directives Test
; Tests: .let, .const, .define, basic .if/.endif

.text

; Test basic variable declarations
.let COUNTER = 0
.let BUFFER_SIZE = 256
.let MODE

; Test constant declarations
.const MAX_VALUE = 1024
.const MIN_VALUE = -512
.const PI = 0x0000000C90FDAA22  ; 3.141592... in 32.32 fixed point

; Test simple text substitution
.define GREETING Hello_World
.def STATUS_OK 0x00
.def STATUS_ERROR 0xFF

; Test basic conditional compilation
.if MAX_VALUE > 512
    .let USE_EXTENDED_MODE = 1
.else
    .let USE_EXTENDED_MODE = 0
.endif

; Test simple macro expansion in instructions
start:
    LD A, MAX_VALUE
    LD B, BUFFER_SIZE
    LD C, STATUS_OK
    
.if USE_EXTENDED_MODE
    LD D, 0x12345678
    ADD A, MIN_VALUE
.else
    LD D, 0x00000001
    SUB A, 100
.endif

    ; Test variable reassignment
    .let COUNTER = COUNTER + 1
    .let MODE = 3

loop:
    LD AL, COUNTER
    CMP AL, 10
    NOP
    
    .let COUNTER = COUNTER + 1
    
end:
    HALT
