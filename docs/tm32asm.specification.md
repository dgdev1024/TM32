# Specification - TMM Assembly Language and Assembler

Below is a specification for the "TMM", an assembly language and assembler tool
used for building programs targeting the TM32 virtual CPU architecture.

## 1. Overview

**TMM** is a human-readable assembly language and assembler tool designed for 
the TM32 virtual CPU. It provides a convenient syntax for writing, organizing, 
and assembling programs that target the TM32 architecture, supporting all 
features of the CPU including its registers, memory map, instruction set, and 
interrupt system.

The TMM assembler produces object files in the TM32 Object Format (.tmo) that 
contain relocatable code and data, symbol tables, and metadata required for 
linking multiple object files together. This modular approach enables separate 
compilation of source files and supports large, complex projects with proper 
dependency management.

## 2. Source File Structure

A source file in the TMM assembly language consists of the following structure:

- **File Extension**: `.tmm` (recommended), `.asm`, `.inc`
    - File extensions are not strictly enforced by the TMM assembler, so any
        file extension (or none) can be used, so as long as that file contains
        valid TMM source code.
- **Sections**: Source files may be divided into logical sections using certain
    directives (`.text`, `.rodata`, `.data`, `.bss`).
- **Regions**: Source files may be divided further into named regions for
    organization purposes, using the `.region` directive.
- **Comments**: A semicolon (`;`) indicates the start of a comment. These lines,
    or the remainder thereof, are ignored by the assembler.
- **Address Labels**: An identifier followed by a colon indicates the definition
    of an address label for code and data locations.
- **Directives**: A peroid (`.`) followed by an identifier indicates a directive
    to be executed by the assembler, rather than the CPU.
- **Instructions**: An identifier followed by operands indicates a CPU
    instruction to be assembled and executed by the CPU.
- **Macros**: Using the `.macro` directive, users may define macros for
    reusable code snippets with parameters.
- **Conditional Assembly**: Using the `.if`, `.else`, `.elif`, and `.endif`
    directives, users may include or exclude code based on conditions.
- **Repetition**: Using the `.repeat` and `.endrepeat` directives, users may
    repeat code blocks a specified number of times.

## 3. Directives

Directives in the TMM assembly language provide instructions to the assembler
for organizing and managing the assembly process. The subsections below detail
the directives supported.

### 3.1 Section and Organization Directives

- `.metadata`: Sets the current section to the metadata section, where read-only
    metadata information, such as program name, version, author, and description
    can be placed.
- `.int <index>`: Sets the current section to one of the TM32's 32 interrupt 
    vectors, where interrupt handler code can be placed. `<index>` is the 
    interrupt vector number (0-31).
- `.text`: Sets the current section to the code section, where executable
    instructions can be placed.
- `.rodata`: Sets the current section to the read-only data section, where
    constant data can be placed.
- `.data`: Sets the current section to the initialized data section, where
    RAM data with initial values can be placed.
- `.bss`: Sets the current section to the uninitialized data section, where
    RAM data without initial values can be placed.
- `.section <name>[, <attributes>]`: Defines a custom section with optional attributes.
    - `<name>` is the section name as a string literal
    - `<attributes>` is a comma-separated list of attributes:
        - `alloc`: Section occupies memory at runtime
        - `write`: Section is writable at runtime
        - `exec`: Section contains executable code
        - `merge`: Section can be merged with identical sections
        - `strings`: Section contains null-terminated strings
- `.pushsection <name>`: Saves current section and switches to specified section
- `.popsection`: Restores the previously saved section
- `.org <address>`: Sets the current assembly address to `<address>`, which can
    be an absolute address or a label.
    - There are actually multiple assembly addresses kept track of by the assembler.
        The following logical sections either have their own or share a tracked
        assembly address:
        - `.metadata`
        - `.int` - one `.org` address for each interrupt vector.
        - `.text` and `.rodata` share a `.org` address.
        - `.data` and `.bss` share a `.org` address.
    - `<address>` must be within a specific range depending on the current section:
        - `.metadata`: `0x00000000` to `0x00000FFF`
        - `.int <index>`: 
            - Start: `0x00001000 + (<index> * 0x100)`
            - End: `0x00001000 + (<index> * 0x100) + 0xFF`
        - `.text` and `.rodata`: `0x00003000` to `0x7FFFFFFF`
        - `.data` and `.bss`: `0x80000000` to `0xFFFDFFFF`, `0xFFFF0000` to `0xFFFFFFFF`
- `.align <value>`: Aligns the current assembly address to the next multiple of
    `<value>`, which must be a positive power of two (1, 2, 4, 8, 16, ...).
- `.section_align <alignment>`: Sets the alignment requirement for the current section
- `.region <name>`: Starts a new named region for organizational purposes.
    - `<name>` is a string literal representing the region name.
- `.endregion`: Ends the current named region. This is optional, as regions
    automatically end at the end of the file or when a new region is started.

### 3.2 Data Definition and Reservation Directives

The following directives may work differently depending on the current section.
These directives are not allowed in the `.int <index>` or `.text` sections.

- `.db <value1>[, <value2>, ...]`, `.byte <value1>[, <value2>, ...]`: 
    - `.metadata`, `.rodata`, `.data`: Defines one or more bytes with the specified
        initial values. Values can be specified as 8-bit integers (decimal or hex),
        character literals, string literals (which define multiple bytes), or
        other expressions which evaluate to 8-bit integers or string literals.
    - `.bss`: Reserves one or more bytes of uninitialized space. Only the first
        value is used, and it must be a positive integer indicating the number
        of bytes to reserve.
- `.dw <value1>[, <value2>, ...]`, `.word <value1>[, <value2>, ...]`:
    - `.metadata`, `.rodata`, `.data`: Defines one or more words (2 bytes each)
        with the specified initial values. Values can be specified as 16-bit
        integers (decimal or hex), character literals, string literals (which
        define multiple words), or other expressions which evaluate to 16-bit
        integers or string literals.
    - `.bss`: Reserves one or more words of uninitialized space. Only the first
        value is used, and it must be a positive integer indicating the number
        of words to reserve.
- `.dd <value1>[, <value2>, ...]`, `.dword <value1>[, <value2>, ...]`:
    - `.metadata`, `.rodata`, `.data`: Defines one or more double words (4 bytes
        each) with the specified initial values. Values can be specified as
        32-bit integers (decimal or hex), character literals, string literals
        (which define multiple double words), or other expressions which
        evaluate to 32-bit integers or string literals.
    - `.bss`: Reserves one or more double words of uninitialized space. Only the
        first value is used, and it must be a positive integer indicating the
        number of double words to reserve.
- `.ds <value>`, `.space <value>`:
    - `.metadata`, `.rodata`, `.data`: Not allowed.
    - `.bss`: Reserves a block of uninitialized space. `<value>` must be a
        positive integer indicating the number of bytes to reserve.
- `.fill <count>, <size>, <value>`: Fills memory with a repeated pattern.
    - `<count>`: Number of repetitions
    - `<size>`: Size of each element (1, 2, or 4 bytes)
    - `<value>`: Value to repeat
