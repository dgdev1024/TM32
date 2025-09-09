# Specification - TM32 Virtual CPU

Below is a specification for a simple, emulator-only virtual CPU which I will
call the "TM32" ("Technical Machine #32").

## 1. Architecture Overview

- Byte order: Little-Endian
    - *Example*: `0x12345678` would be stored in memory as `0x78 0x56 0x34 0x12`.
- Data path width: 32 bits
    - Instructions can work with 8-, 16- and 32-bit data.
- Address bus width: 32 bits
    - Accessible memory can be addressed in the range of `0x00000000` to
      `0xFFFFFFFF`, for a total of 4 GiB.

## 2. Memory Map

The table below specifies the memory ranges making up the TM32's assumed broad
memory map layout, as well as read/write/execute permissions for each range.

### 2.1 Memory Layout

The TM32 CPU features a flat, 32-bit address space, divided into several distinct
regions, as laid out in the table below. Each region has specific purposes and
access permissions. The virtual hardware powered by the TM32 will implement a
more detailed memory map based on this broad layout.

**Permissions**: `R` = Readable, `W` = Writable, `X` = Executable, `*` = See Notes

| Start        | End          | Size            | Permissions   | Description                                                   |
|--------------|--------------|-----------------|---------------|---------------------------------------------------------------|
| `0x00000000` | `0x00000FFF` | 4 KiB           | `R`           | Program Metadata - Information about the program.             |
| `0x00001000` | `0x00002FFF` | 8 KiB           | `R / X`       | Interrupt Service Vectors - Up to 32 vectors, 256 bytes each. |
| `0x00003000` | `0x7FFFFFFF` | Up to ~2 GiB    | `R / X`       | Program Code / Data - Main program code and static data.      |
| `0x80000000` | `0xBFFFFFFF` | Up to 1 GiB     | `R / W`       | RAM - General-purpose read/write memory.                      |
| `0xC0000000` | `0xEFFFFFFF` | Up to 768 MiB   | `R / W / X`   | Executable RAM - Read/write memory that can also be executed. |
| `0xF0000000` | `0xFFFDFFFF` | Up to 4 MiB     | `R / W`       | Hardware-Reserved RAM - Memory reserved for hardware use.     |
| `0xFFFE0000` | `0xFFFEFFFF` | 64 KiB          | `*`           | Stack Memory - Used for the stack.                            |
| `0xFFFF0000` | `0xFFFFFEFF` | Up to ~64 KiB   | `R / W`       | High-Page RAM ("Quick RAM") - Fast access RAM.                |
| `0xFFFFFF00` | `0xFFFFFFFF` | 256 Bytes       | `R / W`       | I/O Registers - Memory-mapped I/O registers.                  |

#### Alignments

- Interrupt Vectors: 256 Bytes

#### Notes

`*`: Only the stack and control-transfer instructions (`PUSH`, `POP`, `CALL` and
    `RET`) can manipulate memory in this space.

- The CPU itself does not have any built-in memory bus. This is to be provided
    by virtual hardware powered by the TM32, like the [TM32 Core](tm32core.specification.md).

## 3. Register File

The TM32 contains a set of quickly-accessible **registers**, some of which can
be used for any general purpose and others which are used for certain special
purposes.

### 3.1 General-Purpose Registers

The TM32 contains four 32-bit general-purpose registers, which can be accessed
in one of the following ways:

- Full 32-bit registers
- Partial 16-bit registers; the lower half of the full 32-bit register
- Partial 8-bit high registers; the upper half of the partial 16-bit register
- Partial 8-bit low registers; the lower half of the partial 16-bit register

The table below illustrates the accessible general-purpose registers in the
TM32's register file.

| 32-bit | 16-bit | 8-bit High | 8-bit Low | Notes                              |
|--------|--------|------------|-----------|------------------------------------|
| `A`    | `AW`   | `AH`       | `AL`      | Primary Accumulator                |
| `B`    | `BW`   | `BH`       | `BL`      |                                    |
| `C`    | `CW`   | `CH`       | `CL`      |                                    |
| `D`    | `DW`   | `DH`       | `DL`      |                                    |

#### Behavior

- Each of the general-purpose registers are initialized to zero.
- Writing to any of the partial registers updates its corresponding full 32-bit
    register.
- Writing to any of the 8- and 16-bit partial registers leaves the upper bits
    of the full 32-bit register unchanged.

### 3.2 Special-Purpose Registers

In addition to the general-purpose registers, the TM32's register file also
contains several special-purpose registers, which are illustrated in the table
below:

| Register | Size    | Description                                                      |
|----------|---------|------------------------------------------------------------------|
| `PC`     | 32 bits | Program Counter - address of next immediate data fetch.          |
| `IP`     | 32 bits | Instruction Pointer - address of next instruction opcode.        |
| `SP`     | 32 bits | Stack Pointer - address in RAM of next push to/pop from stack.   |
| `DA`     | 32 bits | Destination Address - address of next write to memory.           |
| `SA`     | 32 bits | Source Address - address of next non-immediate fetch.            |
| `SD`     | 32 bits | Source Data - fetched data needed by next instruction.           |
| `F`      | 32 bits | Flags - indicates current state of CPU and hardware.             |
| `IF`     | 32 bits | Interrupt Flags - indicates which interrupts are pending.        |
| `IE`     | 32 bits | Interrupt Enable - indicates which interrupts are enabled.       |
| `CI`     | 16 bits | Current Instruction - opcode of next instruction to execute.     |
| `EC`     | 8 bits  | Error Code - either set by `SEC` instruction or by exception.    |
| `XD`     | 8 bits  | Extra Data - additional data needed by next instruction.         |
| `IME`    | 1 bit   | Interrupt Master Enable - should any interrupts be handled?      |
| `EI`     | 1 bit   | Enable Interrupts - should IME be enabled at end of frame?       |

#### Behavior

- The program counter is initialized to the program code section's starting
    address `0x00003000`.
- The instruction pointer is set to the current value of the program counter
    just before the next instruction is fetched.
- The stack pointer is initialized to the RAM/stack memory's ending address
    `0xFFFEFFFC`, growing downward and shrinking upward.
- The stack is considered to be full if `SP` is less than or equal to
    `0xFFFE0003`, and empty if `SP` is at its starting address `0xFFFEFFFC`.
- The address and data registers, and the current instruction register are
    each initialized to zero, and are reset as such just before the next
    instruction is fetched.
- All other special-purpose registers are initialized to zero.
- The `XD` register is used to store the bit number needed by the `BIT`, `TOG`,
    `SET` and `RES` instructions. Future instructions may make use of this
    register, as well.
- In some implementations, the `CI` register can be broken down into a set of
    subregisters:
    - `ITYPE`: The upper 8 bits of the `CI` register contains the type of
        instruction to execute.
    - `IPARAM1`: The next 8 bits contain the first instruction parameter.
    - `IPARAM2`: The lower 8 bits contain the second instruction parameter.
    - See the **5.1 Instruction Opcode Format** section for more information
        on how these parameters are used in the instruction set.

#### Aliases

- `FLAGS` is an alias for the Flags register (`F`).
- `INTFLAGS` is an alias for the Interrupt Flags register (`IF`).
    - Since `if` is a reserved keyword in many programming languages, the
      `INTFLAGS` alias is often used instead.
- `INTENABLE` is an alias for the Interrupt Enable register (`IE`).

### 3.3 The Flags Register

