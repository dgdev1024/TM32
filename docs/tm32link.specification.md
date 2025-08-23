# Specification - TM32 Linker

Below is a comprehensive specification for the TM32 Linker (`tm32link`), a tool that takes one or more TM32 Object Format (.tmo) files produced by the TMM assembler and links them together to produce executable binary files for the TM32 Core System and other TM32-based virtual hardware.

## 1. Overview

The TM32 Linker is a critical component of the TM32 development toolchain that performs the final stage of program compilation. It combines multiple relocatable object files into a single executable binary, resolves symbol references between modules, applies relocations, and organizes code and data according to the target system's memory layout.

### 1.1 Key Features

- **Multi-object Linking**: Combines multiple .tmo object files into a single executable
- **Symbol Resolution**: Resolves external symbol references across object files
- **Relocation Processing**: Applies address relocations for position-independent code
- **Memory Layout Management**: Organizes sections according to TM32 memory map
- **Library Support**: Links with static libraries and provides library search paths
- **Error Detection**: Comprehensive error reporting for linking failures
- **Output Formats**: Supports multiple output formats for different TM32 targets
- **Debug Information**: Preserves and merges debug information from object files

### 1.2 Supported Input Formats

- **TM32 Object Files (.tmo)**: Primary relocatable object format from TMM assembler
- **TM32 Static Libraries (.tma)**: Archive format containing multiple .tmo files
- **Linker Scripts (.lds)**: Custom memory layout and linking behavior definitions

### 1.3 Supported Output Formats

- **TM32 Executable (.tm32)**: Standard executable format for TM32 Core System
- **Raw Binary (.bin)**: Raw memory image without headers or metadata
- **Intel HEX (.hex)**: Intel HEX format for ROM programming and debugging
- **Motorola S-Record (.srec)**: S-Record format for embedded system deployment

## 2. TM32 Object File Format (.tmo)

The TM32 Linker processes object files in the TM32 Object Format, which contains relocatable code, data, symbols, and metadata required for linking.

### 2.1 File Structure

```
TM32 Object File (.tmo)
+-------------------+
| File Header       | 64 bytes
+-------------------+
| Section Headers   | Variable length
+-------------------+
| String Table      | Variable length
+-------------------+
| Symbol Table      | Variable length
+-------------------+
| Relocation Tables | Variable length
+-------------------+
| Section Data      | Variable length
+-------------------+
```

### 2.2 File Header

The file header contains basic information about the object file:

```c
struct TMOHeader {
    uint32_t magic;           // 0x544D4F00 ("TMO\0")
    uint32_t version;         // File format version (currently 1)
    uint32_t flags;           // Object file flags
    uint32_t entry_point;     // Entry point address (0 if not applicable)
    uint16_t section_count;   // Number of sections
    uint16_t string_offset;   // Offset to string table
    uint32_t string_size;     // Size of string table
    uint16_t symbol_count;    // Number of symbols
    uint16_t symbol_offset;   // Offset to symbol table
    uint32_t timestamp;       // Creation timestamp
    uint32_t reserved[8];     // Reserved for future use
};
```

#### Header Flags
| Bit | Name | Description |
|-----|------|-------------|
| 0   | HAS_DEBUG | Object file contains debug information |
| 1   | LITTLE_ENDIAN | Little-endian byte order (always set for TM32) |
| 2   | RELOCATABLE | File contains relocatable code |
| 3   | EXECUTABLE | File is executable (not used in .tmo) |
| 4-31| Reserved | Reserved for future use |

### 2.3 Section Headers

Each section in the object file has a corresponding header:

```c
struct TMOSectionHeader {
    uint32_t name_offset;     // Offset into string table for section name
    uint32_t type;            // Section type (see Section Types)
    uint32_t flags;           // Section flags (see Section Flags)
    uint32_t addr;            // Virtual address (0 for relocatable sections)
    uint32_t offset;          // File offset to section data
    uint32_t size;            // Size of section in bytes
    uint32_t link;            // Link to another section (for relocation tables)
    uint32_t info;            // Additional section information
    uint32_t addralign;       // Address alignment constraint
    uint32_t entsize;         // Size of fixed-size entries (0 if variable)
};
```

