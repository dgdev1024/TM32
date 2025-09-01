; Test file for debug processing and printing directives
; This file tests the .warn, .error, .assert, .file, and .line directives

.text

; Test .warn directive - should emit a warning but continue processing
.warn "This is a test warning message"

; Test .file directive - changes the filename for subsequent tokens
.file "test_changed_file.asm"

; Test .line directive - changes line numbering
.line 100

; Test .assert directive with true condition - should pass
.assert 1, "Basic assertion"

; Test another assertion with simple comparison
.assert 5

; Some regular assembly code (without immediate values for now)
mov a, b
add a, c

; More regular code
ld b, a
ret