- `.skip <bytes>`: Advances the assembly address by `<bytes>` without emitting data.
    Equivalent to `.ds` but more explicit in intent.

### 3.3 Symbol Visibility and Linking Directives

The following directives control symbol visibility and linking behavior for
generating relocatable object files:

- `.global <symbol>`, `.globl <symbol>`: Makes a symbol visible to other object files during linking.
    The symbol must be defined in the current source file.
- `.extern <symbol>`: Declares that a symbol is defined in another object file and
    will be resolved at link time. The symbol should not be defined in the current file.
- `.local <symbol>`: Restricts symbol visibility to the current object file only.
    The symbol will not be visible to other object files during linking.
- `.weak <symbol>`: Defines a weak symbol that can be overridden by a strong symbol
    with the same name from another object file. If no strong symbol is found,
    the weak symbol will be used.
- `.type <symbol>, <type>`: Specifies the type of a symbol for debugging and linking.
    Valid types are:
    - `notype`: No specific type
    - `object`: Data object (variables, arrays, etc.)
    - `func`: Function or subroutine
    - `section`: Section symbol
    - `file`: Source file symbol
- `.size <symbol>, <expression>`: Specifies the size of a symbol in bytes.
    Useful for optimization and debugging information.

### 3.4 Variables and Constants

The directives below may be used to define and affect variables and constants
used only by the assembler, not the CPU. These variables and constants are not
stored in the assembled binary, and can be used in expressions.

- `.let <name>[ = <value>]`: Defines a mutable variable named `<name>`, with an
    optional initial value. If no initial value is provided, the variable is
    initialized to `0`. The value can also be an evaluatable expression. The
    `.let` directive can also be used along with an assignment operator (`=`,
    `+=`, `-=`, etc.) to change a mutable variable's value later on in the
    source file (or another source file including it).
- `.const <name> = <value>`: Defines an immutable constant named `<name>` with
    the specified value. The value can be an evaluatable expression. An initial
    value is required, and it cannot be changed later.

### 3.4 Macros

Macros allow users to define reusable code snippets with a variable number of
parameters. Macro definitions alone do not produce any output in the assembled
binary; as such, they can be placed in any section. The directives below
facilitate macro definition:

- `.macro <name>`: Starts the definition of a macro named `<name>`.
    - `<name>` is an identifier representing the macro name.
    - The macro's body consists of everything between this directive and the
        corresponding `.endmacro` or `.endm` directive. This can include instructions,
        directives, labels, positional parameters (`@1`, `@2`, ...), and special
        parameters (`@0`, `@*`, `@#`).
    - Positional parameters (`@1`, `@2`, ...) are replaced with the corresponding
        argument when the macro is invoked.
    - Special parameters:
        - `@0`: Replaced with all arguments as a single string.
        - `@*`: Replaced with all arguments, separated by commas.
        - `@#`: Replaced with the number of arguments passed to the macro.
- `.shift <count>`: Shifts the positional parameters of the current macro to the
    left by `<count>`. This means that `@1` becomes `@(1 + count)`, `@2` becomes
    `@(2 + count)`, and so on. This is useful for macros that need to process
    a variable number of arguments.
    - `<count>` must be a non-negative integer.
    - This is not allowed outside of a macro body.
- `.endmacro`, `.endm`: Ends the definition of the current macro.

A macro can be invoked using the following syntax:

```
    <name> [<arg1>[, <arg2>, ...]]
```

#### 3.4.1 Mangling

In a macro's invocation, positional arguments passed into that invocation can be
embedded into certain other tokens in the macro's body, such as identifiers in
address labels, in order to "mangle" those tokens and form new ones which are
ulitmately passed into the AST parser.

Lexical tokens which can be "mangled" include, but are not limited to:

- Non-Keyword Identifiers (such as those used for address labels)
- String Literals

For example, consider this macro and its invocation:

```assembly
.macro mangle_example
    steamed_@1:         ; Whatever is passed into '@1' will become part of this
                        ; address label, for instance.
    burned_@2_stuffing: ; Likewise, whatever is passed into '@2' becomes part of
                        ; this address label, too.
.endmacro

.text:
    mangle_example hams, cornbread  ; As such, a macro invocation with these
                                    ; arguments would expand to the following:
                                    ;   steamed_hams:
                                    ;   burned_cornbread_stuffing:
```

A more practical, real-world example of "mangling" can be found in the example
snippet in the **8.9.2 Macros with Arguments** subsection.

### 3.5 Conditional Assembly

Conditional assembly allows users to include or exclude code based on conditions,
whether or not an expression resolves to true (non-zero) or false (zero). The
directives below facilitate conditional assembly:

- `.if <expression>`: Starts a conditional block that is included if the
    `<expression>` evaluates to true (non-zero).
- `.elseif <expression>`, `.elif <expression>`: Starts a new conditional block
    that is included if the previous conditions were false and this `<expression>`
    evaluates to true (non-zero).
- `.else`: Starts a new conditional block that is included if all previous
    conditions were false.
- `.endif`, `.endc`: Ends the current conditional block.

### 3.6 Repetition

Repetition allows users to repeat a block of code a specified number of times,
or based on the continued true evaluation of a condition. The directives below
facilitate repetition:

- `.repeat <count>`: Starts a repetition block that is repeated `<count>` times.
    - `<count>` must be a positive integer.
    - The `@$` token can be used to indicate the current iteration in a `.repeat`
        block. In the event of nested `.repeat` block, `@$` will indicate the
        current iteration of the inner-most `.repeat` block.
- `.endrepeat`, `.endr`: Ends a `.repeat` block.
- `.while <expression>`: Starts a repetition block that continues as long as the
    `<expression>` evaluates to true (non-zero).
- `.endwhile`, `.endw`: Ends a `.while` block.
- `.continue`: Skips the remainder of the current iteration of a `.repeat` or
    `.while` block and begins the next iteration.
- `.break`: Exits the current `.repeat` or `.while` block immediately.
- The `.continue` and `.break` directives are not allowed outside of a `.repeat` or
    `.while` block.

### 3.7 Include Directives

The following directives allow users to include the contents of other files into
the current source file. Included files are processed as if their contents were
directly inserted at the location of the include directive.

- `.include "<filename>"`: Includes the contents of another TMM source file at
    this location. The included file is processed as if its contents were
    directly inserted here.
    - `<filename>` is a string literal representing the path to the file to
        include. This can be an absolute path or a path relative to the current
        file, and is case-sensitive. If a relative path is provided, it is first
        resolved to an absolute path before being processed.
    - Included files can themselves contain `.include` directives, allowing for
        nested includes.
    - If a file specified in an `.include` directive cannot be found or opened,
        the assembler will produce an error.
    - If a file specified in an `.include` directive has already been included
        earlier in the assembly process, it will not be included again, preventing
        infinite inclusion loops.