The Flags register (`F`) is a special-purpose register whose bits are used to
indicate the current state of the TM32, as well as any virtual hardware which
may be powered by the TM32. Below is an illustration of what each bit in this
register indicates:

- Bit `0`: `T` - Stop Flag
    - Set by the `STOP` instruction.
    - CPU enters ultra-low power standby mode when this flag is set.
    - Can only be cleared by external wake-up conditions or resetting the CPU.
- Bit `1`: `L` - Halt Flag
    - Set by the `HALT` instruction.
    - CPU does not execute any further instructions when this flag is set.
    - Provides moderate power savings compared to normal operation.
    - Cleared when an interrupt is pending, even when its corresponding bit in
      `IE` is clear, or the `IME` is off.
    - Can also be cleared by resetting the CPU.
    - **Note**: Unlike `STOP`, `HALT` can be easily woken by any interrupt condition.
- Bit `2`: `P` - Parity Flag
    - Set when the result of the last instruction has an even number of set bits.
    - Used for execution conditions by the control-transfer instructions.
- Bit `3`: `S` - Sign Flag
    - Set when the result of the last signed arithmetic instruction is negative.
    - Used for execution conditions by the control-transfer instructions.
- Bit `4`: `C` - Carry Flag
    - Set when the result of the last instruction overflows its destination register.
    - Used for execution conditions by the control-transfer instructions.
- Bit `5`: `H` - Half-Carry Flag
    - Only for instructions working with 8-bit and 16-bit data and registers.
    - Set when the result of the last instruction overflows all but the most significant
        nibble of its destination register.
    - Used by the `DAA` instruction for binary-coded decimal adjustment.
- Bit `6`: `N` - Subtration Flag
    - Set when the last instruction is or involves a subtraction.
    - Used by the `DAA` instruction for binary-coded decimal adjustment.
    - Also called the "Negate Flag".
- Bit `7`: `Z` - Zero Flag
    - Set when the result of the last instruction is zero.
    - Used for execution conditions by the control-transfer instructions.
- Bits `8` through `15`: Reserved for future use by the TM32.
- Bits `16` through `31`: Reserved for use by software and virtual hardware.
    - The `FLG`, `STF` and `CLF` instructions are used to check and manipulate
        these flag bits.

### 3.4 The Error Code Register

The Error Code register (`EC`) is a special-purpose register which is used to
indicate whether or not something has gone wrong. This register is set by one
of the following circumstances:

- An exception occurs, such as an invalid instruction or an access to an
    invalid memory address. In this case, interrupt vector 0 is called in order
    to handle the exception.
- The `SEC XX` instruction can be used to explicitly set the `EC` register to a
    user-defined value. This does not set the Stop flag.
- The `CEC` instruction sets the `EC` register to zero.

### 3.5 The `STOP` Instruction and Ultra-Low Power Mode

The `STOP` instruction is designed to put the TM32 CPU into an ultra-low power
standby mode, significantly reducing power consumption when the system is idle.
This instruction is particularly useful for battery-powered applications or when
the system hasn't detected user input for an extended period.

#### Behavior

When the `STOP` instruction is executed:

1. **Stop Flag**: The Stop flag (bit 0) in the Flags register is set.
2. **CPU State**: The CPU enters ultra-low power standby mode and ceases normal
   instruction execution.
3. **Hardware Effects**: Virtual hardware components powered by the TM32 may
   enter their own low-power states or maintain minimal functionality.
4. **Memory Preservation**: All register contents and memory state are preserved
   during STOP mode.

#### Wake-up Conditions

The CPU exits STOP mode under these conditions:

- **Input Device Activity**: Any button press or joypad input
- **Serial Communication**: Completion of serial transfers
- **System Reset**: Hardware or software reset
- **External Hardware Events**: Virtual hardware can implement additional wake-up sources

#### What Cannot Wake `STOP`:

- Timer interrupts (system clocks are stopped)
- Regular hardware interrupts (unless specifically designed as wake-up sources)
- Software-generated interrupts

#### Instruction Format

In the Game Boy CPU's implementation, the `STOP` instruction follows a 2-byte
format. However, since opcodes in the TM32 are 16-bit, the second byte here is
implicitly part of the opcode:

- **Opcode**: `0x0100` (16-bit opcode).
- **Length**: 2 bytes
- **Cycles**: 2 cycles to execute, then CPU enters standby mode

#### Power Management Considerations

- **Ultra-Low Power**: STOP mode provides the lowest power consumption state
  available on the TM32.
- **Hardware Coordination**: Virtual hardware should implement their own
  power-saving behaviors when the CPU enters STOP mode.
- **Wake-up Latency**: There may be a brief delay when exiting STOP mode as
  the CPU and hardware components resume normal operation.

#### Programming Best Practices

- Use `STOP` when the application hasn't detected user input for a period
- Ensure proper wake-up conditions are configured before entering STOP mode
- Consider the trade-off between power savings and wake-up responsiveness
- Test STOP behavior thoroughly in the target virtual hardware environment

#### Comparison with `HALT`

| Feature                       | `HALT`                                | `STOP`                                    |
|-------------------------------|---------------------------------------|-------------------------------------------|
| **Power Savings**             | Moderate                              | Ultra-low (maximum)                       |
| **Wake-up Conditions**        | Any interrupt (even disabled)         | External wake-up events only              |
| **Implementation Complexity** | Simple                                | Requires hardware coordination            |
| **Use Case**                  | Brief waits, interrupt-driven tasks   | Long idle periods, battery conservation   |
| **Recovery Time**             | Immediate                             | May have brief latency                    |

### 3.6 The Stack and the Stack Pointer Register

The stack is a region of memory used for temporary storage of data, such as
register data and subroutine return addresses. The Stack Pointer register (`SP`) 
is a special-purpose register which points to the top of the stack, and is used 
to push and pop data to and from the stack.

The stack pointer begins at address `0xFFFEFFFC`, grows downward with each push
and shrinks upward with each pop. Note that only double-words (full register values
and absolute addresses) can be pushed to and popped from the stack, meaning that
the stack pointer is always aligned to a 4-byte boundary.

The following instructions affect the stack and manipulate the stack pointer:

- `PUSH` inserts a register's value onto the stack.
- `POP` removes a value from the stack and places it into a register.
- `CALL` and `INT` both push the program counter onto the stack before moving
    the program counter.
- `RET` and `RETI` both pop a return address from the stack, moving it into
    the program counter.
- Interrupts, when handled, also push the current program counter onto the
    stack before moving the program counter.

#### Exception Codes

The table below illustrates the error codes associated with exceptions that can
be thrown by the TM32:

| Code   | Enumeration              | Description                                                   |
|--------|--------------------------|---------------------------------------------------------------|
| `0x00` | `OK`                     | No error occured.                                             |
| `0x01` | `INVALID_OPCODE`         | Encountered an opcode not associated with an instruction.     |
| `0x02` | `INVALID_ARGUMENT`       | Attempted to execute an instruction with an invalid argument. |
| `0x03` | `INVALID_READ`           | Attempted to read from a non-readable memory address.         |
| `0x04` | `INVALID_WRITE`          | Attempted to read from a non-writable memory address.         |
| `0x05` | `INVALID_EXECUTE`        | Attempted to execute a non-executable memory address.         |
| `0x06` | `STACK_OVERFLOW`         | Attempted to push a value into a full stack.                  |
| `0x07` | `STACK_UNDERFLOW`        | Attempted to pop a value from an empty stack.                 |
| `0x08` | `DIVIDE_BY_ZERO`         | An instruction attempted to divide by zero. `*`               |
| `0x09` | `HARDWARE_ERROR`         | A hardware component raised an error during its clock.        |

