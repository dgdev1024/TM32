# Specification - TM32 Assembler Tool

Below is a specification for an assembler tool for the TM32 assembly language.
This document outlines the features, usage, and options available for the
assembler tool.

## 1. Overview

The TM32 Assembler Tool (`tm32asm`) is a command-line utility that translates
source files written in TM32 assembly language into object code, which is then
processed by the TM32 Linker Tool (`tm32link`) to create executable binary files
which can be run on TM32-compatible hardware or emulators.

### 1.1 References

This document only details the assembler tool itself, but will reference other
related specifications as needed:

- For more information on the TM32 assembly language, refer to the
  [TM32 Assembly Language Specification](tm32asm.language.md).
- For more information on the TM32 linker tool, refer to the
  [TM32 Linker Tool Specification](tm32link.specification.md).
- For more information on the TM32 CPU architecture and its instruction set,
  refer to the [TM32 CPU Specification](tm32cpu.specification.md) and the
  [TM32 Instruction Set Reference](tm32cpu.instructions.md).

## 2. Usage

The TM32 Assembler Tool is invoked from the command line with the following
syntax:

```bash
$ tm32asm [options] <source_file.asm> [-o <output_file.o>]
```

### 2.1 Command-Line Options

The following options are available for the `tm32asm` command:

- `-l, --lex-only`: Perform only lexical analysis on the source file and output
    the tokens to standard output. No further processing (including preprocessing,
    parsing, or code generation) is performed. This option is ignored if
    `--parse-only` or `--preprocess-only` is also specified.
- `-r, --preprocess-only`: Perform lexical analysis and preprocessing on
    the source file, and output the preprocessed assembly code to standard output.
    No parsing or code generation is performed. This option is ignored if
    `--lex-only` or `--parse-only` is also specified.
- `-p, --parse-only`: Perform lexical analysis, preprocessing, and parsing on
    the source file, and output the abstract syntax tree (AST) to standard output.
    No code generation is performed.
- `--output-preprocessed <file.asm>`: Output the preprocessed assembly code
    to the specified file. This option can be used in conjunction with `--parse-only`
    or `--preprocess-only` to save intermediate results. Note that preprocessed
    code will not contain any preprocessor directives, as they will have been
    processed and removed.
- `--variables`: Output the list of all defined preprocessor variables and constants,
    along with their values, to standard output after preprocessing is complete.
    This option can be useful for debugging and verifying the values of variables
    defined via the command line or in the source code. This option is ignored if
    `--lex-only` is specified.
- `-o <output_file.o>, --output-file <output_file.o>`: Specify the name of the
    output object file. If not provided, the output file will have the same base
    name as the source file with a `.o` extension. This option is ignored if
    `--lex-only` or `--parse-only` is specified.
- `-I <include_dir>, --include-dir <include_dir>`: Add a directory to the list
    of directories to search for included files. This option can be specified
    multiple times to add multiple directories.
- `-L <name>[=<value>], --let <name>[=<value>]`: Define a mutable preprocessor variable
    that can be used in the assembly source code. If `<value>` is not provided,
    the variable is initialized to `0`. This option can be specified multiple
    times to define multiple variables. This is akin to using the `.let` directive
    in the source code.
- `-C <name>=<value>, --const <name>=<value>`: Define a preprocessor constant that 
    can be used in the assembly source code. This option can be specified multiple 
    times to define multiple constants. This is akin to using the `.const` directive
    in the source code.
- `-D <name>=<value>, --define <name>=<value>`: Define a simple, single-line
    text substitution macro that can be used in the assembly source code. This
    option can be specified multiple times to define multiple macros. This is
    akin to using the `.define` or `.def` directive in the source code.
- `-W, --warnings-as-errors`: Treat all warnings as errors. If any warnings
    are encountered during the assembly process, the tool will exit with a
    non-zero status code.
- `-V, --verbose`: Enable verbose output, providing additional details
    about the assembly process.
- `-v, --version`: Display the version information of the TM32 Assembler Tool
    and exit.
- `-h, --help`: Display a help message detailing the usage and options of the
    TM32 Assembler Tool, along with the current version information, and exit.

### 2.2 Examples

Assemble a source file into an object file:

```bash
$ tm32asm source.asm -o output.tmo  # Note that any file extension can be used
                                    # to represent an object file.
```

Assemble a source file with custom include directories and defined variables:

