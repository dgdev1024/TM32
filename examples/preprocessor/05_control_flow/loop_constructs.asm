; Test file for loop constructs (.repeat, .while, .for)
; This tests basic loop functionality with simple numeric conditions

.const ARRAY_SIZE = 5
.let counter = 0

; Test .repeat loop
.repeat 3
    ld a, counter
    inc a
    st a, counter
.endrepeat

; Test .repeat with iteration variable
data_array:
.repeat ARRAY_SIZE, i
    .db i * 2           ; Generate array data: 0, 2, 4, 6, 8
.endrepeat

; Test .for loop
lookup_table:
.for angle = 0 to 7
    .db angle + 10      ; Generate: 10, 11, 12, 13, 14, 15, 16, 17
.endfor

; Test .for loop with step
step_table:
.for value = 10 to 20 step 2
    .db value           ; Generate: 10, 12, 14, 16, 18, 20
.endfor

; Test .for loop with negative step
reverse_table:
.for value = 5 to 1 step -1
    .db value           ; Generate: 5, 4, 3, 2, 1
.endfor

; Simple code generation
initialization_code:
.repeat 4, reg
    ld @reg, 0          ; Will become: ld 0, 0; ld 1, 0; ld 2, 0; ld 3, 0
.endrepeat

; Nested loops
multiplication_table:
.for x = 1 to 3
    .for y = 1 to 3
        .db x * y       ; Generate 3x3 multiplication table
    .endfor
.endfor