#### Section Types
| Value | Name | Description |
|-------|------|-------------|
| 0     | NULL | Unused section header |
| 1     | PROGBITS | Program code or data |
| 2     | SYMTAB | Symbol table |
| 3     | STRTAB | String table |
| 4     | RELA | Relocation entries with addends |
| 5     | NOBITS | Uninitialized data (BSS) |
| 6     | REL | Relocation entries without addends |
| 7     | DEBUG | Debug information |

#### Section Flags
| Bit | Name | Description |
|-----|------|-------------|
| 0   | WRITE | Section is writable |
| 1   | ALLOC | Section occupies memory during execution |
| 2   | EXECINSTR | Section contains executable instructions |
| 3   | MERGE | Section may be merged |
| 4   | STRINGS | Section contains null-terminated strings |
| 5-31| Reserved | Reserved for future use |

### 2.4 Symbol Table

The symbol table contains information about all symbols defined or referenced in the object file:

```c
struct TMOSymbol {
    uint32_t name_offset;     // Offset into string table for symbol name
    uint32_t value;           // Symbol value (address or constant)
    uint32_t size;            // Symbol size in bytes
    uint8_t  info;            // Symbol type and binding (see Symbol Info)
    uint8_t  other;           // Symbol visibility
    uint16_t shndx;           // Section header index
};
```

#### Symbol Info Encoding
- **Binding (upper 4 bits)**:
  - 0: LOCAL - Symbol has local binding
  - 1: GLOBAL - Symbol has global binding
  - 2: WEAK - Symbol has weak binding
- **Type (lower 4 bits)**:
  - 0: NOTYPE - Symbol type not specified
  - 1: OBJECT - Symbol is a data object
  - 2: FUNC - Symbol is a function
  - 3: SECTION - Symbol is associated with a section
  - 4: FILE - Symbol gives the name of the source file

### 2.5 Relocation Tables

Relocation entries specify how to modify section contents during linking:

```c
struct TMORelocation {
    uint32_t offset;          // Offset in section where relocation applies
    uint32_t info;            // Relocation type and symbol index
    int32_t  addend;          // Addend for relocation calculation
};
```

#### Relocation Types
| Value | Name | Description | Calculation |
|-------|------|-------------|-------------|
| 0     | R_TM32_NONE | No relocation | - |
| 1     | R_TM32_32 | 32-bit absolute address | S + A |
| 2     | R_TM32_16 | 16-bit absolute address | S + A |
| 3     | R_TM32_8 | 8-bit absolute address | S + A |
| 4     | R_TM32_PC32 | 32-bit PC-relative | S + A - P |
| 5     | R_TM32_PC16 | 16-bit PC-relative | S + A - P |
| 6     | R_TM32_HIGH16 | High 16 bits of 32-bit address | (S + A) >> 16 |
| 7     | R_TM32_LOW16 | Low 16 bits of 32-bit address | (S + A) & 0xFFFF |
| 8     | R_TM32_GOT32 | 32-bit GOT offset | G + A - P |
| 9     | R_TM32_PLT32 | 32-bit PLT offset | L + A - P |

Where:
- S = Symbol value
- A = Addend
- P = Place (offset into section being relocated)
- G = Global Offset Table offset
- L = Procedure Linkage Table offset

## 3. TM32 Static Library Format (.tma)

Static libraries are archives containing multiple object files that can be linked into programs.

### 3.1 Archive Structure

```
TM32 Archive File (.tma)
+-------------------+
| Archive Header    | 64 bytes
+-------------------+
| Member Table      | Variable length
+-------------------+
| Symbol Index      | Variable length
+-------------------+
| Member Files      | Variable length
+-------------------+
```

