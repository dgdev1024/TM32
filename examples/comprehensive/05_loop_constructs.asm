; Loop Constructs Test
; Tests: .repeat, .while, .for, .continue, .break

.text

.let LOOP_COUNTER = 0
.let ACCUMULATOR = 0

main:
    ; Test basic repeat loop
    .repeat 5, iteration
        LD A, iteration
        ADD A, 0x1000
        .let ACCUMULATOR = ACCUMULATOR + iteration
    .endrepeat

    ; Test while loop with condition
    .let LOOP_COUNTER = 0
    .while LOOP_COUNTER < 8
        LD B, LOOP_COUNTER
        SLA B  ; Shift left (multiply by 2)
        .let LOOP_COUNTER = LOOP_COUNTER + 1
    .endwhile

    ; Test for loop with default step
    .for i = 0 to 4
        LD C, i
        OR C, 0x2000
    .endfor

    ; Test for loop with custom step
    .for j = 10 to 2 step -2
        LD D, j
        AND D, 0x00FF
    .endfor

    ; Test nested loops
    .for outer = 1 to 3
        .for inner = 1 to 2
            .let TEMP_VALUE = (outer * 10) + inner
            LD AL, TEMP_VALUE
        .endfor
    .endfor

    ; Test loop with continue and break
    .let SKIP_COUNTER = 0
    .repeat 10, iter
        .if iter == 3
            .continue  ; Skip iteration 3
        .endif
        
        .if iter == 7
            .break     ; Exit loop at iteration 7
        .endif
        
        LD AH, iter
        .let SKIP_COUNTER = SKIP_COUNTER + 1
    .endrepeat

    ; Test complex nested loop with conditions
    .for x = 0 to 3
        .for y = 0 to 3
            .let PRODUCT = x * y
            .if PRODUCT > 4
                .continue
            .endif
            
            .if (x == 2) && (y == 2)
                .break
            .endif
            
            LD BL, PRODUCT
        .endfor
    .endfor

    ; Test while loop with complex condition
    .let A_VAL = 1
    .let B_VAL = 1
    .while (A_VAL < 16) && (B_VAL < 32)
        LD CH, A_VAL
        LD CL, B_VAL
        .let A_VAL = A_VAL * 2
        .let B_VAL = B_VAL + 3
    .endwhile

    ; Verify final accumulator value
    LD A, ACCUMULATOR  ; Should be 1+2+3+4+5 = 15
    LD B, SKIP_COUNTER ; Should be 6 (skipped 3, broke at 7)

    HALT
