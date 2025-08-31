; Mathematical subroutines and utility functions
; This file provides common mathematical operations

; Multiply A and B registers, result in A
; Preserves: C, D registers
; Modifies: A register (result), Flags
multiply_ab:
    push c              ; Save C register
    mv c, a             ; Copy A to C for loop counter
    ld a, 0x00          ; Clear accumulator
    
multiply_loop:
    cmp c, 0x00         ; Check if counter is zero
    jpb zs, multiply_done ; Jump if zero
    
    add a, b            ; Add B to accumulator
    dec c               ; Decrement counter
    jpb nc, multiply_loop ; Continue loop
    
multiply_done:
    pop c               ; Restore C register
    ret nc              ; Return with result in A

; Add two 16-bit values in AW and BW, result in AW
; Preserves: C, D registers
; Modifies: AW register (result), Flags
add16_aw_bw:
    add aw, bw          ; Add 16-bit values
    ret nc              ; Return with result in AW

; Subtract BW from AW, result in AW
; Preserves: C, D registers  
; Modifies: AW register (result), Flags
sub16_aw_bw:
    sub aw, bw          ; Subtract 16-bit values
    ret nc              ; Return with result in AW

; Check if value in AL is even (result in Zero flag)
; Preserves: All registers except Flags
; Modifies: Flags only
is_even:
    bit 0, al           ; Test bit 0 (odd/even bit)
    ret nc              ; Return with Zero flag set if even
