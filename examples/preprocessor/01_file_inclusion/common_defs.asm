; Common definitions and constants
; This file provides shared constants used across multiple files

; Program constants
START_VALUE         equ 0x10        ; Initial value for calculations
MULTIPLIER          equ 0x05        ; Multiplication factor
RESULT_ADDRESS      equ 0x80001000  ; Memory address to store results
MAX_ITERATIONS      equ 0xFF        ; Maximum loop iterations

; Status flags
STATUS_OK           equ 0x00        ; Success status
STATUS_ERROR        equ 0x01        ; Error status
STATUS_OVERFLOW     equ 0x02        ; Overflow status

; Memory regions
WORK_AREA_START     equ 0x80001100  ; Start of working memory
WORK_AREA_END       equ 0x80001FFF  ; End of working memory
STACK_TOP           equ 0xFFFEFFFF  ; Top of stack memory