- `.incbin "<filename>"[, <offset>, <length>]`: Includes raw binary data from a file into
    the current section at the current assembly address.
    - `<filename>` is a string literal representing the path to the binary file
        to include. This can be an absolute path or a path relative to the
        current file.
    - `<offset>` is an optional non-negative integer specifying the byte offset
        within the file from which to start including data. If omitted, it
        defaults to `0`.
    - `<length>` is an optional positive integer specifying the number of bytes
        to include from the file. If omitted, it defaults to including all bytes
        from the specified offset to the end of the file.
    - If a file specified in an `.incbin` directive cannot be found or opened,
        the assembler will produce an error.
    - If `<offset>` is greater than or equal to the size of the file, no data
        will be included.
    - If `<length>` extends beyond the end of the file, only the available bytes
        from `<offset>` to the end of the file will be included.

### 3.8 Preprocessor Directives and Assembler Directives

The directives detailed above can be classified into one of the following two
types of directives:

- **Preprocessor Directives**: These directives are handled and evaluated after
    lexical analysis and before AST parsing. They affect the content of the
    source code itself, rather than the final output binary. The following
    directives are preprocessor directives:
    - Source File Inclusion: `.include`
    - Variable and Constant Definition: `.let`, `.const`
    - Macro Definition and Invocation: `.macro`, `.endmacro`, `.shift`
    - Conditional Assembly: `.if`, `.elseif`, `.else`, `.endif`
    - Repetition: `.repeat`, `.endrepeat`, `.while`, `.endwhile`,
        `.continue`, `.break`
- **Assembler Directives**: These directives are handled after AST parsing and
    can affect the final output binary, memory layout and code/data emission.
    The following directives are assembler directives:
    - Section and Organization: `.metadata`, `.int`, `.text`, `.rodata`,
        `.data`, `.bss`, `.org`, `.align`, `.region`, `.endregion`
    - Data Definition and Reservation: `.db`, `.byte`, `.dw`, `.word`,
        `.dd`, `.dword`, `.ds`, `.space`
    - Binary File Inclusion: `.incbin`

## 3.5. Expressions and Built-in Functions

TMM supports complex expressions using arithmetic operators and built-in functions.

### 3.5.1 Built-in Functions

TMM provides several built-in functions to assist with common assembly programming tasks:

#### String and Symbol Manipulation
- `len("string")` or `len(symbol)`: Returns the length of a string literal or symbol name
- `substr("string", start, length)`: Extracts a substring from a string literal
- `concat("str1", "str2", ...)`: Concatenates multiple strings
- `toupper("string")`: Converts string to uppercase
- `tolower("string")`: Converts string to lowercase

#### Address and Memory Functions
- `low(expression)`: Returns the low 16 bits of a 32-bit value
- `high(expression)`: Returns the high 16 bits of a 32-bit value
- `lowbyte(expression)`: Returns the low 8 bits of a 16-bit value
- `highbyte(expression)`: Returns the high 8 bits of a 16-bit value
- `bank(address)`: Returns the memory bank number for the given address
- `sizeof(symbol)`: Returns the size of a symbol in bytes (requires .size directive)

#### Mathematical Functions
- `min(expr1, expr2, ...)`: Returns the minimum value among arguments
- `max(expr1, expr2, ...)`: Returns the maximum value among arguments
- `abs(expression)`: Returns the absolute value
- `sign(expression)`: Returns -1, 0, or 1 based on the sign of the expression

#### Fixed-Point Arithmetic (16.16 format)
- `float_to_fixed(float_value)`: Converts floating-point to 16.16 fixed-point
- `fixed_to_float(fixed_value)`: Converts 16.16 fixed-point to floating-point
- `fixed_mul(a, b)`: Multiplies two fixed-point numbers
- `fixed_div(a, b)`: Divides two fixed-point numbers

#### Assembler Utility Functions
- `defined(symbol)`: Returns 1 if symbol is defined, 0 otherwise
- `blank(argument)`: Returns 1 if argument is empty or whitespace-only
- `isnumber(argument)`: Returns 1 if argument is a valid number
- `isstring(argument)`: Returns 1 if argument is a string literal

### 3.5.2 Expression Evaluation

Expressions are evaluated using standard operator precedence:
1. Parentheses `()`
2. Unary operators: `+`, `-`, `~`, `!`
3. Multiplicative: `*`, `/`, `%`
4. Additive: `+`, `-`
5. Shift: `<<`, `>>`
6. Relational: `<`, `<=`, `>`, `>=`
7. Equality: `==`, `!=`
8. Bitwise AND: `&`
9. Bitwise XOR: `^`
10. Bitwise OR: `|`
11. Logical AND: `&&`
12. Logical OR: `||`

## 3.6. Object File Format and Linking

When generating object files (.tmo), TMM produces relocatable object files that
contain the following sections:

### 3.6.1 TM32 Object File Structure

```
+-------------------+
| File Header       |
+-------------------+
| Section Headers   |
+-------------------+
| String Table      |
+-------------------+
| Symbol Table      |
+-------------------+
| Relocation Tables |
+-------------------+
| Section Data      |
+-------------------+
```

### 3.6.2 Supported Sections

- `.text`: Executable code
- `.rodata`: Read-only data
- `.data`: Initialized writable data  
- `.bss`: Uninitialized data (zero-filled)
- `.debug_*`: Debug information sections
- Custom sections defined with `.section` directive

### 3.6.3 Symbol Types and Visibility

Symbols in object files have the following attributes:
- **Binding**: Local, Global, Weak
- **Type**: NOTYPE, OBJECT, FUNC, SECTION, FILE
- **Section**: Which section the symbol belongs to
- **Value**: Address or constant value
- **Size**: Size in bytes (if applicable)

### 3.6.4 Relocation Types

TMM supports the following relocation types for linking:
- `R_TM32_32`: 32-bit absolute address
- `R_TM32_16`: 16-bit absolute address
- `R_TM32_8`: 8-bit absolute address
- `R_TM32_PC32`: 32-bit PC-relative address
- `R_TM32_PC16`: 16-bit PC-relative address
- `R_TM32_HIGH16`: High 16 bits of 32-bit address
- `R_TM32_LOW16`: Low 16 bits of 32-bit address

## 5. Instructions

The TMM assembly language supports all instructions of the TM32 CPU. An
instruction consists of an opcode and anywhere between zero and two operands.
In the TMM assembly language, instructions are written in a human-readable format,
as follows:

```
    [<label>:] <mnemonic> [<operand1>[, <operand2>]]   [; optional comment]
```

Consult the **5. Instructions** chapter in the 
[TM32 CPU Specification](tm32.specification.md) for more details on the TM32 
instruction set.

Operands passed into instructions can be one of the following:

- A general-purpose register literal (eg. `A`, `BW`, `CH`, `DL`, etc.)
- An execution condition (eg. `NC`, `ZS`, `CC`, etc.) (control-transfer 
    instructions only)
- An immediate value - any one of the following:
    - Strings (eg. "John Doe")
    - Fixed-Point Numbers (`3.14159`)
    - Hexadecimal Numbers (`0xDEADBEEF`, `0xC0FFEE`)
    - Integers (`34`, `1289`)
    - Octal Numbers (`0o1234`, `0o457`)
    - Binary Numbers (`0b1011`, `0b01010101`)
    - ASCII Characters (`'a'`, `'\n'`, `'\0'`)
- An expression that evaluates to an immediate value (eg. `0x40 + 2`, `label + 4`, etc.)
- A memory address - any literal or expression in brackets (eg. `[0x00C0FFEE]`, 
    `[label]`, `[A]`, etc.)