```bash
$ tm32asm -I ./includes -I /usr/local/tm32/includes \
          -L BUFFER_SIZE=256 -C MAX_VALUE=1024 source.asm \
          -o output.tmo
```

Perform only lexical analysis on a source file:

```bash
$ tm32asm --lex-only source.asm
```

Perform only parsing on a source file:

```bash
$ tm32asm --parse-only source.asm
```

Display help information:

```bash
$ tm32asm --help
```

## 3. Assembler Tool Components

The TM32 Assembler Tool consists of several key components that work together
and/or in sequence to process the assembly source code:

- **Lexical Analyzer (Lexer)**: The lexer reads the source file and converts the
    raw text into a sequence of tokens, which are the basic building blocks of
    the assembly language (e.g., keywords, identifiers, literals, operators).
- **Preprocessor**: The preprocessor handles preprocessing directives encountered,
    such as `.include`, `.define`, `.if`, etc. It processes these directives,
    performing any necessary expressions evaluation, file inclusion, and macro
    expansion and token mangling as needed to produce a modified token stream for
    further processing.
- **Parser**: The parser takes the token stream produced by the lexer (and
    modified by the preprocessor) and constructs an abstract syntax tree (AST)
    that represents the hierarchical structure of the assembly code.
- **Semantic Analyzer**: The semantic analyzer traverses the AST to perform
    semantic checks, ensuring that the assembly code adheres to the rules of the
    TM32 assembly language. It also builds symbol tables and resolves references
    to labels, variables, and constants.
- **Code Generator**: The code generator translates the validated AST into
    object code, producing a binary representation of the assembly instructions
    and data that can be linked into an executable file. Assembler directives
    that affect code generation (e.g., `.data`, `.text`, `.org`) are handled
    during this phase.

For more information on the TM32 assembly language's directives, consult the
[TM32 Assembly Language Specification](tm32asm.language.md).

### 3.1 Lexical Analyzer

The lexical analyzer (lexer) is the first stage of the assembly process. It
reads the source file character by character and converts the raw text into a
sequence of tokens. The lexer performs the following operations:

#### 3.1.1 Tokenization

The lexer recognizes and categorizes the following types of tokens:

- **Keywords**: Reserved words such as instruction mnemonics (`LD`, `ADD`,
  `JMP`, etc.), register names (`A`, `AW`, `AH`, `AL`, etc.), and execution
  conditions (`NC`, `ZC`, `CS`, etc.).
- **Identifiers**: User-defined names for labels, variables, constants, and
  macros. Must start with a letter or underscore, followed by letters, digits,
  or underscores.
- **Literals**: Numeric values in various formats (binary, octal, decimal,
  hexadecimal), character literals, fixed-point literals, graphics literals, and
  string literals.
- **Operators**: Arithmetic, bitwise, assignment, comparison, and logical
  operators as defined in the TM32 assembly language specification.
- **Punctuation**: Colons (`:`) for labels, semicolons (`;`) for comments,
  commas (`,`) for separating operands, and other punctuation marks.
- **Whitespace**: Spaces, tabs, and newlines used for formatting. These are
  ignored.

#### 3.1.2 Comment Handling

The lexer handles comments by recognizing semicolons (`;`) as the start of a
comment. Everything from the semicolon to the end of the line is treated as a
comment and is ignored during further processing.

#### 3.1.3 Error Detection

The lexer detects and reports lexical errors such as:

- Invalid characters that are not part of the TM32 assembly language
- Malformed numeric literals (e.g., invalid hexadecimal digits)
- Unterminated string literals
- Invalid escape sequences in character or string literals

### 3.2 Preprocessor

The preprocessor operates on the token stream produced by the lexer and handles
preprocessor directives. It performs macro expansion, file inclusion, conditional
assembly, and other preprocessing operations before the parser processes the
code.

#### 3.2.1 File Inclusion

The `.include` directive allows the inclusion of other source files at the
current location. The preprocessor:

- Searches for included files in the current directory first
- Then searches in directories specified by the `-I` command-line option
- Recursively processes included files through the full lexer-preprocessor
  pipeline
- Maintains a stack of file contexts to handle nested includes
- Detects and prevents circular includes

When an included file is processed:

- The lexer reads the file and produces a token stream
- The preprocessor processes the token stream, handling any directives and macros
    according to the rules laid out in this section
- The resulting token stream is moved into the main token stream at the point of
    inclusion

