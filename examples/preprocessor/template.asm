;
; @file     examples/preprocessor/template.asm
; @brief    Contains code which demonstrates the TM32 Assembler Tool's
;           Preprocessor Component's ability to do something, which should
;           be specified by the programmer here.
;

; Run this file with:
;   `tm32asm --preprocess-only --output-preprocessed examples/preprocessor/temp/template.asm examples/preprocessor/template.asm`

.text
main:

done:
    jpb nc, done

;
; The preprocessor should resolve the above code to the following:
;
; ```
;   .text
;   main:
;
;   done:
;       jpb nc, done
; ```
;