##### Notes

- `*`: This error is not currently used by any instruction at the time of
    writing, but may be used in the future.
- The stack is considered to be full if `SP` is less than or equal to `0xFFFE0000`.
- The stack is considered to be empty if `SP` is at its starting address `0xFFFEFFFC`.

## 4. Interrupts

The TM32 supports a simple interrupt system, which allows virtual hardware to
signal the CPU to stop whatever it is it's doing in order to handle certain
events.

- **Address Range**: The interrupt vector subroutines are located at the address
    range `0x00001000` to `0x00002FFF`.
- **Vectors**: Up to 32 interrupt vectors are supported, with each vector
    subroutine having 256 (`0x100`) bytes reserved to them.
- **Triggers**: Interrupts can be triggered in one of two ways:
    - Setting the interrupt vector's corresponding bit in the `IF` and `IE`
        registers while the `IME` is set.
    - Explicitly calling them with the `INT XX` instruction, where `XX` is the
        index of the interrupt vector to call, a number between 0 and 31 (`0x00`
        and `0x1F`).
- **Servicing**: When an interrupt is enabled and pending while `IME` is set,
    the interrupt will be serviced according to the following entry sequence:
    - Clear the interrupt's corresponding bit in the `IF` register.
    - Clear `IME`.
    - Clear the `H` flag in the `F` flags register.
    - Push `PC` onto the stack.
    - Move `PC` to the start of the interrupt vector (`0x1000 + (0x100 * iv)`).
- **Returning**: Use one of the return instructions to return from the interrupt
    vector subroutine:
    - `RET` returns from the subroutine in the same way as other subroutines
        called with the `CALL` instruction.
    - `RETI` does the same, but also sets `IME`.
- **Priority**: One interrupt will be serviced at a time per frame. Interrupt
    vectors with lower indices have higher priority, and will be serviced before
    interrupt vectors with higher indices. As `IME` is cleared as part of
    acknowledging the interrupt, nested interrupts are disabled until `RETI` or
    `EI` are called.

### 4.1 Interrupt Vector 0 - Exception Handler

If the TM32 CPU sets the `EC` register due to an exception being thrown (such
as an invalid instruction or an invalid memory access), interrupt vector 0 is
automatically called to handle the exception. This occurs regardless of the 
current state of the CPU's interrupt flags (`IME`, `IE` and `IF`) - equivalent 
to using the `INT 0` instruction.

#### Exception Handling Process

When an exception occurs:

1. **Error Code**: The `EC` register is set to the appropriate exception code
2. **Interrupt Call**: Interrupt vector 0 is immediately invoked
3. **Context Preservation**: The program counter is pushed onto the stack
4. **Handler Execution**: Control transfers to the exception handler at `0x00001000`
5. **Recovery**: The exception handler can examine the `EC` register and take appropriate action

Although interrupt vector 0 can be called like any other interrupt vector and
used for any purpose as needed by software or other hardware, it is **strongly**
recommended that this vector be reserved for handling exceptions to ensure
proper error handling and system stability.

## 5. Instructions

As with any CPU, the TM32 works by executing **instructions** associated with
"operation codes" (**opcodes**) fetched from memory along with any other
immediate data which may be needed.

### 5.1 Instruction Opcode Format

Instruction opcodes in the TM32's instruction set are 16-bit values whose bits
are laid out in the following order:

- **Hexadecimal**: `0xOOXY`
- **Binary**: `0bOOOOOOOOXXXXYYYY`
- **Where**:
    - `O` indicates the type of instruction to execute. These will be laid out
        in the **5.2 Instruction Set** chapter.
    - `X` and `Y` are built-in parameters which affect how the instruction
        works.
        - For most instructions, `X` is used to indicate a destination register
            and `Y` is used to indicate a source register.
        - For certain instructions, such as the increment/decrement instructions,
            the `NOT` instruction, the shift/rotate instructions and the bit
            testing/manipulation instructions, `X` is used to indicate both a
            source register that is read from AND a destination register
            that is written to.
        - For the control-transfer instructions, `X` is used to indicate an
            execution condition.
        - For the `JMP X, Y` instruction, `Y` is a 32-bit destination register
            containing a destination address.
        - For the `SEC` instruction, `XY` contains the error code to set.
        - For more information on what these parameters may mean in each of the
            individual instructions, see the **Glossary** section below, in the
            **5.2 Instruction Set** chapter, as well as the **Instruction Notes**
            section at the end of this chapter.

#### Source and Destination Registers

For most instructions, the `X` and `Y` parameters are used to indicate source
and destination registers. These are one of the general-purpose registers, and
the table below illustrates which of these registers are mapped to these
parameters' values:

| Binary    | Hexadecimal   | Register  |
|-----------|---------------|-----------|
| `0b0000`  | `0x0`         | `A`       |
| `0b0001`  | `0x1`         | `AW`      |
| `0b0010`  | `0x2`         | `AH`      |
| `0b0011`  | `0x3`         | `AL`      |
| `0b0100`  | `0x4`         | `B`       |
| `0b0101`  | `0x5`         | `BW`      |
| `0b0110`  | `0x6`         | `BH`      |
| `0b0111`  | `0x7`         | `BL`      |
| `0b1000`  | `0x8`         | `C`       |
| `0b1001`  | `0x9`         | `CW`      |
| `0b1010`  | `0xA`         | `CH`      |
| `0b1011`  | `0xB`         | `CL`      |
| `0b1100`  | `0xC`         | `D`       |
| `0b1101`  | `0xD`         | `DW`      |
| `0b1110`  | `0xE`         | `DH`      |
| `0b1111`  | `0xF`         | `DL`      |

For certain instructions, the `X` parameter is used to indicate both a source
and destination register. In these cases, the `X` parameter is used to specify
the register that is read from AND the register that is written to. The following
instructions use this behavior:

- Increment/Decrement Instructions `INC`, `DEC`
- Shift/Rotate Instructions `SLA`, `SRA`, `SRL`, `RL`, `RLC`, `RR`, `RRC`
- Bit Testing/Manipulation Instructions `BIT`, `TOG`, `SET`, `RES`

#### Execution Conditions

For the control-transfer instructions (`JMP`, `JPB`, `CALL` and `RET`),
the `X` parameter is used to indicate an **execution condition**, which needs to
be met in order for the instruction to actually use and affect the program
counter. The execution conditions check to see if a certain bit in the Flags
register is either set or clear.

Any call to these instructions **must** include an execution condition, even if the
condition is "no condition" (i.e. always execute the instruction), in which case
the execution condition `NC` ("No Condition") is used. For instance, this code...

```assembly
JMP 0x12345678
RET
```

...would be illegal, but this code would be legal:

```assembly
JMP NC, 0x12345678
RET NC
```

The table below illustrates which execution conditions are mapped to the `X`
parameter's values, which bit in the Flags register the conditions check, and
whether that bit should be set or clear:

| Binary    | Hexadecimal   | Condition | Description                                               |
|-----------|---------------|-----------|-----------------------------------------------------------|
| `0b0000`  | `0x0`         | `NC`      | No condition; execute the instruction regardless.         |
| `0b0001`  | `0x1`         | `ZC`      | Execute the instruction only if the Zero flag is clear.   |
| `0b1001`  | `0x9`         | `ZS`      | Execute the instruction only if the Zero flag is set.     |
| `0b0010`  | `0x2`         | `CC`      | Execute the instruction only if the Carry flag is clear.  |
| `0b1010`  | `0xA`         | `CS`      | Execute the instruction only if the Carry flag is set.    |
| `0b0011`  | `0x3`         | `PC`      | Execute the instruction only if the Parity flag is clear. |
| `0b1011`  | `0xB`         | `PS`      | Execute the instruction only if the Parity flag is set.   |
| `0b0100`  | `0x4`         | `SC`      | Execute the instruction only if the Sign flag is set.     |
| `0b1100`  | `0xC`         | `SS`      | Execute the instruction only if the Sign flag is set.     |

### 5.2 Instruction Set

This chapter lists each instruction in the TM32's instruction set, their opcodes, 
and the bits in the Flags register which they affect. The columns in the
**Instruction Tables** section describe the following:

- **Opcode**: The instruction's operation code.
- **Mnemonic**: The instruction's mnemonic - how the instruction is read in a 
    disassembly thereof.
- **Length/Cycles**: The length of the instruction, in bytes, and the number of
    cycles consumed by this instruction.
    - In terms of length/cycles, an instruction is `X/Y`, which is to be read
        as follows: "It is `X` bytes in length and consumes `Y` cycles".
    - As instruction opcodes are 16-bit, an instruction's minimum length is
        2 bytes.
    - A **cycle** is consumed every time the TM32 accesses (reads to or writes
        from) the address bus.
        - One cycle is consumed every time a byte is read from or written to
            memory.
        - Two cycles are consumed every time a 16-bit value (two bytes)
            is read from or written to memory.
        - Four cycles are consumed every time a 32-bit value (four bytes)
            is read from or written to memory.
    - For the control-transfer and stack instructions, a cycle is also consumed
        every time the `PC` and `SP` registers are manipulated.
    - Whenever a cycle is consumed, the TM32 clocks any virtual hardware 
        attached to it, causing them to perform their specific operations.
    - As instruction opcodes are 16-bit, an instruction entails at least two
        reads from the address bus. As such, an instruction consumes 2 cycles
        at a minimum.
- **Flags**: A sequence of symbols indicating how the instruction affects the
    bits in the Flags register, if at all:
    - The sequence is listed in the following order: `ZNHCSP`, or bits 2 through
        7 of the Flags register.
    - The symbols in the sequence denote the following:
        - `-`: This flag bit is not affected.
        - `0`: This flag bit is cleared.
        - `1`: This flag bit is set.
        - `?`: This flag bit is affected according to the instruction's result, 
            and the flag bit's description in the **3.3 The Flags Register** 
            chapter.
        - `*`: This flag bit is affected in an instruction-specific manner. See
            the **Instruction Notes** section for more information.
- **Description**: A brief description of what the instruction does.

#### Timing

As the TM32 is a virtual-only CPU, the frequency at which cycles are consumed is
dictated by the virtual hardware it powers (ergo, the client application), via
a client-provided clock function. As such, the TM32 itself does not have a
fixed clock speed.

Remember that a clock cycle is consumed every time the TM32 accesses the address
bus, whether it is reading from or writing to memory, or by the control-transfer
and stack instructions manipulating the `PC` and `SP` registers.

#### Glossary

The mnemonics and descriptions shown in the **Instruction Tables** will contain
certain special words and phrases, each of which will be defined in the table
below, unless otherwise explicitly noted by the instruction's **Description**:

| Phrase            | Definition                                                                                            |
|-------------------|-------------------------------------------------------------------------------------------------------|
| `IMM`             | An immediate 8-, 16- or 32-bit integer. `*`                                                           |
| `SIMM16`          | An immediate 16-bit signed integer.                                                                   |
| `[ADDR8]`         | A value in memory at a relative 8-bit address. `***`                                                  |
| `[ADDR16]`        | A value in memory at a relative 16-bit address. `**`                                                  |
| `[ADDR32]`        | A value in memory at an absolute 32-bit address.                                                      |
| `X`               | A destination 8-, 16- or 32-bit register.                                                             |
| `X8`              | A destination 8-bit register.                                                                         |
| `X16`             | A destination 16-bit register.                                                                        |
| `X32`             | A destination 32-bit register.                                                                        |
| `Y`               | A source 8-, 16- or 32-bit register.                                                                  |
| `Y8`              | A source 8-bit register.                                                                              |
| `Y16`             | A source 16-bit register.                                                                             |
| `Y32`             | A source 32-bit register.                                                                             |
| `[X8]`            | A value in memory at a relative 8-bit destination address indicated by 8-bit register `X`. `***`      |
| `[X16]`           | A value in memory at a relative 16-bit destination address indicated by 8-bit register `X`. `**`      |
| `[X32]`           | A value in memory at an absolute 32-bit destination address indicated by 32-bit register `X`.         |
| `[Y8]`            | A value in memory at a relative 8-bit source address indicated by 8-bit register `Y`. `***`           |
| `[Y16]`           | A value in memory at a relative 16-bit source address indicated by 8-bit register `Y`. `**`           |
| `[Y32]`           | A value in memory at an absolute 32-bit source address indicated by 32-bit register `Y`.              |
| `Z`               | An execution condition. See the **Execution Conditions** section above.                               |

##### Notes

- `*`: The exact size of `IMM` depends on the size of the source/destination
    register associated with the instruction.
- `**`: This address is relative to the absolute 32-bit address `0xFFFF0000`.
- `***`: This address is relative to the absolute 32-bit address `0xFFFFFF00`.

#### Instruction Tables

The tables below illustrates the instructions making up the TM32's instruction
set. Keep the following in mind as you read the tables below:

- If the **Length/Cycles** column is blank for a given instruction, then assume
    that the instruction is 2/2 (it is 2 bytes in length and consumes 2 cycles).
- If the **Flags** column is blank for a given instruction, then assume that the
    instruction does not affect bits 2 through 7 of the Flags register.
- If any column for an instruction contains an asterisk `*`, then more
    information on the instruction can be found in the **Instruction Notes**
    section.
- For a full, exhaustive list of all instructions and variants, consult the
    [TM32 CPU Instruction Reference](tm32cpu.instruction-set.md) document.
    
##### General Instructions

| Opcode    | Mnemonic          | Length/Cycles | Flags     | Description                                                   |
|-----------|-------------------|---------------|-----------|---------------------------------------------------------------|
| `0x0000`  | `NOP`             |               |           | Does nothing.                                                 |
| `0x0100`  | `STOP`            | 2/2           |           | Enters ultra-low power standby mode. Sets the Stop flag.      |
| `0x0200`  | `HALT`            |               |           | Enters low power mode. Sets the Halt flag.                    |
| `0x03XX`  | `SEC XX`          |               |           | Sets the `EC` register to byte `XX`.                          |
| `0x0400`  | `CEC`             |               |           | Sets the `EC` register to zero.                               |
| `0x0500`  | `DI`              |               |           | Clears `IME`, disabling interrupts.                           |
| `0x0600`  | `EI`              |               |           | Sets `IME` at end of frame, enabling interrupts.              |
| `0x0700`  | `DAA`             |               | `?-0*-?`  | Decimal-adjusts byte register `AL`.                           |
| `0x0800`  | `SCF`             |               | `-001--`  | Sets the Carry flag in the Flags register.                    |
| `0x0900`  | `CCF`             |               | `-00*--`  | Complements (toggles) the carry flag in the Flags register.   |
| `0x0AX0`  | `FLG X`           |               | `*-----`  | Checks bit (16 + `X`) of the Flags register.                  |
| `0x0BX0`  | `STF X`           |               |           | Sets bit (16 + `X`) of the Flags register.                    |
| `0x0CX0`  | `CLF X`           |               |           | Clears bit (16 + `X`) of the Flags register.                  |