### 5.1 Fixed-Point Numbers

Fixed-point numbers are represented internally in the TMM assembler as 64-bit
unsigned integers, where the 64 bits are composed as follows:

- Bits 32 through 63 make up the "Integer Part" of the fixed-point number.
    - This is the `3` in `3.14159`.
- The remaining bits, bit 0 through 31, make up the "Fractional Part" of the
    fixed-point number.
    - This is the `.14159` in `3.14159`.

### 5.2 Address Labels

Address labels are defined by placing an identifier followed by a colon at the
beginning of a line (eg. `MyLabel:`). Upon creation, the label is assigned the
current assembly address. Labels can then be used in place of immediate values
or memory addresses in instructions and directives. They also can be used in
expressions, just like immediate values and variables.

## 6. Assembler Tool

The TMM assembler tool processes TMM source files in several well-defined stages, 
ensuring correctness, modularity, and robust error handling throughout:

- **Lexer**: Reads a source file and produces a sequence of tokens representing 
    the elements of the TMM language (literals, identifiers, directives, 
    instructions, operators, etc.).
- **Preprocessor**: Handles preprocessor directives such as `.include`, `.macro`, 
    `.if`, `.repeat`, `.let`, and `.const`. The preprocessor runs immediately 
    after lexing and before parsing, expanding macros, including files, 
    evaluating variables/constants, and processing conditional and repetition 
    logic. This produces a fully expanded and resolved token stream for the parser. 
    The order of preprocessing is important: file inclusion and macro expansion 
    occur before variable/constant evaluation, which occurs before conditional 
    and repetition logic. See **3.8 Preprocessor Directives and Assembler Directives** 
    for details.
- **Parser**: Consumes the preprocessed tokens and constructs an abstract syntax 
    tree (AST) representing the hierarchical structure of the program. The parser 
    checks for syntax errors and ensures the program structure is valid.
- **Symbol Table Construction**: Traverses the AST to build a symbol table mapping 
    labels, variables, and constants to their addresses or values. This enables 
    correct resolution of symbols in code and data, and allows for expressions 
    involving labels and variables. The assembler also calculates the expected 
    size of the final binary output at this stage.
- **Code Generation**: Walks the AST and symbol table to emit the final binary 
    output, processing assembler directives, data definitions, and instructions. 
    This stage handles address assignment, binary file inclusion, data layout, 
    and output generation. Arithmetic and logical evaluations required for code 
    and data emission are performed here.
- **Error and Warning Reporting**: Throughout all stages, the assembler reports 
    errors and warnings with precise location information, including syntax errors, 
    undefined symbols, invalid directives, and file or macro issues. This ensures 
    that users receive clear feedback for debugging and correction.
- **Output Generation**: The final binary output, as generated in the
    **Code Generation** stage, is written to a file in the TM32 binary format.

## 7. Binary Output Format

The TMM assembler produces a binary output file in the TM32 binary format, which
is a flat binary representation of the assembled program. The binary file contains
the following sections:

- **Metadata Section**: Contains read-only, platform-specific metadata
    about the program, such as an identifying magic number, program name,
    version, author, and description, RAM size required, and information on
    what other virtual hardware may be needed. What information is contained,
    and how that information is laid out, depends on the virtual hardware being
    powered by the TM32. This section is optional, but *very strongly recommended*. 
    Size: `0x1000` (4096 bytes).
- **Interrupt Vectors**: Contains the interrupt vector table, which consists of
    32 interrupt vector subroutines, each of which is 256 bytes long. Each
    interrupt vector is located at a specific address in the binary file,
    starting at `0x1000` and ending at `0x2FFF`. The first vector is at
    `0x1000`, the second at `0x1100`, and so on, up to the 32nd vector at
    `0x1F00`. Size: `0x2000` (8192 bytes).
- **Code Section**: Contains the executable code and read-only data of the
    program. This section starts at address `0x3000` and can grow up to the
    maximum addressable memory space for read-only data and code, `0x7FFFFFFF`.
    The size of this section is variable, depending on the program, but must
    contain at least one instruction or data definition. Size: variable, up
    to `0x7FFFCFFF` (2,147,483,647 bytes, or ~2 GB).

The minimum size of the binary output file is `0x4000` (16384 bytes), which is
more than enough to accommodate the metadata section, the interrupt vector table,
and a small code section. The assembler ensures that the binary output file
is properly aligned and padded as necessary to meet the minimum size requirement.

See the **2. Memory Map** chapter in the
[TM32 CPU Specification](tm32.specification.md) for more details on the memory
map and how the binary output file is structured in memory.

## 8. Error Reporting

The TMM assembler tool provides detailed error and warning messages during the
assembly process. Errors are reported with the following information:

- **File Name** and **Line Number**: The source file and line number where the
    error occurred, allowing users to quickly locate the issue in their code.
    Each token produced by the lexer/preprocessor will carry this information,
    which is propagated through the parsing and code generation stages.
- **Error Message**: A clear and concise description of the error, explaining
    what went wrong and why it is considered an error.
- **Context**: The assembler may provide additional context, such as the instruction
    or directive that caused the error, the macro which contained the offending code,
    or the specific symbol that was not found.
- **Severity**: Errors are categorized as either errors or warnings. Errors
    indicate critical issues that prevent successful assembly, while warnings
    indicate potential issues that may not prevent assembly but could lead to
    unexpected behavior or runtime errors.
- **Suggestions**: Where applicable, the assembler may provide suggestions for
    how to fix the error, such as correcting a syntax error, defining a missing
    symbol, or adjusting a directive.

## 9. Examples

Below are some examples of TMM assembly code demonstrating various features:

### 8.1 A Simple Program

The program below results in an infinite loop:

```assembly
.text                       ; Declare that we are placing instruction data.
    main:                   ; Declare an address label named 'main'.
        nop                 ; A 'nop' instruction does nothing.
        jmp nc, main        ; Jump, with no condition, to label 'main'.
```

### 8.2 A Simple Program with Metadata

This is the same program as above, but with the addition of specifically-placed
metadata, in order to also demonstrate the `.org` directive.

```assembly
.metadata                           ; Declare that we are placing metadata information.
    .db "TOMBOY", 0x00              ; Place a null-terminated magic string at the start.
    .org 0x10                       ; Place the next data at address '0x10'.
    .db "Hello, World!", 0x00       ; Program name, null-terminated.
    .org 0x20                       ; Place the next data at address '0x20'.
    .db "Jane Doe", 0x00            ; Author name, null-terminated.
    .org 0x30
    .db 0x1, 0x0                    ; Major and minor version.

; Same program as before.
.text                               ; Note that there are multiple location counters.
    main:
        nop
        jmp nc, main
```

**Note**: The `.metadata` sections shown in this specification's examples are 
strictly for demonstration purposes. The actual metadata required, and its
layout, will depend on the platform powered by the TM32. For instance, for the
metadata required by programs running on the TM32 Core, see the
**5.1 Program Structure** section in the 
[TM32 Core Specification](tm32core.specification.md).

### 8.3 A Counter

