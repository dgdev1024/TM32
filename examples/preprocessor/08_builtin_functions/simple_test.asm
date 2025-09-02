; Simple test of high() function

.section .data

.const test_value = 0x12345678
.const high_part = high(0x12345678)

.section .text
start:
    nop
