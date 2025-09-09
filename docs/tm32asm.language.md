# Specification - TM32 Assembly Language

Below is a specification for the assembly language used by software targeting
the TM32 CPU architecture.

## 1. Overview

The **TM32 Assembly Language** is a haman-readable assembly language designed
for the TM32 CPU architecture. It provides a convenient, easy-to-read and
easy-to-learn syntax for writing low-level programs that target hardware powered
by the TM32 CPU architecture, including the TM32 Core. The language supports
all features of the architecture, including its general-purpose register file,
memory map, addressing modes, instruction set and interrupt system.

## 2. Source File Structure

A source file in the TM32 Assembly Language is a line-based text file which may
(and should) consist of the following structure:

- **File Extension**: `.tmm` (recommended), `.asm`, `.s`, `.inc`
    - Any file extension (or none at all) may be used, so as long as the text
        file contains valid TM32 assembly code. `.tmm` is the recommended
        extension for TM32 assembly source files.
- **Encoding**: UTF-8 (recommended), ASCII (supported)
    - Any text encoding may be used, so long as the text file contains valid
        TM32 assembly code. UTF-8 is the recommended encoding for TM32 assembly
        source files, as it supports a wide range of characters and symbols.
- **Line Endings**: LF (recommended), CRLF (supported)
- **Sections**: Source files may be divided into sections using the following
    directives:
    - `.metadata`: Indicates a section containing platform-specific metadata.
    - `.int xx`: Indicates a section containing code for one of the TM32's
        32 interrupt vectors, where `xx` is the interrupt vector number.
    - `.text`, `.code`: Indicates a section containing executable code.
    - `.rodata`: Indicates a section containing read-only data.
    - `.data`: Indicates a section containing initialized data.
    - `.bss`: Indicates a section containing uninitialized data.
- **Comments**: A semicolon (`;`) indicates the start of a comment. These lines,
    or the remainder thereof, are ignored by the assembler.
- **Address Labels**: An identifier followed by a colon indicates the definition
    of an address label for code and data locations.
- **Directives**: A period (`.`) followed by an identifier indicates a directive
    to be executed by the assembler, rather than the CPU.
- **Instructions**: An identifier followed by operands indicates a CPU
    instruction to be assembled and executed by the CPU.
- **Macros**: Using the `.macro`, `.def` and `.define` directives, users may 
    define macros for reusable code snippets with parameters (`.macro` only).
- **Conditional Assembly**: Using the `.if`, `.else`, `.elif`, and `.endif`
    directives, users may include or exclude code based on conditions.
- **Repetition**: Using the `.repeat` and `.endrepeat` directives, users may
    repeat code blocks a specified number of times.

## 3. Directives

Directives in the TM32 Assembly Language are commands to the assembler that
control the assembly process. They are not executed by the CPU, but rather at
different stages of the assembly process. There are three different types of
directives:

- **Preprocessor Directives**: These directives are processed after a source file
    is processed by the lexer, but before the assembler begins the process of
    interpreting the processed tokens for code generation. They are used to
    control the assembly process, such as including other files, defining
    macros, and setting assembly options. These directives mutate the sequence
    of tokens which are ultimately passed into the assembler.
- **Assembler Directives**: These directives are processed by the assembler during
    the assembly process. They are used to define sections, allocate or reserve
    memory, set the origin address, and define data values. These directives
    affect the generated machine code and the layout of the program in memory.

### 3.1 Preprocessor Directives

Preprocessor directives are commands to the assembler that are processed before
the assembly process begins. They are used to control the assembly process, such
as including other files, defining macros, and setting assembly options. These
directives mutate the sequence of tokens which are ultimately passed into the
assembler. The following preprocessor directives are supported:

- `.include "filename"`: Includes the contents of another source file at this
    point in the current source file. The filename must be enclosed in double
    quotes. When processed, the lexer will extract tokens from the included file,
    and the preprocessor will mutate these tokens as appropriate, before inserting
    them into the token stream at the point of inclusion.
    - The `"filename"` may be an absolute or relative path. Relative paths
        are resolved relative to the directory of the source file containing the
        `.include` directive.
    - Included files may themselves contain `.include` directives, allowing for
        nested inclusions.
    - If a file specified by an `.include` directive cannot be found or opened,
        the assembler will raise an error and halt the assembly process.
    - If a file specified by an `.include` directive was already included earlier
        in the assembly process, it will be ignored to prevent infinite inclusion
        loops.
- `.let <name>[ = <value>]`: Defines a mutable variable named `<name>`, optionally
    initializing it to `<value>`. If no value is provided, the variable is
    initialized to `0`. Variables defined with `.let` can be reassigned later
    in the source file (or another included file) using the same `.let` directive.
    The `.let` directive can also be used with certain assignment operators to
    modify the value of an existing variable. The supported assignment operators
    are specified later in this specification.
- `.const <name> = <value>`: Defines an immutable constant named `<name>`, and
    initializes it to `<value>`. Constants must be explicitly initialized at the
    time of definition, and cannot be reassigned later in the source file (or
    another included file). Attempting to reassign a constant will result in an
    error during the assembly process.
- `.define <name>=<snippet>`, `.def <name>=<snippet>`: Defines a simple text
    substitution macro named `<name>`, which will be replaced by `<snippet>`
    wherever it appears in the source file (or another included file) after
    its definition. The `<snippet>` can be any sequence of characters, including
    whitespace, but cannot contain line breaks. The substitution is purely
    textual, and does not involve any parsing or evaluation.
    - Simple macros defined with `.define` or `.def` do not support parameters.
    - If a simple macro is redefined later in the source file (or another
        included file), the new definition will override the previous one.
    - Simple text substitution macros can be used anywhere in the source file
        (or another included file) after their definition. Unlike `.macro`, they
        can even be used in the middle of other instructions and expressions.
- `.macro <name>`: Begins the definition of a macro named `<name>`. Macros are
    reusable code snippets that can take parameters. The macro definition ends
    with the `.endmacro` or `.endm` directive. When the macro is invoked by name,
    the preprocessor will replace the invocation with the contents of the macro,
    substituting any parameters as appropriate.
    - Macros can take any number of variadic parameters, which can be referenced
        within the macro body using positional notation (eg. `@1`, `@2`, etc.).
    - More on macros is specified later in this document.
- `.shift xx`: In a macro invocation, shifts the positional parameters by `xx`
    positions. This allows for more flexible parameter handling in macros.
    - For example, if a macro is defined with three parameters, and it is invoked
        with five arguments, using `.shift 2` within the macro body will allow
        the macro to access the third, fourth, and fifth arguments as `@1`, `@2`,
        and `@3` respectively.
    - The `.shift` directive can only be used within a macro definition.
    - The value of `xx` must be a non-negative integer. If `xx` is greater than
        the number of parameters passed to the macro, an error will be raised
        during the assembly process.
- `.if <condition>`: Begins a conditional assembly block. The code within the
    block will only be included in the assembled output if the `<condition>` is
    true. The conditional block ends with the `.endif` or `.endc` directive. An
    optional `.else`, `.elseif <condition>`, or `.elif <condition>` directive 
    may be used to provide alternative code blocks if the initial condition is
    false.
    - Conditions can include comparisons between variables, constants, and
        literal values using the operators specified later in this document.
    - Logical operators (AND, OR, NOT) can also be used to combine multiple
        conditions.
    - Nested conditional blocks are supported, and encouraged for complex
        conditions.
- `.repeat <count>`, `.rept <count>`: Begins a repetition block that will repeat
    the enclosed code `<count>` times. The repetition block ends with the
    `.endrepeat` or `.endr` directive.
    - The `<count>` must be a non-negative, non-zero integer, or an expression 
        which evaluates to one. If `<count>` is zero, the block will be skipped 
        entirely. If `<count>` is negative, an error will be raised during the 
        assembly process.
    - Nested repetition blocks are supported, and encouraged for complex code
        generation patterns.