### 3.2 Archive Header

```c
struct TMAHeader {
    char     magic[8];        // "!<tm32>\n"
    uint32_t version;         // Archive format version
    uint32_t member_count;    // Number of member files
    uint32_t symbol_count;    // Number of symbols in index
    uint32_t member_offset;   // Offset to member table
    uint32_t symbol_offset;   // Offset to symbol index
    uint32_t data_offset;     // Offset to member file data
    uint32_t reserved[9];     // Reserved for future use
};
```

### 3.3 Member Table Entry

```c
struct TMAMember {
    char     name[32];        // Member file name
    uint32_t timestamp;       // Last modification time
    uint32_t size;            // Size of member file
    uint32_t offset;          // Offset to member file data
    uint32_t checksum;        // CRC32 checksum of member data
    uint32_t reserved[3];     // Reserved for future use
};
```

## 4. TM32 Executable Format (.tm32)

The primary output format of the TM32 Linker is the TM32 Executable format, designed for the TM32 Core System.

### 4.1 File Structure

```
TM32 Executable File (.tm32)
+-------------------+
| File Header       | 64 bytes
+-------------------+
| Program Headers   | Variable length
+-------------------+
| Section Headers   | Variable length
+-------------------+
| String Table      | Variable length
+-------------------+
| Symbol Table      | Variable length (optional)
+-------------------+
| Debug Info        | Variable length (optional)
+-------------------+
| Program Data      | Variable length
+-------------------+
```

### 4.2 Executable Header

```c
struct TM32Header {
    uint32_t magic;           // 0x544D3332 ("TM32")
    uint32_t version;         // File format version
    uint32_t type;            // File type (executable, library, etc.)
    uint32_t machine;         // Target architecture (TM32 = 1)
    uint32_t entry;           // Program entry point address
    uint32_t phoff;           // Program header table offset
    uint32_t shoff;           // Section header table offset
    uint32_t flags;           // Processor-specific flags
    uint16_t ehsize;          // Size of this header
    uint16_t phentsize;       // Size of program header entry
    uint16_t phnum;           // Number of program header entries
    uint16_t shentsize;       // Size of section header entry
    uint16_t shnum;           // Number of section header entries
    uint16_t shstrndx;        // Section header string table index
    uint32_t timestamp;       // Build timestamp
    uint32_t checksum;        // File checksum
};
```

### 4.3 Program Headers

Program headers define memory segments for loading:

```c
struct TM32ProgramHeader {
    uint32_t type;            // Segment type
    uint32_t flags;           // Segment flags
    uint32_t offset;          // File offset to segment data
    uint32_t vaddr;           // Virtual address in memory
    uint32_t paddr;           // Physical address (usually same as vaddr)
    uint32_t filesz;          // Size of segment in file
    uint32_t memsz;           // Size of segment in memory
    uint32_t align;           // Alignment constraint
};
```

#### Program Header Types
| Value | Name | Description |
|-------|------|-------------|
| 0     | NULL | Unused entry |
| 1     | LOAD | Loadable segment |
| 2     | DYNAMIC | Dynamic linking information |
| 3     | INTERP | Interpreter information |
| 4     | NOTE | Auxiliary information |
| 5     | SHLIB | Shared library (reserved) |
| 6     | PHDR | Program header table |

#### Program Header Flags
| Bit | Name | Description |
|-----|------|-------------|
| 0   | EXECUTE | Segment is executable |
| 1   | WRITE | Segment is writable |
| 2   | READ | Segment is readable |

## 5. Linker Command Line Interface

The TM32 Linker provides a comprehensive command-line interface for controlling the linking process.

### 5.1 Basic Syntax

```bash
tm32link [options] object-files...
```

### 5.2 Input/Output Options