If an included file cannot be found or read, the preprocessor reports an error
and halts further processing.

If a file which has already been included is included again, it will be ignored
to prevent multiple inclusions of the same content.

#### 3.2.2 Macro Processing

The preprocessor handles several types of macros:

- **Simple Text Substitution**: Defined with `.define` or `.def`, or via the
  `-D` or `--define` command-line option, these macros perform straightforward 
  text replacement.
- **Parametric Macros**: Defined with `.macro`, these can accept parameters
  and perform more complex code generation.

#### 3.2.3 Variable and Constant Management

The preprocessor manages variables and constants defined with `.let` and `.const`
directives, as well as those specified via command-line options (`-L` and `-C`).
It maintains symbol tables for these definitions and performs expression
evaluation during preprocessing.

#### 3.2.4 Conditional Assembly

The preprocessor handles conditional assembly directives:

- `.if`, `.elif`, `.else`, `.endif`: Standard conditional blocks
- `.ifdef`, `.ifndef`: Check if macros/symbols are defined
- Expression evaluation for conditions, supporting all operators defined in
  the TM32 assembly language

#### 3.2.5 Loop Constructs

The preprocessor supports loop constructs for code generation:

- `.repeat`/`.rept`: Repeat code blocks a specified number of times
- `.while`: Repeat while a condition is true
- `.for`: Iterate with a loop variable
- Expression evaluation for loop conditions and iteration counts, just as with
  conditional assembly
- `.break` and `.continue`: Control loop execution

### 3.3 Parser

The parser takes the preprocessed token stream and constructs an Abstract
Syntax Tree (AST) that represents the hierarchical structure of the assembly
program. The parser is responsible for recognizing the syntactic structure of
the assembly language.

#### 3.3.1 Grammar Recognition

The parser recognizes the following syntactic constructs:

- **Instructions**: CPU instructions with their operands and addressing modes
- **Labels**: Address labels that mark code and data locations
- **Directives**: Assembler directives that control code generation and memory
  layout
- **Data Definitions**: Declarations of initialized and uninitialized data
- **Expressions**: Mathematical and logical expressions used in operands and
  directive parameters

#### 3.3.2 AST Construction

The parser builds an AST with the following node types:

- **Program Node**: Root node containing all top-level declarations
- **Section Nodes**: Represent different sections (`.text`, `.data`, `.bss`,
  etc.)
- **Instruction Nodes**: Represent CPU instructions with operand trees
- **Label Nodes**: Represent address labels and their definitions
- **Directive Nodes**: Represent assembler directives and their parameters
- **Expression Nodes**: Represent arithmetic and logical expressions

#### 3.3.3 Syntax Error Detection

The parser detects and reports syntax errors such as:

- Invalid instruction formats or unknown mnemonics
- Incorrect number or types of operands for instructions
- Malformed expressions or invalid operator usage
- Missing required punctuation or incorrect directive syntax

### 3.4 Semantic Analyzer

The semantic analyzer performs semantic analysis on the AST to ensure the
assembly code adheres to the rules and constraints of the TM32 assembly
language and CPU architecture.

#### 3.4.1 Symbol Table Management

The semantic analyzer builds and manages symbol tables containing:

- **Labels**: Address labels with their associated memory addresses
- **Variables**: Mutable variables defined with `.let`
- **Constants**: Immutable constants defined with `.const`
- **Macros**: Simple-text substitution macros (`.define`/`.def`) and parametric
    macros (`.macro`) with their parameter lists

#### 3.4.2 Reference Resolution

The analyzer resolves references to symbols, ensuring that:

- All referenced labels, variables, and constants are defined
- Forward references to labels are properly resolved
- Circular dependencies are detected and reported
- Scope rules are enforced for nested macro invocations

#### 3.4.3 Type and Range Checking

The semantic analyzer performs type and range checking:

- **Instruction Validation**: Ensures instructions use valid operand types and
  addressing modes supported by the TM32 CPU
- **Register Validation**: Verifies that register operands are valid for the
  instruction and data size
- **Address Range Checking**: Ensures memory addresses fall within valid
  ranges for the target memory map
- **Immediate Value Ranges**: Checks that immediate values fit within the
  specified data size constraints

#### 3.4.4 Expression Evaluation

The analyzer evaluates constant expressions and performs:

- **Arithmetic Operations**: Addition, subtraction, multiplication, division,
  modulus, and exponentiation