- `.while <condition>`: Begins a while loop block that will repeat the enclosed
    code as long as the `<condition>` is true. The while loop block ends with
    the `.endwhile` or `.endw` directive.
    - Conditions can include comparisons between variables, constants, and
        literal values using the operators specified later in this document.
    - Logical operators (AND, OR, NOT) can also be used to combine multiple
        conditions.
    - Nested while loop blocks are supported, and encouraged for complex code
        generation patterns.
    - To prevent infinite loops, a maximum iteration count of 1,000 is enforced.
        If this limit is reached, an error will be raised during the assembly
        process.
- `.for <var> = <start> to <end> [step <step>]`: Begins a for loop block that
    will repeat the enclosed code with the variable `<var>` taking on values
    from `<start>` to `<end>`, incrementing by `<step>` each iteration. The
    for loop block ends with the `.endfor` or `.endf` directive.
    - The `<start>`, `<end>`, and optional `<step>` must be integers, or expressions
        which evaluate to integers. If `<step>` is not provided, it defaults to `1`.
    - If `<step>` is negative, the loop will decrement `<var>` each iteration.
    - If `<start>` is greater than `<end>` and `<step>` is positive, or if
        `<start>` is less than `<end>` and `<step>` is negative, the loop will
        not execute.
    - Nested for loop blocks are supported, and encouraged for complex code
        generation patterns.
- `.continue`: Skips the remainder of the current iteration of a loop and
    begins the next iteration. This directive can be used within `.repeat`,
    `.while`, and `.for` blocks.
- `.break`: Exits the current loop immediately. This directive can be used
    within `.repeat`, `.while`, and `.for` blocks.
- `.warn "message"`: Issues a warning with the specified message during
    the assembly process. The assembly will continue, but the warning will be
    logged for the user's attention.
- `.error "message"`: Issues an error with the specified message during
    the assembly process. The assembly will halt, and the error will be logged
    for the user's attention.
- `.assert <condition> ["message"]`: Asserts that the specified condition is
    true. If the condition is false, an error will be raised during the assembly
    process, optionally including the specified message.
    - Conditions can include comparisons between variables, constants, and
        literal values using the operators specified later in this document.
    - Logical operators (AND, OR, NOT) can also be used to combine multiple
        conditions.
- `.file "filename"`: Sets the current source file name for error reporting
    purposes. This directive is typically used in conjunction with the `.include`
    directive to ensure that error messages reference the correct file.
- `.line <number>`: Sets the current line number for error reporting purposes.
    This directive is typically used in conjunction with the `.include` directive
    to ensure that error messages reference the correct line number in the
    correct file.

### 3.2 Assembler Directives

Assembler directives are commands to the assembler that are processed during the
assembly process. They are used to define sections, allocate or reserve memory,
set the origin address, and define data values. These directives affect the
generated machine code and the layout of the program in memory. The following
assembler directives are supported:

- `.metadata`: Indicates the start of a metadata section. This section is used to
    define platform-specific metadata for the assembled program. The metadata
    section ends when another section directive is encountered, or at the end
    of the source file.
    - The metadata section can only contain data-defining directives (eg.
        `.byte`, `.word`, etc.), which should be used to define the metadata
        values. Documentation and commentary on these fields is very highly
        encouraged.
    - Depending on the target platform, the metadata section may or may not be
        required, but it is highly recommended for target platforms to require
        a metadata section for all assembled programs.
    - The metadata section is typically placed at the beginning of the source
        file, before any interrupt or code sections.
- `.int xx`: Indicates the start of a section containing code for one of the
    TM32's 32 interrupt vectors, where `xx` is the interrupt vector number
    (0-31). The interrupt section ends when another section directive is
    encountered, or at the end of the source file.
    - Each interrupt vector has 256 bytes of space allocated to it. If the code
        within the interrupt section exceeds this limit, an error will be raised
        during the assembly process. If the code exceeds this limit, the
        programmer should consider using a `jmp` instruction to jump to a larger
        code section elsewhere in the program space.
    - If an interrupt section is not defined for a particular interrupt vector,
        the interrupt vector will default to a `nop` instruction followed by a
        `reti` instruction.
    - The interrupt sections are typically placed after the metadata section,
        and before any code sections.
- `.text`, `.code`: Indicates the start of a section containing executable code.
    The code section ends when another section directive is encountered, or at
    the end of the source file.
    - The code section can contain any valid TM32 assembly instructions, as well
        as data-defining directives (eg. `.byte`, `.word`, etc.) for inline
        data, although it is generally recommended to use the `.rodata`, `.data`,
        and `.bss` sections for data definitions.
    - The code section is typically placed after the interrupt sections, and
        before any data sections.
- `.rodata`: Indicates the start of a section containing read-only program data.
    The read-only data section ends when another section directive is encountered,
    or at the end of the source file.
    - The read-only data section can only contain data-defining directives
        (eg. `.byte`, `.word`, etc.) for defining read-only data values.
    - The read-only data section is typically placed after the code section,
        and before the initialized data section.
- `.data`: Indicates the start of a section containing initialized data in RAM.
    The initialized data section ends when another section directive is
    encountered, or at the end of the source file.
    - The initialized data section can only contain data-defining directives
        (eg. `.byte`, `.word`, etc.) for defining initialized data values.
    - The initialized data section is typically placed after the read-only
        data section, and before the uninitialized data section.
- `.bss`: Indicates the start of a section containing uninitialized data in RAM.
    The uninitialized data section ends when another section directive is
    encountered, or at the end of the source file.
    - The uninitialized data section can only contain data-reserving directives
        (eg. `.byte`, `.word`, etc.) for defining uninitialized data values.
    - The data definition directives have different behavior in the `.bss` section
        compared to other sections. In the `.bss` section, these directives
        reserve a number of bytes/words/double words in memory, but do not
        initialize them to any specific value. The reserved memory is typically
        initialized to zero at runtime by the system startup code.
    - The uninitialized data section is typically placed after the initialized
        data section, and at the end of the source file.
- `.org <address>`: Sets the origin address for the subsequent code or data
    definitions. The `<address>` must be a valid memory address within an
    address range which will depend upon the target platform and the current
    section. The origin address can be changed multiple times within a source
    file, but care must be taken to ensure that code and data do not overlap
    in memory.
    - The origin address must be aligned to the size of the data being defined.
        For example, if defining a word (2 bytes), the origin address must be
        even. If defining a double word (4 bytes), the origin address must be
        a multiple of 4.
    - If the origin address is set to an address that is not within the valid
        range for the current section, an error will be raised during the
        assembly process.
    - The origin address is typically set at the beginning of each section, but
        can be changed as needed within the section.
    - Before any instruction or data definition, this address is checked to
        ensure it is within the valid range for the current section. If it is
        not, an error will be raised during the assembly process.
    - The following address ranges are valid for the following sections:
        - `.metadata`: `0x0000` to `0x0FFF` (first 4KB of memory)
        - `.int xx`: `0x1000 + (xx * 0x100)` to `0x10FF + (xx * 0x100)` (256 bytes
            per interrupt vector, starting at `0x1000`)
        - `.text`, `.code`, `.rodata`: `0x3000` to `0x7FFFFFFF` (up to 2GB of
            memory, depending on the target platform)
        - `.data`, `.bss`: `0x80000000` to `0xFFFDFFFF` (up to 2GB of memory,
            depending on the target platform), `0xFFFF0000` to `0xFFFFFFFF`
            (the 64 KB of Quick RAM / I/O Registers)
- `.db <data1>[, <data2>, ...]`, `.byte <data1>[, <data2>, ...]`: Defines one or
    more bytes of data (8 bits each). Each `<data>` can be an integer literal,
    a character literal, an ASCII string literal, a 1BPP graphics literal, a
    variable, a constant, or an expression that evaluates to an integer value,
    truncated to 8 bits.
    - If a graphics literal is provided, it will be treated as a 1BPP graphics
        literal, and translated to its corresponding 8-bit value.
    - If a string literal is provided, each character in the string will be
        converted to its corresponding ASCII value and stored as a byte. The
        string is not null-terminated, and will need to be null-terminated
        explicitly by the programmer, if desired.
    - If an expression is provided, it will be evaluated at assembly time, and
        the resulting value will be truncated to 8 bits and stored as a byte.
    - Multiple data values can be defined in a single directive, separated by
        commas. The data values will be stored in memory in the order they are
        defined.
    - This directive works differently in the `.bss` section, where it takes
        only one argument, which specifies the number of bytes to reserve
        in memory, without initializing them to any specific value.
