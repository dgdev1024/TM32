; Test file for .shift directive functionality
; Tests parameter shifting within macro expansions

; Variadic macro that processes all parameters
.macro PRINT_ALL
    .repeat @#
        .asciz "@1"
        .shift 1
    .endrep
.endmacro

; Macro that skips the first parameter
.macro SKIP_FIRST
    .shift 1
    .asciz "Remaining params: @*"
.endmacro

; Macro that processes pairs of parameters
.macro PROCESS_PAIRS
    .while @# >= 2
        ; Process pair @1, @2
        ld @1, @2
        .shift 2
    .endwhile
    ; Handle remaining parameter if any
    .if @# == 1
        nop  ; Single remaining parameter: @1
    .endif
.endmacro

; Macro demonstrating multiple shifts
.macro FLEXIBLE_MACRO
    .asciz "Original params: @*"
    .shift 2
    .asciz "After shift 2: @*"
    .shift 1
    .asciz "After shift 1 more: @*"
.endmacro

.text
    ; Test print all macro
    PRINT_ALL "Hello", "World", "Test", "Macro"
    
    ; Test skip first
    SKIP_FIRST "ignore", "keep", "these", "params"
    
    ; Test process pairs
    PROCESS_PAIRS a, 0x100, b, 0x200, c, 0x300, d
    
    ; Test flexible macro
    FLEXIBLE_MACRO "one", "two", "three", "four", "five"