##### Data Transfer Instructions

| Opcode    | Mnemonic          | Length/Cycles | Flags     | Description                                                   |
|-----------|-------------------|---------------|-----------|---------------------------------------------------------------|
| `0x10X0`  | `LD X, IMM`       | `*`           |           | Loads `IMM` into `X`.                                         |
| `0x11X0`  | `LD X, [ADDR32]`  | 6/`*`         |           | Loads `[ADDR32]` into `X`.                                    |
| `0x12XY`  | `LD X, [Y32]`     | 2/`*`         |           | Loads `[Y32]` into `X`.                                       |
| `0x13X0`  | `LDH X, [ADDR16]` | 4/`*`         |           | Loads `[ADDR16]` into `X`.                                    |
| `0x14XY`  | `LDH X, [Y16]`    | 2/`*`         |           | Loads `[Y16]` into `X`.                                       |
| `0x15X0`  | `LDP X8, [ADDR8]` | 3/3           |           | Loads `[ADDR8]` into `X8`.                                    |
| `0x16XY`  | `LDP X8, [Y8]`    | 2/3           |           | Loads `[Y8]` into `X8`.                                       |
| `0x170Y`  | `ST [ADDR32], Y`  | 6/`*`         |           | Stores `Y` in `[ADDR32]`.                                     |
| `0x18XY`  | `ST [X32], Y`     | 2/`*`         |           | Stores `Y` in `[X32]`.                                        |
| `0x190Y`  | `STH [ADDR16], Y` | 4/`*`         |           | Stores `Y` in `[ADDR16]`.                                     |
| `0x1AXY`  | `STH [X16], Y`    | 2/`*`         |           | Stores `Y` in `[X16]`.                                        |
| `0x1B0Y`  | `STP [ADDR8], Y8` | 3/3           |           | Stores `Y8` in `[ADDR8]`.                                     |
| `0x1CXY`  | `STP [X8], Y8`    | 2/3           |           | Stores `Y8` in `[X8]`.                                        |
| `0x1DXY`  | `MV X, Y`         |               |           | Moves `Y` into `X`.                                           |
| `0x1EX0`  | `POP X32`         | 2/7           |           | Pops a value from the stack, then loads it into `X32`.        |
| `0x1F0Y`  | `PUSH Y32`        | 2/7           |           | Pushes `Y32` into the stack.                                  |

##### Control Transfer Instructions

| Opcode    | Mnemonic          | Length/Cycles | Flags     | Description                                                   |
|-----------|-------------------|---------------|-----------|---------------------------------------------------------------|
| `0x20Z0`  | `JMP Z, IMM32`    | 6/`*`         |           | Given `Z`, sets `PC` to `IMM32`.                              |
| `0x21ZX`  | `JMP Z, X32`      | 2/`*`         |           | Given `Z`, sets `PC` to `X32`.                                |
| `0x22Z0`  | `JPB Z, SIMM16`   | 4/`*`         |           | Given `Z`, moves `PC` by `SIMM16`.                            |
| `0x23Z0`  | `CALL Z, IMM32`   | 6/`*`         |           | Given `Z`, calls subroutine at address `IMM32`.               |
| `0x24XX`  | `INT XX`          | 2/8           |           | Calls interrupt vector subroutine at index `XX`. `*`          |
| `0x25Z0`  | `RET Z`           | 2/`*`         |           | Given `Z`, returns from current subroutine.                   |
| `0x2600`  | `RETI`            | 2/8           |           | Enables interrupts, then returns from current subroutine.     |

##### Arithmetic Instructions

| Opcode    | Mnemonic          | Length/Cycles | Flags     | Description                                                   |
|-----------|-------------------|---------------|-----------|---------------------------------------------------------------|
| `0x30X0`  | `INC X`           |               | `?0?--?`  | Increments `X` by one.                                        |
| `0x31X0`  | `INC [X32]`       | 2/4           | `?0?--?`  | Increments byte `[X32]` by one.                               |
| `0x32X0`  | `DEC X`           |               | `?1?--?`  | Decrements `X` by one.                                        |
| `0x33X0`  | `DEC [X32]`       | 2/4           | `?1?--?`  | Decrements byte `[X32]` by one.                               |
| `0x34X0`  | `ADD X, IMM`      | `*`           | `?0??0?`  | Adds `IMM` to accumulator `X`.                                |
| `0x35X0`  | `ADD X, [ADDR32]` | 6/`*`         | `?0??0?`  | Adds `[ADDR32]` to accumulator `X`.                           |
| `0x36XY`  | `ADD X, [Y32]`    | 2/`*`         | `?0??0?`  | Adds `[Y32]` to accumulator `X`.                              |
| `0x37X0`  | `ADC X, IMM`      | `*`           | `?0??0?`  | Adds `IMM`  and Carry flag to accumulator `X`.                |
| `0x38X0`  | `ADC X, [ADDR32]` | 6/`*`         | `?0??0?`  | Adds `[ADDR32]`  and Carry flag to accumulator `X`.           |
| `0x39XY`  | `ADC X, [Y32]`    | 2/`*`         | `?0??0?`  | Adds `[Y32]`  and Carry flag to accumulator `X`.              |
| `0x3AX0`  | `SUB X, IMM`      | `*`           | `?1????`  | Subtracts `IMM` from accumulator `X`.                         |
| `0x3BX0`  | `SUB X, [ADDR32]` | 6/`*`         | `?1????`  | Subtracts `[ADDR32]` from accumulator `X`.                    |
| `0x3CXY`  | `SUB X, [Y32]`    | 2/`*`         | `?1????`  | Subtracts `[Y32]` from accumulator `X`.                       |
| `0x3DX0`  | `SBC X, IMM`      | `*`           | `?1????`  | Subtracts `IMM`  and Carry flag from accumulator `X`.         |
| `0x3EX0`  | `SBC X, [ADDR32]` | 6/`*`         | `?1????`  | Subtracts `[ADDR32]`  and Carry flag from accumulator `X`.    |
| `0x3FXY`  | `SBC X, [Y32]`    | 2/`*`         | `?1????`  | Subtracts `[Y32]`  and Carry flag from accumulator `X`.       |

##### Bitwise Instructions

