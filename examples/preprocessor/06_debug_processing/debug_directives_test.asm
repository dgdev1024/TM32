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
.assert 1 == 1, "Basic equality assertion"

; Test another assertion
.assert 5 > 3

; Some regular assembly code
mov a, 42
add a, 1

; This .error directive is commented out because it would halt assembly
; .error "This would stop assembly if uncommented"

; More regular code
ld b, a
ret
