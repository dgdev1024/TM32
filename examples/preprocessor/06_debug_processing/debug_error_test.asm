; Test file for debug processing errors
; This file tests error conditions in debug directives

.text

; Test .error directive - should halt assembly with an error
.error "This error message should halt assembly"

; This code should never be reached
mov a, 1
