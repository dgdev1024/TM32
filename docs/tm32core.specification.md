# Specification: TM32 Core

Below is a specification for the "TM32 Core". The TM32 Core is a virtual machine
powered by the TM32 CPU architecture, with hardware components heavily inspired
by the hardware of the Nintendo Game Boy and Game Boy Color. In other words, the
TM32 Core is a Game Boy (Color), only powered by the TM32 CPU, rather than its
original CPU, the Sharp LR35902.

## 0. Table of Contents

1. [Overview](#overview)
2. [CPU Architecture](#cpu-architecture)
3. [Memory Map](#memory-map)
4. [Hardware Components](#hardware-components)
    - [Boot Process and System Initialization](#boot-process-and-system-initialization)
    - [Port Registers](#port-registers)
    - [Interrupts](#interrupts)
    - [Timer Component](#timer-component)
    - [Real-Time Clock Component](#real-time-clock-component)
    - [Error Handling and Hardware Diagnostics](#error-handling-and-hardware-diagnostics)
    - [Joypad Component](#joypad-component)
    - [Serial Transfer Component](#serial-transfer-component)
    - [PPU Component](#ppu-component)
    - [APU Component](#apu-component)
    - [RAM Component](#ram-component)
5. [Performance and Timing Specifications](#performance-and-timing-specifications)
6. [Programs](#programs)

## 1. Overview

The TM32 Core is a virtual machine that emulates the hardware of the Nintendo
Game Boy and Game Boy Color. The goal of the TM32 Core is to provide a platform
for running programs/games which mimic, as closely as possible, the behavior
of the original hardware, while also providing additional hardware features not
available on the original hardware, and also providing a much larger ROM and RAM
space than the original hardware, without the need for memory banking or bank
switching.

## 2. CPU Architecture

The TM32 Core is powered by the TM32 CPU architecture, which is a 32-bit
CPU architecture. The TM32 CPU provides an instruction set that is reminiscent
of that used by the Nintendo Game Boy, the Sharp LR35902, enhanced with 32-bit
instructions and additional features.

For the full specification of the TM32 CPU architecture, refer to the
[TM32 CPU Specification](tm32cpu.specification.md), and for the full instruction
set reference, refer to the [TM32 CPU Instruction Set Reference](tm32cpu.instruction-set.md).

## 3. Memory Map

The TM32 Core uses a memory map which is derived from the broader memory layout
of the TM32 CPU, and also defines further memory regions for its hardware
components:

| Start        | End          | Size            | Permissions   | Description                                                   |
|--------------|--------------|-----------------|---------------|---------------------------------------------------------------|
| `0x00000000` | `0x00000FFF` | 4 KiB           | `R`           | Program Metadata - Information about the program.             |
| `0x00001000` | `0x00002FFF` | 8 KiB           | `R / X`       | Interrupt Service Vectors - Up to 32 vectors, 256 bytes each. |
| `0x00003000` | `0x7FFFFFFF` | Up to ~2 GiB    | `R / X`       | Program Code / Data - Main program code and static data.      |
| `0x80000000` | `0x9FFFFFFF` | Up to 512 MiB   | `R / W`       | Working RAM (WRAM) - Read/write memory for general use.       |
| `0xA0000000` | `0xBFFFFFFF` | Up to 512 MiB   | `R / W`       | External RAM (SRAM) - Read/write memory for external use.     |
| `0xC0000000` | `0xEFFFFFFF` | Up to 768 MiB   | `R / W / X`   | Executable RAM - Read/write memory that can also be executed. |
| `0xF0000000` | `0xFFFDFFFF` | Up to 4 MiB     | `R / W`       | Hardware-Reserved RAM - Memory reserved for hardware use.     |
| `0xF0008000` | `0xF0009FFF` | 8 KiB           | `R / W`       | Video RAM (VRAM) - Memory for tile graphics data.             |
| `0xF000FE00` | `0xF000FE9F` | 160 Bytes       | `R / W`       | Object Attribute Memory (OAM) - Memory for sprite attributes. |
| `0xFFFE0000` | `0xFFFEFFFF` | 64 KiB          | `*`           | Stack Memory - Used for the stack.                            |
| `0xFFFF0000` | `0xFFFFFEFF` | Up to ~64 KiB   | `R / W`       | High-Page RAM ("Quick RAM") - Fast access RAM.                |
| `0xFFFFFF00` | `0xFFFFFFFF` | 256 Bytes       | `R / W`       | I/O Registers - Memory-mapped I/O registers.                  |

**Permissions**: `R` = Readable, `W` = Writable, `X` = Executable, `*` = See Notes

#### Notes

`*`: Only the stack and control-transfer instructions (`PUSH`, `POP`, `CALL` and
    `RET`) can manipulate memory in this space.

## 4. Hardware Components

The TM32 Core provides a set of virtual hardware components that emulate the
hardware of the Nintendo Game Boy (Color). These components are designed
to provide the same functionality as the original hardware, with minimal
modifications to accommodate the 32-bit address space and the TM32 CPU
architecture.

The TM32 Core also provides some virtual hardware components of its own, which
are not present on the original hardware, but are designed to provide additional
functionality and features.

### 4.1 Boot Process and System Initialization

The TM32 Core follows a structured boot sequence that initializes all hardware
components and prepares the system for program execution. Understanding this
process is crucial for proper system programming and debugging.

#### 4.1.1 Boot Sequence Overview

The boot process occurs in the following phases:

1. **Hardware Reset**: All components are reset to their default states
2. **Memory Initialization**: Memory regions are configured and cleared
3. **Program Loading**: Program code and data are loaded from storage
4. **Hardware Configuration**: Core hardware components are initialized
5. **Program Execution**: Control is transferred to the loaded program

#### 4.1.2 Hardware Reset Phase

During hardware reset, all TM32 Core components are initialized to known states:

**CPU State:**
- Program Counter (`PC`) set to `0x00003000` (program code start)
- Stack Pointer (`SP`) set to `0xFFFEFFFC` (stack top)
- All general-purpose registers (`A`, `B`, `C`, `D`) cleared to `0x00000000`
- All flags cleared except system initialization flags
- Interrupt Master Enable (`IME`) cleared (interrupts disabled)

**Hardware Components:**
- Timer: All timer registers cleared, timer disabled
- RTC: Continues running from previous state if battery-backed
- PPU: Display disabled, all registers set to compatible defaults
- APU: All audio channels disabled, master volume muted
- Serial: Transfer disabled, data registers cleared
- Joypad: Input detection enabled, no keys pressed state

**Memory State:**
- Working RAM (WRAM): Cleared to `0x00` (optional, implementation-dependent)
- External RAM (SRAM): Preserved from previous session
- Video RAM (VRAM): Cleared to `0x00`
- Object Attribute Memory (OAM): Cleared to `0x00`
- High-Page RAM: Cleared to `0x00`
- Stack Memory: Not cleared (will be overwritten during use)

#### 4.1.3 Program Loading Phase

The TM32 Core loads programs from the Program Code/Data memory region 
(`0x00003000` to `0x7FFFFFFF`). The loading process includes:

**Program Metadata Validation:**
- Check program header at `0x00000000` for validity
- Verify program size and checksums
- Validate required system version compatibility
- Confirm memory requirements can be satisfied

**Code and Data Loading:**
- Copy program code to execution region starting at `0x00003000`
- Initialize static data segments
- Set up interrupt vector table if program provides custom vectors
- Configure any required memory banking or paging

**Error Conditions:**
- Invalid program header: System halts with error code `0x01`
- Insufficient memory: System halts with error code `0x02`
- Corrupted program data: System halts with error code `0x03`
- Version incompatibility: System halts with error code `0x04`

#### 4.1.4 Hardware Initialization Phase

After successful program loading, hardware components are configured for
operation:

**Interrupt System:**
- Interrupt vectors configured based on program requirements
- Default exception handler installed at vector `0x00`
- Hardware interrupt sources remain disabled until explicitly enabled

**Memory Configuration:**
- Memory protection boundaries established
- Stack guard pages configured (if supported)
- Memory-mapped I/O regions activated

**Component Initialization Order:**
1. Timer component (for basic timing services)
2. Real-Time Clock (if enabled)
3. Serial transfer component
4. Joypad input component
5. PPU (Picture Processing Unit)
6. APU (Audio Processing Unit)

#### 4.1.5 Program Execution Phase

Finally, control is transferred to the loaded program:

**Execution Start:**
- Program Counter set to entry point (typically `0x00003000`)
- Interrupts remain disabled initially
- Program responsible for enabling required interrupts
- System hardware is ready for use

**Initial Program Responsibilities:**
- Configure required hardware components
- Set up interrupt service routines
- Enable necessary interrupts (`IME`, `IE`, specific interrupt bits)
- Initialize application-specific data structures
- Begin main program execution loop

#### 4.1.6 Boot Time Characteristics

**Typical Boot Times:**
- Hardware reset: < 1ms
- Memory initialization: 1-5ms (depending on RAM size)
- Program loading: 5-50ms (depending on program size)
- Hardware initialization: 2-10ms
- **Total boot time**: Typically 10-70ms

**Power-On vs. Reset:**
- **Power-On Boot**: Full initialization sequence, all memory cleared
- **Soft Reset**: Faster boot, some state may be preserved
- **Debug Reset**: Minimal initialization, state preserved for debugging

#### 4.1.7 Boot Failure Handling

If boot fails at any stage, the system enters a safe error state:

**Error State Behavior:**
- CPU enters HALT mode with error code in `EC` register
- All hardware components disabled except essential systems
- Error code indicates failure point for debugging
- System requires full reset to attempt boot again

**Recovery Mechanisms:**
- External reset button or signal
- Watchdog timer reset (if implemented)
- Power cycle reset
- Debug interface reset (development systems)

### 4.2 Port Registers

The TM32 Core's virtual hardware maps their settings to several port registers,
which are accessible at the address `0xFFFFFF00` to `0xFFFFFFFF`. These
port registers are used to control the behavior of the hardware components,
and to provide additional functionality not present on the original hardware.

The table below lists these port registers, their addresses, and their
descriptions:

| Register | Name       | R/W       | Description                                         |
|----------|------------|-----------|-----------------------------------------------------|
| `0x00`   | `JOYP`     | R/W `*`   | Joypad Input                                        |
| `0x01`   | `SB`       | R/W       | Serial Data                                         |
| `0x02`   | `SC`       | R/W       | Serial Control                                      |
| `0x03`   | `DIV`      | R/W       | Timer Divider                                       |
| `0x04`   | `TIMA`     | R/W       | Timer Counter                                       |
| `0x05`   | `TMA`      | R/W       | Timer Modulo                                        |
| `0x06`   | `TAC`      | R/W       | Timer Control                                       |
| `0x10`   | `NR10`     | R/W       | APU Channel 1 Frequency Sweep                       |
| `0x11`   | `NR11`     | R/W `*`   | APU Channel 1 Length / Duty                         |
| `0x12`   | `NR12`     | R/W       | APU Channel 1 Volume / Envelope                     |
| `0x13`   | `NR13`     | W         | APU Channel 1 Period Low Byte                       |
| `0x14`   | `NR14`     | R/W `*`   | APU Channel 1 Period High 3 Bits / Control          |
| `0x16`   | `NR21`     | R/W `*`   | APU Channel 2 Length / Duty                         |
| `0x17`   | `NR22`     | R/W       | APU Channel 2 Volume / Envelope                     |
| `0x18`   | `NR23`     | W         | APU Channel 2 Period Low Byte                       |
| `0x19`   | `NR24`     | R/W `*`   | APU Channel 2 Period High 3 Bits / Control          |
| `0x1A`   | `NR30`     | R/W       | APU Channel 3 DAC Enable                            |
| `0x1B`   | `NR31`     | W         | APU Channel 3 Length                                |
| `0x1C`   | `NR32`     | R/W       | APU Channel 3 Output Level                          |
| `0x1D`   | `NR33`     | W         | APU Channel 3 Period Low Byte                       |
| `0x1E`   | `NR34`     | R/W `*`   | APU Channel 3 Period High 3 Bits / Control          |
| `0x20`   | `NR41`     | W         | APU Channel 4 Length                                |
| `0x21`   | `NR42`     | R/W       | APU Channel 4 Volume / Envelope                     |
| `0x22`   | `NR43`     | R/W       | APU Channel 4 Frequency / Randomness                |
| `0x23`   | `NR44`     | R/W `*`   | APU Channel 4 Control                               |
| `0x24`   | `NR50`     | R/W       | APU Master Volume Control / VIN Panning             |
| `0x25`   | `NR51`     | R/W       | APU Channel Panning Control                         |
| `0x26`   | `NR52`     | R/W `*`   | APU Master Control / Status                         |
| `0x40`   | `LCDC`     | R/W       | PPU Display Control                                 |
| `0x41`   | `STAT`     | R/W `*`   | PPU Display Status                                  |
| `0x42`   | `SCY`      | R/W       | PPU Viewport Y-Scroll Position                      |
| `0x43`   | `SCX`      | R/W       | PPU Viewport X-Scroll Position                      |
| `0x44`   | `LY`       | R         | PPU Current Scanline                                |
| `0x45`   | `LYC`      | R/W       | PPU Scanline Compare                                |
| `0x46`   | `DMA`      | R/W       | Alias of `ODMA` for OAM DMA Transfer Start          |
| `0x47`   | `BGP`      | R/W       | (DMG) PPU Background Palette Data                   |
| `0x48`   | `OBP0`     | R/W       | (DMG) PPU Object Palette 0 Data                     |
| `0x49`   | `OBP1`     | R/W       | (DMG) PPU Object Palette 1 Data                     |
| `0x4A`   | `WY`       | R/W       | PPU Window Y-Position                               |
| `0x4B`   | `WX`       | R/W       | PPU Window X-Position                               |
| `0x4D`   | `KEY1`     | R/W `*`   | (CGB) Prepare Speed Switch                          |
| `0x4F`   | `VBK`      | R/W       | (CGB) VRAM Bank Select                              |
| `0x56`   | `RP`       | R/W `*`   | (CGB) Infrared Communications Port                  |
| `0x68`   | `BGPI`     | R/W       | (CGB) Background Palette Index                      |
| `0x69`   | `BGPD`     | R/W       | (CGB) Background Palette Data                       |
| `0x6A`   | `OBPI`     | R/W       | (CGB) Object Palette Index                          |
| `0x6B`   | `OBPD`     | R/W       | (CGB) Object Palette Data                           |
| `0x6C`   | `OPRI`     | R/W       | (CGB) Object Priority Mode                          |
| `0x76`   | `PCM12`    | R         | (CGB) APU Digital Output 1 & 2                      |
| `0x77`   | `PCM34`    | R         | (CGB) APU Digital Output 3 & 4                      |
| `0x80`   | `RTCS`     | R         | (TM32) Real-Time Clock Seconds                      |
| `0x81`   | `RTCM`     | R         | (TM32) Real-Time Clock Minutes                      |
| `0x82`   | `RTCH`     | R         | (TM32) Real-Time Clock Hours                        |
| `0x83`   | `RTCD1`    | R         | (TM32) Real-Time Clock Day Byte 1                   |
| `0x84`   | `RTCD0`    | R         | (TM32) Real-Time Clock Day Byte 0                   |
| `0x85`   | `RTCC`     | R/W       | (TM32) Real-Time Clock Control                      |
| `0x87`   | `RTCL`     | W         | (TM32) Real-Time Clock Latch                        |
| `0xA0`   | `ODMAS3`   | W         | (TM32) OAM DMA Source Address Byte 3                |
| `0xA1`   | `ODMAS2`   | W         | (TM32) OAM DMA Source Address Byte 2                |
| `0xA2`   | `ODMAS1`   | W         | (TM32) OAM DMA Source Address Byte 1                |
| `0xA3`   | `ODMA`     | R/W       | (TM32) OAM DMA Start                                |
| `0xA4`   | `HDMAS3`   | W         | (TM32) HDMA Source Address Byte 3                   |
| `0xA5`   | `HDMAS2`   | W         | (TM32) HDMA Source Address Byte 2                   |
| `0xA6`   | `HDMAS1`   | W         | (TM32) HDMA Source Address Byte 1                   |
| `0xA7`   | `HDMAS0`   | W         | (TM32) HDMA Source Address Byte 0                   |
| `0xA8`   | `HDMAD3`   | W         | (TM32) HDMA Destination Address Byte 3              |
| `0xA9`   | `HDMAD2`   | W         | (TM32) HDMA Destination Address Byte 2              |
| `0xAA`   | `HDMAD1`   | W         | (TM32) HDMA Destination Address Byte 1              |
| `0xAB`   | `HDMAD0`   | W         | (TM32) HDMA Destination Address Byte 0              |
| `0xAC`   | `HDMA`     | R/W       | (TM32) HDMA Start / Control                         |
| `0xAD`   | `GRPM`     | R/W       | (TM32) PPU Graphics Mode                            |
| `0xF8`   | `IF3`      | R/W       | (TM32) Interrupt Flag Register Byte 3               |
| `0xF9`   | `IF2`      | R/W       | (TM32) Interrupt Flag Register Byte 2               |
| `0xFA`   | `IF1`      | R/W       | (TM32) Interrupt Flag Register Byte 1               |
| `0xFB`   | `IF0`      | R/W       | (TM32) Interrupt Flag Register Byte 0               |
| `0xFC`   | `IE3`      | R/W       | (TM32) Interrupt Enable Register Byte 3             |
| `0xFD`   | `IE2`      | R/W       | (TM32) Interrupt Enable Register Byte 2             |
| `0xFE`   | `IE1`      | R/W       | (TM32) Interrupt Enable Register Byte 1             |
| `0xFF`   | `IE0`      | R/W       | (TM32) Interrupt Enable Register Byte 0             |

- `*`: Although this register is marked as R/W, certain bits of this register
    are either read-only or write-only. These will be explained later on in
    this chapter.

### 4.3 Interrupts

The TM32 CPU uses a vector-based interrupt system, similar to the one used
by the Game Boy's CPU, but with 32 interrupt vectors instead of 8, with 256
bytes reserved for each interrupt service routine.

The TM32 Core's hardware is capable of requesting interrupts for the CPU to
handle. The following interrupt service vectors are generated by the following
hardware components:

| Vector | Start Address | Hardware Component | Description                                 |
|--------|---------------|--------------------|---------------------------------------------|
| `0x01` | `$00001100`   | PPU                | Vertical Blank (`vblank`) Interrupt         |
| `0x02` | `$00001200`   | PPU                | Display Status (`lcd_stat`) Interrupt       |
| `0x03` | `$00001300`   | Timer              | Timer Overflow Interrupt                    |
| `0x04` | `$00001400`   | Serial Transfer    | Serial Transfer Complete Interrupt          |
| `0x05` | `$00001500`   | Joypad             | Joypad Input Interrupt                      |
| `0x06` | `$00001600`   | Real-Time Clock    | Time Changed Interrupt                      |

### 4.4 Timer Component

The TM32 Core includes a built-in timer component that provides precise timing
functionality for programs. The timer system is based on the Nintendo Game Boy
timer design but enhanced for the TM32's 32-bit architecture. It consists of a
divider register that runs continuously and a programmable timer counter that
can generate interrupts at specified intervals.

The timer component is essential for:
- Generating periodic interrupts for task scheduling
- Implementing timing delays in programs
- Creating time-based game mechanics
- Synchronizing with other hardware components

#### 4.4.1 Timer Registers

The timer component provides four memory-mapped registers accessible through
the port register space:

| Address | Name   | Access | Description                |
|---------|--------|--------|----------------------------|
| `0x03`  | `DIV`  | R/W    | Timer Divider Register     |
| `0x04`  | `TIMA` | R/W    | Timer Counter Register     |
| `0x05`  | `TMA`  | R/W    | Timer Modulo Register      |
| `0x06`  | `TAC`  | R/W    | Timer Control Register     |

##### DIV - Timer Divider Register (0x03)

```
Bit  7   6   5   4   3   2   1   0
    D7  D6  D5  D4  D3  D2  D1  D0
```

The DIV register is a free-running 8-bit counter that increments at a fixed
rate of 16384 Hz (approximately 16779 Hz in practice). This register provides
a stable time reference for the system.

**Behavior:**
- Automatically increments every 256 CPU cycles (at 4.194304 MHz base clock)
- Writing any value to `DIV` resets it to `0x00`
- Cannot be stopped or paused by software
- Reset to `0x00` when the CPU executes a `STOP` instruction
- Continues running during `HALT` state

**Usage:**
- Reading `DIV` provides a continuously incrementing value for basic timing
- Writing to `DIV` provides a way to synchronize timing operations
- Can be used as a source of pseudo-random numbers

##### TIMA - Timer Counter Register (0x04)

```
Bit  7   6   5   4   3   2   1   0
    T7  T6  T5  T4  T3  T2  T1  T0
```

The `TIMA` register is the main programmable timer counter. It increments at a
rate determined by the `TAC` register's clock select bits.

**Behavior:**
- Increments at the frequency specified by `TAC` bits 1-0
- When `TIMA` overflows (0xFF → 0x00), it triggers the following sequence:
  1. `TIMA` is reloaded with the value from `TMA`
  2. Timer interrupt flag is set in `IF0` register (bit 2)
  3. If timer interrupts are enabled, an interrupt is generated

**Usage:**
- Set `TIMA` to a specific value to control when the next overflow occurs
- Use with `TMA` to create custom timing intervals
- Essential for implementing periodic interrupt-driven tasks

##### TMA - Timer Modulo Register (0x05)

```
Bit  7   6   5   4   3   2   1   0
    M7  M6  M5  M4  M3  M2  M1  M0
```

The `TMA` register contains the reload value for `TIMA` when it overflows.

**Behavior:**
- When `TIMA` overflows, its value is reset to `TMA`
- The time between interrupts = (256 - `TMA`) × (clock period from `TAC`)
- Writing to `TMA` during the same cycle as a timer overflow uses the old
  `TMA` value

**Usage Examples:**
- `TMA` = 0xFF: Interrupt every clock cycle (minimum interval)
- `TMA` = 0xFE: Interrupt every 2 clock cycles
- `TMA` = 0x00: Interrupt every 256 clock cycles (maximum interval)

##### TAC - Timer Control Register (0x06)

```
Bit  7   6   5   4   3   2   1   0
    -   -   -   -   -   E  S1  S0
```

The `TAC` register controls the timer's operation and clock frequency.

**Bit Fields:**
- **Bit 2 (E) - Timer Enable:**
  - 0: Timer stopped (`TIMA` does not increment)
  - 1: Timer enabled (`TIMA` increments at selected rate)
  
- **Bits 1-0 (S1-S0) - Clock Select:**
  - 00: 4096 Hz (`TIMA` increments every 1024 CPU cycles)
  - 01: 262144 Hz (`TIMA` increments every 16 CPU cycles)
  - 10: 65536 Hz (`TIMA` increments every 64 CPU cycles)
  - 11: 16384 Hz (`TIMA` increments every 256 CPU cycles)

- **Bits 7-3:** Unused, always read as 0

**Important Notes:**
- `DIV` continues to run regardless of the Timer Enable bit
- Changing clock select bits may cause `TIMA` to increment once immediately
- Writing to `TAC` should be done carefully to avoid unintended timer
  increments

#### 4.4.2 Timer Frequencies and Timing

The timer frequencies are derived from the TM32 CPU's base clock frequency of
4.194304 MHz:

| Clock Select | Frequency | Period  | CPU Cycles | Use Cases                     |
|--------------|-----------|---------|------------|-------------------------------|
| 00           | 4096 Hz   | ~244 μs | 1024       | Long delays, slow periodic    |
|              |           |         |            | tasks                         |
| 01           | 262144 Hz | ~3.8 μs | 16         | High-precision timing, audio  |
| 10           | 65536 Hz  | ~15.3μs | 64         | Medium-frequency events       |
| 11           | 16384 Hz  | ~61 μs  | 256        | Standard timing, matches DIV  |
|              |           |         |            | rate                          |

#### 4.4.3 Timer Interrupts

When the timer's `TIMA` register overflows and wraps around from `0xFF` to `0x00`,
it requests an interrupt by setting bit 3 of the `IF0` register (`0xFB`). This
"timer overflow interrupt" can be used for various purposes, such as:

- Real-time task scheduling
- Periodic game logic updates
- Audio sample rate control
- System heartbeat monitoring

**Interrupt Handling:**
1. Timer overflow sets bit 3 of `IF0` register (`0xFB`)
2. If bit 3 of `IE0` register (`0xFF`) is set, an interrupt is generated
3. CPU jumps to timer interrupt vector
    - The timer uses interrupt vector `03`, starting at address `$00001300`.
4. Software must clear the interrupt flag in the interrupt service routine

#### 4.4.4 Programming Examples

**Assumption: Port Register Constants**
```assembly
.const hDIV   = 0x03  ; Timer Divider Register
.const hTIMA  = 0x04  ; Timer Counter Register
.const hTMA   = 0x05  ; Timer Modulo Register
.const hTAC   = 0x06  ; Timer Control Register
.const hIF0   = 0xFB  ; Interrupt Flag Register Byte 0
```

**Example 1: 60 Hz Interrupt**
```assembly
.text
timer_60hz_setup:
    ; Setup timer for ~60 Hz interrupts
    ; 4096 Hz / 68 ≈ 60.24 Hz
    ld al, 188              ; TMA = 188 (0xBC) = 256-68
    stp [hTMA], al          ; Store to TMA register
    ld al, 4                ; Enable timer, clock select 00 (4096 Hz)
    stp [hTAC], al          ; Store to TAC register
    ret nc                  ; Return to caller
```

**Example 2: Precise Delay**
```assembly
.text
timer_precise_delay:
    ; Wait for exactly 1000 timer cycles at 65536 Hz
    ld al, 0                ; Reset TIMA
    stp [hTIMA], al         ; Store to TIMA register
    ld al, 5                ; Enable timer, clock select 10 (65536 Hz)
    stp [hTAC], al          ; Store to TAC register

delay_loop:
    ldp al, [hTIMA]         ; Read TIMA register
    cmp al, 232             ; Wait for 1000 increments (1000 & 0xFF = 232)
    jpb zc, delay_loop      ; Jump back if not equal (Zero flag clear)

    ld al, 0                ; Disable timer
    stp [hTAC], al          ; Store to TAC register
    ret nc                  ; Return to caller
```

**Example 3: Timer Interrupt Service Routine**
```assembly
.int 2
timer_isr:
    ; Save registers
    push a
    push b

    ; Clear timer interrupt flag
    ldp al, [hIF0]          ; Read IF0 register
    and al, 0xFB            ; Clear bit 3 (0xFB = 251)
    stp [hIF0], al          ; Write back to IF0 register

    ; Perform periodic task
    call nc, update_game_state

    ; Restore registers and return
    pop b
    pop a
    reti

.text
update_game_state:
    ; Application-specific update logic would go here
    ; This is called every timer interrupt
    ret nc
```

### 4.5 Real-Time Clock Component

The TM32 Core includes a Real-Time Clock (RTC) component that provides accurate
timekeeping functionality for programs. The RTC system is based on the Nintendo
Game Boy MBC3 real-time clock feature but enhanced for the TM32's 32-bit
architecture and integrated directly into the core hardware. It maintains
accurate time and date information that persists even when the system is
powered off, enabling features like time-based gameplay mechanics and event
scheduling.

The RTC component operates independently of the CPU and other system components,
maintaining its own internal clock that continues to track time even during
system sleep or halt states. This enables applications to implement sophisticated
time-based features such as daily events, seasonal changes, and real-time
scheduling.

#### 4.5.1 Real-Time Clock Overview

**Timekeeping Capabilities:**
- Accurate second, minute, and hour tracking
- Day counter with 16-bit capacity, intended for calendar year tracking (365-366 days)
- Latching mechanism for atomic time reading and change detection
- Battery-backed operation for persistent timekeeping
- Interrupt generation only when latched time values change
- Precision crystal oscillator for accurate timing

**Time Range:**
- **Seconds**: 0-59 (automatic rollover to minutes)
- **Minutes**: 0-59 (automatic rollover to hours)
- **Hours**: 0-23 (automatic rollover to days)
- **Days**: 0-366 (intended for calendar year tracking, though 16-bit capacity allows up to 65535)

**System Integration:**
- Independent operation from CPU clock
- Interrupt-driven time change notification
- Latching mechanism for atomic time reading
- Battery backup for continuous operation

#### 4.5.2 Real-Time Clock Registers

The RTC component provides eight memory-mapped registers accessible through
the port register space (0x80-0x87):

| Address | Name   | Access | Description                        |
|---------|--------|--------|------------------------------------|
| `0x80`  | `RTCS` | R      | RTC Seconds (0-59)                 |
| `0x81`  | `RTCM` | R      | RTC Minutes (0-59)                 |
| `0x82`  | `RTCH` | R      | RTC Hours (0-23)                   |
| `0x83`  | `RTCD1`| R      | RTC Day Counter High Byte          |
| `0x84`  | `RTCD0`| R      | RTC Day Counter Low Byte           |
| `0x85`  | `RTCC` | R/W    | RTC Control Register               |
| `0x87`  | `RTCL` | W      | RTC Latch Register                 |

**RTCS Register (0x80) - Seconds Counter (Read-Only):**

```
Bit  7   6   5   4   3   2   1   0
    -   -   S5  S4  S3  S2  S1  S0
```

The `RTCS` register contains the current seconds value (0-59).

**Bit Fields:**
- **Bits 5-0**: Seconds value (0-59, binary representation)
- **Bits 7-6**: Unused, always read as 0

**Behavior:**
- Internally increments every second
- Internally rolls over from 59 to 0, incrementing minutes counter
- Read-only, cannot be directly modified by software
- Register value only updates when latched via `RTCL` register

**RTCM Register (0x81) - Minutes Counter (Read-Only):**

```
Bit  7   6   5   4   3   2   1   0
    -   -   M5  M4  M3  M2  M1  M0
```

The `RTCM` register contains the current minutes value (0-59).

**Bit Fields:**
- **Bits 5-0**: Minutes value (0-59, binary representation)
- **Bits 7-6**: Unused, always read as 0

**Behavior:**
- Internally increments when seconds roll over from 59 to 0
- Internally rolls over from 59 to 0, incrementing hours counter
- Read-only, cannot be directly modified by software
- Register value only updates when latched via `RTCL` register

**RTCH Register (0x82) - Hours Counter (Read-Only):**

```
Bit  7   6   5   4   3   2   1   0
    -   -   -   H4  H3  H2  H1  H0
```

The `RTCH` register contains the current hours value (0-23).

**Bit Fields:**
- **Bits 4-0**: Hours value (0-23, binary representation)
- **Bits 7-5**: Unused, always read as 0

**Behavior:**
- Internally increments when minutes roll over from 59 to 0
- Internally rolls over from 23 to 0, incrementing day counter
- Read-only, cannot be directly modified by software
- Register value only updates when latched via `RTCL` register

**RTCD1/RTCD0 Registers (0x83/0x84) - Day Counter (Read-Only):**

```
RTCD1 (0x83):
Bit  7   6   5   4   3   2   1   0
    D15 D14 D13 D12 D11 D10 D9  D8

RTCD0 (0x84):
Bit  7   6   5   4   3   2   1   0
    D7  D6  D5  D4  D3  D2  D1  D0
```

The `RTCD1` and `RTCD0` registers form a 16-bit day counter intended for calendar year tracking.

**Bit Fields:**
- **RTCD1**: High byte of day counter (bits 15-8)
- **RTCD0**: Low byte of day counter (bits 7-0)
- **Combined**: 16-bit day value (intended range 0-366 for calendar years)

**Behavior:**
- Internally increments when hours roll over from 23 to 0
- Although capable of holding values up to 65535, intended for calendar use (0-366 days)
- Read-only, cannot be directly modified by software
- Values only change in read registers when latched via `RTCL` register

**RTCC Register (0x85) - RTC Control Register:**

```
Bit  7   6   5   4   3   2   1   0
    -   -   -   -   -   IE  -   EN
```

The `RTCC` register controls RTC operation and interrupt generation.

**Bit Fields:**
- **Bit 0 (EN) - RTC Enable:**
  - 0: RTC stopped (time does not advance)
  - 1: RTC running (normal operation)

- **Bit 2 (IE) - Interrupt Enable:**
  - 0: RTC interrupts disabled
  - 1: RTC interrupts enabled (generate interrupt on time changes)

- **Bits 7-3, 1**: Unused, always read as 0

**Usage:**
- Set EN bit to start/stop RTC operation
- Set IE bit to enable time change interrupts
- Can be used for power management and debugging

**RTCL Register (0x87) - RTC Latch Register (Write-Only):**

The `RTCL` register is used to latch the current RTC values for atomic reading and change detection.

**Behavior:**
- Writing any value to `RTCL` latches current internal RTC time to read registers
- Only when latching occurs do the read registers (`RTCS`, `RTCM`, `RTCH`, `RTCD1`, `RTCD0`) update
- After latching, all RTC read registers contain stable values from the latch moment
- Multiple reads return the same latched values until next latch operation
- Essential for reading multi-byte time values atomically
- If latching causes any register value to change, an RTC interrupt is requested

#### 4.5.3 RTC Operation

**Timekeeping Process:**

1. **Internal Clock**: RTC uses precision crystal oscillator for accurate timing
2. **Internal Counters**: Time values advance internally based on real time
3. **Rollover Handling**: Each internal time unit rolls over to next higher unit when maximum reached
4. **Persistence**: Internal time continues to advance even when system is powered off
5. **Latching Access**: Read registers only update when latched via `RTCL` register

**Reading RTC Values:**

To read the current time accurately, software must use the latch mechanism:

1. Write any value to `RTCL` register to latch current internal time to read registers
2. Read time registers (`RTCS`, `RTCM`, `RTCH`, `RTCD1`, `RTCD0`) in any order
3. All reads return the same latched time until next latch operation
4. Repeat process when updated time is needed

**Time Calculation Examples:**

```
Total seconds since RTC start = 
    (RTCD1 << 8 | RTCD0) * 86400 + 
    RTCH * 3600 + 
    RTCM * 60 + 
    RTCS

Days elapsed = (RTCD1 << 8 | RTCD0)
Current time = RTCH:RTCM:RTCS (HH:MM:SS format)
```

#### 4.5.4 RTC Interrupt (Vector 0x06)

The RTC component generates an interrupt only when latching the internal time
values causes any of the read-only time registers to change. This provides 
efficient notification of time progression without requiring constant polling,
while avoiding redundant interrupts when no time has actually elapsed.

**RTC Interrupt Trigger:**
- **Condition**: Latching via `RTCL` causes any time register (`RTCS`, `RTCM`, `RTCH`, `RTCD1`, `RTCD0`) to change value
- **Frequency**: Variable - only when latched time differs from previous latched time
- **No Automatic Interrupts**: Time advancing internally does not generate interrupts
- **Flag Setting**: Automatically sets bit 6 of `IF0` register when latching causes changes

**Interrupt Control:**
- **Enable**: Set bit 2 of `RTCC` register (RTC interrupt enable)
- **Global Enable**: Set bit 6 of `IE0` register (RTC interrupt enable)
- **Flag**: Automatically set in `IF0` register (bit 6) when triggered
- **Clear**: Software must clear bit 6 of `IF0` in interrupt handler
- **Priority**: Lower priority than Timer, Serial Transfer, and Joypad interrupts

**Interrupt Use Cases:**
- **Real-time Events**: Trigger actions at specific times
- **Periodic Tasks**: Execute code every second, minute, or hour
- **Time-based Gameplay**: Implement day/night cycles, events
- **Scheduling**: Create time-based game mechanics and features

#### 4.5.5 Programming Examples

**Example 1: Basic RTC Reading**
```assembly
.const hRTCS  = 0x80  ; RTC Seconds Register
.const hRTCM  = 0x81  ; RTC Minutes Register
.const hRTCH  = 0x82  ; RTC Hours Register
.const hRTCD1 = 0x83  ; RTC Day Counter High Byte
.const hRTCD0 = 0x84  ; RTC Day Counter Low Byte
.const hRTCC  = 0x85  ; RTC Control Register
.const hRTCL  = 0x87  ; RTC Latch Register

.text
read_rtc_time:
    ; Read current RTC time atomically
    ; Returns: AL=seconds, BL=minutes, CL=hours, DX=days
    
    ; Latch current time
    ld al, 0x01             ; Any value triggers latch
    stp [hRTCL], al         ; Store to RTCL register
    
    ; Read latched time values
    ldp al, [hRTCS]         ; Read seconds
    ldp bl, [hRTCM]         ; Read minutes  
    ldp cl, [hRTCH]         ; Read hours
    ldp dh, [hRTCD1]        ; Read day counter high byte
    ldp dl, [hRTCD0]        ; Read day counter low byte
    
    ret nc                  ; Return with time in registers
```

**Example 2: RTC Initialization and Interrupt Setup**
```assembly
.const hIE0 = 0xFF  ; Interrupt Enable Register Byte 0
.const hIF0 = 0xFB  ; Interrupt Flag Register Byte 0

.text
initialize_rtc:
    ; Initialize RTC for operation with interrupts
    
    ; Enable RTC and interrupts
    ld al, 0x05             ; EN=1, IE=1 (enable RTC and interrupts)
    stp [hRTCC], al         ; Store to RTCC register
    
    ; Enable RTC interrupt globally
    ldp al, [hIE0]          ; Read interrupt enable
    or al, 0x40             ; Set bit 6 (RTC interrupt enable)
    stp [hIE0], al          ; Write back to IE0
    
    ret nc

.int 6
rtc_isr:
    ; RTC interrupt service routine (called every second)
    push a
    push b
    push c
    push d
    
    ; Clear RTC interrupt flag
    ldp al, [hIF0]          ; Read interrupt flags
    and al, 0xBF            ; Clear bit 6 (RTC interrupt flag)
    stp [hIF0], al          ; Write back to IF0
    
    ; Read current time
    call nc, read_rtc_time
    
    ; Process time-based events
    call nc, process_time_events
    
    ; Restore registers and return
    pop d
    pop c
    pop b
    pop a
    reti
```

**Example 3: Time-based Game Events**
```assembly
.text
process_time_events:
    ; Handle time-based game events
    ; Input: AL=seconds, BL=minutes, CL=hours, DX=days
    
    ; Check for hourly events
    cmp bl, 0               ; Check if minutes = 0 (top of hour)
    jpb zc, check_daily_events
    call nc, hourly_event   ; Trigger hourly event
    
check_daily_events:
    ; Check for daily events (midnight)
    cmp bl, 0               ; Minutes = 0
    jpb zc, check_special_times
    cmp cl, 0               ; Hours = 0
    jpb zc, check_special_times
    call nc, daily_event    ; Trigger daily event
    
check_special_times:
    ; Check for specific time events
    cmp cl, 12              ; Check if noon (12:00)
    jpb zc, check_evening
    cmp bl, 0
    jpb zc, check_evening
    call nc, noon_event     ; Trigger noon event
    
check_evening:
    cmp cl, 18              ; Check if evening (18:00)
    jpb zc, time_events_end
    cmp bl, 0
    jpb zc, time_events_end
    call nc, evening_event  ; Trigger evening event
    
time_events_end:
    ret nc

hourly_event:
    ; Process hourly game events
    ; Example: Update shop inventory
    call nc, update_shop_inventory
    ret nc

daily_event:
    ; Process daily game events  
    ; Example: Reset daily quests
    call nc, reset_daily_quests
    ret nc

noon_event:
    ; Process noon events
    ; Example: Market price changes
    call nc, update_market_prices
    ret nc

evening_event:
    ; Process evening events
    ; Example: Light street lamps
    call nc, activate_street_lights
    ret nc
```

**Example 4: Time Duration Calculation**
```assembly
.text
calculate_elapsed_time:
    ; Calculate time elapsed since last save
    ; Input: Previous time in save_time_data
    ; Output: Elapsed seconds in A register (32-bit)
    
    ; Store time components temporarily for calculations
    stp [seconds_temp], al  ; Store seconds
    stp [minutes_temp], bl  ; Store minutes
    stp [hours_temp], cl    ; Store hours
    stp [days_temp], dx     ; Store days (16-bit)
    
    ; Read current time
    call nc, read_rtc_time
    
    ; Calculate current total seconds
    ; A = (days * 86400) + (hours * 3600) + (minutes * 60) + seconds
    
    ; Convert days to seconds (days * 86400)
    ; Use repeated addition approach for multiplication
    ld a, 0                 ; Initialize total seconds to 0
    ld b, dx                ; Load day counter into B
    cmp b, 0                ; Check if days = 0
    jpb zs, calc_hours      ; Skip if no days (Zero flag set)
    
    ; Multiply days by 86400 using repeated addition
    ld c, 86400             ; Seconds per day
days_multiply_loop:
    add a, c                ; Add 86400 to total
    dec b                   ; Decrement day counter
    jpb zc, days_multiply_loop ; Continue until B = 0 (Zero flag clear)
    
calc_hours:
    ; Add hours in seconds (hours * 3600)
    ldp bl, [hours_temp]    ; Get hours from temp storage
    and bl, 0xFF            ; Ensure 8-bit value
    cmp bl, 0               ; Check if hours = 0
    jpb zs, calc_minutes    ; Skip if no hours (Zero flag set)
    
    ; Multiply hours by 3600 using repeated addition
    ld c, 3600              ; Seconds per hour
hours_multiply_loop:
    add a, c                ; Add 3600 to total
    dec bl                  ; Decrement hour counter
    jpb zc, hours_multiply_loop ; Continue until BL = 0 (Zero flag clear)
    
calc_minutes:
    ; Add minutes in seconds (minutes * 60)
    ldp bl, [minutes_temp]  ; Get minutes from temp storage
    and bl, 0xFF            ; Ensure 8-bit value
    cmp bl, 0               ; Check if minutes = 0
    jpb zs, calc_seconds    ; Skip if no minutes (Zero flag set)
    
    ; Multiply minutes by 60 using repeated addition
    ld c, 60                ; Seconds per minute
minutes_multiply_loop:
    add a, c                ; Add 60 to total
    dec bl                  ; Decrement minute counter
    jpb zc, minutes_multiply_loop ; Continue until BL = 0 (Zero flag clear)
    
calc_seconds:
    ; Add seconds
    ldp bl, [seconds_temp]  ; Get seconds from temp storage
    and bl, 0xFF            ; Ensure 8-bit value
    add a, bl               ; A += seconds
    
    ; Subtract previous time
    ldp b, [save_time_total] ; Load saved total seconds
    sub a, b                ; A = current - previous
    
    ret nc                  ; Return elapsed seconds in A

; Temporary storage for time components
.data
seconds_temp:
    .db 0
minutes_temp:
    .db 0
hours_temp:
    .db 0
days_temp:
    .dw 0                   ; 16-bit day counter storage
save_time_total:
    .dd 0                   ; 32-bit total seconds at save time
```

#### 4.4.6 Advanced Features

**Battery Backup Operation:**
- RTC continues operation when main system power is off
- Dedicated battery maintains timekeeping for months or years
- Automatic power switching between main and battery power
- Low-power crystal oscillator for minimal battery drain

**Precision and Accuracy:**
- Crystal oscillator provides ±20 ppm accuracy (±1.7 seconds per day)
- Temperature compensation available in advanced implementations
- Calibration features for fine-tuning accuracy
- Long-term stability for reliable timekeeping

**Integration with Game Logic:**
- Day/night cycles synchronized with real time
- Seasonal events based on day counter
- Real-time multiplayer synchronization
- Achievement systems with time-based unlocks

**Power Management:**
- RTC can be disabled to save power when not needed
- Interrupt-driven operation reduces CPU polling overhead
- Low-power modes compatible with RTC operation
- Wake-on-time functionality for scheduled events

### 4.5 Serial Transfer Component

The TM32 Core includes a Serial Transfer component that provides communication
capabilities with external devices and other TM32 Core systems. The Serial
Transfer system is based on the Nintendo Game Boy's serial link interface but
enhanced for the TM32's 32-bit architecture and expanded functionality. It
supports both synchronous and asynchronous serial communication with
configurable baud rates and transfer modes.

The Serial Transfer component enables various communication scenarios including
multiplayer gaming, data exchange with peripheral devices, and debugging
interfaces. It uses a simple two-wire interface consisting of serial clock
(SCK) and serial data (SIN/SOUT) lines, with automatic shift register
operation for reliable data transmission.

#### 4.5.1 Serial Transfer Overview

**Communication Capabilities:**
- 8-bit synchronous serial data transfer
- Master and slave operation modes
- Configurable clock speeds for different applications
- Interrupt-driven transfer completion notification
- Full-duplex communication (simultaneous send/receive)
- External clock synchronization support

**Transfer Modes:**
- **External Clock Mode**: Uses external device as clock source (slave mode)
- **Internal Clock Mode**: Uses internal clock generator (master mode)
- **High-Speed Mode**: Enhanced transfer rates for TM32-specific applications
- **Normal Speed Mode**: Compatible with original Game Boy timing

**Signal Interface:**
```
TM32 Core ──── SCK (Serial Clock) ────── External Device
          ──── SIN (Serial Data In) ──────
          ──── SOUT (Serial Data Out) ─────
```

#### 4.5.2 Serial Transfer Registers

The Serial Transfer component provides two memory-mapped registers accessible
through the port register space:

| Address | Name | Access | Description                    |
|---------|------|--------|--------------------------------|
| `0x01`  | `SB` | R/W    | Serial Transfer Data Register  |
| `0x02`  | `SC` | R/W    | Serial Transfer Control Register |

**SB Register (0x01) - Serial Transfer Data:**

```
Bit  7   6   5   4   3   2   1   0
    D7  D6  D5  D4  D3  D2  D1  D0
```

The `SB` register serves dual purposes as both the transmit buffer and receive
buffer for serial data transfers.

**Write Operation (Transmit):**
- Writing to `SB` loads the 8-bit value to be transmitted
- Data is shifted out MSB first (bit 7 → bit 0)
- Should be written before initiating a transfer

**Read Operation (Receive):**
- Reading from `SB` returns the last received 8-bit value
- Data is shifted in MSB first (bit 7 ← bit 0)
- Valid after transfer completion interrupt

**SC Register (0x02) - Serial Transfer Control:**

```
Bit  7   6   5   4   3   2   1   0
    TF  -   -   -   -   -   CK  ST
```

The `SC` register controls serial transfer operation and status.

**Bit Fields:**
- **Bit 7 (TF) - Transfer Flag:**
  - 0: No transfer in progress
  - 1: Transfer in progress (read) / Start transfer (write)
  - Automatically cleared when transfer completes
  - Writing 1 starts a new transfer

- **Bit 1 (CK) - Clock Source:**
  - 0: External clock (slave mode, 0-500 KHz)
  - 1: Internal clock (master mode, configurable speed)

- **Bit 0 (ST) - Speed Type:**
  - 0: Normal speed (8 KHz internal, compatible with Game Boy)
  - 1: High speed (32 KHz internal, TM32 enhanced mode)

- **Bits 6-2:** Unused, always read as 0

#### 4.5.3 Transfer Operation

**Serial Transfer Process:**

1. **Prepare Data**: Write the byte to transmit into `SB` register
2. **Configure Mode**: Set clock source and speed in `SC` register
3. **Start Transfer**: Set bit 7 of `SC` register to begin transmission
4. **Monitor Progress**: Transfer flag (bit 7) remains set during operation
5. **Completion**: Transfer flag automatically clears, interrupt generated
6. **Read Result**: Read received data from `SB` register

**Timing Characteristics:**

| Mode | Clock Source | Frequency | Transfer Time | Use Cases |
|------|--------------|-----------|---------------|-----------|
| Normal Internal | Internal | 8 KHz | 1 ms | Game Boy compatibility |
| High-Speed Internal | Internal | 32 KHz | 250 μs | Fast TM32 communication |
| External Slave | External | 0-500 KHz | Variable | Device synchronization |

**Master Mode Operation:**
- TM32 Core generates serial clock (SCK)
- Transfer rate controlled by SC register speed setting
- Can initiate transfers at any time
- Suitable for controlling external devices

**Slave Mode Operation:**
- External device provides serial clock (SCK)
- TM32 Core responds to external clock edges
- Transfer timing controlled by external master
- Suitable for peripheral device communication

#### 4.5.4 Serial Transfer Interrupt (Vector 0x04)

The Serial Transfer component generates an interrupt when a transfer operation
completes. This provides efficient notification without requiring constant
polling of the transfer status.

**Serial Transfer Interrupt Trigger:**
- **Condition**: 8-bit serial transfer completes (8 clock cycles)
- **Timing**: Triggered immediately after final bit transmission/reception
- **Flag Setting**: Automatically sets bit 3 of `IF0` register
- **Transfer Flag**: TF bit (SC bit 7) automatically cleared

**Interrupt Control:**
- **Enable**: Set bit 3 of `IE0` register (Serial Transfer interrupt enable)
- **Flag**: Automatically set in `IF0` register (bit 3) when triggered
- **Clear**: Software must clear bit 3 of `IF0` in interrupt handler
- **Priority**: Medium priority (after Timer, before Joypad)

**Interrupt Use Cases:**
- **Non-blocking Communication**: Continue other processing during transfers
- **Protocol Implementation**: Handle multi-byte communication protocols
- **Peripheral Control**: Manage external device command/response cycles
- **Data Streaming**: Implement continuous data transfer operations

#### 4.5.5 Programming Examples

**Example 1: Basic Serial Transfer (Master Mode)**
```assembly
.const hSB = 0x01  ; Serial Data Register
.const hSC = 0x02  ; Serial Control Register

.text
serial_send_byte:
    ; Send a byte via serial transfer (master mode)
    ; Input: AL = byte to send
    
    ; Wait for any previous transfer to complete
wait_transfer_complete:
    ldp bl, [hSC]           ; Read SC register
    and bl, 0x80            ; Check transfer flag (bit 7)
    jpb zc, wait_transfer_complete ; Wait if transfer in progress
    
    ; Load data to transmit
    stp [hSB], al           ; Store byte to SB register
    
    ; Start transfer (internal clock, high speed)
    ld bl, 0x83             ; TF=1, CK=1, ST=1 (start, internal, high speed)
    stp [hSC], bl           ; Store to SC register
    
    ; Transfer will complete asynchronously
    ; Use interrupt or poll TF bit for completion
    ret nc
```

**Example 2: Serial Transfer with Interrupt**
```assembly
.const hIE0 = 0xFF  ; Interrupt Enable Register Byte 0
.const hIF0 = 0xFB  ; Interrupt Flag Register Byte 0

.text
enable_serial_interrupt:
    ; Enable serial transfer interrupt
    ldp al, [hIE0]          ; Read interrupt enable
    or al, 0x08             ; Set bit 3 (Serial Transfer enable)
    stp [hIE0], al          ; Write back to IE0
    ret nc

.int 3
serial_transfer_isr:
    ; Serial transfer interrupt service routine
    push a
    push b
    
    ; Clear serial transfer interrupt flag
    ldp al, [hIF0]          ; Read interrupt flags
    and al, 0xF7            ; Clear bit 3 (Serial Transfer flag)
    stp [hIF0], al          ; Write back to IF0
    
    ; Read received data
    ldp al, [hSB]           ; Read received byte from SB
    stp [received_data], al ; Store in memory variable
    
    ; Signal transfer completion
    ld al, 1
    stp [transfer_complete_flag], al
    
    pop b
    pop a
    reti

; Data storage
.data
received_data:
    .db 0
transfer_complete_flag:
    .db 0
```

**Example 3: Bidirectional Communication Protocol**
```assembly
.text
serial_exchange_data:
    ; Implement a simple command/response protocol
    ; Input: AL = command to send
    ; Output: AL = response received
    
    ; Send command byte
    call nc, serial_send_byte
    
    ; Wait for transfer completion
wait_for_response:
    ldp bl, [transfer_complete_flag]
    cmp bl, 0
    jpb zs, wait_for_response
    
    ; Clear completion flag
    ld bl, 0
    stp [transfer_complete_flag], bl
    
    ; Return received response
    ldp al, [received_data]
    ret nc

multi_byte_transfer:
    ; Transfer multiple bytes with protocol
    ; BX = pointer to data buffer
    ; CX = number of bytes to transfer
    
    ld d, 0                 ; Byte counter
    
transfer_loop:
    ; Send next byte
    ldp al, [bx + d]        ; Load byte from buffer
    call nc, serial_send_byte
    
    ; Wait for completion and response
    call nc, wait_for_response
    
    ; Store received byte
    ldp al, [received_data]
    stp [bx + d], al        ; Store back to buffer
    
    ; Next byte
    inc d
    cmp d, cx
    jpb zc, transfer_loop   ; Continue if more bytes
    
    ret nc
```

**Example 4: Slave Mode Operation**
```assembly
.text
setup_serial_slave:
    ; Configure serial port for slave mode operation
    ; Responds to external clock signals
    
    ; Set slave mode (external clock, normal speed)
    ld al, 0x80             ; TF=1, CK=0, ST=0 (ready, external, normal)
    stp [hSC], al           ; Store to SC register
    
    ; Prepare default response data
    ld al, 0xFF             ; Default response byte
    stp [hSB], al           ; Store to SB register
    
    ; Enable interrupt for slave responses
    call nc, enable_serial_interrupt
    ret nc

slave_communication_handler:
    ; Handle incoming communication as slave device
    ; This would be called from the serial interrupt handler
    
    ; Read received command
    ldp al, [hSB]           ; Get received byte
    
    ; Process command and prepare response
    cmp al, 0x01            ; Check for status request
    jpb zs, send_status
    cmp al, 0x02            ; Check for data request
    jpb zs, send_data
    
    ; Unknown command - send error response
    ld al, 0xFF
    jpb nc, prepare_response
    
send_status:
    ; Send status information
    ld al, 0xA5             ; Status byte
    jpb nc, prepare_response
    
send_data:
    ; Send data byte
    ldp al, [data_buffer]   ; Load data to send
    
prepare_response:
    ; Prepare response for next transfer
    stp [hSB], al           ; Load response into SB
    
    ; Re-enable slave mode for next transfer
    ld bl, 0x80             ; TF=1, CK=0, ST=0
    stp [hSC], bl           ; Ready for next transfer
    ret nc

.data
data_buffer:
    .db 0x42                ; Example data byte
```

#### 4.5.6 Advanced Features

**Protocol Implementation:**
- Support for custom communication protocols
- Multi-byte message handling with checksums
- Command/response pattern implementation
- Error detection and retry mechanisms

**Device Integration:**
- External peripheral control (sensors, displays, etc.)
- Multi-player game link cable emulation
- Debugging interface for development tools
- Data logging and telemetry transmission

**Performance Optimization:**
- High-speed mode for reduced transfer latency
- Interrupt-driven operation for efficient CPU usage
- Buffer management for continuous data streams
- Clock synchronization for reliable communication

**Compatibility Considerations:**
- Game Boy link cable protocol compatibility
- Standard serial communication interfaces
- Custom TM32-specific enhancements
- Backward compatibility with existing software

### 4.6 Error Handling and Hardware Diagnostics

The TM32 Core implements comprehensive error detection, reporting, and recovery
mechanisms to ensure system stability and provide debugging capabilities for 
developers. Understanding these error conditions is essential for robust 
program development.

#### 4.6.1 Error Categories

The TM32 Core categorizes errors into several types based on their source and
severity:

**System Errors (Critical):**
- Boot failure conditions
- Memory corruption detection
- Hardware component failures
- CPU exception conditions

**Program Errors (Recoverable):**
- Invalid memory access attempts
- Arithmetic overflow conditions
- Invalid instruction execution
- Stack overflow/underflow

**Hardware Errors (Component-Specific):**
- Timer overflow conditions
- Serial communication failures
- PPU rendering errors
- APU audio processing errors

#### 4.6.2 Error Code System

The TM32 Core uses the CPU's Error Code register (`EC`) to report error 
conditions. Error codes are organized by component and severity:

**Boot Error Codes (0x01-0x0F):**
- `0x01`: Invalid program header
- `0x02`: Insufficient memory for program
- `0x03`: Corrupted program data
- `0x04`: System version incompatibility
- `0x05`: Hardware initialization failure
- `0x06`: Memory map validation failure
- `0x07`: Interrupt vector setup failure
- `0x08`: Critical component failure

**Runtime Error Codes (0x10-0x2F):**
- `0x10`: Memory access violation
- `0x11`: Stack overflow
- `0x12`: Stack underflow
- `0x13`: Invalid instruction
- `0x14`: Arithmetic overflow
- `0x15`: Division by zero
- `0x16`: Invalid register access
- `0x17`: Interrupt handler failure

**Hardware Error Codes (0x30-0x4F):**
- `0x30`: Timer component error
- `0x31`: RTC component error
- `0x32`: Serial component error
- `0x33`: Joypad component error
- `0x34`: PPU component error
- `0x35`: APU component error
- `0x36`: Memory controller error
- `0x37`: DMA transfer error

#### 4.6.3 Error Detection Mechanisms

**Memory Protection:**
- Read/write permission validation
- Stack boundary checking
- Memory region access control
- Buffer overflow detection

**Hardware Monitoring:**
- Component status verification
- Register access validation
- Timing constraint enforcement
- Resource allocation tracking

**Program Validation:**
- Instruction execution verification
- Address boundary checking
- Stack pointer validation
- Interrupt handler integrity

#### 4.6.4 Error Recovery Strategies

**Graceful Degradation:**
- Disable failing hardware components
- Switch to compatible mode operation
- Reduce system performance to maintain stability
- Maintain core functionality when possible

**Automatic Recovery:**
- Component reset and reinitialization
- Memory region remapping
- Alternative hardware path selection
- Error state clearing and retry

**User Intervention:**
- Error notification to application
- Safe mode operation
- Manual recovery procedures
- System reset requirements

#### 4.6.5 Diagnostic Interfaces

**Error Status Registers:**
- Component error flags
- Error severity indicators
- Recovery attempt counters
- System health status

**Debug Information:**
- Error occurrence timestamps
- Component state snapshots
- Memory access logs
- Performance metrics

**Diagnostic Commands:**
- Hardware self-test procedures
- Component validation routines
- Memory integrity checks
- Performance benchmarking

#### 4.6.6 Error Handling Programming

**Exception Handler Setup:**
```assembly
.const hEC = 0xFF      ; Error Code register (CPU)
.const hIF0 = 0xFB     ; Interrupt Flag Register

.int 0
exception_handler:
    ; Save all registers
    push a
    push b
    push c
    push d
    
    ; Read error code
    ldp al, [hEC]
    
    ; Dispatch to appropriate handler
    cmp al, 0x0F
    jpb nc, boot_error_handler
    cmp al, 0x2F
    jpb nc, runtime_error_handler
    jpb nc, hardware_error_handler

boot_error_handler:
    ; Handle boot errors (usually fatal)
    call nc, display_boot_error
    halt                    ; Stop system execution
    
runtime_error_handler:
    ; Handle runtime errors (attempt recovery)
    call nc, log_runtime_error
    call nc, attempt_recovery
    reti                    ; Return if recovery successful
    
hardware_error_handler:
    ; Handle hardware errors (component-specific)
    call nc, diagnose_hardware_error
    call nc, implement_workaround
    reti
```

**Error Logging System:**
```assembly
.text
log_error:
    ; Log error with timestamp and context
    ; Input: AL = error code, BL = component ID
    
    ; Store error information
    stp [error_log_code], al
    stp [error_log_component], bl
    
    ; Get current time for timestamp
    call nc, read_rtc_time
    stp [error_log_time], a
    
    ; Increment error counter
    ldp al, [error_count]
    inc al
    stp [error_count], al
    
    ret nc
```

**Recovery Procedure Example:**
```assembly
.text
recover_from_timer_error:
    ; Attempt to recover from timer component error
    
    ; Reset timer component
    ld al, 0
    stp [hTAC], al          ; Disable timer
    stp [hTIMA], al         ; Clear counter
    stp [hTMA], al          ; Clear modulo
    stp [hDIV], al          ; Reset divider
    
    ; Test timer functionality
    ld al, 0x04             ; Enable timer, 4096 Hz
    stp [hTAC], al
    
    ; Wait and check if timer is counting
    call nc, delay_short
    ldp bl, [hTIMA]
    cmp bl, 0
    jpb zs, timer_recovery_failed
    
    ; Recovery successful
    ld al, 0                ; Clear error code
    stp [hEC], al
    ret nc
    
timer_recovery_failed:
    ; Recovery failed, mark component as disabled
    ld al, 0x30             ; Timer error code
    stp [hEC], al
    call nc, disable_timer_component
    ret nc
```

#### 4.6.7 Best Practices for Error Handling

**Defensive Programming:**
- Always check return values and error codes
- Validate input parameters before use
- Implement timeouts for hardware operations
- Use bounds checking for array access

**Error Prevention:**
- Initialize all variables before use
- Clear hardware registers during setup
- Implement proper interrupt handling
- Use memory barriers for critical sections

**Graceful Failure:**
- Provide meaningful error messages
- Maintain system stability during errors
- Implement fallback functionality
- Allow for partial operation when possible

**Testing and Validation:**
- Test error conditions during development
- Implement comprehensive error scenarios
- Validate recovery procedures
- Monitor system health in production

### 4.7 Joypad Component

The TM32 Core includes a Joypad component that handles input from external
controllers and input devices. The Joypad system is based on the Nintendo Game
Boy's controller interface but enhanced for the TM32's 32-bit architecture and
expanded input capabilities. It provides support for standard directional pad
input, action buttons, and interrupt-driven input handling for responsive
gameplay.

The Joypad component uses a multiplexed input system where button states are
read through a single register by selecting different button groups. This
design minimizes the number of required I/O pins while providing access to
all controller inputs.

#### 4.7.1 Joypad Overview

**Input Capabilities:**
- 8-button standard controller support (D-pad + 4 action buttons)
- Interrupt-driven input detection for low-latency response
- Debounced input handling to prevent false triggers
- Support for multiple controller types through pin configuration
- Pull-up resistor configuration for reliable signal reading

**Button Layout:**
- **D-Pad**: Up, Down, Left, Right directional inputs
- **Action Buttons**: A, B, Select, Start function buttons
- **System Integration**: Direct connection to interrupt system
- **Multiplexed Reading**: Sequential access to button groups

**Input Signal Path:**
```
Controller ──── Joypad Pins ──── Multiplexer ──── P1 Register ──── CPU
                                      │
                               Interrupt Generator
```

#### 4.7.2 Joypad Registers

The Joypad component provides one primary memory-mapped register accessible
through the port register space:

| Address | Name            | Access | Description                     |
|---------|-----------------|--------|---------------------------------|
| `0x00`  | `P1` or `JOYP`  | R/W    | Joypad Input/Output Register    |

**P1 Register (0x00) - Joypad Input/Output:**

| Bit | Name | Access | Description                               |
|-----|------|--------|-------------------------------------------|
| 7   | -    | -      | Unused (always reads 1)                  |
| 6   | -    | -      | Unused (always reads 1)                  |
| 5   | P15  | R/W    | Button Group Select (0=Select, 1=Ignore) |
| 4   | P14  | R/W    | D-Pad Group Select (0=Select, 1=Ignore)  |
| 3   | P13  | R      | Input Bit 3 (inverted: 0=pressed)        |
| 2   | P12  | R      | Input Bit 2 (inverted: 0=pressed)        |
| 1   | P11  | R      | Input Bit 1 (inverted: 0=pressed)        |
| 0   | P10  | R      | Input Bit 0 (inverted: 0=pressed)        |

**Button Group Mapping:**

When P14=0 (D-Pad Group Selected):
- Bit 3: Down button (0=pressed, 1=released)
- Bit 2: Up button (0=pressed, 1=released)
- Bit 1: Left button (0=pressed, 1=released)
- Bit 0: Right button (0=pressed, 1=released)

When P15=0 (Button Group Selected):
- Bit 3: Start button (0=pressed, 1=released)
- Bit 2: Select button (0=pressed, 1=released)
- Bit 1: B button (0=pressed, 1=released)
- Bit 0: A button (0=pressed, 1=released)

#### 4.7.3 Joypad Input Reading

**Reading Button States:**

To read controller input, software must select the appropriate button group
and then read the input bits. The process involves:

1. Select button group by clearing P14 (D-pad) or P15 (buttons)
2. Read the lower 4 bits to get button states
3. Repeat for the other button group if needed
4. Combine results to get complete controller state

**Input Reading Sequence:**
1. Write to P1 to select D-pad group (clear bit 4, set bit 5)
2. Read P1 to get D-pad button states (bits 0-3)
3. Write to P1 to select button group (clear bit 5, set bit 4)
4. Read P1 to get action button states (bits 0-3)
5. Process inverted button values (0=pressed, 1=released)

#### 4.7.4 Joypad Interrupt (Vector 0x05)

The Joypad component can generate interrupts when button state changes are
detected. This provides low-latency input handling without requiring constant
polling of the joypad register.

**Joypad Interrupt Trigger:**
- **Condition**: Any button transitions from released (1) to pressed (0)
- **Timing**: Triggered immediately upon button press detection
- **Debouncing**: Hardware debouncing prevents multiple triggers per press
- **Edge Detection**: Only button press events trigger interrupts (not releases)

**Interrupt Control:**
- **Enable**: Set bit 4 of `IE0` register (Joypad interrupt enable)
- **Flag**: Automatically set in `IF0` register (bit 4) when triggered
- **Clear**: Software must clear bit 4 of `IF0` in interrupt handler
- **Priority**: Lower priority than Timer, PPU interrupts

**Interrupt Use Cases:**
- **Menu Navigation**: Immediate response to button presses in menus
- **Game Pause**: Instant pause functionality on Start button press
- **Input Buffering**: Capture rapid button sequences for combos
- **Power Management**: Wake system from low-power state on input

#### 4.7.5 Programming Examples

**Example 1: Basic Joypad Reading**
```assembly
.const hP1 = 0x00  ; Joypad Register

.text
read_joypad_state:
    ; Read D-pad buttons
    ld al, 0x20             ; Select D-pad group (P14=0, P15=1)
    stp [hP1], al           ; Store to P1
    ldp al, [hP1]           ; Read D-pad state
    and al, 0x0F            ; Mask input bits
    ld bl, al               ; Store D-pad state in BL
    
    ; Read action buttons
    ld al, 0x10             ; Select button group (P14=1, P15=0)
    stp [hP1], al           ; Store to P1
    ldp al, [hP1]           ; Read button state
    and al, 0x0F            ; Mask input bits
    ld bh, al               ; Store button state in BH
    
    ; Reset P1 (deselect all groups)
    ld al, 0x30             ; Deselect both groups (P14=1, P15=1)
    stp [hP1], al           ; Store to P1
    
    ; Return with BL=D-pad, BH=buttons
    ; Note: 0=pressed, 1=released for all buttons
    ret nc
```

**Example 2: Joypad Interrupt Setup**
```assembly
.const hIE0 = 0xFF  ; Interrupt Enable Register Byte 0
.const hIF0 = 0xFB  ; Interrupt Flag Register Byte 0

.text
enable_joypad_interrupt:
    ; Enable joypad interrupt
    ldp al, [hIE0]          ; Read interrupt enable
    or al, 0x10             ; Set bit 4 (Joypad enable)
    stp [hIE0], al          ; Write back to IE0
    ret nc

.int 5
joypad_isr:
    ; Joypad interrupt service routine
    push a
    push b
    
    ; Clear joypad interrupt flag
    ldp al, [hIF0]          ; Read interrupt flags
    and al, 0xEF            ; Clear bit 4 (Joypad flag)
    stp [hIF0], al          ; Write back to IF0
    
    ; Read current joypad state
    call nc, read_joypad_state
    
    ; Process input (example: check for Start button)
    ld al, bh               ; Get button state
    and al, 0x08            ; Check Start button (bit 3)
    jpb zs, start_pressed   ; Jump if Start pressed (bit=0)
    jpb nc, joypad_isr_end
    
start_pressed:
    ; Handle Start button press
    call nc, toggle_pause_state
    
joypad_isr_end:
    pop b
    pop a
    reti
```

**Example 3: Button State Processing**
```assembly
.text
process_joypad_input:
    ; Read joypad state
    call nc, read_joypad_state
    ; BL = D-pad state, BH = button state
    
    ; Check individual D-pad buttons
    ld al, bl               ; Get D-pad state
    and al, 0x01            ; Check Right button
    jpb zs, right_pressed   ; Jump if pressed (0)
    
    ld al, bl               ; Get D-pad state
    and al, 0x02            ; Check Left button
    jpb zs, left_pressed    ; Jump if pressed (0)
    
    ld al, bl               ; Get D-pad state
    and al, 0x04            ; Check Up button
    jpb zs, up_pressed      ; Jump if pressed (0)
    
    ld al, bl               ; Get D-pad state
    and al, 0x08            ; Check Down button
    jpb zs, down_pressed    ; Jump if pressed (0)
    
    ; Check action buttons
    ld al, bh               ; Get button state
    and al, 0x01            ; Check A button
    jpb zs, a_pressed       ; Jump if pressed (0)
    
    ld al, bh               ; Get button state
    and al, 0x02            ; Check B button
    jpb zs, b_pressed       ; Jump if pressed (0)
    
    ret nc

right_pressed:
    ; Handle right movement
    call nc, move_player_right
    ret nc

left_pressed:
    ; Handle left movement
    call nc, move_player_left
    ret nc

up_pressed:
    ; Handle up movement
    call nc, move_player_up
    ret nc

down_pressed:
    ; Handle down movement
    call nc, move_player_down
    ret nc

a_pressed:
    ; Handle A button action
    call nc, player_jump
    ret nc

b_pressed:
    ; Handle B button action
    call nc, player_attack
    ret nc
```

#### 4.7.6 Advanced Features

**Input Debouncing:**
- Hardware debouncing prevents multiple interrupt triggers
- Typical debounce time: 1-2 milliseconds
- Software can implement additional debouncing if needed
- Edge detection ensures clean button press recognition

**Multiple Controller Support:**
- P1 register can be extended for additional controllers
- Software multiplexing enables multi-player input
- Pin configuration allows different controller types
- Interrupt sharing possible with software discrimination

**Power Management:**
- Joypad interrupt can wake CPU from halt state
- Low-power polling mode for battery conservation
- Selective button monitoring for specific wake conditions
- Integration with system power management

**Performance Considerations:**
- Interrupt-driven input provides ~1ms response time
- Polling mode suitable for less time-critical applications
- Button group selection requires 2-3 microsecond settling time
- Input buffering recommended for rapid button sequences

### 4.7 PPU Component

The TM32 Core includes a Pixel Processing Unit (PPU) that handles all graphics
rendering functionality. The PPU is based on the Nintendo Game Boy's display
system but enhanced for the TM32's 32-bit architecture and expanded capabilities.
It provides a 160×144 pixel display with support for backgrounds, windows,
sprites (objects), and various rendering effects.

The PPU operates in a scanline-based rendering mode, drawing the display
progressively from top to bottom, left to right. This allows for mid-frame
modifications to create raster effects and dynamic visual elements.

#### 4.7.1 PPU Overview

**Display Specifications:**
- Resolution: 160×144 pixels (visible area)
- Total frame: 154 scanlines (144 visible + 10 VBlank)
- Refresh rate: ~59.7 Hz (70,224 dots per frame)
- Color depth: 4 shades per pixel (2 bits)
- Background support: 32×32 tile maps (256×256 pixels)
- Sprite support: Up to 40 sprites, 10 per scanline
- Window support: Overlay window with independent scrolling

**Memory Usage:**
- Video RAM (VRAM): 8 KB at `0xC0008000`-`0xC0009FFF`
- Object Attribute Memory (OAM): 160 bytes at `0xC000FE00`-`0xC000FE9F`
- Tile data: 384 tiles (8×8 pixels each)
- Tile maps: Two 32×32 tile maps for backgrounds and windows

#### 4.7.2 PPU Registers

The PPU provides several memory-mapped registers accessible through the port
register space:

| Address | Name        | Access | Description                                         |
|---------|-------------|--------|-----------------------------------------------------|
| `0x40`  | `LCDC`      | R/W    | LCD Display Control                                 |
| `0x41`  | `STAT`      | R/W    | LCD Display Status                                  |
| `0x42`  | `SCY`       | R/W    | Background Viewport Y-Scroll                        |
| `0x43`  | `SCX`       | R/W    | Background Viewport X-Scroll                        |
| `0x44`  | `LY`        | R      | Current Scanline                                    |
| `0x45`  | `LYC`       | R/W    | Scanline Compare                                    |
| `0x46`  | `DMA`       | R/W    | Alias of `ODMA` register (port address `0xA3`)      |
| `0x47`  | `BGP`       | R/W    | Background Palette Data                             |
| `0x48`  | `OBP0`      | R/W    | Object Palette 0 Data                               |
| `0x49`  | `OBP1`      | R/W    | Object Palette 1 Data                               |
| `0x4A`  | `WY`        | R/W    | Window Y-Position                                   |
| `0x4B`  | `WX`        | R/W    | Window X-Position                                   |
| `0xA0`  | `ODMAS3`    | W      | (TM32) OAM DMA Source Address Byte 3                |
| `0xA1`  | `ODMAS2`    | W      | (TM32) OAM DMA Source Address Byte 2                |
| `0xA2`  | `ODMAS1`    | W      | (TM32) OAM DMA Source Address Byte 1                |
| `0xA3`  | `ODMA`      | R/W    | (TM32) OAM DMA Start                                |
| `0xA4`  | `HDMAS3`    | W      | (TM32) HDMA Source Address Byte 3                   |
| `0xA5`  | `HDMAS2`    | W      | (TM32) HDMA Source Address Byte 2                   |
| `0xA6`  | `HDMAS1`    | W      | (TM32) HDMA Source Address Byte 1                   |
| `0xA7`  | `HDMAS0`    | W      | (TM32) HDMA Source Address Byte 0                   |
| `0xA8`  | `HDMAD3`    | W      | (TM32) HDMA Destination Address Byte 3              |
| `0xA9`  | `HDMAD2`    | W      | (TM32) HDMA Destination Address Byte 2              |
| `0xAA`  | `HDMAD1`    | W      | (TM32) HDMA Destination Address Byte 1              |
| `0xAB`  | `HDMAD0`    | W      | (TM32) HDMA Destination Address Byte 0              |
| `0xAC`  | `HDMA`      | R/W    | (TM32) HDMA Start / Control                         |
| `0xAD`  | `GRPM`      | R/W    | (TM32) Graphics Mode Control                        |

##### LCDC - LCD Display Control (0x40)

```
Bit  7   6   5   4   3   2   1   0
    LCD WIN WIN BG  BG  OBJ OBJ BG
    EN  MAP EN  TD  MAP SZ  EN  EN
```

The `LCDC` register controls the main display and PPU functionality.

**Bit Fields:**
- **Bit 7 (LCD EN) - LCD Enable:**
  - 0: LCD and PPU disabled, full VRAM/OAM access
  - 1: LCD and PPU enabled, normal operation
  
- **Bit 6 (WIN MAP) - Window Tile Map Area:**
  - 0: Use tile map at `0x9800`-`0x9BFF`
  - 1: Use tile map at `0x9C00`-`0x9FFF`
  
- **Bit 5 (WIN EN) - Window Enable:**
  - 0: Window disabled
  - 1: Window enabled
  
- **Bit 4 (BG TD) - Background & Window Tile Data Area:**
  - 0: Use tile data at `0x8800`-`0x97FF` (signed addressing)
  - 1: Use tile data at `0x8000`-`0x8FFF` (unsigned addressing)
  
- **Bit 3 (BG MAP) - Background Tile Map Area:**
  - 0: Use tile map at `0x9800`-`0x9BFF`
  - 1: Use tile map at `0x9C00`-`0x9FFF`
  
- **Bit 2 (OBJ SZ) - Object Size:**
  - 0: 8×8 pixel sprites
  - 1: 8×16 pixel sprites
  
- **Bit 1 (OBJ EN) - Object Enable:**
  - 0: Objects (sprites) disabled
  - 1: Objects (sprites) enabled
  
- **Bit 0 (BG EN) - Background & Window Enable/Priority:**
  - 0: Background and window disabled/lose priority
  - 1: Background and window enabled/normal priority

##### STAT - LCD Display Status (0x41)

```
Bit  7   6   5   4   3   2   1   0
    -   LYC LY  M2  M1  M0  LYC MODE
        IE  IE  IE  IE  IE  F   1-0
```

The `STAT` register provides LCD status information and interrupt control.

**Bit Fields:**
- **Bit 6 (LYC IE) - LYC Interrupt Enable:**
  - 0: Disable LYC=LY interrupt
  - 1: Enable LYC=LY interrupt
  
- **Bit 5 (M2 IE) - Mode 2 Interrupt Enable:**
  - 0: Disable OAM scan interrupt
  - 1: Enable OAM scan interrupt
  
- **Bit 4 (M1 IE) - Mode 1 Interrupt Enable:**
  - 0: Disable VBlank interrupt
  - 1: Enable VBlank interrupt
  
- **Bit 3 (M0 IE) - Mode 0 Interrupt Enable:**
  - 0: Disable HBlank interrupt
  - 1: Enable HBlank interrupt
  
- **Bit 2 (LYC F) - LYC Flag:**
  - 0: LY ≠ LYC
  - 1: LY = LYC
  
- **Bits 1-0 (MODE) - PPU Mode:**
  - 00: HBlank (Mode 0)
  - 01: VBlank (Mode 1)
  - 10: OAM Scan (Mode 2)
  - 11: Pixel Transfer (Mode 3)

##### GRPM - Graphics Mode Control (0xAD)

```
Bit  7   6   5   4   3   2   1   0
    -   -   -   -   -   -   -   CGB
```

The `GRPM` register controls the graphics compatibility mode of the PPU,
allowing selection between original Game Boy (DMG) and Game Boy Color (CGB)
rendering modes.

**Bit Fields:**
- **Bit 0 (CGB) - Color Game Boy Mode:**
  - 0: DMG Game Boy mode (monochrome graphics compatibility)
  - 1: CGB Game Boy Color mode (enhanced color graphics)

- **Bits 7-1**: Unused, always read as 0

**Mode Behavior:**
- **DMG Mode (GRPM = 0):**
  - PPU operates in original Game Boy compatibility mode
  - Uses monochrome 4-shade grayscale palette system
  - Background and sprite palettes function as in original Game Boy
  - Sprite priority and attributes follow classic rules (`OPRI` is ignored).
  - Window and background rendering use classic DMG behavior
  - Color-specific features are disabled

- **CGB Mode (GRPM ≠ 0):**
  - PPU operates in Game Boy Color enhanced mode
  - Enables extended color palette functionality
  - Enhanced sprite priority and attribute handling
  - Advanced background and window features available
  - Color mixing and transparency effects enabled

**VRAM Banking:**
The GRPM register setting does not affect VRAM banking operations. VRAM banks
0 and 1 remain accessible regardless of the graphics mode setting. This allows
programs to utilize the both full 8 KB banks of VRAM for tile data and maps in
either mode.

**Programming Considerations:**
- Set GRPM during PPU initialization before enabling the LCD
- Changing GRPM during active display may cause visual artifacts
- If software needs to change GRPM, it should disable the LCD first and, as such,
    should wait until VBlank to avoid display corruption.
- Software should verify CGB mode support before enabling advanced features
- DMG mode ensures maximum compatibility with original Game Boy software

#### 4.7.3 PPU Modes and Timing

The PPU operates in four distinct modes during each frame:

| Mode | Name           | Duration     | Description                     |
|------|----------------|-------------|---------------------------------|
| 0    | HBlank         | 87-204 dots  | Horizontal blank period         |
| 1    | VBlank         | 4560 dots    | Vertical blank period (10 lines)|
| 2    | OAM Scan       | 80 dots      | Searching for sprites on line  |
| 3    | Pixel Transfer | 172-289 dots | Rendering pixels to display     |

**Frame Structure:**
- Total frame time: 70,224 dots (~16.74 ms)
- Scanlines 0-143: Visible display area
- Scanlines 144-153: VBlank period
- Each visible scanline: 456 dots total

**Mode Transitions:**
1. **Mode 2 (OAM Scan)**: Searches OAM for sprites on current scanline
2. **Mode 3 (Pixel Transfer)**: Renders pixels using pixel FIFO
3. **Mode 0 (HBlank)**: Waits until end of scanline
4. **Mode 1 (VBlank)**: Waits until next frame (after line 143)

#### 4.7.4 Pixel Pipeline and FIFO

The PPU uses a sophisticated pixel pipeline based on First In, First Out (FIFO)
queues to render graphics efficiently. This system allows for precise timing
and complex rendering effects.

##### Pixel FIFO Architecture

The pixel pipeline consists of two independent FIFO queues:

**Background FIFO:**
- Holds background and window pixels
- Maximum capacity: 16 pixels
- Minimum operational level: 8 pixels
- Feeds into pixel mixer

**Object FIFO:**
- Holds sprite (object) pixels
- Maximum capacity: 16 pixels
- Operates independently from background FIFO
- Takes priority over background pixels (when not transparent)

**Pixel Properties:**
Each pixel in the FIFO contains four properties:
- **Color**: Value 0-3 (2-bit color index)
- **Palette**: Palette selection for color lookup
- **Sprite Priority**: Object priority information
- **Background Priority**: Background-to-object priority control

##### Pixel Fetcher

The pixel fetcher operates in a 5-step cycle to fill the background FIFO:

**1. Get Tile (2 dots)**
- Determines which background/window tile to fetch
- Calculates tile coordinates based on scroll values
- Handles window vs. background tile selection

**2. Get Tile Data Low (2 dots)**
- Fetches lower byte of tile data from VRAM
- Applies vertical flipping if needed
- Handles VRAM access blocking

**3. Get Tile Data High (2 dots)**
- Fetches upper byte of tile data from VRAM
- Completes tile data retrieval
- Triggers first push attempt

**4. Sleep (2 dots)**
- Idle period for timing alignment
- No operations performed

**5. Push (variable timing)**
- Attempts to push 8 pixels to background FIFO
- Only succeeds if FIFO is empty
- Applies horizontal flipping if needed
- Repeats until successful

##### Sprite Handling

Sprites are processed during pixel transfer with the following steps:

**1. Sprite Detection:**
- Scan current scanline for sprites
- Maximum 10 sprites per scanline
- Priority determined by X-position and OAM index

**2. Sprite Fetching:**
- Fetch sprite tile data from VRAM
- Handle sprite size (8×8 or 8×16)
- Apply horizontal/vertical flipping

**3. Sprite Mixing:**
- Fill object FIFO with transparent pixels if needed
- Mix sprite pixels with existing object FIFO contents
- Respect sprite priority rules

**4. Timing Penalties:**
- Each sprite adds 6-11 dots of penalty to Mode 3
- Penalties depend on sprite position and background scroll
- Special handling for sprites at X-position 0

##### Pixel Rendering

The final pixel rendering combines both FIFOs:

**1. Pixel Selection:**
- Pop one pixel from each FIFO
- Object pixels take priority unless transparent (color 0)
- Background priority flags control final selection

**2. Color Lookup:**
- Apply palette lookup based on pixel properties
- Handle different palette systems for background vs. sprites
- Support for 4-shade grayscale output

**3. Display Output:**
- Push final pixel to LCD
- Advance to next pixel position
- Handle end-of-line and end-of-frame conditions

#### 4.7.5 VRAM and Memory Access

**VRAM Layout (8 KB at 0xC0008000-0xC0009FFF):**

| Address Range      | Size    | Contents                |
|--------------------|---------|-------------------------|
| `0xC0008000-87FF`  | 2 KB    | Tile Data Block 2       |
| `0xC0008800-8FFF`  | 2 KB    | Tile Data Block 1       |
| `0xC0009000-97FF`  | 2 KB    | Tile Data Block 0       |
| `0xC0009800-9BFF`  | 1 KB    | Background Tile Map 0   |
| `0xC0009C00-9FFF`  | 1 KB    | Background Tile Map 1   |

**OAM Layout (160 bytes at 0xC000FE00-0xC000FE9F):**
- 40 sprite entries, 4 bytes each
- Byte 0: Y-position (offset by 16)
- Byte 1: X-position (offset by 8)
- Byte 2: Tile index
- Byte 3: Attribute flags (priority, palette, flip)

**Memory Access Restrictions:**
- VRAM inaccessible during Mode 3 (Pixel Transfer)
- OAM inaccessible during Mode 2 (OAM Scan) and Mode 3
- Palette registers may be blocked during certain operations
- CPU reads return `0xFF` when memory is inaccessible

#### 4.7.6 Programming Examples

**Assumption: Port Register Constants**
```assembly
.const hLCDC = 0x40  ; LCDC Register
.const hSTAT = 0x41  ; STAT Register
.const hSCY  = 0x42  ; SCY Register
.const hSCX  = 0x43  ; SCX Register
.const hLY   = 0x44  ; LY Register
.const hLYC  = 0x45  ; LYC Register
.const hDMA  = 0x46  ; DMA Register
.const hBGP  = 0x47  ; BGP Register
.const hOBP0 = 0x48  ; OBP0 Register
.const hOBP1 = 0x49  ; OBP1 Register
.const hWY   = 0x4A  ; WY Register
.const hWX   = 0x4B  ; WX Register
.const hGRPM = 0xAD  ; GRPM Register
```

**Example 1: Basic Display Setup**
```assembly
.text
display_init:
    ; Set graphics mode (0 = DMG, 1 = CGB)
    ld al, 1                ; Enable CGB mode
    stp [hGRPM], al         ; Store to GRPM

    ; Enable LCD with background and sprites
    ld al, 0x91             ; LCD on, BG on, sprites on
    stp [hLCDC], al         ; Store to LCDC

    ; Set background scroll position
    ld al, 0                ; Reset scroll position
    stp [hSCY], al          ; Store to SCY
    stp [hSCX], al          ; Store to SCX
    ret nc
```

**Example 2: VBlank Wait Loop**
```assembly
.text
vblank_wait:
    ldp al, [hSTAT]         ; Read STAT register
    and al, 3               ; Mask mode bits
    cmp al, 1               ; Check for VBlank (mode 1)
    jpb zc, vblank_wait     ; Loop if not in VBlank

    ; VBlank period - safe to update VRAM/OAM
    ret nc
```

**Example 3: Scanline Interrupt Setup**
```assembly
.text
lyc_interrupt_setup:
    ; Setup LYC interrupt for scanline 100
    ld al, 100              ; Target scanline
    stp [hLYC], al          ; Store to LYC

    ; Enable LYC interrupt
    ldp al, [hSTAT]         ; Read STAT
    or al, 64               ; Set LYC interrupt enable
    stp [hSTAT], al         ; Write back to STAT
```

**Example 4: Graphics Mode Control**
```assembly
.text
set_dmg_mode:
    ; Configure PPU for DMG Game Boy compatibility
    ld al, 0                ; DMG mode
    stp [hGRPM], al         ; Store to GRPM
    
    ; Set monochrome palettes
    ld al, 0xE4             ; Classic Game Boy palette (11 10 01 00)
    stp [hBGP], al          ; Store to BGP
    stp [hOBP0], al         ; Store to OBP0
    stp [hOBP1], al         ; Store to OBP1
    ret nc

set_cgb_mode:
    ; Configure PPU for CGB Game Boy Color mode
    ld al, 1                ; CGB mode
    stp [hGRPM], al         ; Store to GRPM
    
    ; CGB mode enables extended color features
    ; (Color palette setup would go here)
    ret nc

detect_and_set_mode:
    ; Example: Auto-detect and set appropriate graphics mode
    ; This could check for color data or user preference
    
    ; For this example, check if advanced features are needed
    call nc, check_color_graphics_needed
    cmp al, 0
    jpb z, use_dmg_mode
    
use_cgb_mode:
    call nc, set_cgb_mode
    jpb nc, done_mode_setup
    
use_dmg_mode:
    call nc, set_dmg_mode

done_mode_setup:
    ret nc

check_color_graphics_needed:
    ; Placeholder function - would check game requirements
    ; Returns: AL = 0 for DMG, AL = 1 for CGB
    ld al, 1                ; Default to CGB for this example
    ret nc
```

#### 4.7.7 DMA and HDMA Transfers

The TM32 Core provides enhanced Direct Memory Access (DMA) capabilities for
efficient data transfer to video memory. Unlike the original Game Boy, the
TM32 Core supports 32-bit addressing for both source and destination addresses,
enabling transfers from any location in the expanded memory space.

##### OAM DMA (Object Attribute Memory DMA)

OAM DMA provides rapid transfer of sprite data to Object Attribute Memory.
This is essential for updating sprite positions and attributes efficiently.

**OAM DMA Registers:**

| Address | Name     | Access | Description                     |
|---------|----------|--------|---------------------------------|
| `0x46`  | `DMA`    | R/W    | OAM DMA Start (Compatibility)   |
| `0xA0`  | `ODMAS3` | W      | OAM DMA Source Address Byte 3   |
| `0xA1`  | `ODMAS2` | W      | OAM DMA Source Address Byte 2   |
| `0xA2`  | `ODMAS1` | W      | OAM DMA Source Address Byte 1   |
| `0xA3`  | `ODMA`   | R/W    | OAM DMA Start                   |

**Note:** Register `DMA` (0x46) is an alias to `ODMA` (0xA3) for Game Boy
compatibility. Writing to either register initiates the same OAM DMA transfer.

**OAM DMA Operation:**
1. Set source address by writing to `ODMAS3`, `ODMAS2`, `ODMAS1` (high to low)
2. Write any value to `ODMA` to start transfer
3. 160 bytes are transferred from source to OAM (`0xC000FE00`-`0xC000FE9F`)
4. Transfer takes 160 microseconds during which OAM is inaccessible
5. Only high RAM (`0xFFFF0000`-`0xFFFFFFFF`) is accessible during transfer

**Example OAM DMA Transfer:**
```assembly
.const hODMAS3 = 0xA0  ; OAM DMA Source Address Byte 3
.const hODMAS2 = 0xA1  ; OAM DMA Source Address Byte 2
.const hODMAS1 = 0xA2  ; OAM DMA Source Address Byte 1
.const hODMA   = 0xA3  ; OAM DMA Start Register

.text
oam_dma_transfer:
    ; Transfer sprite data from WRAM to OAM
    ; Source address: 0x80001000
    ld al, 0x80             ; High byte of source address
    stp [hODMAS3], al       ; Store to ODMAS3
    ld al, 0x00             ; Byte 2 of source address
    stp [hODMAS2], al       ; Store to ODMAS2
    ld al, 0x10             ; Byte 1 of source address
    stp [hODMAS1], al       ; Store to ODMAS1
    ld al, 0x00             ; Start transfer (value doesn't matter)
    stp [hODMA], al         ; Store to ODMA
    ret nc
```

##### HDMA (Horizontal Blank DMA)

HDMA provides automatic data transfer during horizontal blank periods, enabling
dynamic VRAM updates without CPU intervention. This is particularly useful for
implementing parallax scrolling, palette animation, and other advanced effects.

**HDMA Registers:**

| Address | Name     | Access | Description                       |
|---------|----------|--------|-----------------------------------|
| `0xA4`  | `HDMAS3` | W      | HDMA Source Address Byte 3        |
| `0xA5`  | `HDMAS2` | W      | HDMA Source Address Byte 2        |
| `0xA6`  | `HDMAS1` | W      | HDMA Source Address Byte 1        |
| `0xA7`  | `HDMAS0` | W      | HDMA Source Address Byte 0        |
| `0xA8`  | `HDMAD3` | W      | HDMA Destination Address Byte 3   |
| `0xA9`  | `HDMAD2` | W      | HDMA Destination Address Byte 2   |
| `0xAA`  | `HDMAD1` | W      | HDMA Destination Address Byte 1   |
| `0xAB`  | `HDMAD0` | W      | HDMA Destination Address Byte 0   |
| `0xAC`  | `HDMA`   | R/W    | HDMA Start / Control              |

**HDMA Control Register (0xAC):**
```
Bit  7   6   5   4   3   2   1   0
    M   -   -   -   -   -   -   LEN
```

- **Bit 7 (M) - Transfer Mode:**
  - 0: General Purpose DMA (immediate transfer)
  - 1: HBlank DMA (transfer during HBlank periods)
  
- **Bits 6-1:** Unused, always read as 0

- **Bit 0 (LEN) - Transfer Length:**
  - 0: Transfer 16 bytes per HBlank
  - 1: Transfer 32 bytes per HBlank

**HDMA Operation Modes:**

**1. General Purpose DMA (Bit 7 = 0):**
- Immediate transfer of data upon writing to `HDMA`
- CPU is halted during transfer
- Transfer size: 16 or 32 bytes (based on bit 0)
- Completes in 1-2 scanlines depending on size

**2. HBlank DMA (Bit 7 = 1):**
- Automatic transfer during each HBlank period
- 16 or 32 bytes transferred per scanline
- Continues until manually stopped or VRAM boundary reached
- CPU continues normal operation between transfers

**HDMA Transfer Control:**
- Writing with bit 7 = 0: Start General Purpose DMA
- Writing with bit 7 = 1: Start HBlank DMA
- Writing 0x00 during HBlank DMA: Stop current transfer
- Reading: Returns 0xFF if no transfer active, 0x80-0x81 if HBlank DMA active

**Example HDMA Setup:**
```assembly
.const hHDMAS3 = 0xA4  ; HDMA Source Address Byte 3
.const hHDMAS2 = 0xA5  ; HDMA Source Address Byte 2
.const hHDMAS1 = 0xA6  ; HDMA Source Address Byte 1
.const hHDMAS0 = 0xA7  ; HDMA Source Address Byte 0
.const hHDMAD3 = 0xA8  ; HDMA Destination Address Byte 3
.const hHDMAD2 = 0xA9  ; HDMA Destination Address Byte 2
.const hHDMAD1 = 0xAA  ; HDMA Destination Address Byte 1
.const hHDMAD0 = 0xAB  ; HDMA Destination Address Byte 0
.const hHDMA   = 0xAC  ; HDMA Start / Control Register

.text
hdma_setup_palette_animation:
    ; Setup HBlank DMA for palette animation
    ; Source: 0x80002000, Destination: 0xC0009800, 16 bytes per line

    ; Set source address
    ld al, 0x80             ; Source address byte 3
    stp [hHDMAS3], al       ; Store to HDMAS3
    ld al, 0x00             ; Source address byte 2
    stp [hHDMAS2], al       ; Store to HDMAS2
    ld al, 0x20             ; Source address byte 1
    stp [hHDMAS1], al       ; Store to HDMAS1
    ld al, 0x00             ; Source address byte 0
    stp [hHDMAS0], al       ; Store to HDMAS0

    ; Set destination address (VRAM tile map)
    ld al, 0xC0             ; Destination address byte 3
    stp [hHDMAD3], al       ; Store to HDMAD3
    ld al, 0x00             ; Destination address byte 2
    stp [hHDMAD2], al       ; Store to HDMAD2
    ld al, 0x98             ; Destination address byte 1
    stp [hHDMAD1], al       ; Store to HDMAD1
    ld al, 0x00             ; Destination address byte 0
    stp [hHDMAD0], al       ; Store to HDMAD0

    ; Start HBlank DMA (16 bytes per line)
    ld al, 0x80             ; HBlank mode, 16 bytes
    stp [hHDMA], al         ; Store to HDMA
    ret nc

hdma_stop_transfer:
    ; Stop active HBlank DMA transfer
    ld al, 0x00             ; Stop transfer
    stp [hHDMA], al         ; Store to HDMA
    ret nc
```

##### DMA Timing and Restrictions

**Access Restrictions During DMA:**
- OAM DMA: Only high RAM accessible, VRAM/OAM blocked
- General Purpose DMA: CPU halted, all memory accessible to DMA
- HBlank DMA: Normal memory access between transfers

**Performance Considerations:**
- OAM DMA: 160 μs transfer time
- General Purpose DMA: 1-2 scanlines depending on size
- HBlank DMA: No CPU performance impact
- HDMA ideal for real-time effects and large data updates

**Memory Alignment:**
- Source and destination addresses should be aligned to transfer boundaries
- 32-bit addresses enable access to entire TM32 memory space
- VRAM destination addresses must be within valid VRAM ranges

#### 4.7.8 PPU Interrupts

The PPU generates two types of interrupts that enable precise timing control
for graphics programming: VBlank and LCD STAT interrupts. These interrupts
are essential for synchronizing CPU operations with the display refresh cycle.

##### VBlank Interrupt (Vector 0x01)

The VBlank interrupt is requested at the beginning of the vertical blank period,
when the PPU transitions from scanline 143 to scanline 144 (Mode 1).

**VBlank Interrupt Timing:**
- **Triggered**: At the start of scanline 144 (beginning of VBlank period)
- **Duration**: VBlank period lasts 4560 dots (10 scanlines)
- **Frequency**: Once per frame (~59.7 Hz)
- **Vector**: `0x01` (address `0x00001100`)

**VBlank Interrupt Control:**
- **Enable**: Set bit 4 of `STAT` register (`M1 IE`)
- **Flag**: Automatically set in `IF0` register (bit 1)
- **Clear**: Software must clear bit 1 of `IF0` in interrupt handler

**VBlank Use Cases:**
- **VRAM Updates**: Safe period to modify tile data, tile maps, and palettes
- **OAM Updates**: Update sprite positions and attributes
- **Game Logic**: Perform frame-based calculations and state updates
- **Audio Synchronization**: Coordinate sound effects with visual events

**VBlank Programming Pattern:**
```assembly
.const hSTAT = 0x41  ; STAT Register
.const hIF0  = 0xFB  ; Interrupt Flag Register Byte 0
.const hIE0  = 0xFF  ; Interrupt Enable Register Byte 0

.text
enable_vblank_interrupt:
    ; Enable VBlank interrupt in STAT register
    ldp al, [hSTAT]         ; Read current STAT value
    or al, 0x10             ; Set bit 4 (M1 IE)
    stp [hSTAT], al         ; Write back to STAT
    
    ; Enable VBlank interrupt globally
    ldp al, [hIE0]          ; Read interrupt enable
    or al, 0x02             ; Set bit 1 (VBlank enable)
    stp [hIE0], al          ; Write back to IE0
    ret nc

.int 1
vblank_isr:
    ; VBlank interrupt service routine
    push a
    push b
    
    ; Clear VBlank interrupt flag
    ldp al, [hIF0]          ; Read interrupt flags
    and al, 0xFD            ; Clear bit 1 (VBlank flag)
    stp [hIF0], al          ; Write back to IF0
    
    ; Perform VBlank operations
    call nc, update_sprites
    call nc, update_background
    
    pop b
    pop a
    reti
```

##### LCD STAT Interrupt (Vector 0x02)

The LCD STAT interrupt provides fine-grained control over PPU mode transitions,
enabling precise timing for advanced graphics effects.

**LCD STAT Interrupt Sources:**
- **LYC=LY Coincidence**: When `LY` register equals `LYC` register
- **Mode 0 (HBlank)**: At the beginning of horizontal blank period
- **Mode 2 (OAM Scan)**: At the beginning of OAM scan period
- **Mode 1 (VBlank)**: At the beginning of vertical blank period (alternative to VBlank interrupt)

**LCD STAT Interrupt Control:**
- **LYC Enable**: Set bit 6 of `STAT` register (`LYC IE`)
- **Mode 0 Enable**: Set bit 3 of `STAT` register (`M0 IE`)
- **Mode 2 Enable**: Set bit 5 of `STAT` register (`M2 IE`)
- **Mode 1 Enable**: Set bit 4 of `STAT` register (`M1 IE`)
- **Global Enable**: Set bit 2 of `IE0` register
- **Flag**: Automatically set in `IF0` register (bit 2)
- **Clear**: Software must clear bit 2 of `IF0` in interrupt handler

**LCD STAT Interrupt Timing:**

| Source    | Timing                    | Use Cases                           |
|-----------|---------------------------|-------------------------------------|
| LYC=LY    | When LY matches LYC       | Scanline-specific effects, raster  |
| Mode 0    | Start of HBlank (87-204 dots) | Mid-scanline palette changes   |
| Mode 2    | Start of OAM Scan (80 dots)  | Sprite preprocessing            |
| Mode 1    | Start of VBlank (4560 dots)  | Alternative VBlank handling     |

**Advanced STAT Interrupt Examples:**

**Example 1: Scanline-Specific Effects**
```assembly
.text
setup_raster_effect:
    ; Setup LYC interrupt for scanline 64
    ld al, 64               ; Target scanline
    stp [hLYC], al          ; Store to LYC
    
    ; Enable LYC interrupt
    ldp al, [hSTAT]         ; Read STAT
    or al, 0x40             ; Set bit 6 (LYC IE)
    stp [hSTAT], al         ; Write back to STAT
    
    ; Enable LCD STAT interrupt globally
    ldp al, [hIE0]          ; Read interrupt enable
    or al, 0x04             ; Set bit 2 (LCD STAT enable)
    stp [hIE0], al          ; Write back to IE0
    ret nc

.int 2
lcd_stat_isr:
    ; LCD STAT interrupt service routine
    push a
    push b
    
    ; Clear LCD STAT interrupt flag
    ldp al, [hIF0]          ; Read interrupt flags
    and al, 0xFB            ; Clear bit 2 (LCD STAT flag)
    stp [hIF0], al          ; Write back to IF0
    
    ; Check which condition triggered the interrupt
    ldp al, [hSTAT]         ; Read STAT register
    and al, 0x04            ; Check LYC flag (bit 2)
    jpb zs, not_lyc_match   ; Jump if LYC flag not set
    
lyc_match_handler:
    ; Handle LYC=LY coincidence
    ; Change background palette for raster effect
    ld al, 0xE4             ; New palette value
    stp [hBGP], al          ; Store to background palette
    jpb nc, stat_isr_end
    
not_lyc_match:
    ; Handle other STAT interrupt sources
    ; Check current PPU mode
    ldp al, [hSTAT]         ; Read STAT register
    and al, 0x03            ; Mask mode bits
    cmp al, 0               ; Check for Mode 0 (HBlank)
    jpb zs, hblank_handler
    ; Add handlers for other modes as needed
    
hblank_handler:
    ; Handle HBlank interrupt
    ; Restore original palette
    ld al, 0x1B             ; Original palette value
    stp [hBGP], al          ; Store to background palette
    
stat_isr_end:
    pop b
    pop a
    reti
```

**Example 2: Multi-Mode STAT Interrupt**
```assembly
.text
setup_multi_stat_interrupts:
    ; Enable multiple STAT interrupt sources
    ldp al, [hSTAT]         ; Read current STAT
    or al, 0x78             ; Enable LYC, Mode 2, Mode 1, Mode 0
    stp [hSTAT], al         ; Write back to STAT
    
    ; Enable LCD STAT interrupt globally
    ldp al, [hIE0]          ; Read interrupt enable
    or al, 0x04             ; Set bit 2 (LCD STAT enable)
    stp [hIE0], al          ; Write back to IE0
    ret nc
```

##### Interrupt Priority and Handling

**Interrupt Priority:**
1. VBlank interrupt (Vector 0x01) has higher priority than LCD STAT (Vector 0x02)
2. If both interrupts are pending, VBlank will be handled first
3. Interrupt flags must be cleared manually in software

**Best Practices:**
- **Clear Flags**: Always clear interrupt flags in ISR to prevent infinite loops
- **Minimize ISR Time**: Keep interrupt handlers short and fast
- **Save Registers**: Preserve CPU state in interrupt handlers
- **Atomic Operations**: Use `DI`/`EI` around critical sections
- **VBlank Priority**: Use VBlank for major updates, STAT for timing-critical effects

**Interrupt Enable Example:**
```assembly
.text
enable_ppu_interrupts:
    ; Disable interrupts during setup
    di
    
    ; Enable VBlank interrupt
    ldp al, [hSTAT]         ; Read STAT
    or al, 0x10             ; Set M1 IE (VBlank)
    stp [hSTAT], al         ; Write back to STAT
    
    ; Enable both PPU interrupts globally
    ldp al, [hIE0]          ; Read IE0
    or al, 0x06             ; Set bits 1-2 (VBlank + LCD STAT)
    stp [hIE0], al          ; Write back to IE0
    
    ; Re-enable interrupts
    ei
    ret nc                  ; The `reti` instruction can also be used here to
                            ; enable interrupts and return in the same instruction.
```

#### 4.7.9 Advanced Features

**Window System:**
- Independent scrollable overlay
- Can be positioned anywhere on screen
- Useful for status bars and UI elements
- Controlled by `WX` and `WY` registers

**Sprite System:**
- 40 sprites maximum (10 per scanline)
- 8×8 or 8×16 pixel sizes
- Horizontal and vertical flipping
- Priority and palette control
- Transparent color support

**Raster Effects:**
- Mid-scanline register modifications
- Horizontal parallax scrolling
- Scanline-based palette changes
- Window position animation

**Performance Considerations:**
- Sprite count affects Mode 3 duration
- Background scrolling adds timing penalties
- VRAM access timing critical for effects
- Interrupt timing affects visual quality

### 4.8 APU Component

The TM32 Core includes an Audio Processing Unit (APU) that provides comprehensive
sound generation capabilities. The APU is based on the Nintendo Game Boy's sound
architecture but enhanced for the TM32's 32-bit environment and expanded
functionality. It features four sound channels that can be mixed and routed
through stereo outputs, plus support for external audio input via the VIN
(Video Input/Audio Input) channels.

The APU operates synchronously with the CPU and other system components, running
at the same master clock frequency as the rest of the TM32 Core. This ensures
precise timing for audio generation and synchronization with visual effects.

#### 4.8.1 APU Overview

**Sound Generation Capabilities:**
- 4 independent sound channels with unique characteristics
- 11-bit period resolution for precise frequency control
- Stereo output with individual channel panning
- Master volume control with 8 levels per channel
- Length timers for automatic channel cutoff
- Envelope generation for volume animation
- External audio input support via VIN channels

**Channel Architecture:**
- **Channel 1**: Pulse wave with frequency sweep capability
- **Channel 2**: Pulse wave with duty cycle control
- **Channel 3**: Custom waveform playback (32 4-bit samples)
- **Channel 4**: Pseudo-random noise generation

**Audio Signal Path:**
```
Ch1 ───┐
Ch2 ───┤ Mixer ──── Left Amplifier  ──── Left Output
Ch3 ───┤            Right Amplifier ──── Right Output
Ch4 ───┘
VIN ────
```

#### 4.8.2 APU Registers

The APU provides several memory-mapped registers accessible through the port
register space:

| Address | Name    | Access | Description                          |
|---------|---------|--------|--------------------------------------|
| `0x10`  | `NR10`  | R/W    | Channel 1 Frequency Sweep           |
| `0x11`  | `NR11`  | R/W    | Channel 1 Length / Duty             |
| `0x12`  | `NR12`  | R/W    | Channel 1 Volume / Envelope         |
| `0x13`  | `NR13`  | W      | Channel 1 Period Low Byte           |
| `0x14`  | `NR14`  | R/W    | Channel 1 Period High / Control     |
| `0x16`  | `NR21`  | R/W    | Channel 2 Length / Duty             |
| `0x17`  | `NR22`  | R/W    | Channel 2 Volume / Envelope         |
| `0x18`  | `NR23`  | W      | Channel 2 Period Low Byte           |
| `0x19`  | `NR24`  | R/W    | Channel 2 Period High / Control     |
| `0x1A`  | `NR30`  | R/W    | Channel 3 DAC Enable                |
| `0x1B`  | `NR31`  | W      | Channel 3 Length                    |
| `0x1C`  | `NR32`  | R/W    | Channel 3 Output Level              |
| `0x1D`  | `NR33`  | W      | Channel 3 Period Low Byte           |
| `0x1E`  | `NR34`  | R/W    | Channel 3 Period High / Control     |
| `0x20`  | `NR41`  | W      | Channel 4 Length                    |
| `0x21`  | `NR42`  | R/W    | Channel 4 Volume / Envelope         |
| `0x22`  | `NR43`  | R/W    | Channel 4 Frequency / Randomness    |
| `0x23`  | `NR44`  | R/W    | Channel 4 Control                   |
| `0x24`  | `NR50`  | R/W    | Master Volume / VIN Panning         |
| `0x25`  | `NR51`  | R/W    | Channel Panning Control             |
| `0x26`  | `NR52`  | R/W    | APU Master Control / Status         |

#### 4.8.3 Global Control Registers

##### NR52 - APU Master Control / Status (0x26)

```
Bit  7   6   5   4   3   2   1   0
    APU  -   -   -  CH4 CH3 CH2 CH1
    EN              ON  ON  ON  ON
```

The `NR52` register controls the overall APU functionality and provides channel
status information.

**Bit Fields:**
- **Bit 7 (APU EN) - APU Enable:**
  - 0: APU disabled, all registers become read-only except `NR52`
  - 1: APU enabled, normal operation

- **Bits 6-4:** Unused, always read as 0

- **Bits 3-0 (CHx ON) - Channel Status (Read-only):**
  - Each bit indicates whether the corresponding channel is currently active
  - Writing to these bits has no effect
  - Channels turn on when triggered, turn off when length timer expires or
    other stop conditions are met

**Register Accessibility:**
- When the APU is disabled by clearing `NR52`'s bit 7:
    - All APU registers become read-only, except for `NR52`; writing to these
        other registers will have no effect.

##### NR51 - Channel Panning Control (0x25)

```
Bit  7   6   5   4   3   2   1   0
   CH4 CH3 CH2 CH1 CH4 CH3 CH2 CH1
   L   L   L   L   R   R   R   R
```

The `NR51` register controls which channels are routed to the left and right
audio outputs.

**Bit Fields:**
- **Bits 7-4 (CHx L) - Left Channel Enable:**
  - Setting a bit routes the corresponding channel to the left output
  - 0: Channel not sent to left output
  - 1: Channel sent to left output

- **Bits 3-0 (CHx R) - Right Channel Enable:**
  - Setting a bit routes the corresponding channel to the right output
  - 0: Channel not sent to right output
  - 1: Channel sent to right output

**Panning Effects:**
- Setting both L and R bits: Channel plays in center (mono)
- Setting only L bit: Channel plays hard left
- Setting only R bit: Channel plays hard right
- Setting neither bit: Channel is muted

##### NR50 - Master Volume / VIN Panning (0x24)

```
Bit  7   6   5   4   3   2   1   0
   VIN  L   L   L  VIN  R   R   R
   L   V2  V1  V0  R   V2  V1  V0
```

The `NR50` register controls master volume levels and VIN channel routing.

**Bit Fields:**
- **Bit 7 (VIN L) - VIN Left Enable:**
  - 0: External VIN signal not routed to left output
  - 1: External VIN signal routed to left output

- **Bits 6-4 (L V2-V0) - Left Master Volume:**
  - 000: Volume level 1 (quietest)
  - 001: Volume level 2
  - ...
  - 111: Volume level 8 (loudest)

- **Bit 3 (VIN R) - VIN Right Enable:**
  - 0: External VIN signal not routed to right output
  - 1: External VIN signal routed to right output

- **Bits 2-0 (R V2-V0) - Right Master Volume:**
  - Same scale as left master volume

**VIN Channel Explanation:**
VIN stands for "Video Input" but actually refers to an analog audio input
channel. This feature was originally designed to allow TM32 programs to supply
external audio signals that would be mixed with the internal APU channels.
The VIN input provides:

- **External Audio Support**: TM32 programs can supply analog audio signals
- **Audio Expansion**: Allows for additional sound channels beyond the built-in 4
- **Mixing Capability**: External signals are mixed with internal channels
- **Stereo Routing**: VIN can be independently routed to left/right outputs

In the TM32 Core context, VIN can be used for:
- **Sample Playback**: External hardware providing PCM audio streams
- **Music Expansion**: Additional synthesis chips or audio processors
- **Sound Effects**: Real-time audio input from external sources
- **Audio Interface**: Connection to external audio equipment

#### 4.8.4 Channel 1 - Pulse with Frequency Sweep

Channel 1 generates pulse waves with configurable duty cycles and includes
unique frequency sweep functionality for creating pitch-bend effects.

##### NR10 - Channel 1 Frequency Sweep (0x10)

```
Bit  7   6   5   4   3   2   1   0
    -  P2  P1  P0  DIR S2  S1  S0
```

**Bit Fields:**
- **Bit 7:** Unused, always read as 0

- **Bits 6-4 (P2-P0) - Sweep Pace:**
  - Controls how often sweep iterations occur (128 Hz ticks)
  - 000: Sweep disabled
  - 001: Sweep every 1×128 Hz tick (7.8 ms)
  - 010: Sweep every 2×128 Hz ticks (15.6 ms)
  - ...
  - 111: Sweep every 7×128 Hz ticks (54.7 ms)

- **Bit 3 (DIR) - Sweep Direction:**
  - 0: Increase frequency (period decreases)
  - 1: Decrease frequency (period increases)

- **Bits 2-0 (S2-S0) - Sweep Step:**
  - Controls the amount of change per sweep iteration
  - New period = Current period ± (Current period >> Step)
  - 000: No change per step
  - 001: Change by 1/2 of current period
  - 010: Change by 1/4 of current period
  - ...
  - 111: Change by 1/128 of current period

##### NR11 - Channel 1 Length / Duty (0x11)

```
Bit  7   6   5   4   3   2   1   0
   D1  D0  L5  L4  L3  L2  L1  L0
```

**Bit Fields:**
- **Bits 7-6 (D1-D0) - Duty Cycle:**
  - 00: 12.5% duty cycle (       ▄)
  - 01: 25% duty cycle   (      ▄▄)
  - 10: 50% duty cycle   (    ▄▄▄▄)
  - 11: 75% duty cycle   (  ▄▄▄▄▄▄)

- **Bits 5-0 (L5-L0) - Length Timer:**
  - Initial value for length timer (write-only)
  - Higher values = shorter playback time
  - Timer counts up at 256 Hz until reaching 64

##### NR12 - Channel 1 Volume / Envelope (0x12)

```
Bit  7   6   5   4   3   2   1   0
   V3  V2  V1  V0  DIR E2  E1  E0
```

**Bit Fields:**
- **Bits 7-4 (V3-V0) - Initial Volume:**
  - 0000: Silent
  - 0001: Quietest audible volume
  - ...
  - 1111: Maximum volume

- **Bit 3 (DIR) - Envelope Direction:**
  - 0: Decrease volume over time
  - 1: Increase volume over time

- **Bits 2-0 (E2-E0) - Envelope Pace:**
  - Controls envelope timing (64 Hz ticks)
  - 000: Envelope disabled
  - 001: Change volume every 1×64 Hz tick (15.6 ms)
  - ...
  - 111: Change volume every 7×64 Hz ticks (109 ms)

##### NR13 - Channel 1 Period Low (0x13)

```
Bit  7   6   5   4   3   2   1   0
   P7  P6  P5  P4  P3  P2  P1  P0
```

Contains the lower 8 bits of the 11-bit period value. Higher period values
result in lower frequencies. This register is write-only.

##### NR14 - Channel 1 Period High / Control (0x14)

```
Bit  7   6   5   4   3   2   1   0
   TRG LEN  -   -   -  P10 P9  P8
```

**Bit Fields:**
- **Bit 7 (TRG) - Trigger (Write-only):**
  - Writing 1 starts/restarts the channel
  - Resets envelope, sweep, and length timer
  - Sets volume to initial value

- **Bit 6 (LEN) - Length Enable:**
  - 0: Channel plays indefinitely
  - 1: Channel stops when length timer expires

- **Bits 5-3:** Unused, always read as 0

- **Bits 2-0 (P10-P8) - Period High:**
  - Upper 3 bits of 11-bit period value
  - Combined with `NR13` to form complete period

**Frequency Calculation:**
- Pulse channels are clocked at 1048576 Hz
- Sample rate = 1048576 ÷ (2048 - period) Hz
- Tone frequency = Sample rate ÷ 8 Hz

#### 4.8.5 Channel 2 - Pulse

Channel 2 operates identically to Channel 1 but without frequency sweep
capability. It uses registers `NR21`-`NR24` which function exactly like
`NR11`-`NR14` respectively. Refer to Channel 1 documentation for bit field
descriptions.

#### 4.8.6 Channel 3 - Wave Output

Channel 3 provides custom waveform playback using a 32-sample wavetable stored
in Wave RAM. This allows for complex timbres and sample-based audio.

##### NR30 - Channel 3 DAC Enable (0x1A)

```
Bit  7   6   5   4   3   2   1   0
   DAC  -   -   -   -   -   -   -
   EN
```

**Bit Fields:**
- **Bit 7 (DAC EN) - DAC Enable:**
  - 0: Channel 3 DAC disabled, channel cannot play
  - 1: Channel 3 DAC enabled, channel can play

- **Bits 6-0:** Unused, always read as 0

##### NR32 - Channel 3 Output Level (0x1C)

```
Bit  7   6   5   4   3   2   1   0
    -  V1  V0   -   -   -   -   -
```

**Bit Fields:**
- **Bit 7:** Unused, always read as 0

- **Bits 6-5 (V1-V0) - Output Level:**
  - 00: Mute (0% volume)
  - 01: Full volume (100%)
  - 10: Half volume (50%)
  - 11: Quarter volume (25%)

- **Bits 4-0:** Unused, always read as 0

##### Wave RAM (0xFFFFFF30-0xFFFFFF3F)

Wave RAM contains 16 bytes storing 32 4-bit samples. Each byte contains two
samples: upper nibble first, then lower nibble.

**Memory Layout:**
```
Byte 0: Sample 0 (high) | Sample 1 (low)
Byte 1: Sample 2 (high) | Sample 3 (low)
...
Byte 15: Sample 30 (high) | Sample 31 (low)
```

**Access Restrictions:**
- Wave RAM is accessible when Channel 3 is not playing
- Accessing during playback may return undefined values
- Recommend stopping channel before updating Wave RAM

#### 4.8.7 Channel 4 - Noise

Channel 4 generates pseudo-random noise using a Linear Feedback Shift Register
(LFSR). This channel is essential for percussion, sound effects, and ambient
audio.

##### NR43 - Channel 4 Frequency / Randomness (0x22)

```
Bit  7   6   5   4   3   2   1   0
   S3  S2  S1  S0  WID D2  D1  D0
```

**Bit Fields:**
- **Bits 7-4 (S3-S0) - Clock Shift:**
  - Controls the base frequency shift
  - Higher values = lower frequencies

- **Bit 3 (WID) - LFSR Width:**
  - 0: 15-bit LFSR (more random, noise-like)
  - 1: 7-bit LFSR (more periodic, tone-like)

- **Bits 2-0 (D2-D0) - Clock Divider:**
  - Controls the frequency divider
  - 000: Divider = 0.5
  - 001: Divider = 1
  - 010: Divider = 2
  - ...
  - 111: Divider = 7

**Frequency Calculation:**
Clock frequency = 262144 ÷ (Divider × 2^Shift) Hz

#### 4.8.8 Programming Examples

**Example 1: Basic APU Setup**
```assembly
.const hNR50 = 0x24  ; Master Volume / VIN Panning
.const hNR51 = 0x25  ; Channel Panning Control
.const hNR52 = 0x26  ; APU Master Control

.text
apu_basic_setup:
    ; Enable APU and set master volume
    ld al, 0x80             ; Enable APU
    stp [hNR52], al         ; Store to NR52

    ; Set master volume to maximum, disable VIN
    ld al, 0x77             ; Left vol 7, Right vol 7, no VIN
    stp [hNR50], al         ; Store to NR50

    ; Enable all channels on both outputs
    ld al, 0xFF             ; All channels left and right
    stp [hNR51], al         ; Store to NR51
    ret nc
```

**Example 2: Simple Pulse Wave (Channel 1)**
```assembly
.const hNR10 = 0x10  ; Channel 1 Sweep
.const hNR11 = 0x11  ; Channel 1 Length / Duty
.const hNR12 = 0x12  ; Channel 1 Volume / Envelope
.const hNR13 = 0x13  ; Channel 1 Period Low
.const hNR14 = 0x14  ; Channel 1 Period High / Control

.text
channel1_middle_c:
    ; Setup Channel 1 for middle C (261.63 Hz)
    ; Period = 2048 - (1048576 / (261.63 * 8)) = 1545

    ; Disable sweep
    ld al, 0x00             ; No sweep
    stp [hNR10], al         ; Store to NR10

    ; Set 50% duty cycle
    ld al, 0x80             ; 50% duty, length = 0
    stp [hNR11], al         ; Store to NR11

    ; Set volume and envelope
    ld al, 0xF0             ; Max volume, no envelope
    stp [hNR12], al         ; Store to NR12

    ; Set period (1545 = 0x609)
    ld al, 0x09             ; Low 8 bits
    stp [hNR13], al         ; Store to NR13

    ; Trigger channel
    ld al, 0x86             ; Trigger, length enable, high 3 bits
    stp [hNR14], al         ; Store to NR14
    ret nc
```

**Example 3: VIN Audio Mixing**
```assembly
.text
vin_audio_mixing:
    ; Route external VIN signal with internal channels
    ; VIN signal represents external audio hardware

    ; Enable VIN on both channels with reduced internal volume
    ld al, 0xBB             ; VIN left+right, internal vol 3
    stp [hNR50], al         ; Store to NR50

    ; Mix VIN with Channel 1 only
    ld al, 0x11             ; Ch1 left+right only
    stp [hNR51], al         ; Store to NR51

    ; This creates a mix where:
    ; - External VIN audio plays at full volume
    ; - Internal Channel 1 plays at reduced volume (3/8)
    ; - Other internal channels are muted
    ; - Both signals are mixed and sent to left+right outputs
    ret nc
```

**Example 4: Frequency Sweep Effect**
```assembly
.text
frequency_sweep_effect:
    ; Create ascending frequency sweep on Channel 1

    ; Setup sweep: pace=1, direction=increase, step=1
    ld al, 0x11             ; Pace 1, increase freq, step 1
    stp [hNR10], al         ; Store to NR10

    ; 25% duty cycle
    ld al, 0x40             ; 25% duty
    stp [hNR11], al         ; Store to NR11

    ; Max volume with envelope
    ld al, 0xF3             ; Vol 15, increase, pace 3
    stp [hNR12], al         ; Store to NR12

    ; Start at low frequency (period = 2000)
    ld al, 0xD0             ; Low 8 bits of 2000
    stp [hNR13], al         ; Store to NR13

    ; Trigger with length enabled
    ld al, 0xC7             ; Trigger, length, high 3 bits of 2000
    stp [hNR14], al         ; Store to NR14

    ; The sweep will automatically increase frequency over time
    ; creating a rising pitch effect
    ret nc
```

**Example 5: Using the Wave Channel and Wave RAM**
```assembly
.const hNR30 = 0x1A  ; Channel 3 DAC Enable
.const hNR31 = 0x1B  ; Channel 3 Length
.const hNR32 = 0x1C  ; Channel 3 Output Level
.const hNR33 = 0x1D  ; Channel 3 Period Low
.const hNR34 = 0x1E  ; Channel 3 Period High / Control

.text
wave_channel_sawtooth:
    ; Create a custom sawtooth wave in Wave RAM
    ; Each sample ramps from 0 to 15 over 32 samples

    ; First, disable Channel 3 to safely access Wave RAM
    ld al, 0x00             ; Disable DAC
    stp [hNR30], al         ; Store to NR30

    ; Fill Wave RAM with sawtooth pattern
    ; Wave RAM starts at 0xFFFFFF30
    ld b, 0xFFFFFF30        ; Wave RAM base address
    ld c, 0                 ; Sample counter

wave_fill_loop:
    ; Calculate sample value: C / 2 (0-15 range)
    ld al, cl               ; Get sample counter
    sra al, 1               ; Divide by 2 for 0-15 range
    
    ; Check if we're filling upper or lower nibble
    ld dl, cl               ; Copy counter
    and dl, 1               ; Check if odd (lower nibble)
    jpb zs, upper_nibble    ; If even, fill upper nibble
    
lower_nibble:
    ; Fill lower nibble, preserve upper nibble
    ldp dl, [b]             ; Read current byte
    and dl, 0xF0            ; Clear lower nibble
    or dl, al               ; Set lower nibble
    stp [b], dl             ; Write back
    inc b                   ; Next byte
    jpb nc, wave_next_sample
    
upper_nibble:
    ; Fill upper nibble, clear lower nibble for now
    sla al, 4               ; Shift to upper nibble
    stp [b], al             ; Write upper nibble
    
wave_next_sample:
    inc c                   ; Next sample
    cmp c, 32               ; Check if all 32 samples done
    jpb zc, wave_fill_loop  ; Continue if not done

    ; Now setup Channel 3 to play the sawtooth wave
    ; Enable DAC
    ld al, 0x80             ; Enable DAC
    stp [hNR30], al         ; Store to NR30

    ; Set length timer (optional)
    ld al, 0x80             ; Length = 128 (moderate duration)
    stp [hNR31], al         ; Store to NR31

    ; Set output level to 50%
    ld al, 0x40             ; 50% volume
    stp [hNR32], al         ; Store to NR32

    ; Set frequency for middle C (261.63 Hz)
    ; Wave channel period calculation: 2097152 / (2048 - period) / 32 Hz
    ; For 261.63 Hz: period = 2048 - (2097152 / (261.63 * 32)) = 1796
    ; 1796 = 0x704

    ld al, 0x04             ; Low 8 bits of period
    stp [hNR33], al         ; Store to NR33

    ; Trigger channel with length enabled
    ld al, 0xC7             ; Trigger, length enable, high 3 bits (0x7)
    stp [hNR34], al         ; Store to NR34

    ; Channel 3 will now play the custom sawtooth wave
    ; The sawtooth creates a bright, buzzy tone different from
    ; the simple pulse waves of channels 1 and 2
    ret nc
```

**Example 6: Dynamic Wave RAM Update**
```assembly
; Create a vibrato effect by dynamically updating Wave RAM
; This example modifies the wave shape in real-time

.const wave_base = 0xFFFFFF30

.text
vibrato_effect:
    ; Stop channel 3 temporarily
    ld al, 0x00
    stp [hNR30], al         ; Disable DAC
    
    ; Create a sine-like wave with varying amplitude
    ; Use a simple approximation: alternate between
    ; strong and weak sawtooth patterns
    
    ldp al, [vibrato_counter] ; Get vibrato phase
    and al, 0x1F            ; Limit to 0-31 range
    cmp al, 16              ; Check which half of vibrato cycle
    jpb cs, weak_wave       ; If >= 16, use weak wave
    
strong_wave:
    ; Create strong sawtooth (0-15 range)
    ld b, wave_base
    ld c, 0
strong_loop:
    ld al, cl
    sra al, 1               ; Divide by 2 for range 0-15
    
    ; Pack two samples per byte
    ld dl, cl
    and dl, 1
    jpb zs, strong_upper
    
    ; Lower nibble
    ldp dl, [b]
    and dl, 0xF0
    or dl, al
    stp [b], dl
    inc b
    jpb nc, strong_next
    
strong_upper:
    ; Upper nibble
    sla al, 4
    stp [b], al
    
strong_next:
    inc c
    cmp c, 32
    jpb zc, strong_loop
    jpb nc, restart_wave
    
weak_wave:
    ; Create weak sawtooth (0-7 range, quieter)
    ld b, wave_base
    ld c, 0
weak_loop:
    ld al, cl
    sra al, 2               ; Divide by 4 for range 0-7
    
    ; Pack two samples per byte
    ld dl, cl
    and dl, 1
    jpb zs, weak_upper
    
    ; Lower nibble
    ldp dl, [b]
    and dl, 0xF0
    or dl, al
    stp [b], dl
    inc b
    jpb nc, weak_next
    
weak_upper:
    ; Upper nibble
    sla al, 4
    stp [b], al
    
weak_next:
    inc c
    cmp c, 32
    jpb zc, weak_loop
    
restart_wave:
    ; Re-enable Channel 3
    ld al, 0x80
    stp [hNR30], al         ; Enable DAC
    
    ; Retrigger to apply new wave data
    ld al, 0x80
    stp [hNR34], al         ; Retrigger channel
    
    ; Update vibrato counter
    ldp al, [vibrato_counter]
    inc al
    stp [vibrato_counter], al
    
    ret nc

; Data storage for vibrato effect
.data
vibrato_counter:
    .db 0
```

#### 4.8.9 Advanced Features

**Audio Synchronization:**
- APU runs synchronously with CPU and PPU
- Precise timing for audio-visual synchronization
- Frame-rate locked audio for consistent playback

**Power Management:**
- APU can be disabled to save power
- Individual channel DACs can be controlled
- Wave RAM remains accessible when APU is off

**External Audio Integration:**
- VIN channels allow external audio mixing
- Support for cartridge-based audio expansion
- Real-time audio input processing

**Performance Considerations:**
- Wave RAM access restrictions during playback
- Envelope and sweep calculations run at fixed rates
- Channel mixing occurs in real-time without CPU intervention

### 4.9 RAM Component

The TM32 Core includes a versatile RAM component that provides multiple types of
read-write memory for various application needs. The RAM component is divided into
four main sections:

- **Working RAM (WRAM)**: General-purpose RAM stored internally within the TM32 Core.
- **External RAM (SRAM)**: Optional general-purpose RAM that can be provided by
    the program loaded into the TM32 Core.
- **Executable RAM (XRAM)**: General-purpose RAM that works the same as WRAM,
    but is also executable, allowing programs to run code directly from this memory.
- **Quick RAM (QRAM)**: Also called "High-Page RAM" or "High RAM (HRAM)",
    this is a small, fast-access RAM accessible via a 16-bit relative address,
    rather than a full 32-bit address.

#### 4.9.1 Working RAM (WRAM)

Working RAM (WRAM) is a general-purpose RAM area used for temporary data storage
during program execution. It is internally located within the TM32 Core and is
the primary RAM area used by most programs. TM32 Core Programs can request anywhere
between 4 KB and 512 MB of WRAM, preferably in powers of two, although this is not
a strict requirement.

#### 4.9.2 External RAM (SRAM)

External RAM, also called Static (or Save) RAM (SRAM), is an optional RAM area
that can be requested by TM32 Core Programs for storing persistent data, such as
game saves or user settings. This RAM area is not included by default and must be
explicitly requested by the program in its metadata header. Programs can request
between 0 KB and 512 MB of External RAM, preferably in powers of two, although
this is not a strict requirement. If a program does not require persistent storage,
it can set the requested size to 0 KB.

#### 4.9.3 Executable RAM (XRAM)

Executable RAM (XRAM) is a special type of RAM that functions like WRAM but also
allows code execution directly from this memory area. This is useful for programs
that require writable and executable memory, such as Just-In-Time (JIT) compilation
or self-modifying code. Programs can request between 0 bytes and 768 MB of Executable
RAM, preferably in powers of two, although this is not a strict requirement. If a
program does not require executable RAM, it can set the requested size to 0 bytes.

#### 4.9.4 Quick RAM (QRAM)

Quick RAM (QRAM), also known as High-Page RAM or High RAM (HRAM), is a small,
fast-access RAM area that is directly addressable via a 16-bit relative address,
which will be translated by the TM32 CPU to a full 32-bit address in the range
`0xFFFF0000` to `0xFFFFFEFF`. This allows for quick access to frequently used
data without the overhead of full 32-bit addressing. The QRAM area is fixed at
65,280 bytes (64 KB - 256 bytes) and is always available to programs.

## 5. Performance and Timing Specifications

The TM32 Core provides predictable performance characteristics essential for
real-time applications and game development. This section documents timing
specifications, performance metrics, and optimization considerations.

### 5.1 CPU Performance Characteristics

**Base Clock Frequency:** 4.194304 MHz (matches original Game Boy frequency)

**CPU Cycle Definition:**
A CPU cycle is consumed every time the TM32 CPU accesses the address bus:
- **Memory Access**: One cycle per byte read from or written to memory
  - 8-bit access: 1 cycle
  - 16-bit access: 2 cycles (2 bytes)
  - 32-bit access: 4 cycles (4 bytes)
- **Control Flow**: One cycle for each PC/SP register manipulation by 
  control-transfer and stack instructions

**Instruction Timing:**
- **Minimum**: 2 cycles (16-bit opcode fetch requires 2 bytes = 2 cycles)
- **Memory operations**: Base cycles + data access cycles
  - 8-bit load/store: 2 + 1 = 3 cycles
  - 16-bit load/store: 2 + 2 = 4 cycles  
  - 32-bit load/store: 2 + 4 = 6 cycles
- **Register-only operations**: 2 cycles (opcode fetch only)
- **Branch instructions**: 2 cycles + PC manipulation = 3 cycles
- **Stack operations**: 
  - PUSH: 2 + 4 + 1 = 7 cycles (opcode + data write + SP manipulation)
  - POP: 2 + 4 + 1 = 7 cycles (opcode + data read + SP manipulation)
- **Function calls**: 
  - CALL: 2 + 4 + 1 = 7 cycles (opcode + PC push + PC manipulation)
  - RET: 2 + 4 + 1 = 7 cycles (opcode + PC pop + PC manipulation)

**Memory Access Characteristics:**
All memory regions have identical access timing based on data width:
- 8-bit access: 1 cycle per access
- 16-bit access: 2 cycles per access  
- 32-bit access: 4 cycles per access
- No cache or wait states - direct memory bus access
- PPU VRAM access may introduce bus contention during rendering

### 5.2 Hardware Component Performance

#### 5.2.1 Timer Component Timing

**DIV Register:**
- Update frequency: Every 256 CPU cycles (16.384 kHz)
- Read access: 2 cycles (opcode fetch) + 1 cycle (8-bit read) = 3 cycles
- Write access: 2 cycles (opcode fetch) + 1 cycle (8-bit write) = 3 cycles

**TIMA Register:**
- Update frequencies:
  - 4096 Hz mode: Every 1024 CPU cycles
  - 16384 Hz mode: Every 256 CPU cycles
  - 65536 Hz mode: Every 64 CPU cycles
  - 262144 Hz mode: Every 16 CPU cycles
- Read access: 3 cycles total (2 + 1)
- Write access: 3 cycles total (2 + 1)
- Interrupt generation: Automatic when TIMA overflows from 0xFF to 0x00

#### 5.2.2 Real-Time Clock Performance

**Time Accuracy:**
- Crystal oscillator tolerance: ±20 ppm (±0.002%)
- Daily accuracy: ±1.7 seconds per day
- Temperature stability: ±2 ppm per °C

**Access Timing:**
- Latch operation: 2 cycles (opcode) + 1 cycle (8-bit write) = 3 cycles
- Register read: 2 cycles (opcode) + 1 cycle (8-bit read) = 3 cycles
- Multi-register read: 3 cycles per register accessed

#### 5.2.3 Serial Transfer Performance

**Transfer Rates:**
- Internal clock: 8192 Hz, 4096 Hz, 2048 Hz, 1024 Hz
- External clock: Up to 16384 Hz
- Transfer completion time: 8 bits × clock period
- Register access: 3 cycles per 8-bit read/write operation

#### 5.2.4 Joypad Input Performance

**Scan Rate:**
- Maximum scan frequency: 1024 Hz
- Typical application scan: 60 Hz
- Input debounce time: 10ms (implementation dependent)
- Register access: 3 cycles per 8-bit read/write operation

### 5.3 Memory Performance Optimization

#### 5.3.1 Memory Region Selection

**Performance Characteristics:**
All memory regions have identical base access timing:
- 8-bit access: 1 cycle per operation
- 16-bit access: 2 cycles per operation  
- 32-bit access: 4 cycles per operation

**Memory Region Notes:**
- Working RAM (WRAM): Standard access timing
- External RAM (SRAM): Standard access timing
- Executable RAM (XRAM): Standard access timing
- Quick RAM (QRAM): Standard access timing  
- Video RAM (VRAM): May experience bus contention during PPU rendering
- I/O Registers: Standard 8-bit access timing (1 cycle per register)

**Optimization Guidelines:**
- Data width affects cycle count more than memory region
- Use 8-bit operations when possible for fastest access
- VRAM access timing may vary during active display periods
- No performance difference between WRAM, SRAM, XRAM, and QRAM

#### 5.3.2 Cache and Pipeline Considerations

**Instruction Pipeline:**
- Simple fetch-execute model: no complex pipeline
- All instructions complete before next instruction starts
- No pipeline stalls or branch prediction
- Instruction fetch always requires 2 cycles (16-bit opcode)

**Memory Bus Utilization:**
- Single 32-bit memory bus shared by CPU and hardware components
- CPU cycle consumption drives all hardware component clocking
- PPU VRAM access during rendering operates on same bus
- I/O register access bypasses main memory bus

### 5.4 Power Management and Performance States

#### 5.4.1 CPU Power States

**Active State:**
- Full performance: 4.194304 MHz operation
- Power consumption: 100% (baseline)
- All components operational

**HALT State:**
- CPU halted, hardware continues
- Power consumption: ~60% of active
- Interrupt wake-up time: 2-4 CPU cycles

**STOP State:**
- Ultra-low power mode
- Power consumption: ~5% of active
- Wake-up time: 50-100ms (hardware dependent)
- Limited wake-up sources

#### 5.4.2 Component Power Management

**Timer Component:**
- Always active (except in STOP mode)
- DIV register continues in HALT mode
- Minimal power impact when disabled

**Real-Time Clock:**
- Independent power supply (battery backed)
- Continues operation in all CPU power states
- Ultra-low power consumption: <1μA

**PPU/APU Components:**
- Significant power consumers when active
- Can be disabled for power savings
- Gradual enable/disable to prevent glitches

### 5.5 Performance Benchmarking

#### 5.5.1 Standard Benchmarks

**Memory Access Benchmark:**
```assembly
; Test memory access speed across regions
memory_benchmark:
    ld b, 1000              ; 2 cycles: opcode fetch
    ld c, 0x80000000        ; 6 cycles: opcode + 32-bit immediate
    
benchmark_loop:
    ldp al, [c]             ; 3 cycles: opcode + 8-bit read
    stp [c], al             ; 3 cycles: opcode + 8-bit write  
    add c, 4                ; 2 cycles: register operation
    dec b                   ; 2 cycles: register operation
    jpb zc, benchmark_loop  ; 3 cycles: opcode + PC manipulation
    
    ; Total per iteration: 13 cycles
    ; 1000 iterations: ~13,000 CPU cycles + setup overhead
    ret nc
```

**Arithmetic Performance:**
```assembly
; Test arithmetic operation speeds  
arithmetic_benchmark:
    ld b, 1000              ; 2 cycles: opcode fetch
    ld a, 12345             ; 6 cycles: opcode + 32-bit immediate
    
arith_loop:
    add a, 67               ; 2 cycles: register operation only
    sub a, 3                ; 2 cycles: register operation only
    inc a                   ; 2 cycles: register operation only
    dec b                   ; 2 cycles: register operation only
    jpb zc, arith_loop      ; 3 cycles: opcode + PC manipulation
    
    ; Total per iteration: 11 cycles
    ; 1000 iterations: ~11,000 CPU cycles + setup overhead
    ret nc
```

#### 5.5.2 Real-World Performance Metrics

**Typical Game Loop Performance:**
- 60 FPS target: 69,905 CPU cycles per frame
- Input processing: ~150-300 CPU cycles (5-10 register accesses)
- Game logic: ~2,000-10,000 CPU cycles (varies by complexity)
- Graphics updates: ~5,000-20,000 CPU cycles (VRAM access dependent)
- Audio processing: ~1,000-5,000 CPU cycles (register writes)
- Remaining cycles: 40,000-60,000 available for additional features

**Interrupt Service Overhead:**
- Interrupt entry: 7 cycles (PC push + PC manipulation)
- Context save/restore: ~20-50 cycles (depends on registers saved)
- Interrupt exit: 7 cycles (PC pop + PC manipulation)
- Total per interrupt: ~34-64 cycles minimum
- VBlank interrupt: Typically adds 100-500 cycles for game logic
- Timer interrupt: Typically adds 50-200 cycles for updates
- Total interrupt overhead: Typically <5% of CPU time

### 5.6 Optimization Strategies

#### 5.6.1 Code Optimization

**Instruction Selection:**
- Use 8-bit operations when possible (1 cycle vs 2/4 cycles)
- Register operations are fastest (2 cycles: opcode only)
- Memory operations add data access cycles (1/2/4 additional cycles)
- Avoid unnecessary memory round-trips by using registers effectively

**Loop Optimization:**
- Unroll small loops to reduce branch overhead (3 cycles per branch)
- Use countdown loops (DEC + conditional branch more efficient)
- Minimize memory accesses within loops
- Consider cycle budget: tight loops should stay under 20-30 cycles

#### 5.6.2 Memory Layout Optimization

**Data Access Patterns:**
- Sequential memory access has no additional overhead
- Data width affects access time: 8-bit (1 cycle) vs 32-bit (4 cycles)
- Group related data to minimize address calculations
- Separate read-only and read-write data
- Use packed structures only when memory is constrained

**Memory Region Strategy:**
- Quick RAM: Loop counters, temporary variables
- Working RAM: Main application data structures
- External RAM: Save game data, large lookup tables
- Executable RAM: Self-modifying code, dynamic routines

## 6. Programs

The TM32 Core is designed to run software programs written specifically for its
TM32 CPU architecture. At this point, programs can only be written using the
TM32 Assembly language, as no high-level language compilers currently exist for
the TM32 architecture. However, the assembly language is designed to be
straightforward and efficient, allowing developers to create complex programs
with relative ease.

### 6.1 Program Structure

A TM32 program's data is loaded into the first half of the TM32's 32-bit address
space, which covers the following regions:

- **Program Metadata (`0x00000000` - `0x00000FFF`)**:
    - Contains a program header with essential information about the program:
        - `0x00000000`: The identifying magic string: `TM32CORE` (8 bytes)
        - `0x00000008`: The program's major, minor and patch version, laid out
            as follows: `0xMMmmpppp` (4 bytes, where `MM` is major, `mm` is
            minor, and `pppp` is patch laid out in little-endian format).
        - `0x00000010`: The program's requested ROM size, in bytes (4 bytes,
            little-endian). Must be between `0x00004000` (16 KB) and
            `0x80000000` (2 GB, the maximum addressable ROM space), and should
            be a power of two (not required, but recommended).
        - `0x00000014`: The program's requested Work RAM (WRAM) size, in bytes
            (4 bytes, little-endian). Must be between `0x00001000` (4 KB) and
            `0x20000000` (512 MB, the maximum addressable WRAM space), and
            should be a power of two (not required, but recommended).
        - `0x00000018`: The program's requested External RAM/Save RAM (SRAM)
            size, in bytes (4 bytes, little-endian). Must be between
            `0x00000000` (0 KB) and `0x20000000` (512 MB, the maximum
            addressable External RAM space), and should be a power of two
            (not required, but recommended). This memory region is used to
            store persistent data, such as game saves or user settings. If the
            program does not require persistent storage, this value should be
            set to `0x00000000`.
        - `0x0000001C`: The program's requested Executable RAM size, in bytes
            (4 bytes, little-endian). Must be between `0x00000000` (0 bytes) to
            `0x30000000` (768 MB, the maximum addressable Executable RAM space),
            and should be a power of two (not required, but recommended). This
            memory region is used for programs that require writable and
            executable memory, such as Just-In-Time (JIT) compilation or
            self-modifying code. If the program does not require executable RAM,
            this value should be set to `0x00000000`.
        - `0x00000020`: The program's title (32 bytes, ASCII, null-terminated)
        - `0x00000040`: The program's author (32 bytes, ASCII, null-terminated)
        - `0x00000060`: The program's description (64 bytes, ASCII, null-terminated)
    - The remaining bytes in this region are reserved for future use and
        should be set to zero.
- **Interrupt Vector Table (`0x00001000` - `0x00002FFF`)**:
    - Contains the TM32's interrupt service routines (ISRs) for handling various
        system events. Each routine in this section is 256 bytes long, and the
        table includes the following ISRs:
        - `0x00001100`: VBlank Interrupt
        - `0x00001200`: LCD STAT Interrupt
        - `0x00001300`: Timer Interrupt
        - `0x00001400`: Serial Interrupt
        - `0x00001500`: Joypad Interrupt
        - `0x00001600`: Real-Time Clock (RTC) Interrupt
    - Additional interrupt vectors may be used by the software as needed, and
        accessed via the `INT` instruction. The TM32 Core can handle up to 32
        interrupt vectors, so the remaining vectors `0x10000000` and from
        `0x00001700` to `0x00002F00` are available for software use, but may
        also be reserved for future hardware features.
    - Interrupt vector `0x10000000` should be used as some kind of crash-handler
        or "fatal error" handler (a softlock), as needed by the software. In
        this case, `INT 0` would be akin to the Game Boy's `RST 38h` opcode,
        generally used for fatal errors.
    - Interrupt vector subroutines should end with the `RETI` instruction to
        properly return from the interrupt. Unused interrupt vectors should begin
        with a `RETI` instruction to prevent unexpected behavior.
- **Program Code and Data (`0x00003000` - `0x7FFFFFFF`)**:
    - Contains the main code and read-only data for the program. This region
        starts at `0x00003000` and extends up to the size specified in the
        program metadata. The program's entry point (the address where
        execution begins) is located at `0x00003000`.
    - This region should be organized using TMM assembly sections:
        - `.text` for executable code
        - `.rodata` for read-only data and constants
        - `.data` for initialized data (copied to RAM during startup)
        - `.bss` for uninitialized data (zero-initialized in RAM)

### 6.2 Program Lifecycle

#### 6.2.1 Program Loading and Initialization

When a TM32 Core program is loaded, the following sequence occurs:

1. **Header Validation**: The magic string `TM32CORE` is verified at offset 0x00000000
2. **Memory Allocation**: RAM regions are allocated based on metadata requests:
   - Work RAM (WRAM) allocation in range `0x80000000` - `0xFFFDFFFF`
   - External RAM (SRAM) allocation if requested
   - Stack memory initialized in range `0xFFFE0000` - `0xFFFEFFFF`
3. **Data Section Initialization**: `.data` sections are copied from ROM to RAM
4. **BSS Zero-Initialization**: `.bss` sections are zero-filled in RAM
5. **Interrupt Vector Setup**: ISR addresses are loaded into the interrupt controller
6. **Entry Point Execution**: Program execution begins at `0x00003000`

#### 6.2.2 Runtime Memory Layout

During program execution, the memory is organized as follows:

```
0x00000000 ┌─────────────────────────────────────┐
           │ Program Metadata (4KB)              │
0x00001000 ├─────────────────────────────────────┤
           │ Interrupt Vector Table (8KB)        │
0x00003000 ├─────────────────────────────────────┤
           │ Program Code (.text)                │
           │ Read-Only Data (.rodata)            │
           │ ...                                 │
0x80000000 ├─────────────────────────────────────┤
           │ RAM (WRAM, SRAM, VRAM, OAM)         │
           │ - Initialized Data (.data)          │
           │ - Uninitialized Data (.bss)         │
           │ - Heap (dynamic allocation)         │
           │ ...                                 │
0xFFFE0000 ├─────────────────────────────────────┤
           │ Stack Memory (64KB)                 │
0xFFFF0000 ├─────────────────────────────────────┤
           │ Hardware I/O Registers              │
0xFFFFFFFF └─────────────────────────────────────┘
```

### 6.3 TMM Assembly Integration

Programs for the TM32 Core are written using TMM Assembly Language. The following structure should be used for TM32 Core programs:

#### 6.3.1 Basic Program Structure

```assembly
; Program metadata section
.metadata
.org 0x0000
    .db "TM32CORE", 0x00        ; Magic string (8 bytes)
.org 0x0008
    .dd 0x00010000              ; Version 1.0.0 (4 bytes)
    .dd 0x00000000              ; Reserved (4 bytes)
.org 0x0010
    .dd program_rom_size        ; ROM size request (4 bytes)
.org 0x0014
    .dd program_wram_size       ; WRAM size request (4 bytes)
.org 0x0018
    .dd program_sram_size       ; SRAM size request (4 bytes)
    .dd 0x00000000              ; Reserved (4 bytes)
.org 0x0020
    .db "Program Title", 0x00   ; Title (32 bytes, null-terminated)
.org 0x0040
    .db "Author Name", 0x00     ; Author (32 bytes, null-terminated)
.org 0x0060
    .db "Program Description"   ; Description (64 bytes, null-terminated)
    .db 0x00

; Interrupt Service Routines
.int 0
crash_handler:
    ; Fatal error handler - perform cleanup and halt
    di                          ; Disable interrupts
    ld a, 0x00                  ; Turn off display
    stp [hLCDC], al
    crash_loop:
        halt                    ; Wait indefinitely
        jpb nc, crash_loop
    
.int 1
vblank_isr:
    ; VBlank interrupt - called every frame
    push a
    ; VBlank processing code here
    pop a
    reti

.int 2
lcd_stat_isr:
    ; LCD STAT interrupt - called on LCD status events
    push a
    ; LCD STAT processing code here
    pop a
    reti

.int 3
timer_isr:
    ; Timer interrupt - called on timer events
    push a
    ; Timer processing code here
    pop a
    reti

.int 4
serial_isr:
    ; Serial interrupt - called on serial events
    push a
    ; Serial processing code here
    pop a
    reti

.int 5
joypad_isr:
    ; Joypad interrupt - called on button events
    push a
    ; Joypad processing code here
    pop a
    reti

.int 6
rtc_isr:
    ; RTC interrupt - called on real-time clock events
    push a
    ; RTC processing code here
    pop a
    reti

.int 7
user_isr_7:
    ; User-defined interrupt vector 7
    ret                         ; Note that interrupt vectors called using the
                                ; INT instruction ignore the CPU's interrupt-
                                ; related registers and do not undergo the
                                ; usual process of acknowledging interrutpts.
                                ;
                                ; As such, the RET instruction can safely be
                                ; used here instead of RETI.

; Main program code
.text
main:
    ; Program entry point at 0x00003000
    call nc, system_init        ; Initialize hardware systems
    call nc, game_init          ; Initialize game-specific data
    
main_loop:
    halt                        ; Wait for interrupt
    call nc, update_game        ; Update game logic
    jmp nc, main_loop          ; Continue main loop

; System initialization
system_init:
    ; Initialize hardware components
    call nc, ppu_init          ; Initialize PPU
    call nc, apu_init          ; Initialize APU
    call nc, timer_init        ; Initialize Timer
    call nc, joypad_init       ; Initialize Joypad
    call nc, serial_init       ; Initialize Serial
    call nc, rtc_init          ; Initialize RTC
    int 7                      ; Call interrupt vector 7
    ret nc

; Read-only data section
.rodata
sprite_data:
    .db 0x7E, 0x7E, 0x42, 0x42  ; Example sprite data
    .db 0x42, 0x42, 0x7E, 0x7E

color_palette:
    .dw 0x001F, 0x03E0, 0x7C00, 0x7FFF  ; Example color palette

; Initialized data section
.data
player_x:
    .dd 80                      ; Player X position
player_y:
    .dd 72                      ; Player Y position
score:
    .dd 0                       ; Game score

; Uninitialized data section
.bss
temp_buffer:
    .space 256                  ; 256-byte temporary buffer
game_state:
    .space 4                    ; Game state variable

; Constants
.const program_rom_size  = 0x00010000   ; 64KB ROM
.const program_wram_size = 0x00008000   ; 32KB WRAM
.const program_sram_size = 0x00002000   ; 8KB SRAM
```

#### 6.3.2 Hardware Component Initialization

Each hardware component should be properly initialized during program startup:

**PPU Initialization Example:**
```assembly
ppu_init:
    ; Turn off display during setup
    ld al, 0x00
    stp [hLCDC], al
    
    ; Set graphics mode
    ld al, 0x00                 ; DMG mode
    stp [hGRPM], al
    
    ; Configure LCD control
    ld al, 0x91                 ; Enable display, BG, sprites
    stp [hLCDC], al
    
    ; Enable VBlank interrupt
    ldp al, [hIE0]
    or al, 0x01                 ; Set VBlank enable bit
    stp [hIE0], al
    
    ret nc
```

**APU Initialization Example:**
```assembly
apu_init:
    ; Initialize audio system
    ld al, 0x80                 ; Enable APU
    stp [hNR52], al
    
    ; Set master volume
    ld al, 0x77                 ; Max volume both channels
    stp [hNR50], al
    
    ; Enable all channels
    ld al, 0xFF                 ; All channels left and right
    stp [hNR51], al
    
    ret nc
```

**Timer Initialization Example:**
```assembly
timer_init:
    ; Configure timer for 60Hz interrupts
    ld a, 0x00FF0000           ; Timer period for 60Hz
    stp [hTM0], a
    
    ; Enable timer and interrupt
    ld al, 0x85                ; Enable, interrupt, 1Hz base
    stp [hTMCR], al
    
    ; Enable timer interrupt
    ldp al, [hIE0]
    or al, 0x08                ; Set timer enable bit
    stp [hIE0], al
    
    ret nc
```

### 6.4 Programming Patterns and Best Practices

#### 6.4.1 Interrupt Service Routine Guidelines

**ISR Template:**
```assembly
.int N
my_interrupt_handler:
    ; Save registers that will be modified
    push a
    push b
    push c
    
    ; Perform interrupt-specific processing
    ; Keep processing time minimal
    
    ; Restore registers
    pop c
    pop b
    pop a
    
    ; Return from interrupt
    reti
```

**Best Practices:**
- Keep ISR execution time minimal
- Save and restore all modified registers
- Use `RETI` instruction to return from interrupts
- Avoid complex operations in ISRs
- Use flags to communicate with main program

#### 6.4.2 Memory Management

**Stack Usage:**
```assembly
; Function that uses local variables
my_function:
    ; Allocate 16 bytes local space on stack
    ld a, 0                     ; Dummy value for allocation
    push a                      ; Allocate 4 bytes
    push a                      ; Allocate 4 bytes
    push a                      ; Allocate 4 bytes
    push a                      ; Allocate 4 bytes
    
    ; Use [sp], [sp+4], [sp+8], [sp+12] for local variables
    
    ; Restore stack
    pop a                       ; Deallocate 4 bytes
    pop a                       ; Deallocate 4 bytes
    pop a                       ; Deallocate 4 bytes
    pop a                       ; Deallocate 4 bytes
    ret nc
```

**Data Organization:**
```assembly
.data
; Group related data together
player_data:
    player_x: .dd 0
    player_y: .dd 0
    player_health: .dd 100
    player_score: .dd 0

; Use meaningful names and comments
enemy_count: .dd 0              ; Number of active enemies
level_number: .dd 1             ; Current level (1-based)
```

#### 6.4.3 Hardware Component Coordination

**VBlank Synchronization:**
```assembly
vblank_isr:
    push a
    
    ; Set VBlank flag for main loop
    ld al, 1
    stp [vblank_flag], al
    
    ; Perform VBlank-safe operations
    call nc, update_sprites     ; Update OAM during VBlank
    call nc, update_palettes    ; Update color palettes
    
    pop a
    reti

main_loop:
    ; Wait for VBlank
wait_vblank:
    ldp al, [vblank_flag]
    cmp al, 0
    jpb zs, wait_vblank
    
    ; Clear VBlank flag
    ld al, 0
    stp [vblank_flag], al
    
    ; Perform frame logic
    call nc, update_game_logic
    jmp nc, main_loop

.bss
vblank_flag: .space 1
```

**Multi-Component Audio-Visual Effects:**
```assembly
; Coordinate timer, PPU, and APU for synchronized effects
sync_flash_effect:
    ; Set up timer for effect duration
    ld a, 0x00010000           ; 1 second duration
    stp [hTM1], a
    
    ; Trigger sound effect
    ld al, 0x86                ; Trigger pulse channel
    stp [hNR14], al
    
    ; Flash screen by modifying palette
    ld al, 0xFF                ; White palette
    stp [hBGP], al
    
    ; Enable timer interrupt to restore normal state
    ldp al, [hIE0]
    or al, 0x10                ; Enable timer 1 interrupt
    stp [hIE0], al
    
    ret nc
```

### 6.5 Development Workflow

#### 6.5.1 Build Process

**Standard Build Steps:**
1. **Source Organization**: Organize source files into logical modules
2. **Assembly**: Use TMM assembler to create object files (.tmo)
3. **Linking**: Link object files to create final program binary
4. **Validation**: Verify program header and memory requirements
5. **Testing**: Test on TM32 Core emulator or hardware

**Example Build Script:**
```bash
#!/bin/bash
# Build script for TM32 Core programs

# Assemble source files
tm32asm -o main.tmo main.tmm
tm32asm -o graphics.tmo graphics.tmm
tm32asm -o audio.tmo audio.tmm

# Link object files
tm32link -o program.tm32 main.tmo graphics.tmo audio.tmo

echo "Build complete: program.tm32"
```

For more information on the TMM assembler and linker, and their command-line
options, please refer to the [TM32 Assembler Tool Specification](docs/tm32assembler.specification.md)
and the [TM32 Linker Tool Specification](docs/tm32linker.specification.md).

#### 6.5.2 Debugging Strategies

**Debug Output via Serial:**
```assembly
; Simple debug print function
debug_print:
    ; Wait for serial ready
debug_wait:
    ldp al, [hSC]
    and al, 0x01               ; Check busy bit
    jpb zc, debug_wait
    
    ; Send debug character
    stp [hSB], al
    
    ; Start transfer
    ld al, 0x81                ; Start transfer, internal clock
    stp [hSC], al
    
    ret nc

; Debug print hex value
debug_print_hex:
    ; Convert and print 32-bit value in A register
    push a
    push b
    push c
    
    ; Extract and print each hex digit
    ; Implementation details...
    
    pop c
    pop b
    pop a
    ret nc
```

**Performance Profiling:**
```assembly
; Use timer for performance measurement
profile_start:
    ldp a, [hTM0]              ; Read current timer value
    stp [profile_start_time], a
    ret nc

profile_end:
    ldp a, [hTM0]              ; Read current timer value
    ldp b, [profile_start_time]
    sub a, b                   ; Calculate elapsed time
    stp [profile_duration], a
    ret nc

.bss
profile_start_time: .space 4
profile_duration: .space 4
```

### 6.6 Advanced Programming Techniques

#### 6.6.1 DMA and HDMA Usage

**Efficient Sprite Updates:**
```assembly
update_sprites_dma:
    ; Set up DMA transfer to OAM
    ld a, sprite_buffer        ; Source address
    stp [hODMAS0], al          ; Source byte 0
    
    ; Shift right 8 times to get byte 1
    ld c, 8
shift1_loop:
    srl a
    dec c
    jpb nz, shift1_loop
    stp [hODMAS1], al          ; Source byte 1
    
    ; Shift right another 8 times to get byte 2
    ld c, 8
shift2_loop:
    srl a
    dec c
    jpb nz, shift2_loop
    stp [hODMAS2], al          ; Source byte 2
    
    ; Shift right another 8 times to get byte 3
    ld c, 8
shift3_loop:
    srl a
    dec c
    jpb nz, shift3_loop
    stp [hODMAS3], al          ; Source byte 3
    
    ; Start DMA transfer (160 bytes)
    ld al, 160
    stp [hODMA], al
    
    ret nc

.bss
sprite_buffer: .space 160      ; OAM shadow buffer
```

**Background Tile Streaming:**
```assembly
setup_hdma_bg_stream:
    ; Set up HDMA for streaming background tiles
    
    ; Source address byte extraction
    ld a, tile_stream_buffer   ; Source address
    stp [hHDMAS0], al          ; Source byte 0
    
    ld c, 8
src_shift1:
    srl a
    dec c
    jpb nz, src_shift1
    stp [hHDMAS1], al          ; Source byte 1
    
    ld c, 8
src_shift2:
    srl a
    dec c
    jpb nz, src_shift2
    stp [hHDMAS2], al          ; Source byte 2
    
    ld c, 8
src_shift3:
    srl a
    dec c
    jpb nz, src_shift3
    stp [hHDMAS3], al          ; Source byte 3
    
    ; Destination address byte extraction
    ld a, 0x00008000           ; VRAM tile data
    stp [hHDMAD0], al          ; Destination byte 0
    
    ld c, 8
dst_shift1:
    srl a
    dec c
    jpb nz, dst_shift1
    stp [hHDMAD1], al          ; Destination byte 1
    
    ld c, 8
dst_shift2:
    srl a
    dec c
    jpb nz, dst_shift2
    stp [hHDMAD2], al          ; Destination byte 2
    
    ld c, 8
dst_shift3:
    srl a
    dec c
    jpb nz, dst_shift3
    stp [hHDMAD3], al          ; Destination byte 3
    
    ; Configure HDMA: 16 bytes per HBlank
    ld al, 0x8F                ; Enable HDMA, 16 bytes
    stp [hHDMA], al
    
    ret nc
```

#### 6.6.2 Real-Time Clock Integration

**Time-Based Game Events:**
```assembly
check_time_events:
    ; Read current time
    ldp al, [hRTCH]            ; Hours
    ldp bl, [hRTCM]            ; Minutes
    ldp cl, [hRTCS]            ; Seconds
    
    ; Check for specific time-based events
    cmp al, 12                 ; Check if noon
    jpb z, trigger_noon_event
    
    cmp al, 0                  ; Check if midnight
    jpb z, trigger_midnight_event
    
    ret nc

trigger_noon_event:
    ; Trigger noon-specific game event
    call nc, noon_dialog
    ret nc

trigger_midnight_event:
    ; Trigger midnight-specific game event
    call nc, midnight_bonus
    ret nc
```

### 6.7 Error Handling and Validation

#### 6.7.1 Runtime Error Detection

**Memory Bounds Checking:**
```assembly
safe_memory_access:
    ; Check if address is in valid range
    cmp a, 0x80000000          ; Check if >= WRAM start
    jpb cs, invalid_address    ; Jump if address too low (Carry Set)
    
    cmp a, 0xFFFDFFFF          ; Check if <= WRAM end
    jpb zc, invalid_address    ; Jump if address too high
    
    ; Address is valid, proceed with access
    ret nc

invalid_address:
    ; Handle invalid memory access
    int 0                      ; Trigger crash handler
```

**Stack Overflow Detection:**
```assembly
check_stack_overflow:
    ; Note: TM32 does not provide direct access to the stack pointer
    ; Stack overflow detection must use alternative methods
    
    ; Alternative: Maintain a software stack counter
    ldp a, [software_sp_counter]
    cmp a, 0xFFFE0000          ; Check software counter against lower bound
    jpb cs, stack_overflow     ; Jump if counter < lower bound
    
    cmp a, 0xFFFEFFFF          ; Check software counter against upper bound  
    jpb cc, stack_overflow     ; Jump if counter > upper bound
    
    ret nc

stack_overflow:
    ; Handle stack overflow
    int 0                      ; Trigger crash handler

; Software stack counter (must be maintained by program)
.bss
software_sp_counter: .space 4
```

#### 6.7.2 Program Validation

Programs should implement self-validation routines to ensure data integrity:

```assembly
validate_program_data:
    ; Validate critical game data
    ldp a, [player_health]
    cmp a, 0
    jpb cs, invalid_health     ; Health cannot be negative (Carry Set)
    
    cmp a, 100
    jpb zc, invalid_health     ; Health cannot exceed maximum
    
    ; Validate other critical data...
    ret nc

invalid_health:
    ; Reset to valid state
    ld a, 100
    stp [player_health], a
    ret nc
```

## 6. Conclusion

The TM32 Core provides a robust and flexible platform for developing advanced
programs with a Game Boy-like feel, while introducing modern features and
capabilities. By leveraging the TM32 CPU architecture, TMM assembly language,
and the comprehensive hardware components, developers can create rich and
engaging applications. The detailed documentation and examples provided in this
manual serve as a foundation for understanding and utilizing the TM32 Core to
its fullest potential. As the TM32 ecosystem continues to evolve, developers are
encouraged to explore new techniques and share their experiences to foster a
vibrant community around this exciting platform.