| Option | Description |
|--------|-------------|
| `-o <file>` | Specify output file name |
| `-L <dir>` | Add directory to library search path |
| `-l <library>` | Link with library (searches for liblibrary.tma) |
| `-T <script>` | Use custom linker script |
| `--format=<fmt>` | Set output format (tm32, bin, hex, srec) |
| `--input-format=<fmt>` | Set input format (auto-detected by default) |

### 5.3 Symbol and Linking Options

| Option | Description |
|--------|-------------|
| `-e <symbol>` | Set entry point symbol |
| `-u <symbol>` | Force undefined symbol to be resolved |
| `-g` | Include debug information in output |
| `-s` | Strip all symbols from output |
| `-S` | Strip debug symbols only |
| `--defsym <sym>=<val>` | Define symbol with value |
| `--undefined=<symbol>` | Force symbol to be undefined |

### 5.4 Memory Layout Options

| Option | Description |
|--------|-------------|
| `--section-start=<sec>=<addr>` | Set section start address |
| `--image-base=<addr>` | Set base address for executable |
| `--stack-size=<size>` | Set stack size |
| `--heap-size=<size>` | Set heap size |
| `-z <keyword>` | Set linker-specific options |

### 5.5 Optimization and Debug Options

| Option | Description |
|--------|-------------|
| `-O <level>` | Set optimization level (0-3) |
| `--gc-sections` | Remove unused sections |
| `--print-gc-sections` | Print removed sections |
| `--map=<file>` | Generate link map file |
| `--cref` | Generate cross-reference table |
| `--verbose` | Enable verbose output |

### 5.6 Error Handling Options

| Option | Description |
|--------|-------------|
| `--no-undefined` | Report undefined symbols as errors |
| `--allow-multiple-definition` | Allow multiple symbol definitions |
| `--warn-common` | Warn about common symbols |
| `--fatal-warnings` | Treat warnings as errors |

## 6. Linking Process

The TM32 Linker follows a multi-stage process to combine object files into executables.

### 6.1 Input Processing

1. **Object File Parsing**: Parse .tmo files and extract sections, symbols, and relocations
2. **Library Processing**: Extract required objects from .tma archive files
3. **Script Processing**: Parse linker script if provided
4. **Dependency Resolution**: Build dependency graph of input files

### 6.2 Symbol Resolution

1. **Symbol Collection**: Gather all defined and undefined symbols
2. **Global Symbol Table**: Build comprehensive symbol table
3. **Weak Symbol Resolution**: Handle weak symbol binding rules
4. **Undefined Symbol Checking**: Verify all symbols are resolved

#### Symbol Resolution Rules

1. **Strong symbols** always override weak symbols
2. **Multiple strong symbols** with the same name cause an error
3. **Weak symbols** can be overridden by strong symbols
4. **Common symbols** are allocated in BSS section
5. **Library symbols** are only included if referenced

### 6.3 Section Merging

1. **Section Grouping**: Group sections by name and type
2. **Alignment Handling**: Apply section alignment constraints
3. **Address Assignment**: Assign virtual addresses to sections
4. **Size Calculation**: Calculate final section sizes

#### Standard Section Order

Based on TM32 Core System memory layout:

1. **Metadata sections** → `0x00000000` region
2. **Interrupt vectors** → `0x00001000` region  
3. **Text sections** → `0x00003000` region
4. **Read-only data** → Following text sections
5. **Data sections** → `0x80000000` region (WRAM)
6. **BSS sections** → Following data sections

### 6.4 Relocation Processing

1. **Relocation Collection**: Gather all relocation entries
2. **Address Calculation**: Calculate final symbol addresses
3. **Relocation Application**: Apply relocations to section data
4. **Cross-Reference Validation**: Verify relocation targets

#### Relocation Algorithm

For each relocation entry:
1. Retrieve symbol value from symbol table
2. Calculate relocation value using type-specific formula
3. Apply relocation to target location
4. Verify result fits in target field size
5. Update section data with relocated value

