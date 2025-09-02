; Common definitions shared across multiple files
; This file should be included by other test files

.const SYSTEM_CLOCK = 16000000
.const MEMORY_BASE = 0x80000000
.const INTERRUPT_MASK = 0x0000FFFF

.const TRUE = 1
.const FALSE = 0

.let ERROR_COUNT = 0
.let DEBUG_LEVEL = 2

.define SUCCESS 0x00
.define FAILURE 0xFF