This program uses the `.data` directive to store a counter in RAM, which is
incremented on a loop, and a `.const` variable to keep track of a target value.

This program assumes that the platform powered by the TM32 has at least 4 bytes
of RAM available. It is the responsibility of the platform to allocate this RAM.
In this case, the amount of RAM required is stored in the metadata section of
this program.

```assembly
.const target = 4000                ; Declare an assembler constant named 'target'.
.metadata
    .dd 0x4                         ; The example's platform's way of requesting 4 bytes of RAM.
.data                               ; Declare that we are reserving data in RAM.
    counter:    .dd 1               ; Reserve one double-word to store the counter.
.text
    main:
        ld d, [counter]             ; Load, into register D, the value in memory at address 'counter'.
        inc d                       ; Increment the value of D.
        st [counter], d             ; Store the value of D in memory at address 'counter'.
        jmp zc, main                ; Jump, if the zero flag is clear, to address 'main'.
        
    done:
        jpb nc, done                ; Infinite loop to end program
```

### 8.4 Conditional Assembly

The programs below demonstrate the use of the conditional assembly directives.

#### 8.4.1 The `.if` Statement

```assembly
.const is_debug = 1
.text
    main:
        nop
        .if is_debug        ; Equivalent to `.if is_debug != 0`
            ld aw, 0xdead   ; This code will only run in 'is_debug' is non-zero.
            ld bw, 0xbeef
        .endif
        jmp nc, main
```

#### 8.4.2 Using `.if` with `.else`

```assembly
.const is_debug = 0
.text
    main:
        nop
        .if is_debug != 0
            ld d, 0xDEADBEEF
        .else
            ld d, 0xC0FFEE
        .endif
        jmp nc, main
```

#### 8.4.3 `.elif`

```assembly
.const my_number = 15
.text
    main:
        nop
        .if my_number == 20
            ld d, 0x12345678
        .elif my_number == 15
            ld d, 0xC0FFEE
        .else
            ld d, 0xDEADBEEF
        .endif
        jmp nc, main
```

### 8.5 Repetition

The programs below demonstrate the use of the repetition and repetition management
directives.

#### 8.5.1 The `.repeat` Statement

```assembly
.text
    main:
        .repeat 10
            ld al, @$       ; '@$' indicates the number of the current iteration.
            daa
        .endrepeat

    done:
        jpb nc, done        ; Infinite loop to end program
```

#### 8.5.2 The `.while` Statement

```assembly
.let counter = 0            ; Declare a mutable variable named 'counter'.

.text
    main:
        .while counter < 10
            ld al, counter
            .let counter += 1   ; `.let` can also be used with assignment operators to modify
                                ; mutable variables.
        .endwhile

    done:
        jpb nc, done            ; Infinite loop to end program
```

#### 8.5.3 The `.continue` Statement

The `.continue` directive can be used within a `.repeat` or `.while` block to
immediately end the current iteration of the loop and begin the next iteration
(or end the loop if that was the final iteration).

```assembly
.let counter = 0

.text
    main:
        .repeat 15
            .if @$ % 3 == 0
                .continue
            .endif

            ld al, @$
        .endrepeat

        .while counter < 30
            .if counter % 5 == 0
                .continue
            .elif counter % 3 == 0
                ld bl, counter
            .else
                ld d, 0xDEADBEEF
            .endif

            .let counter += 1
        .endwhile

    done:
        jpb nc, done            ; Infinite loop to end program
```

#### 8.5.4 The `.break` Statement

The `.break` directive is used within a `.repeat` or `.while` block to immediately
exit the loop. In the event of nested loop, this statement will only exit out
of the innermost loop.

```assembly
.text
    main:
        .repeat 100
            .if @$ == 42
                .break
            .endif

            ld al, @$
        .endrepeat

        jmp nc, main
```

### 8.6 Interrupt Vectors

The `.int` directive is used to indicate that instructions should be placed in
one of the TM32's 32 interrupt vector subroutines. This directive expects an
integer parameter indicating which of these vectors are being written to.

The program below illustrates an example of interrupt vector subroutines being
written and used.

```assembly
.int 0                  ; Declare that we are placing instructions in interrupt vector 0.
    ld aw, 0x1234
    reti                ; Enable interrupts and return.

.int 1
    ld b, 0xC0FFEE
    reti

.text
    main:
        ei              ; Enable interrupts
        ld dl, 0x00

    loop:
        inc dl
        jmp zc, loop

    int 1               ; The `int` instruction explicitly calls an interrupt vector.

    done:
        jpb nc, done    ; Infinite loop to end program
```

### 8.7 Including Files

The program below demonstrates how to include source files with the `.include`
preprocessor directive, and how to embed binary files with the `.incbin`
assembler directive.

```assembly
; @file other_file.tmm

.rodata                         ; Declare that we are placing/embedding non-executable data.
    .org 0x3500                 ; Embed the below binary file at address 0x3500
    image_data:
        .incbin "./logo.bin"    ; Assume the presence of a binary image file.
    image_data_end:             ; Place a label to indicate the end of the image data.
```

```assembly
; @file main_file.tmm

.include "./other_file.tmm"     ; Include the above source file.
.text
    main:
        ld d, image_data
        ld c, image_data_end
        ld b, (image_data_end - image_data) ; Address labels also act as variables which can be used in expressions.
    
    loop:
        ld al, [d]
        daa
        inc d
        dec b
        jmp zc, loop

    done:
        jpb nc, done            ; Infinite loop to end program

```

### 8.8 Uninitialized Data

The `.bss` directive can be used to indicate that un-initialized memory is being
reserved for future use.

```assembly
.bss                            ; Declare that we are reserving un-initialized data.
    buffer:         .ds 0xFF    ; Reserve a 256-byte buffer.
    bytes:          .db 42      ; Reserve a 42-byte buffer.
    words:          .dw 25      ; Reserve 25 words worth of data (2 bytes each, or 50 bytes total).
    double_words:   .dd 10      ; Reserve 10 double words worth of data (4 bytes each, or 40 bytes total).
```

### 8.9 Using Macros

The programs below demonstrate the use of macros to define reusable code snippets.

#### 8.9.1 A Simple Macro

This program defines a simple macro which takes no parameters and clears the
registers A, B, C, and D. 

```assembly
.macro clear_registers
    ld a, 0
    ld b, 0
    ld c, 0
    ld d, 0
.endmacro

.text
    main:
        clear_registers         ; Invoke the 'clear_registers' macro. It takes no arguments.

    done:
        jpb nc, done            ; Infinite loop to end program
```

#### 8.9.2 Macros with Arguments

This program defines a few macros which take arguments.

```assembly
.macro add_to_a
    add a, @1
.endmacro

.macro multiply
    ld a, (@1 * @2 * @3)
.endmacro

.macro define_block
    ; @1 = name
    ; @2 = bytes
    @1: .db @2
    @1_end:                     ; A positional argument passed into a macro can be concatenated with
                                ; or embedded into certain other tokens, like the identifier in this
                                ; address label, to form a new token.
.endmacro

.bss
    define_block my_block, 42   ; This expands to the following:
                                ;   my_block: .db 42
                                ;   my_block_end:

.text
    main:
        add_to_a 0x20           ; Invoke the 'add_to_a' macro, passing '0x20' to argument '@1'.
                                ; This will expand to the following:
                                ;   add a, 0x20
        multiply 5, 2, 3        ; Invoke the 'multiply' macro, passing the values 5, 2, 3 into '@1',
                                ; '@2' and '@3', respectively.
                                ; The expansion of this macro will handle the multiplication expression
                                ; in the macro's body, and as such will expand to the following line:
                                ;   ld a, 30
    
    done:
        jpb nc, done            ; Infinite loop to end program
```