### 6.5 Output Generation

1. **Format Selection**: Choose output format based on options
2. **Header Generation**: Create file headers and metadata
3. **Section Writing**: Write section data to output file
4. **Symbol Table Generation**: Generate output symbol table
5. **Debug Information**: Include debug data if requested

## 7. Memory Layout Management

The TM32 Linker manages memory layout according to the TM32 Core System specification.

### 7.1 Default Memory Layout

Based on TM32 Core System memory map:

```
Memory Region Layout:
┌─────────────────┬─────────────────┬──────────────────┐
│ Region          │ Start Address   │ End Address      │
├─────────────────┼─────────────────┼──────────────────┤
│ Metadata        │ 0x00000000      │ 0x00000FFF       │
│ Interrupt       │ 0x00001000      │ 0x00002FFF       │
│ Code/Data       │ 0x00003000      │ 0x7FFFFFFF       │
│ WRAM            │ 0x80000000      │ 0xBFFFFFFF       │
│ Cartridge RAM   │ 0xC0000000      │ 0xDFFFFFFF       │
│ VRAM            │ 0xE0008000      │ 0xE0009FFF       │
│ OAM             │ 0xE000FE00      │ 0xE000FE9F       │
│ Stack           │ 0xFFFE0000      │ 0xFFFEFFFF       │
│ High RAM        │ 0xFFFF0000      │ 0xFFFFFFFF       │
│ I/O Registers   │ 0xFFFFFF00      │ 0xFFFFFFFF       │
└─────────────────┴─────────────────┴──────────────────┘
```

### 7.2 Section Placement Rules

1. **`.metadata`** sections → Metadata region (`0x00000000`)
2. **`.int<N>`** sections → Interrupt vectors (`0x00001000 + N * 0x100`)
3. **`.text`** sections → Code region (`0x00003000` and up)
4. **`.rodata`** sections → Code region (after text)
5. **`.data`** sections → WRAM region (`0x80000000` and up)
6. **`.bss`** sections → WRAM region (after data)
7. **Custom sections** → As specified by linker script

### 7.3 Linker Scripts

Linker scripts provide fine-grained control over memory layout:

```ld
/* TM32 Default Linker Script */
MEMORY {
    METADATA : ORIGIN = 0x00000000, LENGTH = 4K
    INTERRUPT : ORIGIN = 0x00001000, LENGTH = 8K  
    ROM : ORIGIN = 0x00003000, LENGTH = 2G-12K
    WRAM : ORIGIN = 0x80000000, LENGTH = 1G
    SRAM : ORIGIN = 0xC0000000, LENGTH = 512M
}

SECTIONS {
    .metadata : {
        *(.metadata)
    } > METADATA
    
    .vectors : {
        *(.int*)
    } > INTERRUPT
    
    .text : {
        *(.text)
        *(.text.*)
    } > ROM
    
    .rodata : {
        *(.rodata)
        *(.rodata.*)
    } > ROM
    
    .data : {
        *(.data)
        *(.data.*)
    } > WRAM
    
    .bss : {
        *(.bss)
        *(.bss.*)
        *(COMMON)
    } > WRAM
}

ENTRY(_start)
```

## 8. Library Support

The TM32 Linker provides comprehensive static library support for modular development.

### 8.1 Library Search Process

1. **Library Name Resolution**: Convert `-l<name>` to `lib<name>.tma`
2. **Search Path Traversal**: Search directories in order:
   - Current directory
   - Directories specified with `-L`
   - System library directories
3. **Archive Processing**: Extract and link required object files
4. **Dependency Resolution**: Recursively resolve library dependencies

### 8.2 Archive Creation

Use the `tm32link` tool with the `--archive` option to create static libraries:

```bash
tm32link --archive libmath.tma math.tmo trig.tmo algebra.tmo
```

### 8.3 Symbol Index

Archives maintain a symbol index for efficient linking:

```c
struct TMASymbolIndex {
    uint32_t symbol_offset;   // Offset to symbol name in string table
    uint32_t member_index;    // Index of member containing symbol
    uint32_t symbol_value;    // Symbol value
    uint8_t  symbol_type;     // Symbol type and binding
    uint8_t  reserved[3];     // Reserved for alignment
};
```

## 9. Error Reporting and Diagnostics

The TM32 Linker provides comprehensive error reporting and diagnostic capabilities.

### 9.1 Error Categories

#### Symbol Resolution Errors
- **Undefined Symbol**: Symbol referenced but not defined
- **Multiple Definition**: Symbol defined in multiple objects
- **Weak Symbol Conflict**: Multiple weak symbols with same name

#### Relocation Errors  
- **Relocation Overflow**: Target value too large for relocation type
- **Invalid Relocation**: Unsupported relocation type
- **Misaligned Relocation**: Target address violates alignment

#### Memory Layout Errors
- **Section Overflow**: Section too large for assigned memory region
- **Address Collision**: Sections overlap in memory
- **Invalid Address**: Section assigned invalid memory address

#### File Format Errors
- **Invalid Magic**: Unrecognized file format
- **Corrupted Header**: Invalid header fields
- **Missing Section**: Referenced section not found

### 9.2 Error Message Format

```
filename:section:offset: error: message
filename:section:offset: note: additional information
```

Example:
```
main.tmo:.text:0x1234: error: undefined reference to 'printf'
main.tmo:.text:0x1234: note: referenced from main function
```

### 9.3 Warning Categories

- **Unused Section**: Section defined but not referenced
- **Common Symbol**: Usage of common symbols (deprecated)
- **Size Mismatch**: Symbol size differs between declarations
- **Alignment Warning**: Inefficient section alignment

### 9.4 Diagnostic Options

| Option | Description |
|--------|-------------|
| `--error-limit=N` | Stop after N errors |
| `--no-warnings` | Suppress all warnings |
| `--Wl,option` | Pass option to linker |
| `--verbose` | Enable verbose diagnostic output |
| `--print-map` | Print memory map |
| `--trace` | Trace file and symbol resolution |

## 10. Link Map Generation

The linker can generate detailed link maps showing memory layout and symbol information.

### 10.1 Map File Format

```
TM32 Link Map
Generated: 2025-08-17 14:30:00
Command: tm32link -o program.tm32 main.tmo math.tmo

Memory Configuration:
Name            Origin          Length          Attributes
METADATA        0x00000000      0x00001000      r
INTERRUPT       0x00001000      0x00002000      rx  
ROM             0x00003000      0x7fffd000      rx
WRAM            0x80000000      0x40000000      rw

Linker Script and Memory Map:

.metadata       0x00000000      0x100
                0x00000000      program_info
                0x00000000      0x100   main.tmo:(.metadata)

.vectors        0x00001000      0x800
                0x00001000      _vectors_start = .
                0x00001000      0x100   main.tmo:(.int0)
                0x00001100      0x100   main.tmo:(.int1)
                ...

.text           0x00003000      0x2000
                0x00003000      _start
                0x00003000      0x1000  main.tmo:(.text)
                0x00004000      0x1000  math.tmo:(.text)

Symbol Table:
Address         Size    Type    Section         Name
0x00003000      4       FUNC    .text          _start
0x00003100      32      FUNC    .text          main
0x00004000      64      FUNC    .text          add_numbers
0x80000000      4       OBJECT  .data          global_var
```

### 10.2 Cross-Reference Table

When requested with `--cref`, the linker generates cross-reference information:

```
Cross Reference Table:

Symbol          File            Section         References
_start          main.tmo        .text          program entry
main            main.tmo        .text          _start+0x10
add_numbers     math.tmo        .text          main+0x20, main+0x45
global_var      main.tmo        .data          main+0x30
```

## 11. Advanced Features

