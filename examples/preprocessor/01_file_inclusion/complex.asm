; Complex example demonstrating nested includes and include paths
; This shows how the preprocessor handles multiple levels of inclusion

.include "common_defs.asm"       ; Include from same directory
.include "subdir/advanced.asm"   ; Include from subdirectory

main:
    ; Use constants from common_defs.asm
    ld a, START_VALUE
    ld b, ADVANCED_CONSTANT
    
    ; Call function from nested include
    call nc, advanced_function
    
    ; Store result
    st [RESULT_ADDRESS], a
    
done:
    jpb nc, done
