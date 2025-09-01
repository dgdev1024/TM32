# Debug Processing and Printing Pass

This directory contains examples and tests for the **Debug Processing and Printing** pass, which is the 6th pass in the TM32 assembler's multi-pass preprocessor architecture.

## Overview

The Debug Processing and Printing pass handles debug directives that provide runtime debugging, assertion testing, error reporting, and source file tracking capabilities during assembly.

## Debug Directives

### `.warn "message"`
Emits a warning message during assembly but allows assembly to continue.

**Example:**
```assembly
.warn "This is a warning message"
```

**Output:**
```
[TM32ASM_ProcessWarnDirective | WARN] Warning at line 5: "This is a warning message"
```

### `.error "message"`
Emits an error message and immediately halts the assembly process.

**Example:**
```assembly
.error "This will stop assembly"
```

**Output:**
```
[TM32ASM_ProcessErrorDirective | ERROR] Error at line 3: "This will stop assembly"
[main | ERROR] Preprocessing failed
```

### `.assert condition [, "message"]`
Evaluates a condition expression and emits an error if the condition is false (0). An optional custom error message can be provided.

The condition can be any valid expression including:
- Simple values: `.assert 1`
- Arithmetic expressions: `.assert 5 + 3`
- Comparison expressions: `.assert 1 == 1`, `.assert 5 > 3`, `.assert x != 0`
- Complex expressions: `.assert (a + b) >= (c * 2)`

**Examples:**
```assembly
.assert 1                          ; Simple assertion
.assert 5 > 3                      ; Relational comparison
.assert 1 == 1, "Basic equality"   ; Equality with message
.assert x != 0, "X must not be zero"  ; Inequality with message
```

**Available comparison operators:**
- `==` (equal to)
- `!=` (not equal to)  
- `<` (less than)
- `<=` (less than or equal to)
- `>` (greater than)
- `>=` (greater than or equal to)

**Success:** Assembly continues silently
**Failure:**
```
[TM32ASM_ProcessAssertDirective | ERROR] Assertion failed at line 7: "Basic equality"
```

### `.file "filename"`
Changes the filename tracking for subsequent tokens until the next `.file` directive. This is useful for tracking source file information across included files or generated code.

**Example:**
```assembly
.file "virtual_file.asm"
; All subsequent tokens will show "virtual_file.asm" as their source
```

### `.line number`
Changes the line number tracking for subsequent tokens. Line numbers are offset relative to the directive's position.

**Example:**
```assembly
.line 1000
mov a, b    ; This will show as line 1003 (1000 + 3 line offset)
```

## Test Files

- **`debug_simple_test.asm`**: Basic functionality test with warnings, file/line directives, and passing assertions
- **`debug_error_test.asm`**: Tests the `.error` directive (will halt assembly)
- **`debug_assert_simple_failure.asm`**: Tests assertion failure (will halt assembly)
- **`comprehensive_debug_test.asm`**: Complete test showing all directives working together

## Usage

Test the debug directives using the TM32 assembler's preprocess-only mode:

```bash
# Test successful debug directives
./tm32asm --preprocess-only debug_simple_test.asm

# Test error directive (will fail)
./tm32asm --preprocess-only debug_error_test.asm

# Test assertion failure (will fail)
./tm32asm --preprocess-only debug_assert_simple_failure.asm

# Comprehensive test
./tm32asm --preprocess-only comprehensive_debug_test.asm
```

## Implementation Notes

- Debug directives are processed in the 6th pass of the preprocessor, after control flow processing
- `.warn` directives emit warnings but allow assembly to continue
- `.error` and failed `.assert` directives halt assembly immediately
- `.file` and `.line` directives modify token metadata for better source tracking
- All debug directive tokens are removed from the final token stream
- Expression evaluation in assertions uses the same engine as other preprocessor expressions
