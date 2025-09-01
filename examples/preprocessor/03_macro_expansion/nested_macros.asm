; Test file for nested macro expansions
; Tests macros calling other macros

; Base utility macros
.define WORD_SIZE "2"
.define BYTE_SIZE "1"

.macro LOAD_WORD
    ld @1, [@2]
    ld @1 + 1, [@2 + WORD_SIZE]
.endmacro

.macro STORE_WORD
    st [@2], @1
    st [@2 + WORD_SIZE], @1 + 1
.endmacro

; Higher-level macro that uses other macros
.macro COPY_WORD
    LOAD_WORD temp_reg, @1
    STORE_WORD temp_reg, @2
.endmacro

; Macro that defines other macros
.macro DEFINE_ACCESSOR
    .macro GET_@1
        LOAD_WORD @2, @1_address
    .endmacro
    
    .macro SET_@1
        STORE_WORD @2, @1_address
    .endmacro
.endmacro

; Recursive-style macro (limited depth)
.macro REPEAT_LD
    ld @1, @2
    .if @# > 2
        .shift 2
        REPEAT_LD @*
    .endif
.endmacro

.data
    temp_reg: .dw 0
    source_addr: .dw 0x1000
    dest_addr: .dw 0x2000
    
.text
    ; Test nested macro calls
    COPY_WORD source_addr, dest_addr
    
    ; Test macro-generated macros
    DEFINE_ACCESSOR STATUS, a
    GET_STATUS a
    SET_STATUS a
    
    ; Test recursive-style macro
    REPEAT_LD a, 0x100, b, 0x200, c, 0x300