#### 8.9.3 Macros with Variadic Arguments

```assembly
.macro add_numbers
    .let sum = 0
    .repeat @#          ; '@#' indicates the number of arguments passed into the macro.
        .let sum += @1
        .shift 1        ; The '.shift' directive shifts the first argument passed into the macro out
                        ; of the arguments list. After that, the argument that was '@2' becomes '@1',
                        ; '@3' becomes '@2' and so on...
    .endrepeat

    ld a, sum
.endmacro

.text
    main:
        add_numbers 1, 2, 3, 4, 5   ; This expands to 'ld a, 15'.
    
    done:
        jpb nc, done                ; Infinite loop to end program
```

### 9.4 Object File and Linking Example

This example demonstrates modular compilation using object files:

**math.tmm** (math library):
```assembly
; Math library functions
.section .text

.global add16
.type add16, func
.size add16, add16_end - add16
add16:
    ; Add two 16-bit values in AW and BW, result in AW
    ; Store BW value in memory, then add it to AW
    st [bw_temp], bw
    add aw, [bw_temp]
    ret nc
add16_end:

.global multiply8
.type multiply8, func  
.size multiply8, multiply8_end - multiply8
multiply8:
    ; Multiply AL by BL, result in AW
    ; Simple multiplication by repeated addition
    ld aw, 0                    ; Clear result
    mv ch, bl                   ; Store multiplier in CH
    cmp ch, 0                   ; Check if multiplier is zero
    jmp zs, multiply8_done      ; If zero, we're done
multiply8_loop:
    ; Store AL value and add it to result
    st [al_temp], al
    add aw, [al_temp]
    dec ch                      ; Decrement counter
    jmp zc, multiply8_loop      ; Continue if not zero
multiply8_done:
    ret nc
multiply8_end:

.section .data
bw_temp: .dw 0                 ; Temporary storage for BW value
al_temp: .db 0                 ; Temporary storage for AL value

.section .rodata
.global math_version
.type math_version, object
.size math_version, 4
math_version: .dw 0x0102        ; Version 1.2
```

**main.tmm** (main program):
```assembly
; Main program that uses math library
.extern add16
.extern multiply8
.extern math_version

.section .text
.global _start
_start:
    ; Test math functions
    ld aw, 100
    ld bw, 200
    st [bw_temp], bw            ; Store BW value for add16 function
    call nc, add16              ; AW = 300
    
    ld al, 5
    ld bl, 7
    st [al_temp], al            ; Store AL value for multiply8 function
    call nc, multiply8          ; AW = 35
    
    ; Print version using built-in functions
    ld aw, [math_version]
    call nc, print_version
    
    done:
        jpb nc, done            ; Infinite loop to end program

print_version:
    ; Print version number
    ; Implementation details...
    ret nc

.section .bss
result: .ds 2
```

Compilation process:
```bash
# Compile to object files
tm32asm -c math.tmm -o math.tmo
tm32asm -c main.tmm -o main.tmo

# Link object files
tm32link math.tmo main.tmo -o program.tm32
```

## 10. Quality Assurance and Validation

The TMM assembler includes built-in quality assurance features and integrates seamlessly with the TM32 ROM Validator (`tm32validate`) to ensure code quality and TM32 Core System compliance.

### 10.1 Built-in Assembler Validation

The TMM assembler performs several validation checks during the assembly process:

#### 10.1.1 Syntax and Structure Validation

**Memory Layout Compliance**:
- Section address range validation
- Interrupt vector alignment verification
- Memory region boundary checking
- Section size constraint enforcement

**Symbol Table Validation**:
- Undefined symbol detection
- Symbol redefinition checking
- Symbol visibility verification
- Cross-reference validation

**Instruction Set Compliance**:
- Opcode validity verification
- Operand type checking
- Addressing mode validation
- Condition code verification

#### 10.1.2 TM32 Core System Compliance

**Metadata Section Validation**:
```assembly
; TMM assembler validates metadata structure
.metadata
.org 0x0000
    .db "TM32CORE", 0x00        ; Magic string validation
.org 0x0008
    .dd 0x00010000              ; Version format validation
    .dd 0x00000000              ; Reserved field verification
.org 0x0010
    .validate_metadata_size 4096  ; Size constraint checking
```

**Interrupt Vector Validation**:
```assembly
; TMM validates interrupt vector structure
.int 1
vblank_isr:
    .validate_isr_entry         ; Entry point validation
    push a
    ; ISR body...
    pop a
    reti                        ; Exit instruction validation
    .validate_isr_size 256      ; Size constraint checking
```

#### 10.1.3 Assembler Validation Directives

The TMM assembler provides comprehensive validation directives to ensure code quality, performance, and TM32 Core System compliance. These directives perform compile-time checks and generate warnings or errors when violations are detected.

##### Section and Memory Validation

**`.validate_section <constraints>`**: Validates current section against specified constraints
- **Syntax**: `.validate_section constraint1[, constraint2, ...]`
- **Constraints**:
  - `size<=<value>`: Maximum section size in bytes
  - `size>=<value>`: Minimum section size in bytes
  - `align=<value>`: Required alignment (must be power of 2)
  - `writable=<bool>`: Section writability requirement
  - `executable=<bool>`: Section execution permission requirement
- **Examples**:
  ```assembly
  .validate_section size<=65536, align=16
  .validate_section writable=true, executable=false
  .validate_section size>=1024, align=4
  ```

**`.validate_memory <start>, <end>, <permissions>`**: Validates memory region usage
- **Syntax**: `.validate_memory <start_addr>, <end_addr>, <permission_flags>`
- **Permission Flags**: `read`, `write`, `exec`, combined with `|` (e.g., `read|write`)
- **Purpose**: Ensures memory access stays within permitted regions
- **Examples**:
  ```assembly
  .validate_memory 0x80000000, 0x80010000, read|write
  .validate_memory 0x00003000, 0x7FFFFFFF, read|exec
  .validate_memory 0x00000000, 0x00000FFF, read
  ```

**`.validate_alignment <value>`**: Ensures data/code alignment requirements
- **Syntax**: `.validate_alignment <alignment_value>`
- **Requirements**: `<alignment_value>` must be a positive power of 2
- **Purpose**: Validates that current assembly address is aligned to specified boundary
- **Examples**:
  ```assembly
  .validate_alignment 4     ; Ensure 4-byte alignment
  .validate_alignment 16    ; Ensure 16-byte alignment
  .validate_alignment 2     ; Ensure word alignment
  ```

##### Symbol and Function Validation

