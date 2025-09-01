# TM32 Assembler Control Flow Implementation

## Overview

This document describes the implementation of control flow directives (`.if`, `.elif`, `.else`, `.endif`) in the TM32 assembler preprocessor.

## Implemented Features

### 1. Basic Control Flow Directives

- **`.if <condition>`** - Begins a conditional block
- **`.elif <condition>` / `.elseif <condition>`** - Else-if clause  
- **`.else`** - Else clause
- **`.endif`** - Ends a conditional block

### 2. Supported Condition Types

Currently, the following condition types are supported:

#### Numeric Literals
```asm
.if 1        ; True (non-zero)
.if 0        ; False (zero)
.if -5       ; True (non-zero, even negative)
```

#### Constants and Variables
```asm
.const DEBUG = 1
.let optimization = 2

.if DEBUG           ; True if DEBUG is non-zero
.if optimization    ; True if optimization is non-zero
```

#### Simple Arithmetic (after variable substitution)
```asm
.const VALUE = 10
.if VALUE          ; True if VALUE is non-zero (substituted as 10)
```

### 3. Control Flow Logic

#### `.if` Directive
- Evaluates the condition expression
- If true (non-zero), includes the following code until `.elif`, `.else`, or `.endif`
- If false (zero), skips the code and continues to next directive

#### `.elif` Directive  
- Can only appear after `.if` or another `.elif`
- Only evaluated if no previous condition in the chain was true
- If true, includes the following code until next `.elif`, `.else`, or `.endif`
- Multiple `.elif` directives are supported in sequence

#### `.else` Directive
- Can only appear after `.if` or `.elif`  
- Only one `.else` per conditional block is allowed
- Executed only if no previous condition in the chain was true
- Includes code until `.endif`

#### `.endif` Directive
- Ends the conditional block
- Must match every `.if` directive
- Pops the conditional context from the stack

### 4. Nesting Support

Control flow directives support arbitrary nesting:

```asm
.if OUTER_CONDITION
    ; Outer block code
    
    .if INNER_CONDITION
        ; Inner block code
    .else
        ; Inner else code
    .endif
    
    ; More outer block code
.endif
```

Maximum nesting depth: 16 levels (configurable via `TM32ASM_MAX_LOOP_NESTING`)

### 5. Error Handling

The implementation includes comprehensive error checking:

- **Mismatched directives**: `.elif`/`.else`/`.endif` without matching `.if`
- **Multiple `.else`**: Only one `.else` per conditional block
- **Unclosed blocks**: `.if` without matching `.endif`  
- **Nesting overflow**: More than maximum allowed nesting levels
- **Expression evaluation**: Invalid or empty condition expressions

## Implementation Details

### Data Structures

#### Control Flow Context Stack
```c
typedef struct {
    TM32ASM_ControlFlowType type;           // TM32ASM_CFT_CONDITIONAL
    bool                    isActive;       // Whether this context is active
    uint32_t                startLine;      // Line where construct starts
    const char*             filename;       // File where construct is defined
    
    struct {
        bool            conditionMet;       // Whether any condition was met
        bool            inElseBlock;        // Whether we're in else block
    } conditional;
} TM32ASM_ControlFlowContext;
```

#### Preprocessor State
- `controlFlowStack`: Array of control flow contexts
- `controlFlowDepth`: Current nesting depth
- `TM32ASM_MAX_LOOP_NESTING`: Maximum allowed nesting (16)

### Processing Algorithm

1. **Token Stream Processing**: Iterate through all tokens in the input stream
2. **Directive Detection**: Check for control flow directive tokens (`TM32ASM_TT_DIRECTIVE`)
3. **Context Management**: Push/pop contexts on the control flow stack
4. **Condition Evaluation**: Use existing expression evaluator for conditions
5. **Code Emission**: Only emit tokens when all nested contexts are active

### Key Functions

- `TM32ASM_ProcessIfDirective()`: Handles `.if` directives
- `TM32ASM_ProcessElifDirective()`: Handles `.elif`/`.elseif` directives  
- `TM32ASM_ProcessElseDirective()`: Handles `.else` directives
- `TM32ASM_ProcessEndifDirective()`: Handles `.endif` directives
- `TM32ASM_ShouldEmitCode()`: Determines if code should be included
- `TM32ASM_PushControlFlowContext()`: Pushes new context onto stack
- `TM32ASM_PopControlFlowContext()`: Pops context from stack

## Current Limitations

### 1. Expression Evaluation
Comparison operators are not yet implemented in the expression evaluator:
- `==`, `!=`, `<`, `>`, `<=`, `>=` - **NOT SUPPORTED**
- `&&`, `||`, `!` - **NOT SUPPORTED**  
- Bitwise operations in conditions - **LIMITED**

### 2. Workarounds
For complex conditions, use variables to simulate:
```asm
.const PLATFORM = 2
.let is_linux = 1        ; Simulate PLATFORM == 2
.let not_windows = 1     ; Simulate PLATFORM != 1

.if is_linux
    ; Linux-specific code
.endif
```

## Testing

Comprehensive test files demonstrate all features:

1. **`simple_if.asm`** - Basic `.if`/`.endif` functionality
2. **`if_elif_else.asm`** - Complete `.if`/`.elif`/`.else`/`.endif` chains
3. **`nested_conditionals.asm`** - Deep nesting scenarios  
4. **`edge_cases.asm`** - Edge cases and error conditions
5. **`comprehensive_test.asm`** - All features combined

All tests pass successfully and demonstrate correct conditional compilation behavior.

## Future Enhancements

1. **Expression Evaluation**: Add support for comparison and logical operators
2. **Loop Constructs**: Implement `.repeat`, `.while`, `.for` directives
3. **Additional Directives**: `.ifdef`, `.ifndef` for symbol checking
4. **Error Messages**: More detailed error reporting with context

## Integration

The control flow processing is integrated as Pass 5 in the preprocessor pipeline:

1. File Inclusion
2. Symbol Declaration  
3. Macro Expansion
4. Variable Evaluation
5. **Control Flow Processing** ← This implementation
6. Finalization
7. Newline Optimization

This ensures that all variables and constants are resolved before control flow evaluation, enabling proper condition assessment.
