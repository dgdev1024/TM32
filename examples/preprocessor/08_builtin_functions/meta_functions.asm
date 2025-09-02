; Test meta/symbol inquiry built-in functions  
; Tests: defined(), isconst(), isvariable(), isvar()

.section .data

; Define some symbols for testing
test_const .const = 42
test_var .var = 100

; Test defined() function
def_const .const = defined(test_const)     ; Should be 1 (true)
def_var .const = defined(test_var)         ; Should be 1 (true)
def_missing .const = defined(nonexistent) ; Should be 0 (false)

; Test defined() with string argument
def_str_const .const = defined("test_const") ; Should be 1 (true)
def_str_var .const = defined("test_var")     ; Should be 1 (true)
def_str_missing .const = defined("missing")  ; Should be 0 (false)

; Test isconst() function
is_const_const .const = isconst(test_const)   ; Should be 1 (true)
is_const_var .const = isconst(test_var)       ; Should be 0 (false)
is_const_missing .const = isconst(undefined)  ; Should be 0 (false)

; Test isvariable() and isvar() alias
is_var_const .const = isvariable(test_const) ; Should be 0 (false)
is_var_var .const = isvariable(test_var)     ; Should be 1 (true)
is_var_alias .const = isvar(test_var)        ; Should be 1 (true - alias test)
is_var_missing .const = isvariable(missing)  ; Should be 0 (false)

; Additional test symbols
another_const .const = 0xFF
another_var .var = 0x00

; More defined tests
def_another_const .const = defined(another_const) ; Should be 1
def_another_var .const = defined(another_var)     ; Should be 1

; More type tests
type_test1 .const = isconst(another_const)        ; Should be 1
type_test2 .const = isvariable(another_const)     ; Should be 0
type_test3 .const = isconst(another_var)          ; Should be 0
type_test4 .const = isvariable(another_var)       ; Should be 1

.section .text
start:
    nop