| Opcode    | Mnemonic          | Length/Cycles | Flags     | Description                                                   |
|-----------|-------------------|---------------|-----------|---------------------------------------------------------------|
| `0x40X0`  | `AND X, IMM`      | `*`           | `?0100?`  | Bitwise ANDs `IMM` with accumulator `X`.                      |
| `0x41X0`  | `AND X, [ADDR32]` | 6/`*`         | `?0100?`  | Bitwise ANDS `[ADDR32]` with accumulator `X`.                 |
| `0x42XY`  | `AND X, [Y32]`    | 2/`*`         | `?0100?`  | Bitwise ANDS `[Y32]` with accumulator `X`.                    |
| `0x43X0`  | `OR X, IMM`       | `*`           | `?0000?`  | Bitwise ORs `IMM` with accumulator `X`.                       |
| `0x44X0`  | `OR X, [ADDR32]`  | 6/`*`         | `?0000?`  | Bitwise ORS `[ADDR32]` with accumulator `X`.                  |
| `0x45XY`  | `OR X, [Y32]`     | 2/`*`         | `?0000?`  | Bitwise ORS `[Y32]` with accumulator `X`.                     |
| `0x46X0`  | `XOR X, IMM`      | `*`           | `?0000?`  | Bitwise XORs `IMM` with accumulator `X`.                      |
| `0x47X0`  | `XOR X, [ADDR32]` | 6/`*`         | `?0000?`  | Bitwise XORS `[ADDR32]` with accumulator `X`.                 |
| `0x48XY`  | `XOR X, [Y32]`    | 2/`*`         | `?0000?`  | Bitwise XORS `[Y32]` with accumulator `X`.                    |
| `0x4CX0`  | `NOT X`           |               | `-11---`  | Bitwise NOTs (complements) `X`.                               |
| `0x4DX0`  | `NOT [X32]`       | 2/4           | `-11---`  | Bitwise NOTs (complements) byte `[X32]`.                      |

##### Comparison Instructions

| Opcode    | Mnemonic          | Length/Cycles | Flags     | Description                                                   |
|-----------|-------------------|---------------|-----------|---------------------------------------------------------------|
| `0x50X0`  | `CMP X, IMM`      | `*`           | `?1???-`  | Compares accumulator `X` with `IMM`.                          |
| `0x51X0`  | `CMP X, [ADDR32]` | 6/`*`         | `?1???-`  | Compares accumulator `X` with `[ADDR32]`.                     |
| `0x52XY`  | `CMP X, [Y32]`    | 2/`*`         | `?1???-`  | Compares accumulator `X` with `[Y32]`.                        |

##### Shift and Rotate Instructions

| Opcode    | Mnemonic          | Length/Cycles | Flags     | Description                                                   |
|-----------|-------------------|---------------|-----------|---------------------------------------------------------------|
| `0x60X0`  | `SLA X`           |               | `?00*-?`  | Shifts `X` left arithmetically. `*`                           |
| `0x61X0`  | `SLA [X32]`       | 2/4           | `?00*-?`  | Shifts byte `[X32]` left arithmetically. `*`                  |
| `0x62X0`  | `SRA X`           |               | `?00*-?`  | Shifts `X` right arithmetically. `*`                          |
| `0x63X0`  | `SRA [X32]`       | 2/4           | `?00*-?`  | Shifts byte `[X32]` right arithmetically. `*`                 |
| `0x64X0`  | `SRL X`           |               | `?00*-?`  | Shifts `X` right logically. `*`                               |
| `0x65X0`  | `SRL [X32]`       | 2/4           | `?00*-?`  | Shifts byte `[X32]` right logically. `*`                      |
| `0x66X0`  | `RL X`            |               | `?00*-?`  | Rotates `X` left, through the Carry flag. `*`                 |
| `0x67X0`  | `RL [X32]`        | 2/4           | `?00*-?`  | Rotates byte `[X32]` left, through the Carry flag. `*`        |
| `0x68X0`  | `RLC X`           |               | `?00*-?`  | Rotates `X` left. `*`                                         |
| `0x69X0`  | `RLC [X32]`       | 2/4           | `?00*-?`  | Rotates byte `[X32]` left. `*`                                |
| `0x6AX0`  | `RR X`            |               | `?00*-?`  | Rotates `X` right, through the Carry flag. `*`                |
| `0x6BX0`  | `RR [X32]`        | 2/4           | `?00*-?`  | Rotates byte `[X32]` right, through the Carry flag. `*`       |
| `0x6CX0`  | `RRC X`           |               | `?00*-?`  | Rotates `X` right. `*`                                        |
| `0x6DX0`  | `RRC [X32]`       | 2/4           | `?00*-?`  | Rotates byte `[X32]` right. `*`                               |

##### Bit Testing and Manipulation Instructions

| Opcode    | Mnemonic          | Length/Cycles | Flags     | Description                                                   |
|-----------|-------------------|---------------|-----------|---------------------------------------------------------------|
| `0x70X0`  | `BIT XD, X`       | 3/3           | `*01---`  | Tests bit `XD` of `X`.                                        |
| `0x71X0`  | `BIT XD, [X32]`   | 3/4           | `*01---`  | Tests bit `XD` of byte `[X32]`.                               |
| `0x72X0`  | `TOG XD, X`       | 3/3           |           | Toggles (complements) bit `XD` of `X`.                        |
| `0x73X0`  | `TOG XD, [X32]`   | 3/5           |           | Toggles (complements) bit `XD` of byte `[X32]`.               |
| `0x74X0`  | `SET XD, X`       | 3/3           |           | Sets bit `XD` of `X`.                                         |
| `0x75X0`  | `SET XD, [X32]`   | 3/5           |           | Sets bit `XD` of byte `[X32]`.                                |
| `0x76X0`  | `RES XD, X`       | 3/3           |           | Clears bit `XD` of `X`.                                       |
| `0x77X0`  | `RES XD, [X32]`   | 3/5           |           | Clears bit `XD` of byte `[X32]`.                              |

#### Instruction Notes

##### General Instructions

- `DAA` sets the Carry flag if the upper nibble of the byte register needs to be
    decimal adjusted, and the Carry flag is not already set.
- `CCF` sets the Carry flag if clear, and clears it if set.
- `FLG` sets the Zero flag if bit (16 + `X`) of the Flags register is clear.

###### STOP Instruction Behavior

The `STOP` instruction (`0x0100`) implements Game Boy-inspired ultra-low power standby mode:

- **Power Mode**: Enters the lowest power consumption state available
- **CPU State**: All registers and memory contents are preserved during STOP mode
- **Exit Conditions**: Can only be exited by external wake-up events or system reset
- **Hardware Effects**: Virtual hardware components should implement their own low-power behaviors
- **Wake-up Latency**: Brief delay may occur when resuming from STOP mode
- **Use Cases**: Ideal for idle periods, battery conservation, or when awaiting user input
- **Exception Handling**: Unlike previous behavior, exceptions no longer set the Stop flag; they invoke interrupt vector 0 instead

Unlike `HALT` which can be woken by any interrupt, `STOP` requires specific external wake-up conditions to be configured by the virtual hardware implementation.

##### Data Transfer Instructions

- `LD X, IMM` is 3/3 if `X` is an 8-bit register, 4/4 if `X` is a 16-bit
    register and 6/6 if `X` is a 32-bit register.
- `LD X, [ADDR32]` is 6/7 if `X` is an 8-bit register, 6/8 if `X` is a 16-bit
    register and 6/10 if `X` is a 32-bit register.
- `LD X, [Y32]` is 2/3 if `X` is an 8-bit register, 2/4 if `X` is a 16-bit
    register and 2/6 if `X` is a 32-bit register.
- `LDH X, [ADDR16]` is 4/5 if `X` is an 8-bit register, 4/6 if `X` is a 16-bit
    register and 4/8 if `X` is a 32-bit register.
- `LDH X, [Y16]` is 2/3 if `X` is an 8-bit register, 2/4 if `X` is a 16-bit
    register and 2/6 if `X` is a 32-bit register.
