; Comprehensive test for debug processing and printing directives
; This file demonstrates all debug directives working correctly

.text

; Test .warn directive - emits warning but continues
.warn "Starting debug directive tests"

; Test .file directive - changes source file tracking
.file "virtual_file.asm"

; Test .line directive - changes line number tracking  
.line 1000

; Test .assert with simple true condition
.assert 1

; Test .assert with true condition and custom message
.assert 1, "Simple assertion passed"

; Show that line numbers are now offset
mov a, b    ; This should show as line 1006

; Test another warning
.warn "All tests completed successfully"

; Some final assembly code
ld c, a
ret