- `.dw <data1>[, <data2>, ...]`, `.word <data1>[, <data2>, ...]`: Defines one or
    more words of data (16 bits each). Each `<data>` can be an integer literal,
    a character literal, a 2BPP graphics literal, a variable, a constant, or an
    expression that evaluates to an integer value, truncated to 16 bits.
    - If a graphics literal is provided, it will be treated as a 2BPP graphics
        literal, and translated to its corresponding 16-bit value.
    - If an expression is provided, it will be evaluated at assembly time, and
        the resulting value will be truncated to 16 bits and stored as a word.
    - Multiple data values can be defined in a single directive, separated by
        commas. The data values will be stored in memory in the order they are
        defined.
    - This directive works differently in the `.bss` section, where it takes
        only one argument, which specifies the number of words to reserve
        in memory, without initializing them to any specific value.
- `.dd <data1>[, <data2>, ...]`, `.dword <data1>[, <data2>, ...]`: Defines one or
    more double words of data (32 bits each). Each `<data>` can be an integer
    literal, 4BPP graphics literal, a variable, a constant, or an
    expression that evaluates to an integer value, truncated to 32 bits.
    - If a graphics literal is provided, it will be treated as a 4BPP graphics
        literal, and translated to its corresponding 32-bit value.
    - If an expression is provided, it will be evaluated at assembly time, and
        the resulting value will be truncated to 32 bits and stored as a double
        word.
    - Multiple data values can be defined in a single directive, separated by
        commas. The data values will be stored in memory in the order they are
        defined.
    - This directive works differently in the `.bss` section, where it takes
        only one argument, which specifies the number of double words to reserve
        in memory, without initializing them to any specific value.
- `.asciz "string1"[, "string2", ...]`: Defines one or more null-terminated ASCII
    strings. Each string must be enclosed in double quotes. The strings will be
    stored in memory in the order they are defined, with each string followed
    by a null terminator (`0x00`).
    - Multiple strings can be defined in a single directive, separated by
        commas.
    - This directive is not allowed in the `.bss` section, as it requires
        initialization of memory.
- `.align <boundary>`: Aligns the next data or code definition to the specified
    `<boundary>`. The `<boundary>` must be a power of two (1, 2, 4, 8, 16, etc.).
    - For example, `.align 4` will align the next definition to a 4-byte boundary.
    - If the current address is already aligned to the specified boundary, no
        action is taken.
    - This directive is useful for ensuring that data and code are aligned to
        specific boundaries, which can improve performance on some architectures.
- `.ds <size>`, `.space <size>`: Reserves a block of memory of the specified
    `<size>` in bytes. The reserved memory is not initialized to any specific
    value, and will typically be initialized to zero at runtime by the system
    startup code.
    - The `<size>` must be a non-negative integer. If `<size>` is zero, no
        memory will be reserved. If `<size>` is negative, an error will be
        raised during the assembly process.
    - This directive is not allowed outside of the `.bss` section, as it is
        intended for reserving uninitialized data.
- `.incbin "filename"[, <offset>[, <length>]]`: Includes a binary file at this
    point in the current source file. The filename must be enclosed in double
    quotes. When processed, the assembler will read the specified binary file
    and include its contents in the assembled output at the current location.
    - The `"filename"` may be an absolute or relative path. Relative paths
        are resolved relative to the directory of the source file containing the
        `.incbin` directive.
    - If an `<offset>` is provided, it specifies the number of bytes to skip
        from the beginning of the binary file before including its contents.
        If no offset is provided, it defaults to `0`.
    - If a `<length>` is provided, it specifies the number of bytes to include
        from the binary file after applying the offset. If no length is provided,
        it defaults to including all remaining bytes in the file after the offset.
    - If a file specified by an `.incbin` directive cannot be found or opened,
        the assembler will raise an error and halt the assembly process.
    - If the specified `<offset>` is greater than or equal to the size of the
        binary file, no data will be included.
    - If the specified `<length>` exceeds the remaining size of the binary file
        after applying the offset, only the available bytes will be included.

## 4. Expressions and Operators

Expressions in the TM32 Assembly Language can be used in various contexts,
such as in directives, instructions, and data definitions. Expressions can
include literals, variables, constants, labels, and operators. The following
types of literals and operators are supported:

### 4.1 Literals

- **Integer Literals**: Binary (`0b0101`), Octal (`0o123`), Decimal (`123`),
    Hexadecimal (`0x7B`).
- **Fixed-Point Literals**: Represented as a floating-point number (eg. `3.14`,
    `0.5`, `-2.75`).
    - Fixed-point literals are internally converted into a 64-bit unsigned
        integer representation, in which the upper 32 bits represent the integer
        part, and the lower 32 bits represent the fractional part.