**`.validate_symbol <symbol>, <type>, <constraints>`**: Validates symbol properties
- **Syntax**: `.validate_symbol <symbol_name>, <symbol_type>, <constraint_list>`
- **Symbol Types**: `function`, `object`, `notype`, `section`, `file`
- **Constraints**: `global`, `local`, `weak`, `defined`, `size=<value>`
- **Examples**:
  ```assembly
  .validate_symbol main_loop, function, global
  .validate_symbol player_data, object, size=64
  .validate_symbol temp_buffer, object, local|defined
  ```

**`.validate_function_entry`**: Marks function entry point for validation
- **Purpose**: Enables function-level validation including stack tracking and call analysis
- **Placement**: Should be the first directive after function label
- **Example**:
  ```assembly
  calculate_distance:
      .validate_function_entry
      push c
      ; Function implementation...
  ```

**`.validate_function_exit`**: Validates function exit conditions
- **Purpose**: Ensures proper function cleanup (stack balance, register restoration)
- **Placement**: Should precede return instructions
- **Example**:
  ```assembly
      pop c
      .validate_function_exit
      ret nc
  ```

##### Stack and Register Validation

**`.validate_stack_entry`**: Begins stack usage tracking
- **Purpose**: Monitors stack push/pop operations for balance verification
- **Usage**: Place at function entry after `.validate_function_entry`
- **Example**:
  ```assembly
  safe_function:
      .validate_function_entry
      .validate_stack_entry
      push a
      push b
  ```

**`.validate_stack_exit`**: Validates stack balance before function exit
- **Purpose**: Ensures all pushed values have been popped
- **Usage**: Place before `.validate_function_exit`
- **Example**:
  ```assembly
      pop b
      pop a
      .validate_stack_exit
      .validate_function_exit
      ret nc
  ```

##### Interrupt and System Validation

**`.validate_isr_entry`**: Marks interrupt service routine entry point
- **Purpose**: Validates ISR structure and ensures proper interrupt handling patterns
- **Requirements**: Must be followed by proper register preservation
- **Example**:
  ```assembly
  vblank_isr:
      .validate_isr_entry
      push a
      push b
      ; ISR implementation...
  ```

**`.validate_isr_size <size>`**: Ensures ISR size constraints
- **Syntax**: `.validate_isr_size <max_bytes>`
- **Purpose**: Validates that interrupt handler fits within allocated vector space
- **TM32 Requirement**: Each ISR must be ≤ 256 bytes
- **Example**:
  ```assembly
      pop b
      pop a
      reti
      .validate_isr_size 256
  ```

**`.validate_metadata_size <size>`**: Ensures metadata section size compliance
- **Syntax**: `.validate_metadata_size <required_bytes>`
- **Purpose**: Validates metadata section meets platform requirements
- **TM32 Requirement**: Metadata section must be exactly 4096 bytes
- **Example**:
  ```assembly
  .metadata
  ; Metadata content...
  .validate_metadata_size 4096
  ```

##### Data Integrity and Security Validation

**`.validate_checksum <algorithm>`**: Adds checksum validation for data integrity
- **Syntax**: `.validate_checksum <algorithm_name>`
- **Algorithms**: `crc32`, `md5`, `sha256`, `simple_sum`
- **Purpose**: Generates and validates checksums for critical data sections
- **Examples**:
  ```assembly
  .rodata
  critical_data:
      .db 0x12, 0x34, 0x56, 0x78
      .validate_checksum crc32
  
  .validate_checksum sha256    ; For security-critical sections
  ```

##### Advanced Analysis Directives

**`.analyze_function <name>, <properties>`**: Enables static analysis for functions
- **Syntax**: `.analyze_function <function_name>, property1=value1[, property2=value2, ...]`
- **Properties**:
  - `recursive=<bool>`: Whether function is recursive
  - `max_stack=<bytes>`: Maximum stack usage
  - `pure=<bool>`: Function has no side effects
  - `const=<bool>`: Function doesn't modify global state
- **Example**:
  ```assembly
  .analyze_function main_loop, recursive=false, max_stack=64
  .analyze_function calculate_sin, pure=true, const=true
  ```

**`.analyze_variable <name>, <properties>`**: Enables static analysis for variables
- **Syntax**: `.analyze_variable <variable_name>, property1=value1[, property2=value2, ...]`
- **Properties**:
  - `type=<typename>`: Variable type hint
  - `size=<bytes>`: Variable size in bytes
  - `volatile=<bool>`: Variable may change unexpectedly
  - `const=<bool>`: Variable is read-only
- **Example**:
  ```assembly
  .analyze_variable player_data, type=struct, size=32
  .analyze_variable hardware_register, volatile=true, size=1
  ```

**`.analyze_memory_usage <start>, <end>, <usage_type>`**: Analyzes memory region usage patterns
- **Syntax**: `.analyze_memory_usage <start_addr>, <end_addr>, <usage_category>`
- **Usage Categories**: `heap`, `stack`, `dma`, `hardware`, `cache`
- **Purpose**: Provides memory usage analysis for optimization
- **Example**:
  ```assembly
  .analyze_memory_usage 0x80000000, 0x80010000, heap
  .analyze_memory_usage 0x40000000, 0x400000FF, hardware
  ```

##### Validation Control Directives

**`.validation_push`**: Saves current validation state
- **Purpose**: Temporarily disable or modify validation settings
- **Usage**: Paired with `.validation_pop`

**`.validation_pop`**: Restores previous validation state
- **Purpose**: Restore validation settings after temporary changes

**`.validation_disable <category>`**: Temporarily disable specific validation categories
- **Categories**: `stack`, `alignment`, `symbols`, `memory`, `performance`
- **Example**:
  ```assembly
  .validation_push
  .validation_disable alignment
  ; Code that requires relaxed alignment...
  .validation_pop
  ```

**`.validation_level <level>`**: Sets validation strictness level
- **Levels**: `0` (disabled), `1` (basic), `2` (standard), `3` (strict), `4` (paranoid)
- **Example**:
  ```assembly
  .validation_level 3    ; Enable strict validation
  ```

These validation directives work together to provide comprehensive quality assurance during the assembly process, helping developers create reliable, efficient, and TM32-compliant assembly code.

### 10.2 Integration with TM32 ROM Validator

#### 10.2.1 Assembler Output Validation

**Automatic Validation on Assembly**:
```bash
# Enable automatic validation during assembly
tm32asm --validate=basic file.tmm -o file.tmo
tm32asm --validate=standard file.tmm -o file.tmo
tm32asm --validate=comprehensive file.tmm -o file.tmo
```

**Validation Level Configuration**:
```bash
# Configure validation level in assembler
tm32asm --validation-level=2 --validation-rules=tm32core.rules file.tmm
```

#### 10.2.2 Build System Integration

**Makefile Integration with Validation**:
```makefile
# Enhanced Makefile with validation
ASMFLAGS = --validate=standard --warnings-as-errors
VALFLAGS = --level=2 --target=tm32core

%.tmo: %.tmm
	tm32asm $(ASMFLAGS) -o $@ $<
	tm32validate $(VALFLAGS) $@

%.tm32: $(OBJECTS)
	tm32link -o $@ $(OBJECTS)
	tm32validate --level=3 --performance $@

validate-all: $(OBJECTS) $(TARGET)
	tm32validate --batch --level=4 --security $(OBJECTS) $(TARGET)
```