- `ST [ADDR32], Y` is 6/7 if `X` is an 8-bit register, 6/8 if `X` is a 16-bit
    register and 6/10 if `X` is a 32-bit register.
- `ST [X32], Y` is 2/3 if `Y` is an 8-bit register, 2/4 if `Y` is a 16-bit
    register and 2/6 if `Y` is a 32-bit register.
- `STH [ADDR16], Y` is 4/5 if `Y` is an 8-bit register, 4/6 if `Y` is a 16-bit
    register and 4/8 if `Y` is a 32-bit register.
- `STH [X16], Y` is 2/3 if `Y` is an 8-bit register, 2/4 if `Y` is a 16-bit
    register and 2/6 if `Y` is a 32-bit register.

##### Control Transfer Instructions

- `JMP Z, IMM32` is 6/7 if `Z` is fulfilled, 6/6 otherwise.
- `JMP Z, X32` is 2/3 if `Z` is fulfilled, 2/2 otherwise.
- `JPB Z, SIMM16` is 4/5 if `Z` is fulfilled, 4/4 otherwise.
- `CALL Z, IMM32` is 6/12 if `Z` is fulfilled, 6/6 otherwise.
- For `INT XX`, `XX` must be a value between `0x00` and `0x1F`.
- `RET Z` is 2/8 if `Z` is fulfilled, 2/2 otherwise.

##### Arithmetic Instructions

- `ADD X, IMM`, `ADC X, IMM`, `SUB X, IMM` and `SBC X, IMM` are 3/3 if `X` is an 
    8-bit register, 4/4 if `X` is a 16-bit register and 6/6 if `X` is a 32-bit 
    register.
- `ADD X, [ADDR32]`, `ADC X, [ADDR32]`, `SUB X, [ADDR32]` and `SBC X, [ADDR32]`
    are 6/7 if `X` is an 8-bit register, 6/8 if `X` is a 16-bit register and 
    6/10 if `X` is a 32-bit register.
- `ADD X, [Y32]`, `ADC X, [Y32]`, `SUB X, [Y32]` and `SBC X, [Y32]` are 2/3 if 
    `X` is an 8-bit register, 2/4 if `X` is a 16-bit register and 2/6 if `X` is 
    a 32-bit register.

##### Bitwise Instructions

- `AND X, IMM`, `OR X, IMM` and `XOR X, IMM` are 3/3 if `X` is an 8-bit 
    register, 4/4 if `X` is a 16-bit register and 6/6 if `X` is a 32-bit 
    register.
- `AND X, [ADDR32]`, `OR X, [ADDR32]` and `XOR X, [ADDR32]` are 6/7 if `X` is an 
    8-bit register, 6/8 if `X` is a 16-bit register and 6/10 if `X` is a 32-bit 
    register.
- `AND X, [Y32]`, `OR X, [Y32]` and `XOR X, [Y32]` are 2/3 if `X` is an 8-bit 
    register, 2/4 if `X` is a 16-bit register and 2/6 if `X` is a 32-bit 
    register.

##### Comparison Instructions

- `CMP X, IMM` is 3/3 if `X` is an 8-bit register, 4/4 if `X` is a 16-bit
    register and 6/6 if `X` is a 32-bit register.
- `CMP X, [ADDR32]` is 6/7 if `X` is an 8-bit register, 6/8 if `X` is a 16-bit
    register and 6/10 if `X` is a 32-bit register.
- `CMP X, [Y32]` is 2/3 if `X` is an 8-bit register, 2/4 if `X` is a 16-bit
    register and 2/6 if `X` is a 32-bit register.

##### Shift and Rotate Instructions

- `SLA` sets the Carry flag to the leftmost bit before the shift, then sets the
    rightmost bit to zero after the shift.
- `SRA` sets the Carry flag to the rightmost bit before the shift and leaves the
    leftmost bit unchanged.
- `SRL` sets the Carry flag to the rightmost bit before the shift, then sets the
    leftmost bit to zero after the shift.
- `RL` sets the Carry flag to the leftmost bit before the rotate, then sets the
    rightmost bit to the old state of the Carry flag after the rotate.
- `RR` sets the Carry flag to the rightmost bit before the rotate, then sets the
    leftmost bit to the old state of the Carry flag after the rotate.
- `RLC` sets the Carry flag to the leftmost bit before the rotate, then sets the
    rightmost bit to the old state of the leftmost bit after the rotate.
- `RRC` sets the Carry flag to the rightmost bit before the rotate, then sets the
    leftmost bit to the old state of the leftmost bit after the rotate.

##### Bit Testing and Manipulation Instructions

- For all of these instructions, `X` is both a source and destination register.
- For all of these instructions, `XD` must be between `0x00` and `0x07` if `X`
    is an 8-bit register, between `0x00` and `0x0F` if `X` is a 16-bit register,
    and `0x00` and `0x1F` if `X` is a 32-bit register.
- `BIT` sets the Zero flag if bit `XD` is clear.

## 6. CPU Pipeline and Execution Model

The TM32 CPU implements a simplified execution model optimized for predictable 
timing and ease of implementation. Understanding the execution pipeline helps 
developers write efficient code and predict program behavior.

### 6.1 Instruction Execution Pipeline

The TM32 uses a simple two-stage pipeline:

1. **Fetch Stage**: Read instruction opcode and immediate data from memory
2. **Execute Stage**: Perform the instruction operation and update state

#### Pipeline Characteristics

- **No Overlap**: Instructions complete fully before the next instruction begins
- **Deterministic Timing**: Every instruction has a fixed, predictable cycle count
- **No Branch Prediction**: All branches have the same timing regardless of outcome
- **No Speculative Execution**: Only the current instruction is processed

#### Instruction Fetch Process

1. **Opcode Fetch**: Read 16-bit instruction opcode (2 cycles)
2. **Parameter Fetch**: Read additional immediate data if required (variable cycles)
3. **Decode**: Identify instruction type and operands (0 cycles - overlapped)

#### Execution Process

1. **Operand Resolution**: Calculate effective addresses and fetch data
2. **Operation**: Perform the instruction's primary function
3. **Writeback**: Store results to registers or memory
4. **PC Update**: Advance program counter or handle control flow

### 6.2 Memory Access Patterns

#### Memory Bus Utilization

The TM32 CPU uses a unified memory bus for all accesses:

- **Instruction Fetch**: 2 cycles minimum for 16-bit opcode
- **Data Access**: 1/2/4 cycles based on data width (8/16/32-bit)
- **No Cache**: All accesses go directly to the memory subsystem
- **No Prefetch Buffer**: Instructions are fetched as needed

#### Address Calculation

Memory addressing follows these patterns:

- **Direct Addressing**: Address specified in instruction immediate data
- **Register Indirect**: Address taken from register contents
- **Register Offset**: Base register plus immediate offset
- **Stack Operations**: SP register manipulation with automatic adjustment

### 6.3 Performance Characteristics

#### Instruction Categories by Performance

**Fastest (2 cycles):**
- Register-only operations: `MV A, B`, `INC C`, `NOT D`
- Immediate loads to registers: `LD A, 0x1234`

**Fast (3-4 cycles):**
- 8-bit memory operations: `LD AL, [address]`, `ST [address], BL`
- Register indirect access: `LD A, [B]`, `ST [C], D`

**Moderate (4-6 cycles):**
- 16-bit memory operations: `LD AW, [address]`, `ST [address], BW`
- Arithmetic with memory operands: `ADD A, [address]`