- **Bitwise Operations**: AND, OR, XOR, NOT, left shift, and right shift
- **Logical Operations**: Logical AND, OR, and NOT
- **Comparison Operations**: Equality, inequality, and relational comparisons

### 3.5 Code Generator

The code generator is the final stage of the assembly process. It translates
the validated AST into object code, producing binary representations of
instructions and data that can be linked into executable files.

#### 3.5.1 Instruction and Data Encoding

The code generator encodes TM32 instructions and data definitions according to 
the CPU specification:

- **Opcode Generation**: Converts instruction mnemonics to their corresponding
  16-bit opcodes
- **Data Encoding**: Encodes data definitions (e.g., `.byte`, `.word`) into
  the appropriate binary format
- **Operand Encoding**: Encodes register numbers, immediate values, and
  addressing modes into the instruction format
- **Multi-byte Instructions**: Handles instructions with immediate data that
  extend beyond the base 2-byte opcode

#### 3.5.2 Memory Layout

The code generator manages memory layout and addresses:

- **Section Placement**: Places code and data in appropriate memory sections
  according to the TM32 memory map
- **Address Assignment**: Assigns memory addresses to labels and data
  definitions
- **Alignment Handling**: Ensures proper alignment for data and code as
  specified by `.align` directives

#### 3.5.3 Relocation Information

The code generator produces relocation information for the linker:

- **Symbol References**: Records references to external symbols that must be
  resolved at link time
- **Address Relocations**: Identifies addresses that need adjustment when the
  final program layout is determined
- **Cross-references**: Maintains information about symbol usage for debugging
  and linking purposes

#### 3.5.4 Object Code Output

The final output includes:

- **Machine Code**: Binary representation of assembled instructions and data
- **Symbol Tables**: Information about defined and referenced symbols
- **Relocation Tables**: Data needed by the linker to resolve addresses
- **Debug Information**: Optional debugging data for use with debuggers and
  emulators

## 4. Errors and Warnings

The TM32 Assembler Tool provides comprehensive error and warning reporting to
help developers identify and resolve issues in their assembly code.

### 4.1 Error Handling

Errors are critical issues that prevent successful assembly of the source code.
When an error is encountered, the assembler will:

- Continue parsing to find additional errors when possible
- Report the file name, line number, and column where the error occurred
- Provide a descriptive error message explaining the issue
- Exit with a non-zero status code after reporting all errors

#### 4.1.1 Error Categories

**Lexical Errors:**
- Invalid characters in the source file
- Malformed numeric literals
- Unterminated string or character literals
- Invalid escape sequences

**Syntax Errors:**
- Invalid instruction mnemonics or formats
- Incorrect number of operands for instructions
- Missing required punctuation (commas, colons, etc.)
- Malformed expressions or invalid operator usage
- Unmatched preprocessor directives (e.g., `.if` without `.endif`)

**Semantic Errors:**
- Undefined labels, variables, or constants
- Duplicate symbol definitions
- Invalid register names for instruction operands
- Address values outside valid memory ranges
- Immediate values that exceed size constraints
- Type mismatches in expressions
- Circular include dependencies

**Code Generation Errors:**
- Instructions that cannot be encoded for the target CPU
- Memory sections that exceed available address space
- Alignment conflicts or invalid alignment values

### 4.2 Warnings

Warnings indicate potential issues that don't prevent assembly but may indicate
programmer errors or suboptimal code. The assembler continues processing after
issuing warnings unless the `-W` or `--warnings-as-errors` option is specified.

#### 4.2.1 Warning Categories

**Code Quality Warnings:**
- Unreferenced labels or variables
- Instructions that have no effect (e.g., `MV A, A`)
- Potentially inefficient instruction sequences
- Dead code after unconditional jumps

**Portability Warnings:**
- Use of architecture-specific features
- Hard-coded addresses that may not be portable
- Endianness-dependent data layouts

**Style Warnings:**
- Inconsistent naming conventions
- Long instruction sequences without labels
- Complex expressions that could be simplified

### 4.3 Warning Levels

The assembler supports different warning levels to control the verbosity of
warning messages:

**Level 0 (Silent):** No warnings are reported
**Level 1 (Default):** Report significant warnings that likely indicate errors
**Level 2 (Verbose):** Report all warnings including style and optimization
    suggestions

Warning levels can be controlled through command-line options or assembler
directives within the source code.

### 4.4 Exit Codes