### 10.3 Code Quality Guidelines

#### 10.3.1 Recommended Coding Standards

**Section Organization**:
```assembly
; Recommended file structure
.metadata
; Program metadata here

.int 0
crash_handler:
    ; Fatal error handler
    
.int 1
vblank_isr:
    ; VBlank interrupt handler
    
; Additional interrupt handlers...

.text
main:
    ; Main program entry point
    
; Function definitions...

.rodata
; Read-only data...

.data
; Initialized data...

.bss
; Uninitialized data...
```

**Symbol Naming Conventions**:
```assembly
; Use descriptive names with consistent conventions
.global main_loop           ; Function names: lowercase with underscores
.global player_data         ; Variable names: lowercase with underscores
.const MAX_ENEMIES = 10     ; Constants: UPPERCASE with underscores
.local .temp_buffer         ; Local symbols: prefixed with dot

; Hardware register access
.const hLCDC = 0x40         ; Hardware registers: h prefix
.const hSTAT = 0x41
```

**Documentation Standards**:
```assembly
; Function header documentation
; Function: calculate_distance
; Purpose: Calculate distance between two points
; Input: AX = x1, AY = y1, BX = x2, BY = y2
; Output: A = distance (fixed-point)
; Modifies: A, B, C, flags
; Stack usage: 8 bytes maximum
calculate_distance:
    .validate_function_entry
    push c
    ; Implementation...
    pop c
    ret nc
    .validate_function_exit
```

#### 10.3.2 Performance Guidelines

**Efficient Memory Access**:
```assembly
; Prefer aligned memory access
.align 4
aligned_data:
    .dd 0x12345678          ; 32-bit aligned

; Use appropriate data sizes
.align 2
sprite_data:
    .dw 0x1234, 0x5678      ; 16-bit aligned for efficiency
```

**Optimal Instruction Usage**:
```assembly
; Use efficient instruction sequences
; Preferred: Direct register operations
ld a, 100
add a, 50

; Avoid: Unnecessary memory operations
; ld a, 100
; stp [temp], a
; ldp a, [temp]
; add a, 50
```

#### 10.3.3 Safety Guidelines

**Stack Management**:
```assembly
; Always balance push/pop operations
safe_function:
    .validate_stack_entry
    push a                  ; Save registers
    push b
    
    ; Function body...
    
    pop b                   ; Restore in reverse order
    pop a
    ret nc
    .validate_stack_exit
```

**Hardware Register Safety**:
```assembly
; Safe hardware register access
safe_ppu_setup:
    ; Disable PPU during setup
    ld al, 0x00
    stp [hLCDC], al
    
    ; Configure PPU registers
    ld al, 0x91
    stp [hLCDC], al         ; Re-enable PPU
    ret nc
```

### 10.4 Error Handling and Debugging

#### 10.4.1 Assembler Error Messages

The TMM assembler provides detailed error messages with context information:

```
Error: tm32asm: file.tmm:45:12: Invalid instruction operand
   |
45 | ld a, invalid_register
   |           ^^^^^^^^^^^^^
   | Expected: register name or immediate value
   | Got: unrecognized identifier
   |
   | Valid operands for 'ld' instruction:
   |   Registers: a, aw, ah, al, b, bw, bh, bl, c, cw, ch, cl, d, dw, dh, dl
   |   Immediate: numeric literal or symbol reference
```

#### 10.4.2 Debug Information Generation

**Debug Symbol Generation**:
```bash
# Generate debug information
tm32asm --debug --source-map file.tmm -o file.tmo
```

**Source Line Mapping**:
```assembly
; Source line information is preserved in debug builds
.file "main.tmm"
.line 100
main_loop:                  ; Line 100 in main.tmm
    .line 101
    call nc, update_game    ; Line 101 in main.tmm
    .line 102
    jmp nc, main_loop       ; Line 102 in main.tmm
```

#### 10.4.3 Static Analysis Integration

**Code Analysis Directives**:
```assembly
; Static analysis hints
.analyze_function main_loop, recursive=false, max_stack=64
.analyze_variable player_data, type=struct, size=32
.analyze_memory_usage 0x80000000, 0x80010000, heap
```

### 10.5 Validation Configuration

#### 10.5.1 Validation Rule Files

**Custom Validation Rules** (tm32core.rules):
```yaml
# TMM assembler validation rules
assembler_rules:
  section_constraints:
    metadata:
      max_size: 4096
      required_fields: [magic, version, rom_size, wram_size]
    interrupt_vectors:
      max_size: 256
      required_exit: [reti]
      
  symbol_rules:
    naming_convention: "snake_case"
    max_symbol_length: 63
    reserved_prefixes: ["__tm32_", "_internal_"]
    
  instruction_rules:
    deprecated_warnings: true
    condition_code_required: ["jmp", "call", "ret", "jpb"]
    stack_balance_checking: true
    
  memory_rules:
    alignment_enforcement: true
    overlap_detection: true
    bounds_checking: true
```

#### 10.5.2 Project Configuration

**TMM Project Configuration** (.tmmconfig):
```ini
[assembler]
target = tm32core
validation_level = 2
warnings_as_errors = false
debug_info = true

[validation]
check_stack_usage = true
check_interrupt_safety = true
analyze_performance = false
security_analysis = false

[output]
object_format = tmo
preserve_comments = true
generate_listings = false
create_map_file = true

[paths]
include_paths = ["./include", "../common/include"]
library_paths = ["./lib", "../common/lib"]
```

## 11. Conclusion

The TMM assembly language and assembler tool provide a powerful and flexible
environment for developing programs targeting the TM32 virtual CPU architecture.
With its comprehensive set of directives, macros, conditional assembly features,
object file support, built-in functions, modern linking capabilities, and
integrated quality assurance features, TMM enables both simple single-file
programs and complex modular software projects while ensuring code quality
and TM32 Core System compliance.

Key features of TMM include:

- **Modular Development**: Object file format (.tmo) supporting separate compilation
  and linking for large-scale software development
- **Symbol Management**: Comprehensive symbol visibility control with global, extern,
  local, and weak symbol types
- **Built-in Functions**: Extensive library of functions for string manipulation,
  address calculation, mathematical operations, and fixed-point arithmetic
- **Advanced Directives**: Modern assembler features including section management,
  data definition with fill/skip operations, and flexible memory organization
- **Expression System**: Powerful expression evaluation with operator precedence
  and built-in function integration
- **Quality Assurance**: Integrated validation, static analysis, and code quality
  checking with seamless TM32 ROM Validator integration
- **Debugging Support**: Comprehensive debug information generation and error
  reporting for efficient development workflow
- **Compatibility**: Full support for TM32 CPU instruction set while maintaining
  clean, readable assembly syntax

TMM serves as the foundation for the TM32 development toolchain, providing
developers with the tools necessary to create efficient, maintainable, and
validated assembly language programs for the TM32 virtual CPU architecture.
The assembler tool ensures that programs are correctly assembled into binary
format, with robust error handling, quality assurance, and detailed reporting
to aid in debugging and development.
