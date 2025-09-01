; Test file for debug assertion failures
; This file tests assertion failures

.text

; Test .assert directive with false condition - should fail
.assert 0, "This assertion should fail"

; This code should never be reached
mov a, b
