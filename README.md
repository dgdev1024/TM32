# TM32 Virtual CPU Emulator

The TM32 is a custom, virtual, emulator-only CPU complete with virtual hardware platform, assembler, linker and emulator. This project provides a complete toolchain for developing and running programs on the TM32 architecture.

## Features

- **32-bit Virtual CPU**: Complete CPU emulator with 16 registers, 1MB memory
- **Assembler**: Converts TM32 assembly language to bytecode
- **Linker**: Links object files into executable programs
- **Emulator**: Executes TM32 programs with debugging capabilities
- **Cross-platform**: Written in C99, runs on Linux, macOS, and Windows

## Architecture Overview

- **Word Size**: 32 bits
- **Registers**: 16 general-purpose registers (R0-R15)
- **Memory**: 1MB addressable memory space
- **Stack**: 64KB dedicated stack space
- **Instruction Set**: Simple RISC-style instructions

## Building

### Prerequisites
- GCC compiler (or compatible C compiler)
- Make build system

### Build Instructions
```bash
# Clone the repository
git clone https://github.com/dgdev1024/TM32.git
cd TM32

# Build the project
make

# Run tests
make test

# Clean build artifacts
make clean
```

## Usage

The TM32 emulator provides a command-line interface with the following commands:

### Assembling Source Code
```bash
./tm32 asm input.s output.o
```

### Linking Object Files
```bash
./tm32 link object1.o object2.o output.exe
```

### Running Programs
```bash
./tm32 run program.exe
```

### Built-in Test
```bash
./tm32 test
```

## Example Workflow

1. **Write assembly code** (`hello.s`):
   ```assembly
   NOP
   NOP
   HALT
   ```

2. **Assemble to object file**:
   ```bash
   ./tm32 asm hello.s hello.o
   ```

3. **Link to executable**:
   ```bash
   ./tm32 link hello.o hello.exe
   ```

4. **Run the program**:
   ```bash
   ./tm32 run hello.exe
   ```

## Project Structure

```
TM32/
├── src/                 # Source code
│   ├── main.c          # Main program entry point
│   ├── tm32.c          # CPU emulator core
│   ├── tm32_assembler.c # Assembler implementation
│   ├── tm32_linker.c   # Linker implementation
│   └── tm32_emulator.c # Emulator utilities
├── include/            # Header files
│   ├── tm32.h          # Core CPU definitions
│   ├── tm32_assembler.h
│   ├── tm32_linker.h
│   └── tm32_emulator.h
├── examples/           # Example programs
├── tests/              # Test scripts
├── docs/               # Documentation (specifications)
├── Makefile           # Build system
└── README.md          # This file
```

## Contributing

This project is part of a custom CPU development effort. Contributions are welcome!

## License

MIT License - see LICENSE file for details.
