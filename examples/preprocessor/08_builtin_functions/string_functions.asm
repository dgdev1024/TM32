; Test string manipulation built-in functions
; Tests: strlen(), strupper(), strupr(), strlower(), strlwr(), strcat(), strcmp(), strfind(), strrfind(), strbyte()

.section .data

; Test strlen() function
test_string .const = "Hello World"
str_len .const = strlen("Hello World")  ; Should be 11

; Test strbyte() function  
first_char .const = strbyte("Hello", 0) ; Should be 'H' (72)
last_char .const = strbyte("Hello", 4)  ; Should be 'o' (111)
space_char .const = strbyte("A B", 1)   ; Should be ' ' (32)

; Test strlen with different strings
empty_len .const = strlen("")           ; Should be 0
single_len .const = strlen("A")         ; Should be 1
long_len .const = strlen("This is a longer test string") ; Should be 33

; Test strbyte with special characters
newline_char .const = strbyte("A\nB", 1) ; Should be '\n' (10)
tab_char .const = strbyte("A\tB", 1)    ; Should be '\t' (9)
quote_char .const = strbyte("A\"B", 1)  ; Should be '"' (34)

; Test various string positions
str_test .const = "Testing"
char_0 .const = strbyte("Testing", 0)   ; 'T' (84)
char_1 .const = strbyte("Testing", 1)   ; 'e' (101) 
char_2 .const = strbyte("Testing", 2)   ; 's' (115)
char_6 .const = strbyte("Testing", 6)   ; 'g' (103)

.section .text
start:
    nop