### 11.1 Garbage Collection

The `--gc-sections` option removes unused sections to reduce executable size:

1. **Root Set Identification**: Start with entry point and exported symbols
2. **Reachability Analysis**: Mark all reachable sections and symbols  
3. **Dead Code Elimination**: Remove unreachable sections
4. **Relocation Updates**: Update relocations for remaining sections

### 11.2 Link-Time Optimization

The linker can perform basic optimizations:

- **Dead Symbol Elimination**: Remove unused symbols
- **Section Merging**: Combine compatible sections
- **Alignment Optimization**: Optimize section alignment
- **Constant Propagation**: Resolve constant expressions

### 11.3 Integration with TM32 ROM Validator

The TM32 Linker integrates seamlessly with the TM32 ROM Validator (`tm32validate`) to ensure output compliance and quality.

#### 11.3.1 Automatic Validation Options

**Built-in Validation During Linking**:
```bash
# Enable automatic validation during linking
tm32link --validate=basic -o program.tm32 *.tmo
tm32link --validate=standard -o program.tm32 *.tmo
tm32link --validate=comprehensive -o program.tm32 *.tmo
```

**Validation Level Configuration**:
| Option | Description |
|--------|-------------|
| `--validate=<level>` | Set validation level (none, basic, standard, comprehensive) |
| `--validation-rules=<file>` | Use custom validation rules |
| `--fail-on-validation-errors` | Treat validation errors as link errors |
| `--validation-report=<file>` | Generate validation report |

#### 11.3.2 Post-Link Validation Integration

**Automatic Post-Link Validation**:
```bash
# Run validator automatically after successful link
tm32link --post-validate --validation-level=2 -o program.tm32 *.tmo

# Custom validation command
tm32link --post-validate-cmd="tm32validate --level=3 --security" -o program.tm32 *.tmo
```

**Build System Integration**:
```makefile
# Enhanced Makefile with integrated validation
LINKFLAGS = --validate=standard --post-validate
VALFLAGS = --level=2 --target=tm32core

$(TARGET): $(OBJECTS)
	tm32link $(LINKFLAGS) -o $@ $(OBJECTS)
	@echo "Linking and validation completed successfully"

release: LINKFLAGS += --validation-level=comprehensive --security-analysis
release: $(TARGET)
	tm32validate --level=4 --security --performance $<
```

#### 11.3.3 Validation-Aware Linking

**Memory Layout Validation**:
The linker can perform TM32 Core System specific validation:

```bash
# Validate memory layout during linking
tm32link --check-memory-layout --target=tm32core -o program.tm32 *.tmo
```

**Symbol Table Validation**:
```bash
# Enable symbol validation
tm32link --validate-symbols --check-undefined --no-multiple-definition -o program.tm32 *.tmo
```

**Interrupt Vector Validation**:
```bash
# Validate interrupt vector placement and structure
tm32link --validate-interrupts --check-isr-structure -o program.tm32 *.tmo
```

#### 11.3.4 Quality Assurance Features

**Link-Time Safety Checks**:
- Stack usage analysis across linked modules
- Function call graph validation
- Memory access pattern verification
- Hardware register usage validation

**Performance Analysis Integration**:
- Code size optimization recommendations
- Memory layout efficiency analysis
- Function placement optimization
- Cache performance predictions

### 11.4 Debug Information Handling

The linker preserves and merges debug information:

- **DWARF Support**: Handle DWARF debug information
- **Line Number Tables**: Merge line number information
- **Source File Tracking**: Maintain source file references
- **Symbol Debug Info**: Preserve symbol debugging data

### 11.4 Position Independent Code

Support for position-independent executables:

- **GOT Generation**: Create Global Offset Table
- **PLT Generation**: Create Procedure Linkage Table  
- **PIC Relocations**: Handle position-independent relocations
- **Dynamic Symbol Table**: Generate dynamic symbol information