**Slower (6-8 cycles):**
- 32-bit memory operations: `LD A, [address]`, `ST [address], A`
- Complex addressing modes: `LD A, [B + offset]`

**Slowest (7+ cycles):**
- Stack operations: `PUSH A`, `POP B`, `CALL function`, `RET NC`
- Interrupt processing: `INT vector`, interrupt entry/exit

#### Performance Optimization Guidelines

**Register Usage:**
- Keep frequently accessed data in registers
- Use smaller register sizes when possible (AL vs AW vs A)
- Minimize register-to-memory transfers

**Memory Access:**
- Group memory operations by data width
- Use register indirect addressing when possible
- Avoid complex address calculations in hot loops

**Control Flow:**
- Prefer conditional execution over branching when possible
- Keep subroutines short to minimize call overhead
- Use local labels to reduce branch distances

### 6.4 Edge Cases and Undefined Behavior

#### Instruction Edge Cases

**Invalid Opcodes:**
- Unrecognized instruction opcodes trigger exception vector 0
- EC register set to `INVALID_OPCODE` (0x01)
- Execution halts until exception handler is invoked

**Invalid Operands:**
- Out-of-range register numbers cause `INVALID_ARGUMENT` (0x02)
- Invalid bit positions for bit operations trigger exceptions
- Misaligned memory accesses may have platform-specific behavior

**Memory Access Violations:**
- Reads from non-readable memory trigger `INVALID_READ` (0x03)
- Writes to non-writable memory trigger `INVALID_WRITE` (0x04)
- Execution of non-executable memory triggers `INVALID_EXECUTE` (0x05)

#### Stack Edge Cases

**Stack Overflow:**
- Pushing to a full stack (SP <= 0xFFFE0000) triggers `STACK_OVERFLOW` (0x06)
- Stack pointer automatically decrements but operation fails
- Program state remains consistent for exception handling

**Stack Underflow:**
- Popping from empty stack (SP >= 0xFFFEFFFC) triggers `STACK_UNDERFLOW` (0x07)
- Stack pointer automatically increments but operation fails
- Return address or data value is unpredictable

#### Interrupt Edge Cases

**Nested Interrupts:**
- Interrupts are disabled during interrupt service routine entry
- `RETI` instruction re-enables interrupts automatically
- Manual `EI` instruction can enable interrupts within ISR (dangerous)

**Interrupt During HALT:**
- `HALT` instruction can be interrupted by any enabled interrupt
- CPU resumes normal execution after interrupt service completes
- If no interrupts are enabled, `HALT` continues indefinitely

**Interrupt During STOP:**
- Only wake-up events can exit STOP mode (not regular interrupts)
- Hardware platform determines which events can wake the CPU
- Regular interrupt handling resumes after wake-up

#### Flag Register Interactions

**Undefined Flag States:**
- Some instruction combinations may leave flags in undefined states
- Programs should not rely on flag preservation across subroutine calls
- Critical flag values should be explicitly saved/restored

**Flag Dependencies:**
- Instructions that require accumulator may implicitly modify flags
- Comparison instructions always affect flags regardless of register used
- Arithmetic instructions update flags based on the result size

### 6.5 Programming Model Best Practices

#### Predictable Timing

For applications requiring precise timing:

```assembly
; Example: Precise delay loop
precise_delay:
    ld b, DELAY_COUNT    ; 2 cycles
delay_loop:
    dec b                ; 2 cycles
    jmp zc, delay_loop   ; 3 cycles (taken) or 2 cycles (not taken)
    ret nc               ; 7 cycles
; Total: 2 + (DELAY_COUNT * 5) + 2 + 7 = (DELAY_COUNT * 5) + 11 cycles
```

#### Memory Layout Optimization

```assembly
; Organize data by access patterns
.data
; Hot data - accessed every frame
player_x:       .dd 0
player_y:       .dd 0
player_health:  .db 100

; Warm data - accessed occasionally  
score:          .dd 0
level:          .db 1

; Cold data - rarely accessed
high_scores:    .ds 40    ; 10 scores * 4 bytes
```

#### Error Handling Patterns

```assembly
; Robust error handling
safe_divide:
    ; A = dividend, B = divisor, result in A
    cmp b, 0             ; Check for division by zero
    jmp zs, divide_error ; Jump if divisor is zero
    
    ; Perform division (simplified)
    ; ... division implementation ...
    ret nc
    
divide_error:
    sec 0x08             ; Set error code for divide by zero
    ld a, 0              ; Return zero as safe default
    ret nc
```

#### Interrupt-Safe Programming

```assembly
; Critical section protection
critical_section:
    di                   ; Disable interrupts
    
    ; Critical code that must not be interrupted
    ld a, [shared_data]
    inc a
    st [shared_data], a
    
    ei                   ; Re-enable interrupts
    ret nc

; Atomic operations using temporary interrupt disable
atomic_increment:
    di                   ; Disable interrupts
    ld a, [counter]      ; Load current value
    inc a                ; Increment
    st [counter], a      ; Store back
    ei                   ; Re-enable interrupts
    ret nc
```

## 7. Virtual Hardware Integration

The TM32 CPU is designed to work with virtual hardware platforms that provide
memory management, I/O capabilities, and other system services. This section
describes how the CPU interfaces with virtual hardware systems.

### 7.1 Hardware Abstraction Interface

#### Memory Management Integration

The TM32 CPU delegates all memory management to the virtual hardware platform:

- **Address Translation**: Virtual hardware translates CPU addresses to physical memory
- **Permission Checking**: Hardware enforces read/write/execute permissions per memory region
- **Memory Mapping**: Platform-specific memory layout implemented by virtual hardware

#### I/O Register Interface

I/O operations use dedicated address ranges:

- **High Page (0xFFFF0000-0xFFFFFEFF)**: Fast access to platform-specific RAM
- **I/O Registers (0xFFFFFF00-0xFFFFFFFF)**: Memory-mapped hardware registers
- **Port Instructions**: `LDP`/`STP` provide optimized access to I/O region

### 7.2 Clock and Timing Integration

#### Hardware Clock Coordination

The CPU drives virtual hardware timing:

- **Cycle-Based Clocking**: Each CPU cycle triggers hardware component updates
- **Synchronized Operation**: All virtual hardware operates in lockstep with CPU
- **Deterministic Behavior**: Hardware state changes are predictable and repeatable

#### Power Management Integration

Power management involves coordination between CPU and virtual hardware:

- **HALT State**: CPU stops while hardware continues operation
- **STOP State**: Both CPU and most hardware enter low-power mode
- **Wake Events**: Hardware platform determines which events can wake the CPU

### 7.3 Exception and Interrupt Integration

#### Hardware-Generated Exceptions

Virtual hardware can generate CPU exceptions:

- **Hardware Errors**: Platform reports errors via `HARDWARE_ERROR` exception (0x09)
- **Invalid Access**: Memory management unit reports access violations
- **Device Errors**: I/O devices can signal error conditions

#### Interrupt Coordination

Interrupt handling requires hardware platform support:

- **Interrupt Sources**: Platform defines which hardware generates interrupts
- **Priority Management**: Hardware may implement interrupt priority schemes
- **Vector Routing**: Platform routes interrupt sources to appropriate vectors

This comprehensive expansion of the CPU architecture documentation provides
developers with detailed information about the TM32's execution model,
performance characteristics, and integration with virtual hardware platforms.
    