The TM32 Assembler Tool uses the following exit codes to indicate the result
of the assembly process:

- **0**: Success - No errors or warnings, or only warnings when warnings are
  not treated as errors
- **1**: Error - One or more errors occurred during assembly, or warnings
  occurred when `-W`/`--warnings-as-errors` is specified
- **2**: Usage Error - Invalid command-line arguments or options
- **3**: File Error - Unable to read input file or write output file due to
  file system issues
- **4**: Internal Error - An unexpected internal error occurred in the assembler

## 5. Object File Format

The TM32 Assembler produces object files that contain the assembled machine code,
symbol information, and metadata necessary for linking. The object file format
is designed to be simple yet flexible enough to support the TM32 architecture's
requirements.

### 5.1 File Structure

The object file consists of several sections arranged in the following order:

1. **Header**: Contains file identification and metadata
2. **String Table**: Stores symbol names and other string data
3. **Symbol Table**: Contains symbol definitions and references
4. **Section Headers**: Describes each data section in the file
5. **Section Data**: The actual assembled code and data
6. **Relocation Tables**: Information for address resolution during linking

### 5.2 Header Format

The object file header contains:

| Offset | Size | Description |
|--------|------|-------------|
| 0x00 | 4 | Magic number (0x324D5454 - "TM2T") |
| 0x04 | 2 | Object file format version |
| 0x06 | 2 | Target CPU architecture identifier |
| 0x08 | 4 | Timestamp of assembly |
| 0x0C | 4 | Number of sections |
| 0x10 | 4 | String table offset |
| 0x14 | 4 | String table size |
| 0x18 | 4 | Symbol table offset |
| 0x1C | 4 | Symbol table size |
| 0x20 | 4 | Relocation table offset |
| 0x24 | 4 | Relocation table size |
| 0x28 | 8 | Reserved for future use |

### 5.3 Symbol Table

The symbol table contains entries for all symbols defined or referenced in the
assembly source:

| Offset | Size | Description |
|--------|------|-------------|
| 0x00 | 4 | Symbol name offset in string table |
| 0x04 | 1 | Symbol type (label, variable, constant, etc.) |
| 0x05 | 1 | Symbol scope (local, global, external) |
| 0x06 | 2 | Section index (0xFFFF for undefined) |
| 0x08 | 4 | Symbol value or address |
| 0x0C | 4 | Symbol size (for data symbols) |

### 5.4 Section Types

The object file supports the following section types corresponding to the TM32
memory map:

- **Metadata Section**: Program metadata and platform-specific information
- **Interrupt Vectors**: Interrupt service routines for each vector
- **Text Section**: Executable code
- **Read-only Data**: Constants and initialized read-only data
- **Data Section**: Initialized read-write data
- **BSS Section**: Uninitialized data (size information only)

### 5.5 Relocation Entries

Relocation entries specify how addresses should be adjusted during linking:

| Offset | Size | Description |
|--------|------|-------------|
| 0x00 | 4 | Offset within section where relocation applies |
| 0x04 | 2 | Section index |
| 0x06 | 1 | Relocation type (absolute, relative, etc.) |
| 0x07 | 1 | Symbol table index |
| 0x08 | 4 | Addend value |

## 6. Conclusion

The TM32 Assembler Tool provides a comprehensive solution for translating TM32
assembly language source code into object files suitable for linking into
executable programs. Its modular design, with distinct lexical analysis,
preprocessing, parsing, semantic analysis, and code generation phases, ensures
robust and reliable assembly processing.

Key features of the assembler include:

- **Comprehensive Language Support**: Full support for the TM32 assembly
  language specification, including all instructions, directives, and
  addressing modes
- **Advanced Preprocessing**: Powerful macro system, conditional assembly,
  and file inclusion capabilities
- **Robust Error Handling**: Detailed error and warning reporting with precise
  location information
- **Flexible Output**: Configurable object file generation with support for
  debugging and linking metadata
- **Developer-Friendly**: Command-line options for various development
  workflows, from simple assembly to complex project builds

The assembler integrates seamlessly with other TM32 development tools,
particularly the TM32 Linker, to provide a complete toolchain for TM32-based
development. Its design prioritizes both ease of use for simple projects and
scalability for complex applications.

For detailed information about the TM32 assembly language syntax and semantics,
refer to the TM32 Assembly Language Specification. For information about
linking object files into executable programs, consult the TM32 Linker Tool
Specification.