## 12. Integration with Build Systems

### 12.1 Makefile Integration

```makefile
# TM32 Toolchain Variables
TM32ASM = tm32asm
TM32LINK = tm32link
TM32AR = tm32ar

# Build Rules
%.tmo: %.tmm
	$(TM32ASM) -c $< -o $@

program.tm32: main.tmo math.tmo
	$(TM32LINK) -o $@ $^ -lstandard

libmath.tma: math.tmo trig.tmo
	$(TM32AR) rcs $@ $^
```

### 12.2 CMake Integration

```cmake
# TM32 Toolchain File
set(CMAKE_SYSTEM_NAME TM32)
set(CMAKE_ASM_COMPILER tm32asm)
set(CMAKE_LINKER tm32link)

# Custom Target
add_custom_target(tm32_program
    COMMAND tm32link -o program.tm32 main.tmo math.tmo
    DEPENDS main.tmo math.tmo
)
```

## 13. Performance Considerations

### 13.1 Linking Performance

- **Incremental Linking**: Only relink changed objects
- **Parallel Processing**: Utilize multiple cores where possible
- **Memory Management**: Efficient memory usage for large projects
- **Symbol Table Optimization**: Fast symbol lookup algorithms

### 13.2 Output Size Optimization

- **Section Alignment**: Minimize padding between sections
- **String Table Compression**: Deduplicate string table entries
- **Symbol Pruning**: Remove unnecessary symbols
- **Debug Info Compression**: Compress debug information

## 14. Examples

### 14.1 Basic Linking Example

```bash
# Compile source files
tm32asm -c main.tmm -o main.tmo
tm32asm -c utils.tmm -o utils.tmo

# Link executable
tm32link -o program.tm32 main.tmo utils.tmo
```

### 14.2 Library Linking Example

```bash
# Create library
tm32ar rcs libutils.tma utils.tmo string.tmo

# Link with library
tm32link -o program.tm32 main.tmo -L. -lutils
```

### 14.3 Custom Memory Layout

```bash
# Use custom linker script
tm32link -T custom.lds -o program.tm32 main.tmo
```

### 14.4 Debug Build

```bash
# Include debug information
tm32asm -g -c main.tmm -o main.tmo
tm32link -g --map=program.map -o program.tm32 main.tmo
```

## 15. Conclusion

The TM32 Linker is a sophisticated tool that serves as the final stage in the TM32 development toolchain. It provides comprehensive linking capabilities including symbol resolution, relocation processing, memory layout management, multiple output formats, and integrated quality assurance through seamless TM32 ROM Validator integration. The linker's design supports both simple single-file programs and complex multi-module projects with static libraries while ensuring code quality and compliance.

Key strengths of the enhanced TM32 Linker include:

- **Comprehensive Format Support**: Full support for TM32 object files, archives, and multiple output formats
- **Flexible Memory Management**: Configurable memory layout with linker script support and TM32 Core System compliance validation
- **Advanced Optimization**: Dead code elimination, link-time optimization, and performance analysis capabilities
- **Robust Error Handling**: Detailed error reporting and diagnostic capabilities with integrated validation feedback
- **Quality Assurance**: Built-in validation features and seamless integration with TM32 ROM Validator for comprehensive quality control
- **Integration-Friendly**: Seamless integration with build systems, CI/CD pipelines, and development workflows
- **Extensibility**: Plugin architecture and custom format support for specialized requirements
- **Security and Performance**: Integrated security analysis and performance optimization recommendations

The enhanced TM32 Linker with integrated validation capabilities ensures that linked executables meet the highest standards of quality, safety, and performance for the TM32 Core System and other TM32-based virtual hardware. This integration eliminates the gap between linking and validation, providing developers with immediate feedback and ensuring that only compliant, high-quality programs are produced. The comprehensive toolchain integration supports efficient development workflows from initial assembly through final deployment, making it an essential component for professional TM32 software development.