- **Character Literals**: Enclosed in single quotes (eg. `'A'`, `'\n'`, `'\x41'`).
- **Graphics Literal**: RGBDS-style, Game Boy-inspired graphics literals are a
    sequence of specific numbers prefixed by a single backtick (`` ` ``). The
    number of digits, and what those digits can be depend upon the specific
    graphics format being used (e.g., 1BPP, 2BPP)
    - **One Bit Per Pixel (1BPP)**: After the backtick, eight digits between 0
        and 1 are provided, resulting in one byte of tile data which would
        produce that row of pixels.
        - *Example*: `` `01011001 `` would produce the byte `0x59`.
    - **Two Bits Per Pixel (2BPP)**: After the backtick, eight digits between 0
        and 3 are provided, resulting in two bytes of tile data which would
        produce that row of pixels.
        - *Example*: `` `01012323 `` would produce the word `0x0F55`.
    - **Four Bits Per Pixel (4BPP)**: After the backtick, eight digits between 0
        and 15 are provided, resulting in four bytes of tile data which would
        produce that row of pixels.
        - *Example*: `` `01234567 `` would produce the double word `0x55330F00`.
- **String Literals**: Enclosed in double quotes (eg. `"Hello, World!"`).
    - Escape sequences are supported (eg. `"\n"`, `"\t"`, `"\x41"`).
    - String literals can be used in data definition directives (eg. `.db`,
        `.asciz`), but not in instructions or other contexts where an integer
        value is expected.
- **Identifiers**: Variable and constant names used in the `.let` and `.const`
    directives, as well as address labels defined in the source file and other
    files included via the `.include` directive (postfixed by a colon).
    - Identifiers must start with a letter (A-Z, a-z), underscore (`_`) or
        period (`.`), followed by any combination of letters, digits (0-9), 
        underscores and periods.
    - Identifiers are case-sensitive (eg. `MyVar` and `myvar` are different).
    - Identifiers cannot be the same as reserved keywords (eg. directive names,
        instruction names, etc.).

### 4.2 Operators

- **Arithmetic Operators**: `+`, `-`, `*`, `**`, `/`, `%` (for addition, subtraction,
    multiplication, exponentiation, division, and modulus respectively).
- **Bitwise Operators**: `&`, `|`, `^`, `~`, `<<`, `>>` (for AND, OR, XOR, NOT,
    left shift, and right shift respectively).
- **Assignment Operators**: `=`, `+=`, `-=`, `*=`, `**=`, `/=`, `%=`, `&=`, `|=`, 
    `^=`, `<<=`, `>>=` (for assignment and compound assignment respectively).
- **Comparison Operators**: `==`, `!=`, `<`, `>`, `<=`, `>=` (for equality, 
    inequality, less than, greater than, less than or equal to, and greater than
    or equal to respectively).
- **Logical Operators**: `&&`, `||`, `!` (for logical AND, logical OR, and
    logical NOT respectively).
- **Ternary Operator**: `? :` (for conditional expressions, in the form
    `<condition> ? <true_value> : <false_value>`).

#### 4.2.1 Operator Precedence

Expressions in the TM32 Assembly Language are evaulated according to the following
operator precedence:

1. **Logical NOT, Bitwise NOT**: `!`, `~` (highest precedence, evaluated first)
2. **Exponentiation**: `**`
3. **Multiplication/Division**: `*`, `/`, `%`
4. **Addition/Subtraction**: `+`, `-`
5. **Bitwise Shift**: `<<`, `>>`
6. **Bitwise AND**: `&`
7. **Bitwise XOR**: `^`
8. **Bitwise OR**: `|`
9. **Comparison**: `==`, `!=`, `<`, `>`, `<=`, `>=`
10. **Logical AND**: `&&`
11. **Logical OR**: `||`
12. **Ternary Operators**: `? :` (lowest precedence, evaluated last)

## 4.3 Built-in Functions

The TM32 Assembly Language supports a collection of built-in functions which can
be used within expressions. These functions perform common operations and
calculations. The following built-in functions are supported:

### 4.3.1 Integer Functions

- `high(x), low(x)`: Returns the upper and lower halves, respectively, of an
    integer value `x`. For a 32-bit integer, these are the upper and lower
    words (16 bits each). For a 16-bit integer, these are the upper and lower
    bytes (8 bits each). For an 8-bit integer, these are the upper and lower
    nibbles (4 bits each).
- `bitwidth(x)`: Returns the minimum number of bits required to represent the
    integer value `x`.

### 4.3.2 Fixed-Point Functions

- `int(x)`: Returns the 32-bit integer part of a fixed-point value `x`.
- `fraction(x)`, `frac(x)`: Returns the 32-bit integer representation of the
    fractional part of a fixed-point value `x`.
- `round(x)`, `ceil(x)`, `floor(x)`: Returns the nearest integer to a fixed-point value
    `x`, rounding up or down as appropriate.

### 4.3.3 Trigonometric Functions

The trigonometric functions operate on angles measured in "turns", where 1.0 turn
equals 360 degrees or 2π radians. This makes it convenient for working with
circular mathematics and animations:

- `sin(x)`: Returns the sine of angle `x`.
- `cos(x)`: Returns the cosine of angle `x`.
- `tan(x)`: Returns the tangent of angle `x`.
- `asin(x)`: Returns the inverse sine (arcsine) of `x`, where `x` is in the
    range [-1.0, 1.0]. The result is an angle in the range [-0.25, 0.25] turns.
- `acos(x)`: Returns the inverse cosine (arccosine) of `x`, where `x` is in the
    range [-1.0, 1.0]. The result is an angle in the range [0.0, 0.5] turns.
- `atan(x)`: Returns the inverse tangent (arctangent) of `x`. The result is an
    angle in the range [-0.25, 0.25] turns.
- `atan2(y, x)`: Returns the angle between the positive x-axis and the point
    (x, y), taking into account the signs of both arguments to determine the
    correct quadrant. The result is an angle in the range [-0.5, 0.5] turns.

These functions are particularly useful for generating lookup tables, animation
curves, and mathematical calculations. For example, to generate a sine wave
table with 128 entries covering a quarter circle:

```assembly
.const TABLE_SIZE = 128
.rodata
sine_table:
.for i = 0 to TABLE_SIZE
    .db int((sin(i / (TABLE_SIZE * 4.0)) * 127.0) + 128.0)
.endfor
```

### 4.3.4 String Functions

- `strlen(s)`: Returns the length of the string `s`, excluding the null terminator.
- `strupper(s)`, `strupr(s)`: Returns a new string with all characters in `s` 
    converted to uppercase.
- `strlower(s)`, `strlwr(s)`: Returns a new string with all characters in `s` 
    converted to lowercase.
- `strcat(s1, s2)`: Returns a new string that is the concatenation of strings `s1`
    and `s2`.
- `strcmp(s1, s2)`: Compares two strings `s1` and `s2`. Returns `0` if they are
    equal, a negative value if `s1` is less than `s2`, and a positive value if
    `s1` is greater than `s2`.
- `strfind(s, sub)`, `strrfind(s, sub)`: Returns the index of the first (or last)
    occurrence of the substring `sub` in the string `s`, or `-1` if not found.
- `strbyte(s, index)`: Returns the byte value at the specified `index` in the string `s`.

### 4.3.5 Symbol and Meta Functions

- `defined(name)`: Returns `1` if the variable, constant, or label `name` is
    defined, otherwise returns `0`.
- `isconst(name)`: Returns `1` if `name` is defined as a constant, otherwise
    returns `0`.
- `isvar(name)`: Returns `1` if `name` is defined as a variable (mutable or
    constant), otherwise returns `0`.
- `section()`: Returns a  16-bit integer representing the current section:
    - `0x000`: Metadata section
    - `0x100` to `0x11F`: Interrupt section (0x00 to 0x1F)
    - `0x200`: Code section
    - `0x201`: Read-only data section
    - `0x300`: Initialized data section
    - `0x301`: Uninitialized data section
- `origin()`: Returns the current origin address as a 32-bit integer.
- `sizeof(name)`: Returns the size in bytes of the variable, constant, or label
    `name`. For labels, this is the size of the data or code defined at that
    label. For variables and constants, this is the size of their type (1 byte
    for byte, 2 bytes for word, 4 bytes for double word).
- `startof(name)`: Returns the starting address of a label `name` as a 32-bit
    integer.

## 4.4 Expression Contexts

Expressions can be used in various contexts within the TM32 Assembly Language,
including:

- **Preprocessor Directives**: Expressions can be used in preprocessor directives
    such as `.let`, `.const`, `.if`, `.elseif`, `.elif`, and `.assert` to define
    variable and constant values, and to evaluate conditions for conditional
    assembly.
- **Assembler Directives**: Expressions can be used in assembler directives such
    as `.org`, `.db`, `.dw`, `.dd`, `.asciz`, and `.ds` to define origin addresses,
    data values, and memory sizes.
- **Instructions**: Expressions can be used as operands in instructions to
    specify immediate values, addresses, and offsets.
- **Data Definitions**: Expressions can be used in data definition directives to
    specify the values of data elements.

## 4.5 Expression Evaluation

Expressions are evaluated during two different stages of the assembly process:

- **Preprocessor Stage**: Expressions used in preprocessor directives (eg. `.let`,
    `.const`, `.if`, etc.) are evaluated during the preprocessor stage, before
    the assembly process begins. The results of these evaluations are used to
    control the assembly process, such as defining variable values and
    determining which code blocks to include.
- **Assembly Stage**: Expressions used in assembler directives (eg. `.org`,
    `.db`, `.dw`, etc.), data definitions, and instructions are evaluated during
    the assembly and code generation stage. The results of these evaluations
    are used to generate the final machine code and layout of the program in
    memory.

## 5. Instructions

Instructions in the TM32 Assembly Language correspond directly to the
instructions in the TM32 CPU architecture. Each instruction consists of an
opcode and one or more operands. The opcode specifies the operation to be
performed, while the operands specify the data to be operated on. The following
types of operands are supported:

- **Registers**: Any of the TM32 CPU's general-purpose registers.
    - `a`, `b`, `c`, `d`: Full 32-bit registers.
    - `aw`, `bw`, `cw`, `dw`: Lower 16 bits of the corresponding
        full register.
    - `ah`, `bh`, `ch`, `dh`: Upper 8 bits of the lower 16 bits of the
        corresponding full register.
    - `al`, `bl`, `cl`, `dl`: Lower 8 bits of the corresponding full register.
- **Execution Conditions**: For the control-transfer instructions (`jmp`, `jpb`,
    `call`, and `ret`), an execution condition must be specified as the first
    operand. The supported execution conditions are:
    - `nc`: No condition (always execute).
    - `zs`: Zero set (execute if the zero flag is set).
    - `zc`: Zero clear (execute if the zero flag is clear).
    - `cs`: Carry set (execute if the carry flag is set).
    - `cc`: Carry clear (execute if the carry flag is clear).
    - `ps`: Parity set (execute if the parity flag is set).
    - `pc`: Parity clear (execute if the parity flag is clear).
    - `ss`: Sign set (execute if the sign flag is set).
    - `sc`: Sign clear (execute if the sign flag is clear).
- **Immediate Values**: Integer literals, fixed-point literals, character
    literals, graphics literals, variables, constants, or expressions that evaluate
    to an integer value.
    - For fixed-point literals, the 32-bit portion representing the integer part
        is used as the immediate value, and the fractional part is discarded.
- **Memory Addresses**: Direct memory addresses, labels, or expressions that
    evaluate to a memory address. These must be enclosed in square brackets (eg.
    `[0x1000]`, `[label]`, `[B]`, `[A + 4]`). These can be:
    - **Direct**: A direct memory address (`[0x12345678]`, `[0x10FF]`, `[0x46]`),
        or an expression that evaluates to a direct memory address. Whether this 
        is absolute or relative will depend on the instruction being fed the 
        operand (eg. `LD` expects a full 32-bit absolute address, while `LDH` 
        (Load High) expects a 16-bit address relative to address  `0xFFFF0000` 
        and `LDP` (Load Port) expects an 8-bit address relative to  address 
        `0xFFFFFF00`).
    - **Register Indirect**: An absolute or relative memory address stored in a
        register (`[A]`, `[B]`, `[C]`, `[D]`, `[AW]`, `[BW]`, `[CW]`, `[DW]`,
        `[AH]`, `[BH]`, `[CH]`, `[DH]`, `[AL]`, `[BL]`, `[CL]`, `[DL]`).
    - **Address Label**: A memory address defined by an address label in the
        source file or another included file (`[label]`, `[start]`, `[loop]`).
    - **Register Offset**: A memory address calculated by adding/subtracting an 
        immediate value (or an expression evaluating to one) to/from a register 
        value (`[A + 4]`, `[BW - 2]`, `[CH + 0x10]`, `[DL - 1]`).
    - **Port Addresses**: For the port I/O instructions (`ldp`, `stp`), an 8-bit
        port address must be specified as the operand, either as an immediate
        value, a variable, a constant, or an expression that evaluates to an
        8-bit integer value.

### 5.1 Instruction Format and Syntax

All TM32 instructions follow a consistent format:

```assembly
instruction [operand1[, operand2]]
```

Instructions are case-insensitive, but operands (particularly register names and 
labels) maintain their defined case sensitivity. Each instruction may take zero, 
one or two operands depending on the specific instruction.

### 5.2 Register Usage Guidelines

When writing TM32 assembly code, consider these register usage conventions:

- **Register A**: Primary accumulator for arithmetic and logical operations
- **Register B**: Secondary operand and general-purpose storage
- **Register C**: Loop counters and array indexing
- **Register D**: Data manipulation and temporary storage

These are suggestions for consistency and readability - the TM32 architecture 
allows any general-purpose register to be used in any supported context.

#### Sub-register Usage Patterns

The TM32's hierarchical register structure allows efficient handling of 
different data sizes:

- **32-bit operations**: Use full registers (`A`, `B`, `C`, `D`) for addresses, 
    large integers, and fixed-point values
- **16-bit operations**: Use word registers (`AW`, `BW`, `CW`, `DW`) for 
    medium-range values and offset calculations
- **8-bit operations**: Use byte registers (`AH/AL`, `BH/BL`, `CH/CL`, `DH/DL`) 
    for character data, flags, and small counters

#### Register Preservation

When writing subroutines and interrupt handlers:

- Document which registers are modified by the routine
- Preserve caller-saved registers when necessary using `PUSH`/`POP`
- Consider using less commonly used registers for temporary storage to minimize 
    stack operations

### 5.3 Instruction Categories

The TM32 instruction set includes several categories of instructions:

- **Data Transfer**: Move data between registers, memory, and immediate values 
    (`LD`, `ST`, `MV`, `PUSH`, `POP`)
- **Arithmetic**: Mathematical operations (`ADD`, `SUB`, `INC`, `DEC`, `ADC`, 
    `SBC`)
- **Logical**: Bitwise operations (`AND`, `OR`, `XOR`, `NOT`)
- **Shift/Rotate**: Bit manipulation (`SLA`, `SRA`, `SRL`, `RL`, `RR`, `RLC`, 
    `RRC`)
- **Comparison**: Value comparison (`CMP`)
- **Bit Operations**: Individual bit manipulation (`BIT`, `SET`, `RES`, `TOG`)
- **Control Transfer**: Program flow control (`JMP`, `JPB`, `CALL`, `RET`, 
    `INT`)
- **System**: CPU state control (`NOP`, `HALT`, `STOP`, `DI`, `EI`)

### 5.4 Addressing Modes

The TM32 supports several addressing modes for memory operations:

- **Immediate**: Direct values (`LD A, 0x1234`)
- **Direct**: Absolute memory addresses (`LD A, [0x12345678]`, 
    `ST [0x12345678], A`)
- **Register Indirect**: Address stored in register (`LD A, [B]`, `ST [C], A`)
- **High Page**: 16-bit address relative to `0xFFFF0000` (`LDH A, [0x1234]`, 
    `STH [0x1234], A`)
- **Port**: 8-bit address relative to `0xFFFFFF00` (`LDP AL, [0x34]`, 
    `STP [0x34], AL`)

### 5.5 Complete Instruction Reference

For a comprehensive list of all TM32 instructions, including detailed 
descriptions, opcodes, operand types, flag effects, cycle counts, and usage 
examples, please refer to the 
[TM32 CPU Complete Instruction Reference](tm32cpu.instructions.md).

Additionally, the [TM32 CPU Specification](tm32cpu.specification.md) provides 
detailed information about the underlying CPU architecture, register file, memory 
mapping, and instruction execution behavior.

## 6. Macros

Macros in the TM32 Assembly Language are reusable code snippets that can take
any variadic number of parameters. Macros are defined using the `.macro` directive,
and are invoked just like instructions or data definitions. When a macro is invoked,
the preprocessor will replace the invocation with the contents of the macro,
substituting any parameters as appropriate. The following rules apply to macros:

- **Definition**: A macro is defined using the `.macro <name>` directive, and
    ends with the `.endmacro` or `.endm` directive. Macro names are case-sensitive,
    and must follow the same rules as identifiers.
- **Parameters**: Macros can take any number of variadic parameters, which can be
    referenced within the macro body using positional notation (eg. `@1`, `@2`,
    etc.). The parameters are replaced with the corresponding arguments when the
    macro is invoked.
    - Positional arguments can be placed anywhere within the macro body, even
        inside of certain literal tokens (string literals, identifiers) in order
        to create new tokens.
    - An example of this will be provided later in this document.
    - **Special Parameters Tokens**:
        - `@0`: Expands to the name of the macro being invoked.
        - `@#`, `@NARG`: Expands to the number of arguments present in the macro invocation.
            The `.shift` directive will affect this value. `NARG` is case-insensitive.
        - `@*`: Expands to all arguments passed to the macro, separated by commas.
            This is useful for forwarding all arguments to another macro or
            instruction that takes the same arguments.
        - `@-`: Expands to all arguments except the first, separated by commas.
            This is useful for forwarding all arguments except the first to
            another macro or instruction that takes the same arguments.
        - `@@`: Expands to a single `@` character. This is useful for
            situations where an `@` character is needed in the macro body, but
            should not be interpreted as a parameter reference.
        - `@?`: Used in mangling of identifiers (like those used in `.let` and
            `.const` directives, and address labels) to create unique names based
            on the macro invocation context. This is particularly useful for
            creating local variables within macros that do not conflict with
            variables outside the macro.
            - When `@?` is used in an identifier, it will be replaced with a
                unique suffix based on the macro name and the line number of the
                macro invocation. This ensures that each invocation of the macro
                generates a unique identifier.
            - For example, if a macro named `MyMacro` is invoked on line 42 of
                the source file, any occurrence of `@?` within an identifier in
                the macro body will be replaced with `_MyMacro_42`.
            - This allows for the creation of local variables within macros
                without the risk of name collisions with variables defined
                outside the macro.
- **Body**: The body of the macro can contain any valid assembly code, including
    instructions, directives, and even other macro invocations. The body can
    span multiple lines, and can include comments.
- **Termination**: The macro definition must be terminated with the `.endmacro` or
    `.endm` directive. This indicates the end of the macro definition.
- **Invocation**: A macro is invoked just like an instruction or data definition
    would be invoked, with the same syntax and rules.
    - When a macro is invoked, the preprocessor will replace the invocation
        with the contents of the macro, substituting any parameters as
        appropriate.
- **Nesting**: Macros can be invoked within other macros, allowing for
    complex code generation patterns. Macros, however, cannot be defined within
    other macros.
- **Scope**: Macros are globally scoped, meaning they can be invoked from
    anywhere in the source file or any included files after their definition.
- **Recursion**: Macros can invoke themselves recursively, but care must be
    taken to avoid infinite recursion, which will result in an error during the
    assembly process.

### 6.1 The `.shift` Directive

The `.shift xx` directive can be used within a macro body to shift the positional
parameters passed into its invocation by `xx` positions. This allows for more
flexible parameter handling in macros. The following rules apply to the `.shift`
directive:

- **Usage**: The `.shift xx` directive is placed within the macro body, and
    shifts the positional parameters by `xx` positions. The value of `xx` must
    be a non-negative, non-zero integer, or an expression that evaluates to such.
- **Effect**: After the `.shift` directive is processed, the positional parameters
    are shifted by `xx` positions. This means that `@1` now refers to the
    `(xx + 1)`-th argument passed to the macro, `@2` refers to the `(xx + 2)`-th
    argument, and so on.
    - For example, if a macro is defined with three parameters, and it is invoked
        with five arguments, using `.shift 2` within the macro body will allow
        the macro to access the third, fourth, and fifth arguments as `@1`, `@2`,
        and `@3` respectively.
- **Limitations**: The `.shift` directive can only be used within a macro body.
    - If `xx` is greater than the number of parameters passed to the macro,
        an error will be raised during the assembly process.

### 6.2 The `.define` Directive (Simple Text Substitution)

Another, simpler form of macro definition and use is the simple text substitution
macro. These are defined using the `.define` or `.def` directives, and allow for
basic text replacement in the source code.

Unlike `.macro`, these simple text substitution macros do not support parameters
or complex code generation, and are limited to straightforward text replacement.
However, these macros can be used anywhere in the source file (or any file it
includes) after its definition, even in the middle of (or as an operand of)
other instructions, and even can be used within expressions.

The syntax for defining a simple text substitution macro is as follows:

```assembly
.define MACRO_NAME      "replacement_text"          ; The replacement text must be a string.
.define MACRO_NAME_2    "replacement_text_2"
                        "more_replacement_text"     ; The replacement text can also be multiple
                                                    ; strings in sequence. They will be concatenated.
```

## 7. Format and Style Guidelines

The TM32 Assembly Language is a line-oriented language, where each line in the
source file represents a single instruction, directive or label with an inline
comment, a single label with an inline comment, a standalone comment, or is blank.

### 7.1 Line Structure

Lines are structured as follows:

```assembly
[label:] [instruction|directive|macro] [comma-separated operands] [; comment]
```

- **Label**: An optional address label, followed by a colon. Labels must be
    defined at the beginning of a line, and should not be indented, although
    leading whitespace is ignored.
- **Instruction/Directive**: An optional instruction or directive, which must
    be preceded by at least one whitespace character if a label is present.
    If a label is not present, this should be indented with at least two spaces.
    - Instructions and directives are case-insensitive.
    - Directives must begin with a period (`.`).
- **Operands**: An optional list of comma-separated operands, which must be
    preceded by at least one whitespace character if an instruction or directive
    is present.
    - Operands are case-insensitive, except for string and character literals,
        and identifiers (variables, constants, and labels).
    - Commas separating operands may be followed by any amount of whitespace,
        but should be one space character.
- **Comment**: An optional inline comment, which begins with a semicolon (`;`)
    and continues to the end of the line. Comments may be preceded by at least
    one whitespace character if an instruction, directive, or label is present.
- **Blank Lines**: Lines that contain only whitespace characters are ignored.

### 7.2 Naming Conventions

For consistency and readability, the following naming conventions are recommended:

- **Labels**: Use descriptive names in snake_case (e.g., `main_loop`, `init_hardware`)
- **Constants**: Use ALL_CAPS with underscores (e.g., `SCREEN_WIDTH`, `MAX_PLAYERS`)
- **Variables**: Use lowercase with underscores (e.g., `player_score`, `current_level`)
- **Macros**: Use PascalCase (e.g., `SetPixel`, `DrawSprite`)

Use any naming convention you wish, but be consistent within your project.

### 7.3 Indentation and Spacing

- Use consistent indentation (2 or 4 spaces recommended)
- Align operands vertically when it improves readability
- Use blank lines to separate logical sections of code
- Place comments on separate lines for detailed explanations

### 7.4 Documentation

Good assembly code should be well-documented:

- Include file headers describing the purpose and any dependencies
- Comment complex algorithms and data structures
- Explain register usage conventions within functions
- Document any platform-specific assumptions or requirements

## 8. Examples

This section provides comprehensive examples demonstrating the features and 
capabilities of the TM32 Assembly Language, from basic operations to advanced 
programming techniques.

### 8.1 Basic Data Transfer and Arithmetic

```assembly
; Basic register operations
.text
start:
    ld   a, 0x12345678        ; Load immediate 32-bit value
    ld   bw, 0x1234           ; Load immediate 16-bit value  
    ld   ch, 0x56             ; Load immediate 8-bit value
    mv   d, a                 ; Move register to register
    
    ; Arithmetic operations (accumulator register required)
    add  a, 100               ; Add immediate value to A
    mv   a, bw                ; Move BW to A for arithmetic
    sub  a, 50                ; Subtract immediate value from A
    inc  ch                   ; Increment register (any register)
    dec  dl                   ; Decrement register (any register)
```

### 8.2 Memory Operations and Addressing Modes

```assembly
.bss
buffer:     .ds 256           ; Reserve 256 bytes
counter:    .db 42            ; Initialize byte value

.text
memory_demo:
    ; Direct addressing
    ld   a, [buffer]          ; Load from absolute address
    st   [counter], al        ; Store to absolute address
    
    ; Register indirect addressing  
    ld   b, buffer            ; Load buffer address
    ld   al, [b]              ; Load byte via register
    st   [b], ch              ; Store byte via register
    
    ; High page addressing (relative to 0xFFFF0000)
    ldh  dh, [0x50]           ; Load from high page
    sth  [0x51], dl           ; Store to high page
    
    ; Port addressing (relative to 0xFFFFFF00)
    ldp  ah, [0x10]           ; Load from port
    stp  [0x11], al           ; Store to port
```

### 8.3 Control Flow and Conditional Execution

```assembly
.text
control_demo:
    ld   a, 10                ; Initialize counter
    ld   b, 0                 ; Initialize sum
    
loop_start:
    cmp  a, 0                 ; Compare A with zero (accumulator required)
    jmp  zs, loop_end         ; Jump if zero (loop done)
    
    mv   ah, a                ; Move A to AH for arithmetic
    add  ah, b                ; Add B to AH (accumulator required)
    mv   b, ah                ; Move result back to B
    dec  a                    ; Decrement counter
    jmp  nc, loop_start       ; Unconditional jump back
    
loop_end:
    ; Result in register B
    ret  nc                   ; Return unconditionally
```

### 8.4 Subroutines and Stack Operations

```assembly
.text
main:
    ld   a, 15                ; Load argument
    call nc, factorial        ; Call factorial function
    ; Result now in register A
    halt                      ; Stop execution

factorial:
    ; Calculate factorial of value in A
    push a                    ; Save argument
    push b                    ; Save working register
    
    ld   b, 1                 ; Initialize result
    cmp  a, 1                 ; Check if A <= 1
    jmp  cc, fact_base        ; Jump if A <= 1
    
fact_loop:
    cmp  a, 1                 ; Check if done
    jmp  cc, fact_done        ; Exit if A <= 1
    
    ; Multiply B by A (using accumulator for arithmetic)
    push c                    ; Save counter
    ld   c, a                 ; Copy multiplier
    dec  c                    ; Adjust for loop
    mv   a, b                 ; Move B to accumulator
    
mult_loop:
    add  a, a                 ; Double A (accumulator required)
    dec  c                    ; Decrement counter
    mv   al, c                ; Move counter to accumulator for comparison
    cmp  al, 0                ; Check counter (accumulator required)
    jmp  zs, mult_done        ; Exit if done
    jmp  nc, mult_loop        ; Continue loop
    
mult_done:
    mv   b, a                 ; Move result back to B
    pop  c                    ; Restore counter
    dec  a                    ; Decrement factorial argument
    jmp  nc, fact_loop        ; Continue factorial loop
    
fact_done:
    mv   a, b                 ; Move result to A
    
fact_base:
    pop  b                    ; Restore working register
    pop  c                    ; Clean stack (original A)
    ret  nc                   ; Return
```

### 8.5 Bit Manipulation and Logical Operations

```assembly
.text
bit_demo:
    ld   a, 0b10110100        ; Load binary pattern
    
    ; Test individual bits
    bit  7, a                 ; Test bit 7
    jmp  zs, bit7_clear       ; Jump if bit 7 is clear
    
    ; Set and clear bits
    set  0, a                 ; Set bit 0
    res  3, a                 ; Clear bit 3
    tog  5, a                 ; Toggle bit 5
    
    ; Logical operations (accumulator required)
    and  a, 0x0F              ; Mask upper nibble (accumulator required)
    or   a, 0x80              ; Set bit 7 (accumulator required)
    xor  a, 0xFF              ; Invert all bits (accumulator required)
    not  a                    ; Complement register (any register)
    
bit7_clear:
    ret  nc
```

### 8.6 Shift and Rotate Operations

```assembly
.text
shift_demo:
    ld   a, 0x12345678        ; Load test value
    
    ; Arithmetic shifts
    sla  a                    ; Shift left arithmetic
    sra  a                    ; Shift right arithmetic
    
    ; Logical shift
    srl  a                    ; Shift right logical
    
    ; Rotations
    rl   a                    ; Rotate left through carry
    rr   a                    ; Rotate right through carry
    rlc  a                    ; Rotate left circular
    rrc  a                    ; Rotate right circular
    
    ret  nc
```

### 8.7 Variables, Constants, and Expressions

```assembly
; Constants and variables
.const BUFFER_SIZE = 512
.const MAX_ITERATIONS = 100
.let current_pos = 0

.bss
work_buffer: .ds BUFFER_SIZE

.data
status_flag: .db 0

.text
expression_demo:
    ; Using expressions
    ld   a, BUFFER_SIZE * 2   ; Calculate buffer size
    ld   b, high(BUFFER_SIZE) ; Get high byte of constant
    ld   c, low(work_buffer)  ; Get low byte of address
    
    ; Update variable
    .let current_pos = current_pos + 1
    ld   d, current_pos       ; Load updated value
    
    ret  nc
```

### 8.8 Macros and Code Generation

```assembly
; Define useful macros demonstrating basic parameter substitution
.macro LoadWord
    ld @1, ((high(@2) << 8) | low(@2))
.endmacro

.macro SaveRegisters
    push a                    ; Save all registers
    push b
    push c
    push d
.endmacro

.macro RestoreRegisters
    pop  d                    ; Restore in reverse order
    pop  c
    pop  b
    pop  a
.endmacro

; Macro demonstrating identifier mangling with parameters
.macro DefineBlock
    .block_@1:
        .ds @2
    .block_@1_end:
.endmacro

; Advanced macro demonstrating special parameter tokens
.macro DebugPrint
    ; @0 expands to macro name: "DebugPrint"
    ; @# or @NARG expands to number of arguments
    ; @* expands to all arguments separated by commas
    ; @- expands to all arguments except the first
    ; @@ expands to a literal @ character
    ; @? creates unique identifier suffix for this invocation
    
.if DEBUG_MODE
    ; Create unique debug counter for this invocation
    .let debug_counter_@? = debug_counter_@? + 1
    
    ; Use macro name in debug message
    .asciz "[@0] Call #", debug_counter_@?, " with @# args: @*"
    
    debug_print_@?:
        push a
        push b
        
        ; Process each argument (demonstrates @- token)
        .if @# > 1
            ; Forward all arguments except the first to another macro
            ProcessArgs @-
        .endif
            
            ; Use first argument
            ld   a, @1
            ; ... debug output code ...
            
            pop  b
            pop  a
        .endif
.endmacro

; Macro demonstrating @? for local variables and labels
.macro LocalLoop
    ; Create unique local variables for this macro invocation
    .let loop_counter_@? = 0
    .let loop_limit_@? = @1
    
    local_loop_start_@?:
        .let loop_counter_@? = loop_counter_@? + 1
        ld   a, loop_counter_@?
        cmp  a, loop_limit_@?     ; Compare with limit (A = accumulator)
        jmp  cs, local_loop_end_@?
        
        ; Execute loop body (second parameter)
        @2
        
        jmp  nc, local_loop_start_@?
        
    local_loop_end_@?:
        ; Unique label ensures no conflicts between invocations
.endmacro

; Macro showing variadic parameter handling with .shift
.macro DefineArray
    ; @1 = array name, @2 = size, @3... = initial values
    @1:
    .if @# > 2
        ; We have initial values - use .shift to access them
        .shift 2              ; Skip name and size parameters
        
        ; Now @1 is first value, @2 is second value, etc.
        .for i = 1 to (@# - 2)
            .db @@i           ; Use @@ to get literal @ for @@i
        .endfor
        
        ; Fill remaining space if needed
        .if @2 > (@# - 2)
            .ds @2 - (@# - 2), 0
        .endif
    .else
        ; No initial values - just reserve space
        .ds @2
    .endif
.endmacro

; Macro demonstrating complex identifier mangling
.macro CreateModule
    ; Creates a complete module with unique namespace
    ; @1 = module name, @2 = data size, @3 = function count
    
    ; Module data section with mangled names
    .module_@1_data:
        .ds @2
    .module_@1_data_end:
    
    ; Module function table
    .module_@1_functions:
.for func_idx = 0 to @3 - 1
        .dd module_@1_func_func_idx
.endfor
    
    ; Module initialization function
    module_@1_init:
        push a
        ld   a, @1_@?_init_marker    ; Unique init marker
        st   [.module_@1_data], a
        pop  a
        ret  nc
    
    ; Define init marker constant with unique value
    .const @1_@?_init_marker = 0x@1@?    ; Combines name and unique suffix
.endmacro

.bss
; Initialize debug counter for DebugPrint macro
.let debug_counter_DefineBlock_1085 = 0  ; Unique per invocation

; This macro demonstrates how identifiers can be mangled
; The invocation: DefineBlock data, 128
; Expands to:
; .block_data:
;     .ds 128
; .block_data_end:
DefineBlock data, 128

; Create another block to show unique expansion
DefineBlock temp, 64

.data
; Demonstrate DefineArray macro with various parameter counts
DefineArray simple_array, 8           ; Just name and size
DefineArray init_array, 5, 10, 20, 30 ; With initial values
DefineArray byte_values, 4, 0xFF, 0xAA, 0x55, 0x00

.text
macro_demo:
    SaveRegisters             ; Use basic macro
    
    LoadWord bw, 0x1234       ; Load word using macro
    
    ; Demonstrate DebugPrint with different argument counts
    DebugPrint "Message 1"
    DebugPrint "Message 2", a, b
    DebugPrint "Complex", 0x1234, [buffer], "end"
    
    ; Demonstrate LocalLoop macro (creates unique labels/variables)
    LocalLoop 5, "nop"        ; Loop 5 times executing NOP
    LocalLoop 3, "inc d"      ; Loop 3 times incrementing D
    
    ; Create modules with unique namespaces
    CreateModule graphics, 256, 4
    CreateModule sound, 128, 2
    
    ; Do some work...
    ld   a, 42
    
    RestoreRegisters          ; Restore using macro
    ret  nc

; Helper macro for DebugPrint demonstration
.macro ProcessArgs
    .if @# > 0
        ; Process each remaining argument
        ld   b, @1
        ; ... process argument ...
        .if @# > 1
            ProcessArgs @-            ; Recursive call with remaining args
        .endif
    .endif
.endmacro
```

### 8.9 Simple Text Substitution

```assembly
.define TARGET_ADDRESS "[0x46]"
.define LOAD_PORT "ldp"
.define STORE_PORT "stp"
.define MOVE_TO_B "mv b, a"

.text
main:
    LOAD_PORT a, TARGET_ADDRESS
    MOVE_TO_B
    STORE_PORT TARGET_ADDRESS, a
```

### 8.9 Conditional Assembly and Loops

```assembly
.const DEBUG_MODE = 1
.const ARRAY_SIZE = 16

.if DEBUG_MODE
debug_info:
    .asciz "Debug mode enabled"
.endif

.data
test_array:
.for i = 0 to ARRAY_SIZE - 1
    .db i * 2                 ; Generate array data
.endfor

.rodata
lookup_table:
.for angle = 0 to 255
    .db int((sin(angle / 256.0) * 127.0) + 128.0)  ; Sine table
.endfor

.text
conditional_demo:
.if DEBUG_MODE
    ; Debug code only included if DEBUG_MODE is set
    ld   a, [debug_info]
    ; ... debug operations ...
.endif
    
    ; Process array
    ld   b, 0                 ; Array index
    ld   c, ARRAY_SIZE        ; Array size
    
process_loop:
    cmp  b, c                 ; Check if done
    jmp  cs, process_done     ; Exit if B >= C
    
    ; Process array element
    ld   a, test_array
    add  a, b                 ; Calculate element address (A = accumulator)
    ld   d, [a]               ; Load element
    
    ; ... process element in D ...
    
    inc  b                    ; Next element
    jmp  nc, process_loop     ; Continue loop
    
process_done:
    ret  nc
```

### 8.10 Interrupt Service Routines

```assembly
; This example assumes that at least the low bytes of the TM32's interrupt flags
; and enable registers are memory-mapped by the hardware platform.
.const hIE0 = 0xFB
.const hIF0 = 0xFF

; Interrupt vector 1 - Timer interrupt
.int 1
timer_isr:
    ; Save context (only necessary registers)
    push a
    push b
    
    ; Handle timer interrupt
    ldp  al, [0x10]           ; Read timer status
    and  al, 0x01             ; Check timer flag (AL = accumulator)
    jmp  zs, timer_done       ; Skip if not timer
    
    ; Timer processing
    ld   b, [timer_counter]   ; Load counter
    inc  b                    ; Increment
    st   [timer_counter], b   ; Store back
    
    stp  [0x10], al           ; Clear timer flag
    
timer_done:
    ; Restore context
    pop  b
    pop  a
    reti                      ; Return and enable interrupts

.data
timer_counter: .dd 0

.text
main_with_interrupts:
    ld al, 0b10
    stp [hIE0], al            ; Enable timer interrupt
    ei                        ; Enable interrupts
    ld al, 0
    
main_loop:
    ; Main program loop
    nop                       ; Do work
    jmp  nc, main_loop        ; Continue forever
```

**Note**: The interrupt vectors used, and how they are requested, depend upon 
the specific hardware platform and are not defined by the TM32 architecture 
itself. For this example, a hardware platform that triggers interrupt vector 1 
for a timer event is assumed. Also, this particular example assumes that the
TM32's interrupt enable register is memory-mapped by this hardware platform.

### 8.11 Fixed-Point Mathematics

```assembly
.const FIXED_SCALE = 256      ; 8.8 fixed-point

.text
fixed_point_demo:
    ; Convert integer to fixed-point
    ld   a, 5                 ; Integer 5
    ld   b, FIXED_SCALE       ; Scale factor
    ; Multiply A by B (simplified)
    
    ; Fixed-point addition
    ld   c, (3 * FIXED_SCALE) ; 3.0 in fixed-point
    add  a, c                 ; Add fixed-point values (A = accumulator)
    
    ; Convert back to integer
    ld   b, FIXED_SCALE
    ; Divide A by B (simplified)
    
    ret  nc
```

### 8.12 String Processing

```assembly
.data
hello_msg:  .asciz "Hello, World!"
buffer:     .ds 32

.text
string_demo:
    ; Copy string
    ld   a, hello_msg         ; Source address
    ld   b, buffer            ; Destination address
    
copy_loop:
    ld   al, [a]              ; Load character to accumulator
    st   [b], al              ; Store character
    cmp  al, 0                ; Check for null terminator (AL = accumulator)
    jmp  zs, copy_done        ; Exit if null
    
    inc  a                    ; Next source
    inc  b                    ; Next destination
    jmp  nc, copy_loop        ; Continue
    
copy_done:
    ret  nc
```

### 8.13 Complete Program Example

```assembly
; Complete TM32 program demonstrating multiple features
; Calculates sum of squares from 1 to N

.const MAX_NUMBER = 10

.data
result:     .dd 0             ; Store final result
temp:       .dd 0             ; Temporary storage

.text
main:
    ld   a, MAX_NUMBER        ; Load maximum number
    call nc, sum_of_squares   ; Calculate sum of squares
    st   [result], a          ; Store result
    halt                      ; End program

sum_of_squares:
    ; Calculate sum of squares from 1 to A
    push b                    ; Save registers
    push c
    push d
    
    mv   b, a                 ; Save max number in B
    ld   a, 0                 ; Initialize sum in accumulator
    ld   c, 1                 ; Initialize counter
    
sos_loop:
    cmp  a, c                 ; Compare counter with sum for overflow check
    jmp  cs, sos_done         ; Safety check
    cmp  c, b                 ; Check if counter > max
    jmp  cs, sos_done         ; Exit if done
    
    ; Calculate square of C
    mv   d, c                 ; Copy counter
    call nc, multiply         ; D = C * C
    add  a, d                 ; Add to sum (A = accumulator)
    
    inc  c                    ; Next number
    jmp  nc, sos_loop         ; Continue loop
    
sos_done:
    ; Result already in A
    pop  d                    ; Restore registers
    pop  c
    pop  b
    ret  nc

multiply:
    ; Multiply C by itself, result in D
    ; Simple implementation (C * C)
    push a
    push b
    
    ld   a, c                 ; Multiplicand
    ld   b, c                 ; Multiplier
    ld   d, 0                 ; Initialize result
    
mult_loop:
    cmp  b, 0                 ; Check if done
    jmp  zs, mult_done        ; Exit if multiplier is 0
    
    bit  0, b                 ; Check if multiplier is odd
    jmp  zc, mult_even        ; Skip if even
    add  d, a                 ; Add multiplicand to result
    
mult_even:
    sla  a                    ; Shift multiplicand left
    srl  b                    ; Shift multiplier right
    jmp  nc, mult_loop        ; Continue
    
mult_done:
    pop  b
    pop  a
    ret  nc
```

These examples demonstrate the comprehensive capabilities of the TM32 Assembly 
Language, covering all major instruction categories, addressing modes, and 
programming techniques available on the TM32 architecture.

