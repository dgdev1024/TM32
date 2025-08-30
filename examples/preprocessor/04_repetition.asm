;
; @file     examples/preprocessor/04_repetition.asm
; @brief    Contains code which demonstrates the TM32 Assembler Tool's
;           Preprocessor Component's repetition directives: `.repeat`,
;           `.while` and `.for`.
;

; Run this file with:
;   `tm32asm --preprocess-only examples/preprocessor/04_repetition.asm`

.text
main:
    ; Example 1: Basic .repeat directive without iteration variable
    .repeat 3
        nop
        inc a
    .endrepeat
    
    ; Example 2: .repeat with iteration variable
    .repeat 4, i
        ld al, i        ; Load iteration count into AL
        add a, 10       ; Add 10 to A
    .endrepeat
    
    ; Example 3: .while loop with condition
    .let counter = 0
    .while counter < 3
        ld al, counter
        inc al
        .let counter = counter + 1
    .endwhile
    
    ; Example 4: .for loop with default step (1)
    .for j = 1 to 3
        ld ah, j
        sla ah          ; Shift left arithmetically
    .endfor
    
    ; Example 5: .for loop with custom step
    .for k = 10 to 4 step -2
        ld bl, k
        sub bl, 1
    .endfor
    
    ; Example 6: Nested loops demonstration
    .for outer = 1 to 2
        .for inner = 1 to 3
            ld ch, outer
            ld cl, inner
            add ch, cl
        .endfor
    .endfor
    
    ; Example 7: Using .continue in a loop
    .for m = 1 to 5
        .if m == 3
            .continue   ; Skip when m equals 3
        .endif
        ld dh, m
        inc dh
    .endfor
    
    ; Example 8: Using .break in a loop
    .let n = 0
    .while n < 10
        .if n == 4
            .break      ; Exit loop when n equals 4
        .endif
        ld dl, n
        .let n = n + 1
    .endwhile

done:
    jpb nc, done

.rodata
    ; Example 9: Using repetition to generate data tables
    lookup_table:
        .repeat 8, idx
            .db idx * 2     ; Generate table: 0, 2, 4, 6, 8, 10, 12, 14
        .endrepeat
    
    ; Example 10: Generate string data with loops
    .for letter = 65 to 68  ; ASCII 'A' to 'D'
        .db letter
    .endfor
    .db 0   ; Null terminator

;
; The preprocessor should resolve the above code to the following:
;
; ```
;   .text
;   main:
;       ; Example 1: Basic .repeat directive without iteration variable
;       nop
;       inc a
;       nop
;       inc a
;       nop
;       inc a
;       
;       ; Example 2: .repeat with iteration variable
;       ld al, 0        ; Load iteration count into AL
;       add a, 10       ; Add 10 to A
;       ld al, 1        ; Load iteration count into AL
;       add a, 10       ; Add 10 to A
;       ld al, 2        ; Load iteration count into AL
;       add a, 10       ; Add 10 to A
;       ld al, 3        ; Load iteration count into AL
;       add a, 10       ; Add 10 to A
;       
;       ; Example 3: .while loop with condition
;       ld al, 0
;       inc al
;       ld al, 1
;       inc al
;       ld al, 2
;       inc al
;       
;       ; Example 4: .for loop with default step (1)
;       ld ah, 1
;       sla ah          ; Shift left arithmetically
;       ld ah, 2
;       sla ah          ; Shift left arithmetically
;       ld ah, 3
;       sla ah          ; Shift left arithmetically
;       
;       ; Example 5: .for loop with custom step
;       ld bl, 10
;       sub bl, 1
;       ld bl, 8
;       sub bl, 1
;       ld bl, 6
;       sub bl, 1
;       ld bl, 4
;       sub bl, 1
;       
;       ; Example 6: Nested loops demonstration
;       ld ch, 1
;       ld cl, 1
;       add ch, cl
;       ld ch, 1
;       ld cl, 2
;       add ch, cl
;       ld ch, 1
;       ld cl, 3
;       add ch, cl
;       ld ch, 2
;       ld cl, 1
;       add ch, cl
;       ld ch, 2
;       ld cl, 2
;       add ch, cl
;       ld ch, 2
;       ld cl, 3
;       add ch, cl
;       
;       ; Example 7: Using .continue in a loop
;       ld dh, 1
;       inc dh
;       ld dh, 2
;       inc dh
;       ld dh, 4
;       inc dh
;       ld dh, 5
;       inc dh
;       
;       ; Example 8: Using .break in a loop
;       ld dl, 0
;       ld dl, 1
;       ld dl, 2
;       ld dl, 3
;   
;   done:
;       jpb nc, done
;   
;   .rodata
;       ; Example 9: Using repetition to generate data tables
;       lookup_table:
;           .db 0       ; Generate table: 0, 2, 4, 6, 8, 10, 12, 14
;           .db 2       ; Generate table: 0, 2, 4, 6, 8, 10, 12, 14
;           .db 4       ; Generate table: 0, 2, 4, 6, 8, 10, 12, 14
;           .db 6       ; Generate table: 0, 2, 4, 6, 8, 10, 12, 14
;           .db 8       ; Generate table: 0, 2, 4, 6, 8, 10, 12, 14
;           .db 10      ; Generate table: 0, 2, 4, 6, 8, 10, 12, 14
;           .db 12      ; Generate table: 0, 2, 4, 6, 8, 10, 12, 14
;           .db 14      ; Generate table: 0, 2, 4, 6, 8, 10, 12, 14
;       
;       ; Example 10: Generate string data with loops
;       .db 65
;       .db 66
;       .db 67
;       .db 68
;       .db 0   ; Null terminator
; ```
;
