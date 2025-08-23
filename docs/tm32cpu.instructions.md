# TM32 CPU Complete Instruction Reference

Below is the complete reference for all instructions and variants available in
the TM32 CPU's instruction set.

For additional information on the TM32's instruction set, including information
on opcode structure, execution conditions, length, cycles, flag affects and other
important notes and details, please refer to the **5. Instructions** chapter of 
the [TM32 CPU Specification](tm32cpu.specification.md).

## Instructions

| Opcode    | Mnemonic          | Length/Cycles | Flags     | Description                                                           |
|-----------|-------------------|---------------|-----------|-----------------------------------------------------------------------|
| `0x0000`  | `NOP`             | 2/2           |           | No operation. Does nothing.                                           |
| `0x0100`  | `STOP`            | 2/2           |           | Enters ultra-low power standby mode. Sets the Stop flag.             |
| `0x0200`  | `HALT`            | 2/2           |           | Enters low power mode. Sets the Halt flag.                           |
| `0x03XX`  | `SEC XX`          | 2/2           |           | Sets the `EC` register to byte `XX`.                                  |
| `0x0400`  | `CEC`             | 2/2           |           | Sets the `EC` register to zero.                                       |
| `0x0500`  | `DI`              | 2/2           |           | Clears `IME`, disabling interrupts.                                   |
| `0x0600`  | `EI`              | 2/2           |           | Sets `IME` at end of frame, enabling interrupts.                      |
| `0x0700`  | `DAA`             | 2/2           | `?-0*-?`  | Decimal-adjusts byte register `AL`.                                   |
| `0x0800`  | `SCF`             | 2/2           | `-001--`  | Sets the Carry flag in the Flags register.                            |
| `0x0900`  | `CCF`             | 2/2           | `-00*--`  | Complements (toggles) the carry flag in the Flags register.           |
| `0x0AX0`  | `FLG X`           | 2/2           | `*-----`  | Checks bit (16 + `X`) of the Flags register.                          |
| `0x0BX0`  | `STF X`           | 2/2           |           | Sets bit (16 + `X`) of the Flags register.                            |
| `0x0CX0`  | `CLF X`           | 2/2           |           | Clears bit (16 + `X`) of the Flags register.                          |
| `0x1000`  | `LD A, IMM32`     | 6/6           |           | Loads immediate 32-bit value into 32-bit register `A`.                |
| `0x1010`  | `LD AW, IMM16`    | 4/4           |           | Loads immediate 16-bit value into 16-bit register `AW`.               |
| `0x1020`  | `LD AH, IMM8`     | 3/3           |           | Loads immediate 8-bit value into 8-bit register `AH`.                 |
| `0x1030`  | `LD AL, IMM8`     | 3/3           |           | Loads immediate 8-bit value into 8-bit register `AL`.                 |
| `0x1040`  | `LD B, IMM32`     | 6/6           |           | Loads immediate 32-bit value into 32-bit register `B`.                |
| `0x1050`  | `LD BW, IMM16`    | 4/4           |           | Loads immediate 16-bit value into 16-bit register `BW`.               |
| `0x1060`  | `LD BH, IMM8`     | 3/3           |           | Loads immediate 8-bit value into 8-bit register `BH`.                 |
| `0x1070`  | `LD BL, IMM8`     | 3/3           |           | Loads immediate 8-bit value into 8-bit register `BL`.                 |
| `0x1080`  | `LD C, IMM32`     | 6/6           |           | Loads immediate 32-bit value into 32-bit register `C`.                |
| `0x1090`  | `LD CW, IMM16`    | 4/4           |           | Loads immediate 16-bit value into 16-bit register `CW`.               |
| `0x10A0`  | `LD CH, IMM8`     | 3/3           |           | Loads immediate 8-bit value into 8-bit register `CH`.                 |
| `0x10B0`  | `LD CL, IMM8`     | 3/3           |           | Loads immediate 8-bit value into 8-bit register `CL`.                 |
| `0x10C0`  | `LD D, IMM32`     | 6/6           |           | Loads immediate 32-bit value into 32-bit register `D`.                |
| `0x10D0`  | `LD DW, IMM16`    | 4/4           |           | Loads immediate 16-bit value into 16-bit register `DW`.               |
| `0x10E0`  | `LD DH, IMM8`     | 3/3           |           | Loads immediate 8-bit value into 8-bit register `DH`.                 |
| `0x10F0`  | `LD DL, IMM8`     | 3/3           |           | Loads immediate 8-bit value into 8-bit register `DL`.                 |
| `0x1100`  | `LD A, [ADDR32]`  | 6/10          |           | Loads 32-bit value from memory address into 32-bit register `A`.      |
| `0x1110`  | `LD AW, [ADDR32]` | 6/8           |           | Loads 16-bit value from memory address into 16-bit register `AW`.     |
| `0x1120`  | `LD AH, [ADDR32]` | 6/7           |           | Loads 8-bit value from memory address into 8-bit register `AH`.       |
| `0x1130`  | `LD AL, [ADDR32]` | 6/7           |           | Loads 8-bit value from memory address into 8-bit register `AL`.       |
| `0x1140`  | `LD B, [ADDR32]`  | 6/10          |           | Loads 32-bit value from memory address into 32-bit register `B`.      |
| `0x1150`  | `LD BW, [ADDR32]` | 6/8           |           | Loads 16-bit value from memory address into 16-bit register `BW`.     |
| `0x1160`  | `LD BH, [ADDR32]` | 6/7           |           | Loads 8-bit value from memory address into 8-bit register `BH`.       |
| `0x1170`  | `LD BL, [ADDR32]` | 6/7           |           | Loads 8-bit value from memory address into 8-bit register `BL`.       |
| `0x1180`  | `LD C, [ADDR32]`  | 6/10          |           | Loads 32-bit value from memory address into 32-bit register `C`.      |
| `0x1190`  | `LD CW, [ADDR32]` | 6/8           |           | Loads 16-bit value from memory address into 16-bit register `CW`.     |
| `0x11A0`  | `LD CH, [ADDR32]` | 6/7           |           | Loads 8-bit value from memory address into 8-bit register `CH`.       |
| `0x11B0`  | `LD CL, [ADDR32]` | 6/7           |           | Loads 8-bit value from memory address into 8-bit register `CL`.       |
| `0x11C0`  | `LD D, [ADDR32]`  | 6/10          |           | Loads 32-bit value from memory address into 32-bit register `D`.      |
| `0x11D0`  | `LD DW, [ADDR32]` | 6/8           |           | Loads 16-bit value from memory address into 16-bit register `DW`.     |
| `0x11E0`  | `LD DH, [ADDR32]` | 6/7           |           | Loads 8-bit value from memory address into 8-bit register `DH`.       |
| `0x11F0`  | `LD DL, [ADDR32]` | 6/7           |           | Loads 8-bit value from memory address into 8-bit register `DL`.       |
| `0x1200`  | `LD A, [A]`       | 2/6           |           | Loads 32-bit value from address in `A` into `A`.                      |
| `0x1210`  | `LD AW, [A]`      | 2/4           |           | Loads 16-bit value from address in `A` into `AW`.                     |
| `0x1220`  | `LD AH, [A]`      | 2/3           |           | Loads 8-bit value from address in `A` into `AH`.                      |
| `0x1230`  | `LD AL, [A]`      | 2/3           |           | Loads 8-bit value from address in `A` into `AL`.                      |
| `0x1240`  | `LD B, [A]`       | 2/6           |           | Loads 32-bit value from address in `A` into `B`.                      |
| `0x1250`  | `LD BW, [A]`      | 2/4           |           | Loads 16-bit value from address in `A` into `BW`.                     |
| `0x1260`  | `LD BH, [A]`      | 2/3           |           | Loads 8-bit value from address in `A` into `BH`.                      |
| `0x1270`  | `LD BL, [A]`      | 2/3           |           | Loads 8-bit value from address in `A` into `BL`.                      |
| `0x1280`  | `LD C, [A]`       | 2/6           |           | Loads 32-bit value from address in `A` into `C`.                      |
| `0x1290`  | `LD CW, [A]`      | 2/4           |           | Loads 16-bit value from address in `A` into `CW`.                     |
| `0x12A0`  | `LD CH, [A]`      | 2/3           |           | Loads 8-bit value from address in `A` into `CH`.                      |
| `0x12B0`  | `LD CL, [A]`      | 2/3           |           | Loads 8-bit value from address in `A` into `CL`.                      |
| `0x12C0`  | `LD D, [A]`       | 2/6           |           | Loads 32-bit value from address in `A` into `D`.                      |
| `0x12D0`  | `LD DW, [A]`      | 2/4           |           | Loads 16-bit value from address in `A` into `DW`.                     |
| `0x12E0`  | `LD DH, [A]`      | 2/3           |           | Loads 8-bit value from address in `A` into `DH`.                      |
| `0x12F0`  | `LD DL, [A]`      | 2/3           |           | Loads 8-bit value from address in `A` into `DL`.                      |
| `0x1204`  | `LD A, [B]`       | 2/6           |           | Loads 32-bit value from address in `B` into `A`.                      |
| `0x1214`  | `LD AW, [B]`      | 2/4           |           | Loads 16-bit value from address in `B` into `AW`.                     |
| `0x1224`  | `LD AH, [B]`      | 2/3           |           | Loads 8-bit value from address in `B` into `AH`.                      |
| `0x1234`  | `LD AL, [B]`      | 2/3           |           | Loads 8-bit value from address in `B` into `AL`.                      |
| `0x1244`  | `LD B, [B]`       | 2/6           |           | Loads 32-bit value from address in `B` into `B`.                      |
| `0x1254`  | `LD BW, [B]`      | 2/4           |           | Loads 16-bit value from address in `B` into `BW`.                     |
| `0x1264`  | `LD BH, [B]`      | 2/3           |           | Loads 8-bit value from address in `B` into `BH`.                      |
| `0x1274`  | `LD BL, [B]`      | 2/3           |           | Loads 8-bit value from address in `B` into `BL`.                      |
| `0x1284`  | `LD C, [B]`       | 2/6           |           | Loads 32-bit value from address in `B` into `C`.                      |
| `0x1294`  | `LD CW, [B]`      | 2/4           |           | Loads 16-bit value from address in `B` into `CW`.                     |
| `0x12A4`  | `LD CH, [B]`      | 2/3           |           | Loads 8-bit value from address in `B` into `CH`.                      |
| `0x12B4`  | `LD CL, [B]`      | 2/3           |           | Loads 8-bit value from address in `B` into `CL`.                      |
| `0x12C4`  | `LD D, [B]`       | 2/6           |           | Loads 32-bit value from address in `B` into `D`.                      |
| `0x12D4`  | `LD DW, [B]`      | 2/4           |           | Loads 16-bit value from address in `B` into `DW`.                     |
| `0x12E4`  | `LD DH, [B]`      | 2/3           |           | Loads 8-bit value from address in `B` into `DH`.                      |
| `0x12F4`  | `LD DL, [B]`      | 2/3           |           | Loads 8-bit value from address in `B` into `DL`.                      |
| `0x1208`  | `LD A, [C]`       | 2/6           |           | Loads 32-bit value from address in `C` into `A`.                      |
| `0x1218`  | `LD AW, [C]`      | 2/4           |           | Loads 16-bit value from address in `C` into `AW`.                     |
| `0x1228`  | `LD AH, [C]`      | 2/3           |           | Loads 8-bit value from address in `C` into `AH`.                      |
| `0x1238`  | `LD AL, [C]`      | 2/3           |           | Loads 8-bit value from address in `C` into `AL`.                      |
| `0x1248`  | `LD B, [C]`       | 2/6           |           | Loads 32-bit value from address in `C` into `B`.                      |
| `0x1258`  | `LD BW, [C]`      | 2/4           |           | Loads 16-bit value from address in `C` into `BW`.                     |
| `0x1268`  | `LD BH, [C]`      | 2/3           |           | Loads 8-bit value from address in `C` into `BH`.                      |
| `0x1278`  | `LD BL, [C]`      | 2/3           |           | Loads 8-bit value from address in `C` into `BL`.                      |
| `0x1288`  | `LD C, [C]`       | 2/6           |           | Loads 32-bit value from address in `C` into `C`.                      |
| `0x1298`  | `LD CW, [C]`      | 2/4           |           | Loads 16-bit value from address in `C` into `CW`.                     |
| `0x12A8`  | `LD CH, [C]`      | 2/3           |           | Loads 8-bit value from address in `C` into `CH`.                      |
| `0x12B8`  | `LD CL, [C]`      | 2/3           |           | Loads 8-bit value from address in `C` into `CL`.                      |
| `0x12C8`  | `LD D, [C]`       | 2/6           |           | Loads 32-bit value from address in `C` into `D`.                      |
| `0x12D8`  | `LD DW, [C]`      | 2/4           |           | Loads 16-bit value from address in `C` into `DW`.                     |
| `0x12E8`  | `LD DH, [C]`      | 2/3           |           | Loads 8-bit value from address in `C` into `DH`.                      |
| `0x12F8`  | `LD DL, [C]`      | 2/3           |           | Loads 8-bit value from address in `C` into `DL`.                      |
| `0x120C`  | `LD A, [D]`       | 2/6           |           | Loads 32-bit value from address in `D` into `A`.                      |
| `0x121C`  | `LD AW, [D]`      | 2/4           |           | Loads 16-bit value from address in `D` into `AW`.                     |
| `0x122C`  | `LD AH, [D]`      | 2/3           |           | Loads 8-bit value from address in `D` into `AH`.                      |
| `0x123C`  | `LD AL, [D]`      | 2/3           |           | Loads 8-bit value from address in `D` into `AL`.                      |
| `0x124C`  | `LD B, [D]`       | 2/6           |           | Loads 32-bit value from address in `D` into `B`.                      |
| `0x125C`  | `LD BW, [D]`      | 2/4           |           | Loads 16-bit value from address in `D` into `BW`.                     |
| `0x126C`  | `LD BH, [D]`      | 2/3           |           | Loads 8-bit value from address in `D` into `BH`.                      |
| `0x127C`  | `LD BL, [D]`      | 2/3           |           | Loads 8-bit value from address in `D` into `BL`.                      |
| `0x128C`  | `LD C, [D]`       | 2/6           |           | Loads 32-bit value from address in `D` into `C`.                      |
| `0x129C`  | `LD CW, [D]`      | 2/4           |           | Loads 16-bit value from address in `D` into `CW`.                     |
| `0x12AC`  | `LD CH, [D]`      | 2/3           |           | Loads 8-bit value from address in `D` into `CH`.                      |
| `0x12BC`  | `LD CL, [D]`      | 2/3           |           | Loads 8-bit value from address in `D` into `CL`.                      |
| `0x12CC`  | `LD D, [D]`       | 2/6           |           | Loads 32-bit value from address in `D` into `D`.                      |
| `0x12DC`  | `LD DW, [D]`      | 2/4           |           | Loads 16-bit value from address in `D` into `DW`.                     |
| `0x12EC`  | `LD DH, [D]`      | 2/3           |           | Loads 8-bit value from address in `D` into `DH`.                      |
| `0x12FC`  | `LD DL, [D]`      | 2/3           |           | Loads 8-bit value from address in `D` into `DL`.                      |
| `0x1300`  | `LDH A, [ADDR16]` | 4/8           |           | Loads 32-bit value from 16-bit memory address into `A`.               |
| `0x1310`  | `LDH AW, [ADDR16]`| 4/6           |           | Loads 16-bit value from 16-bit memory address into `AW`.              |
| `0x1320`  | `LDH AH, [ADDR16]`| 4/5           |           | Loads 8-bit value from 16-bit memory address into `AH`.               |
| `0x1330`  | `LDH AL, [ADDR16]`| 4/5           |           | Loads 8-bit value from 16-bit memory address into `AL`.               |
| `0x1340`  | `LDH B, [ADDR16]` | 4/8           |           | Loads 32-bit value from 16-bit memory address into `B`.               |
| `0x1350`  | `LDH BW, [ADDR16]`| 4/6           |           | Loads 16-bit value from 16-bit memory address into `BW`.              |
| `0x1360`  | `LDH BH, [ADDR16]`| 4/5           |           | Loads 8-bit value from 16-bit memory address into `BH`.               |
| `0x1370`  | `LDH BL, [ADDR16]`| 4/5           |           | Loads 8-bit value from 16-bit memory address into `BL`.               |
| `0x1380`  | `LDH C, [ADDR16]` | 4/8           |           | Loads 32-bit value from 16-bit memory address into `C`.               |
| `0x1390`  | `LDH CW, [ADDR16]`| 4/6           |           | Loads 16-bit value from 16-bit memory address into `CW`.              |
| `0x13A0`  | `LDH CH, [ADDR16]`| 4/5           |           | Loads 8-bit value from 16-bit memory address into `CH`.               |
| `0x13B0`  | `LDH CL, [ADDR16]`| 4/5           |           | Loads 8-bit value from 16-bit memory address into `CL`.               |
| `0x13C0`  | `LDH D, [ADDR16]` | 4/8           |           | Loads 32-bit value from 16-bit memory address into `D`.               |
| `0x13D0`  | `LDH DW, [ADDR16]`| 4/6           |           | Loads 16-bit value from 16-bit memory address into `DW`.              |
| `0x13E0`  | `LDH DH, [ADDR16]`| 4/5           |           | Loads 8-bit value from 16-bit memory address into `DH`.               |
| `0x13F0`  | `LDH DL, [ADDR16]`| 4/5           |           | Loads 8-bit value from 16-bit memory address into `DL`.               |
| `0x1401`  | `LDH A, [AW]`     | 2/6           |           | Loads 32-bit value from address in `AW` into `A`.                     |
| `0x1411`  | `LDH AW, [AW]`    | 2/4           |           | Loads 16-bit value from address in `AW` into `AW`.                    |
| `0x1421`  | `LDH AH, [AW]`    | 2/3           |           | Loads 8-bit value from address in `AW` into `AH`.                     |
| `0x1431`  | `LDH AL, [AW]`    | 2/3           |           | Loads 8-bit value from address in `AW` into `AL`.                     |
| `0x1441`  | `LDH B, [AW]`     | 2/6           |           | Loads 32-bit value from address in `AW` into `B`.                     |
| `0x1451`  | `LDH BW, [AW]`    | 2/4           |           | Loads 16-bit value from address in `AW` into `BW`.                    |
| `0x1461`  | `LDH BH, [AW]`    | 2/3           |           | Loads 8-bit value from address in `AW` into `BH`.                     |
| `0x1471`  | `LDH BL, [AW]`    | 2/3           |           | Loads 8-bit value from address in `AW` into `BL`.                     |
| `0x1481`  | `LDH C, [AW]`     | 2/6           |           | Loads 32-bit value from address in `AW` into `C`.                     |
| `0x1491`  | `LDH CW, [AW]`    | 2/4           |           | Loads 16-bit value from address in `AW` into `CW`.                    |
| `0x14A1`  | `LDH CH, [AW]`    | 2/3           |           | Loads 8-bit value from address in `AW` into `CH`.                     |
| `0x14B1`  | `LDH CL, [AW]`    | 2/3           |           | Loads 8-bit value from address in `AW` into `CL`.                     |
| `0x14C1`  | `LDH D, [AW]`     | 2/6           |           | Loads 32-bit value from address in `AW` into `D`.                     |
| `0x14D1`  | `LDH DW, [AW]`    | 2/4           |           | Loads 16-bit value from address in `AW` into `DW`.                    |
| `0x14E1`  | `LDH DH, [AW]`    | 2/3           |           | Loads 8-bit value from address in `AW` into `DH`.                     |
| `0x14F1`  | `LDH DL, [AW]`    | 2/3           |           | Loads 8-bit value from address in `AW` into `DL`.                     |
| `0x1405`  | `LDH A, [BW]`     | 2/6           |           | Loads 32-bit value from address in `BW` into `A`.                     |
| `0x1415`  | `LDH AW, [BW]`    | 2/4           |           | Loads 16-bit value from address in `BW` into `AW`.                    |
| `0x1425`  | `LDH AH, [BW]`    | 2/3           |           | Loads 8-bit value from address in `BW` into `AH`.                     |
| `0x1435`  | `LDH AL, [BW]`    | 2/3           |           | Loads 8-bit value from address in `BW` into `AL`.                     |
| `0x1445`  | `LDH B, [BW]`     | 2/6           |           | Loads 32-bit value from address in `BW` into `B`.                     |
| `0x1455`  | `LDH BW, [BW]`    | 2/4           |           | Loads 16-bit value from address in `BW` into `BW`.                    |
| `0x1465`  | `LDH BH, [BW]`    | 2/3           |           | Loads 8-bit value from address in `BW` into `BH`.                     |
| `0x1475`  | `LDH BL, [BW]`    | 2/3           |           | Loads 8-bit value from address in `BW` into `BL`.                     |
| `0x1485`  | `LDH C, [BW]`     | 2/6           |           | Loads 32-bit value from address in `BW` into `C`.                     |
| `0x1495`  | `LDH CW, [BW]`    | 2/4           |           | Loads 16-bit value from address in `BW` into `CW`.                    |
| `0x14A5`  | `LDH CH, [BW]`    | 2/3           |           | Loads 8-bit value from address in `BW` into `CH`.                     |
| `0x14B5`  | `LDH CL, [BW]`    | 2/3           |           | Loads 8-bit value from address in `BW` into `CL`.                     |
| `0x14C5`  | `LDH D, [BW]`     | 2/6           |           | Loads 32-bit value from address in `BW` into `D`.                     |
| `0x14D5`  | `LDH DW, [BW]`    | 2/4           |           | Loads 16-bit value from address in `BW` into `DW`.                    |
| `0x14E5`  | `LDH DH, [BW]`    | 2/3           |           | Loads 8-bit value from address in `BW` into `DH`.                     |
| `0x14F5`  | `LDH DL, [BW]`    | 2/3           |           | Loads 8-bit value from address in `BW` into `DL`.                     |
| `0x1409`  | `LDH A, [CW]`     | 2/6           |           | Loads 32-bit value from address in `CW` into `A`.                     |
| `0x1419`  | `LDH AW, [CW]`    | 2/4           |           | Loads 16-bit value from address in `CW` into `AW`.                    |
| `0x1429`  | `LDH AH, [CW]`    | 2/3           |           | Loads 8-bit value from address in `CW` into `AH`.                     |
| `0x1439`  | `LDH AL, [CW]`    | 2/3           |           | Loads 8-bit value from address in `CW` into `AL`.                     |
| `0x1449`  | `LDH B, [CW]`     | 2/6           |           | Loads 32-bit value from address in `CW` into `B`.                     |
| `0x1459`  | `LDH BW, [CW]`    | 2/4           |           | Loads 16-bit value from address in `CW` into `BW`.                    |
| `0x1469`  | `LDH BH, [CW]`    | 2/3           |           | Loads 8-bit value from address in `CW` into `BH`.                     |
| `0x1479`  | `LDH BL, [CW]`    | 2/3           |           | Loads 8-bit value from address in `CW` into `BL`.                     |
| `0x1489`  | `LDH C, [CW]`     | 2/6           |           | Loads 32-bit value from address in `CW` into `C`.                     |
| `0x1499`  | `LDH CW, [CW]`    | 2/4           |           | Loads 16-bit value from address in `CW` into `CW`.                    |
| `0x14A9`  | `LDH CH, [CW]`    | 2/3           |           | Loads 8-bit value from address in `CW` into `CH`.                     |
| `0x14B9`  | `LDH CL, [CW]`    | 2/3           |           | Loads 8-bit value from address in `CW` into `CL`.                     |
| `0x14C9`  | `LDH D, [CW]`     | 2/6           |           | Loads 32-bit value from address in `CW` into `D`.                     |
| `0x14D9`  | `LDH DW, [CW]`    | 2/4           |           | Loads 16-bit value from address in `CW` into `DW`.                    |
| `0x14E9`  | `LDH DH, [CW]`    | 2/3           |           | Loads 8-bit value from address in `CW` into `DH`.                     |
| `0x14F9`  | `LDH DL, [CW]`    | 2/3           |           | Loads 8-bit value from address in `CW` into `DL`.                     |
| `0x140D`  | `LDH A, [DW]`     | 2/6           |           | Loads 32-bit value from address in `DW` into `A`.                     |
| `0x141D`  | `LDH AW, [DW]`    | 2/4           |           | Loads 16-bit value from address in `DW` into `AW`.                    |
| `0x142D`  | `LDH AH, [DW]`    | 2/3           |           | Loads 8-bit value from address in `DW` into `AH`.                     |
| `0x143D`  | `LDH AL, [DW]`    | 2/3           |           | Loads 8-bit value from address in `DW` into `AL`.                     |
| `0x144D`  | `LDH B, [DW]`     | 2/6           |           | Loads 32-bit value from address in `DW` into `B`.                     |
| `0x145D`  | `LDH BW, [DW]`    | 2/4           |           | Loads 16-bit value from address in `DW` into `BW`.                    |
| `0x146D`  | `LDH BH, [DW]`    | 2/3           |           | Loads 8-bit value from address in `DW` into `BH`.                     |
| `0x147D`  | `LDH BL, [DW]`    | 2/3           |           | Loads 8-bit value from address in `DW` into `BL`.                     |
| `0x148D`  | `LDH C, [DW]`     | 2/6           |           | Loads 32-bit value from address in `DW` into `C`.                     |
| `0x149D`  | `LDH CW, [DW]`    | 2/4           |           | Loads 16-bit value from address in `DW` into `CW`.                    |
| `0x14AD`  | `LDH CH, [DW]`    | 2/3           |           | Loads 8-bit value from address in `DW` into `CH`.                     |
| `0x14BD`  | `LDH CL, [DW]`    | 2/3           |           | Loads 8-bit value from address in `DW` into `CL`.                     |
| `0x14CD`  | `LDH D, [DW]`     | 2/6           |           | Loads 32-bit value from address in `DW` into `D`.                     |
| `0x14DD`  | `LDH DW, [DW]`    | 2/4           |           | Loads 16-bit value from address in `DW` into `DW`.                    |
| `0x14ED`  | `LDH DH, [DW]`    | 2/3           |           | Loads 8-bit value from address in `DW` into `DH`.                     |
| `0x14FD`  | `LDH DL, [DW]`    | 2/3           |           | Loads 8-bit value from address in `DW` into `DL`.                     |
| `0x1520`  | `LDP AH, [ADDR8]` | 3/3           |           | Loads 8-bit value from 8-bit page address into `AH`.                  |
| `0x1530`  | `LDP AL, [ADDR8]` | 3/3           |           | Loads 8-bit value from 8-bit page address into `AL`.                  |
| `0x1560`  | `LDP BH, [ADDR8]` | 3/3           |           | Loads 8-bit value from 8-bit page address into `BH`.                  |
| `0x1570`  | `LDP BL, [ADDR8]` | 3/3           |           | Loads 8-bit value from 8-bit page address into `BL`.                  |
| `0x15A0`  | `LDP CH, [ADDR8]` | 3/3           |           | Loads 8-bit value from 8-bit page address into `CH`.                  |
| `0x15B0`  | `LDP CL, [ADDR8]` | 3/3           |           | Loads 8-bit value from 8-bit page address into `CL`.                  |
| `0x15E0`  | `LDP DH, [ADDR8]` | 3/3           |           | Loads 8-bit value from 8-bit page address into `DH`.                  |
| `0x15F0`  | `LDP DL, [ADDR8]` | 3/3           |           | Loads 8-bit value from 8-bit page address into `DL`.                  |
| `0x1622`  | `LDP AH, [AH]`    | 2/3           |           | Loads 8-bit value from page address in `AH` into `AH`.                |
| `0x1632`  | `LDP AL, [AH]`    | 2/3           |           | Loads 8-bit value from page address in `AH` into `AL`.                |
| `0x1662`  | `LDP BH, [AH]`    | 2/3           |           | Loads 8-bit value from page address in `AH` into `BH`.                |
| `0x1672`  | `LDP BL, [AH]`    | 2/3           |           | Loads 8-bit value from page address in `AH` into `BL`.                |
| `0x16A2`  | `LDP CH, [AH]`    | 2/3           |           | Loads 8-bit value from page address in `AH` into `CH`.                |
| `0x16B2`  | `LDP CL, [AH]`    | 2/3           |           | Loads 8-bit value from page address in `AH` into `CL`.                |
| `0x16E2`  | `LDP DH, [AH]`    | 2/3           |           | Loads 8-bit value from page address in `AH` into `DH`.                |
| `0x16F2`  | `LDP DL, [AH]`    | 2/3           |           | Loads 8-bit value from page address in `AH` into `DL`.                |
| `0x1623`  | `LDP AH, [AL]`    | 2/3           |           | Loads 8-bit value from page address in `AL` into `AH`.                |
| `0x1633`  | `LDP AL, [AL]`    | 2/3           |           | Loads 8-bit value from page address in `AL` into `AL`.                |
| `0x1663`  | `LDP BH, [AL]`    | 2/3           |           | Loads 8-bit value from page address in `AL` into `BH`.                |
| `0x1673`  | `LDP BL, [AL]`    | 2/3           |           | Loads 8-bit value from page address in `AL` into `BL`.                |
| `0x16A3`  | `LDP CH, [AL]`    | 2/3           |           | Loads 8-bit value from page address in `AL` into `CH`.                |
| `0x16B3`  | `LDP CL, [AL]`    | 2/3           |           | Loads 8-bit value from page address in `AL` into `CL`.                |
| `0x16E3`  | `LDP DH, [AL]`    | 2/3           |           | Loads 8-bit value from page address in `AL` into `DH`.                |
| `0x16F3`  | `LDP DL, [AL]`    | 2/3           |           | Loads 8-bit value from page address in `AL` into `DL`.                |
| `0x1626`  | `LDP AH, [BH]`    | 2/3           |           | Loads 8-bit value from page address in `BH` into `AH`.                |
| `0x1636`  | `LDP AL, [BH]`    | 2/3           |           | Loads 8-bit value from page address in `BH` into `AL`.                |
| `0x1666`  | `LDP BH, [BH]`    | 2/3           |           | Loads 8-bit value from page address in `BH` into `BH`.                |
| `0x1676`  | `LDP BL, [BH]`    | 2/3           |           | Loads 8-bit value from page address in `BH` into `BL`.                |
| `0x16A6`  | `LDP CH, [BH]`    | 2/3           |           | Loads 8-bit value from page address in `BH` into `CH`.                |
| `0x16B6`  | `LDP CL, [BH]`    | 2/3           |           | Loads 8-bit value from page address in `BH` into `CL`.                |
| `0x16E6`  | `LDP DH, [BH]`    | 2/3           |           | Loads 8-bit value from page address in `BH` into `DH`.                |
| `0x16F6`  | `LDP DL, [BH]`    | 2/3           |           | Loads 8-bit value from page address in `BH` into `DL`.                |
| `0x1627`  | `LDP AH, [BL]`    | 2/3           |           | Loads 8-bit value from page address in `BL` into `AH`.                |
| `0x1637`  | `LDP AL, [BL]`    | 2/3           |           | Loads 8-bit value from page address in `BL` into `AL`.                |
| `0x1667`  | `LDP BH, [BL]`    | 2/3           |           | Loads 8-bit value from page address in `BL` into `BH`.                |
| `0x1677`  | `LDP BL, [BL]`    | 2/3           |           | Loads 8-bit value from page address in `BL` into `BL`.                |
| `0x16A7`  | `LDP CH, [BL]`    | 2/3           |           | Loads 8-bit value from page address in `BL` into `CH`.                |
| `0x16B7`  | `LDP CL, [BL]`    | 2/3           |           | Loads 8-bit value from page address in `BL` into `CL`.                |
| `0x16E7`  | `LDP DH, [BL]`    | 2/3           |           | Loads 8-bit value from page address in `BL` into `DH`.                |
| `0x16F7`  | `LDP DL, [BL]`    | 2/3           |           | Loads 8-bit value from page address in `BL` into `DL`.                |
| `0x162A`  | `LDP AH, [CH]`    | 2/3           |           | Loads 8-bit value from page address in `CH` into `AH`.                |
| `0x163A`  | `LDP AL, [CH]`    | 2/3           |           | Loads 8-bit value from page address in `CH` into `AL`.                |
| `0x166A`  | `LDP BH, [CH]`    | 2/3           |           | Loads 8-bit value from page address in `CH` into `BH`.                |
| `0x167A`  | `LDP BL, [CH]`    | 2/3           |           | Loads 8-bit value from page address in `CH` into `BL`.                |
| `0x16AA`  | `LDP CH, [CH]`    | 2/3           |           | Loads 8-bit value from page address in `CH` into `CH`.                |
| `0x16BA`  | `LDP CL, [CH]`    | 2/3           |           | Loads 8-bit value from page address in `CH` into `CL`.                |
| `0x16EA`  | `LDP DH, [CH]`    | 2/3           |           | Loads 8-bit value from page address in `CH` into `DH`.                |
| `0x16FA`  | `LDP DL, [CH]`    | 2/3           |           | Loads 8-bit value from page address in `CH` into `DL`.                |
| `0x162B`  | `LDP AH, [CL]`    | 2/3           |           | Loads 8-bit value from page address in `CL` into `AH`.                |
| `0x163B`  | `LDP AL, [CL]`    | 2/3           |           | Loads 8-bit value from page address in `CL` into `AL`.                |
| `0x166B`  | `LDP BH, [CL]`    | 2/3           |           | Loads 8-bit value from page address in `CL` into `BH`.                |
| `0x167B`  | `LDP BL, [CL]`    | 2/3           |           | Loads 8-bit value from page address in `CL` into `BL`.                |
| `0x16AB`  | `LDP CH, [CL]`    | 2/3           |           | Loads 8-bit value from page address in `CL` into `CH`.                |
| `0x16BB`  | `LDP CL, [CL]`    | 2/3           |           | Loads 8-bit value from page address in `CL` into `CL`.                |
| `0x16EB`  | `LDP DH, [CL]`    | 2/3           |           | Loads 8-bit value from page address in `CL` into `DH`.                |
| `0x16FB`  | `LDP DL, [CL]`    | 2/3           |           | Loads 8-bit value from page address in `CL` into `DL`.                |
| `0x162E`  | `LDP AH, [DH]`    | 2/3           |           | Loads 8-bit value from page address in `DH` into `AH`.                |
| `0x163E`  | `LDP AL, [DH]`    | 2/3           |           | Loads 8-bit value from page address in `DH` into `AL`.                |
| `0x166E`  | `LDP BH, [DH]`    | 2/3           |           | Loads 8-bit value from page address in `DH` into `BH`.                |
| `0x167E`  | `LDP BL, [DH]`    | 2/3           |           | Loads 8-bit value from page address in `DH` into `BL`.                |
| `0x16AE`  | `LDP CH, [DH]`    | 2/3           |           | Loads 8-bit value from page address in `DH` into `CH`.                |
| `0x16BE`  | `LDP CL, [DH]`    | 2/3           |           | Loads 8-bit value from page address in `DH` into `CL`.                |
| `0x16EE`  | `LDP DH, [DH]`    | 2/3           |           | Loads 8-bit value from page address in `DH` into `DH`.                |
| `0x16FE`  | `LDP DL, [DH]`    | 2/3           |           | Loads 8-bit value from page address in `DH` into `DL`.                |
| `0x162F`  | `LDP AH, [DL]`    | 2/3           |           | Loads 8-bit value from page address in `DL` into `AH`.                |
| `0x163F`  | `LDP AL, [DL]`    | 2/3           |           | Loads 8-bit value from page address in `DL` into `AL`.                |
| `0x166F`  | `LDP BH, [DL]`    | 2/3           |           | Loads 8-bit value from page address in `DL` into `BH`.                |
| `0x167F`  | `LDP BL, [DL]`    | 2/3           |           | Loads 8-bit value from page address in `DL` into `BL`.                |
| `0x16AF`  | `LDP CH, [DL]`    | 2/3           |           | Loads 8-bit value from page address in `DL` into `CH`.                |
| `0x16BF`  | `LDP CL, [DL]`    | 2/3           |           | Loads 8-bit value from page address in `DL` into `CL`.                |
| `0x16EF`  | `LDP DH, [DL]`    | 2/3           |           | Loads 8-bit value from page address in `DL` into `DH`.                |
| `0x16FF`  | `LDP DL, [DL]`    | 2/3           |           | Loads 8-bit value from page address in `DL` into `DL`.                |
| `0x1700`  | `ST [ADDR32], A`  | 6/10          |           | Stores 32-bit register `A` at memory address.                         |
| `0x1701`  | `ST [ADDR32], AW` | 6/8           |           | Stores 16-bit register `AW` at memory address.                        |
| `0x1702`  | `ST [ADDR32], AH` | 6/7           |           | Stores 8-bit register `AH` at memory address.                         |
| `0x1703`  | `ST [ADDR32], AL` | 6/7           |           | Stores 8-bit register `AL` at memory address.                         |
| `0x1704`  | `ST [ADDR32], B`  | 6/10          |           | Stores 32-bit register `B` at memory address.                         |
| `0x1705`  | `ST [ADDR32], BW` | 6/8           |           | Stores 16-bit register `BW` at memory address.                        |
| `0x1706`  | `ST [ADDR32], BH` | 6/7           |           | Stores 8-bit register `BH` at memory address.                         |
| `0x1707`  | `ST [ADDR32], BL` | 6/7           |           | Stores 8-bit register `BL` at memory address.                         |
| `0x1708`  | `ST [ADDR32], C`  | 6/10          |           | Stores 32-bit register `C` at memory address.                         |
| `0x1709`  | `ST [ADDR32], CW` | 6/8           |           | Stores 16-bit register `CW` at memory address.                        |
| `0x170A`  | `ST [ADDR32], CH` | 6/7           |           | Stores 8-bit register `CH` at memory address.                         |
| `0x170B`  | `ST [ADDR32], CL` | 6/7           |           | Stores 8-bit register `CL` at memory address.                         |
| `0x170C`  | `ST [ADDR32], D`  | 6/10          |           | Stores 32-bit register `D` at memory address.                         |
| `0x170D`  | `ST [ADDR32], DW` | 6/8           |           | Stores 16-bit register `DW` at memory address.                        |
| `0x170E`  | `ST [ADDR32], DH` | 6/7           |           | Stores 8-bit register `DH` at memory address.                         |
| `0x170F`  | `ST [ADDR32], DL` | 6/7           |           | Stores 8-bit register `DL` at memory address.                         |
| `0x1800`  | `ST [A], A`       | 2/6           |           | Stores 32-bit register `A` at address in `A`.                         |
| `0x1801`  | `ST [A], AW`      | 2/4           |           | Stores 16-bit register `AW` at address in `A`.                        |
| `0x1802`  | `ST [A], AH`      | 2/3           |           | Stores 8-bit register `AH` at address in `A`.                         |
| `0x1803`  | `ST [A], AL`      | 2/3           |           | Stores 8-bit register `AL` at address in `A`.                         |
| `0x1804`  | `ST [A], B`       | 2/6           |           | Stores 32-bit register `B` at address in `A`.                         |
| `0x1805`  | `ST [A], BW`      | 2/4           |           | Stores 16-bit register `BW` at address in `A`.                        |
| `0x1806`  | `ST [A], BH`      | 2/3           |           | Stores 8-bit register `BH` at address in `A`.                         |
| `0x1807`  | `ST [A], BL`      | 2/3           |           | Stores 8-bit register `BL` at address in `A`.                         |
| `0x1808`  | `ST [A], C`       | 2/6           |           | Stores 32-bit register `C` at address in `A`.                         |
| `0x1809`  | `ST [A], CW`      | 2/4           |           | Stores 16-bit register `CW` at address in `A`.                        |
| `0x180A`  | `ST [A], CH`      | 2/3           |           | Stores 8-bit register `CH` at address in `A`.                         |
| `0x180B`  | `ST [A], CL`      | 2/3           |           | Stores 8-bit register `CL` at address in `A`.                         |
| `0x180C`  | `ST [A], D`       | 2/6           |           | Stores 32-bit register `D` at address in `A`.                         |
| `0x180D`  | `ST [A], DW`      | 2/4           |           | Stores 16-bit register `DW` at address in `A`.                        |
| `0x180E`  | `ST [A], DH`      | 2/3           |           | Stores 8-bit register `DH` at address in `A`.                         |
| `0x180F`  | `ST [A], DL`      | 2/3           |           | Stores 8-bit register `DL` at address in `A`.                         |
| `0x1840`  | `ST [B], A`       | 2/6           |           | Stores 32-bit register `A` at address in `B`.                         |
| `0x1841`  | `ST [B], AW`      | 2/4           |           | Stores 16-bit register `AW` at address in `B`.                        |
| `0x1842`  | `ST [B], AH`      | 2/3           |           | Stores 8-bit register `AH` at address in `B`.                         |
| `0x1843`  | `ST [B], AL`      | 2/3           |           | Stores 8-bit register `AL` at address in `B`.                         |
| `0x1844`  | `ST [B], B`       | 2/6           |           | Stores 32-bit register `B` at address in `B`.                         |
| `0x1845`  | `ST [B], BW`      | 2/4           |           | Stores 16-bit register `BW` at address in `B`.                        |
| `0x1846`  | `ST [B], BH`      | 2/3           |           | Stores 8-bit register `BH` at address in `B`.                         |
| `0x1847`  | `ST [B], BL`      | 2/3           |           | Stores 8-bit register `BL` at address in `B`.                         |
| `0x1848`  | `ST [B], C`       | 2/6           |           | Stores 32-bit register `C` at address in `B`.                         |
| `0x1849`  | `ST [B], CW`      | 2/4           |           | Stores 16-bit register `CW` at address in `B`.                        |
| `0x184A`  | `ST [B], CH`      | 2/3           |           | Stores 8-bit register `CH` at address in `B`.                         |
| `0x184B`  | `ST [B], CL`      | 2/3           |           | Stores 8-bit register `CL` at address in `B`.                         |
| `0x184C`  | `ST [B], D`       | 2/6           |           | Stores 32-bit register `D` at address in `B`.                         |
| `0x184D`  | `ST [B], DW`      | 2/4           |           | Stores 16-bit register `DW` at address in `B`.                        |
| `0x184E`  | `ST [B], DH`      | 2/3           |           | Stores 8-bit register `DH` at address in `B`.                         |
| `0x184F`  | `ST [B], DL`      | 2/3           |           | Stores 8-bit register `DL` at address in `B`.                         |
| `0x1880`  | `ST [C], A`       | 2/6           |           | Stores 32-bit register `A` at address in `C`.                         |
| `0x1881`  | `ST [C], AW`      | 2/4           |           | Stores 16-bit register `AW` at address in `C`.                        |
| `0x1882`  | `ST [C], AH`      | 2/3           |           | Stores 8-bit register `AH` at address in `C`.                         |
| `0x1883`  | `ST [C], AL`      | 2/3           |           | Stores 8-bit register `AL` at address in `C`.                         |
| `0x1884`  | `ST [C], B`       | 2/6           |           | Stores 32-bit register `B` at address in `C`.                         |
| `0x1885`  | `ST [C], BW`      | 2/4           |           | Stores 16-bit register `BW` at address in `C`.                        |
| `0x1886`  | `ST [C], BH`      | 2/3           |           | Stores 8-bit register `BH` at address in `C`.                         |
| `0x1887`  | `ST [C], BL`      | 2/3           |           | Stores 8-bit register `BL` at address in `C`.                         |
| `0x1888`  | `ST [C], C`       | 2/6           |           | Stores 32-bit register `C` at address in `C`.                         |
| `0x1889`  | `ST [C], CW`      | 2/4           |           | Stores 16-bit register `CW` at address in `C`.                        |
| `0x188A`  | `ST [C], CH`      | 2/3           |           | Stores 8-bit register `CH` at address in `C`.                         |
| `0x188B`  | `ST [C], CL`      | 2/3           |           | Stores 8-bit register `CL` at address in `C`.                         |
| `0x188C`  | `ST [C], D`       | 2/6           |           | Stores 32-bit register `D` at address in `C`.                         |
| `0x188D`  | `ST [C], DW`      | 2/4           |           | Stores 16-bit register `DW` at address in `C`.                        |
| `0x188E`  | `ST [C], DH`      | 2/3           |           | Stores 8-bit register `DH` at address in `C`.                         |
| `0x188F`  | `ST [C], DL`      | 2/3           |           | Stores 8-bit register `DL` at address in `C`.                         |
| `0x18C0`  | `ST [D], A`       | 2/6           |           | Stores 32-bit register `A` at address in `D`.                         |
| `0x18C1`  | `ST [D], AW`      | 2/4           |           | Stores 16-bit register `AW` at address in `D`.                        |
| `0x18C2`  | `ST [D], AH`      | 2/3           |           | Stores 8-bit register `AH` at address in `D`.                         |
| `0x18C3`  | `ST [D], AL`      | 2/3           |           | Stores 8-bit register `AL` at address in `D`.                         |
| `0x18C4`  | `ST [D], B`       | 2/6           |           | Stores 32-bit register `B` at address in `D`.                         |
| `0x18C5`  | `ST [D], BW`      | 2/4           |           | Stores 16-bit register `BW` at address in `D`.                        |
| `0x18C6`  | `ST [D], BH`      | 2/3           |           | Stores 8-bit register `BH` at address in `D`.                         |
| `0x18C7`  | `ST [D], BL`      | 2/3           |           | Stores 8-bit register `BL` at address in `D`.                         |
| `0x18C8`  | `ST [D], C`       | 2/6           |           | Stores 32-bit register `C` at address in `D`.                         |
| `0x18C9`  | `ST [D], CW`      | 2/4           |           | Stores 16-bit register `CW` at address in `D`.                        |
| `0x18CA`  | `ST [D], CH`      | 2/3           |           | Stores 8-bit register `CH` at address in `D`.                         |
| `0x18CB`  | `ST [D], CL`      | 2/3           |           | Stores 8-bit register `CL` at address in `D`.                         |
| `0x18CC`  | `ST [D], D`       | 2/6           |           | Stores 32-bit register `D` at address in `D`.                         |
| `0x18CD`  | `ST [D], DW`      | 2/4           |           | Stores 16-bit register `DW` at address in `D`.                        |
| `0x18CE`  | `ST [D], DH`      | 2/3           |           | Stores 8-bit register `DH` at address in `D`.                         |
| `0x18CF`  | `ST [D], DL`      | 2/3           |           | Stores 8-bit register `DL` at address in `D`.                         |
| `0x1900`  | `STH [ADDR16], A` | 4/8           |           | Stores 32-bit register `A` at 16-bit page address.                    |
| `0x1901`  | `STH [ADDR16], AW`| 4/6           |           | Stores 16-bit register `AW` at 16-bit page address.                   |
| `0x1902`  | `STH [ADDR16], AH`| 4/5           |           | Stores 8-bit register `AH` at 16-bit page address.                    |
| `0x1903`  | `STH [ADDR16], AL`| 4/5           |           | Stores 8-bit register `AL` at 16-bit page address.                    |
| `0x1904`  | `STH [ADDR16], B` | 4/8           |           | Stores 32-bit register `B` at 16-bit page address.                    |
| `0x1905`  | `STH [ADDR16], BW`| 4/6           |           | Stores 16-bit register `BW` at 16-bit page address.                   |
| `0x1906`  | `STH [ADDR16], BH`| 4/5           |           | Stores 8-bit register `BH` at 16-bit page address.                    |
| `0x1907`  | `STH [ADDR16], BL`| 4/5           |           | Stores 8-bit register `BL` at 16-bit page address.                    |
| `0x1908`  | `STH [ADDR16], C` | 4/8           |           | Stores 32-bit register `C` at 16-bit page address.                    |
| `0x1909`  | `STH [ADDR16], CW`| 4/6           |           | Stores 16-bit register `CW` at 16-bit page address.                   |
| `0x190A`  | `STH [ADDR16], CH`| 4/5           |           | Stores 8-bit register `CH` at 16-bit page address.                    |
| `0x190B`  | `STH [ADDR16], CL`| 4/5           |           | Stores 8-bit register `CL` at 16-bit page address.                    |
| `0x190C`  | `STH [ADDR16], D` | 4/8           |           | Stores 32-bit register `D` at 16-bit page address.                    |
| `0x190D`  | `STH [ADDR16], DW`| 4/6           |           | Stores 16-bit register `DW` at 16-bit page address.                   |
| `0x190E`  | `STH [ADDR16], DH`| 4/5           |           | Stores 8-bit register `DH` at 16-bit page address.                    |
| `0x190F`  | `STH [ADDR16], DL`| 4/5           |           | Stores 8-bit register `DL` at 16-bit page address.                    |
| `0x1A10`  | `STH [AW], A`     | 2/6           |           | Stores 32-bit register `A` at page address in `AW`.                   |
| `0x1A11`  | `STH [AW], AW`    | 2/4           |           | Stores 16-bit register `AW` at page address in `AW`.                  |
| `0x1A12`  | `STH [AW], AH`    | 2/3           |           | Stores 8-bit register `AH` at page address in `AW`.                   |
| `0x1A13`  | `STH [AW], AL`    | 2/3           |           | Stores 8-bit register `AL` at page address in `AW`.                   |
| `0x1A14`  | `STH [AW], B`     | 2/6           |           | Stores 32-bit register `B` at page address in `AW`.                   |
| `0x1A15`  | `STH [AW], BW`    | 2/4           |           | Stores 16-bit register `BW` at page address in `AW`.                  |
| `0x1A16`  | `STH [AW], BH`    | 2/3           |           | Stores 8-bit register `BH` at page address in `AW`.                   |
| `0x1A17`  | `STH [AW], BL`    | 2/3           |           | Stores 8-bit register `BL` at page address in `AW`.                   |
| `0x1A18`  | `STH [AW], C`     | 2/6           |           | Stores 32-bit register `C` at page address in `AW`.                   |
| `0x1A19`  | `STH [AW], CW`    | 2/4           |           | Stores 16-bit register `CW` at page address in `AW`.                  |
| `0x1A1A`  | `STH [AW], CH`    | 2/3           |           | Stores 8-bit register `CH` at page address in `AW`.                   |
| `0x1A1B`  | `STH [AW], CL`    | 2/3           |           | Stores 8-bit register `CL` at page address in `AW`.                   |
| `0x1A1C`  | `STH [AW], D`     | 2/6           |           | Stores 32-bit register `D` at page address in `AW`.                   |
| `0x1A1D`  | `STH [AW], DW`    | 2/4           |           | Stores 16-bit register `DW` at page address in `AW`.                  |
| `0x1A1E`  | `STH [AW], DH`    | 2/3           |           | Stores 8-bit register `DH` at page address in `AW`.                   |
| `0x1A1F`  | `STH [AW], DL`    | 2/3           |           | Stores 8-bit register `DL` at page address in `AW`.                   |
| `0x1A50`  | `STH [BW], A`     | 2/6           |           | Stores 32-bit register `A` at page address in `BW`.                   |
| `0x1A51`  | `STH [BW], AW`    | 2/4           |           | Stores 16-bit register `AW` at page address in `BW`.                  |
| `0x1A52`  | `STH [BW], AH`    | 2/3           |           | Stores 8-bit register `AH` at page address in `BW`.                   |
| `0x1A53`  | `STH [BW], AL`    | 2/3           |           | Stores 8-bit register `AL` at page address in `BW`.                   |
| `0x1A54`  | `STH [BW], B`     | 2/6           |           | Stores 32-bit register `B` at page address in `BW`.                   |
| `0x1A55`  | `STH [BW], BW`    | 2/4           |           | Stores 16-bit register `BW` at page address in `BW`.                  |
| `0x1A56`  | `STH [BW], BH`    | 2/3           |           | Stores 8-bit register `BH` at page address in `BW`.                   |
| `0x1A57`  | `STH [BW], BL`    | 2/3           |           | Stores 8-bit register `BL` at page address in `BW`.                   |
| `0x1A58`  | `STH [BW], C`     | 2/6           |           | Stores 32-bit register `C` at page address in `BW`.                   |
| `0x1A59`  | `STH [BW], CW`    | 2/4           |           | Stores 16-bit register `CW` at page address in `BW`.                  |
| `0x1A5A`  | `STH [BW], CH`    | 2/3           |           | Stores 8-bit register `CH` at page address in `BW`.                   |
| `0x1A5B`  | `STH [BW], CL`    | 2/3           |           | Stores 8-bit register `CL` at page address in `BW`.                   |
| `0x1A5C`  | `STH [BW], D`     | 2/6           |           | Stores 32-bit register `D` at page address in `BW`.                   |
| `0x1A5D`  | `STH [BW], DW`    | 2/4           |           | Stores 16-bit register `DW` at page address in `BW`.                  |
| `0x1A5E`  | `STH [BW], DH`    | 2/3           |           | Stores 8-bit register `DH` at page address in `BW`.                   |
| `0x1A5F`  | `STH [BW], DL`    | 2/3           |           | Stores 8-bit register `DL` at page address in `BW`.                   |
| `0x1A90`  | `STH [CW], A`     | 2/6           |           | Stores 32-bit register `A` at page address in `CW`.                   |
| `0x1A91`  | `STH [CW], AW`    | 2/4           |           | Stores 16-bit register `AW` at page address in `CW`.                  |
| `0x1A92`  | `STH [CW], AH`    | 2/3           |           | Stores 8-bit register `AH` at page address in `CW`.                   |
| `0x1A93`  | `STH [CW], AL`    | 2/3           |           | Stores 8-bit register `AL` at page address in `CW`.                   |
| `0x1A94`  | `STH [CW], B`     | 2/6           |           | Stores 32-bit register `B` at page address in `CW`.                   |
| `0x1A95`  | `STH [CW], BW`    | 2/4           |           | Stores 16-bit register `BW` at page address in `CW`.                  |
| `0x1A96`  | `STH [CW], BH`    | 2/3           |           | Stores 8-bit register `BH` at page address in `CW`.                   |
| `0x1A97`  | `STH [CW], BL`    | 2/3           |           | Stores 8-bit register `BL` at page address in `CW`.                   |
| `0x1A98`  | `STH [CW], C`     | 2/6           |           | Stores 32-bit register `C` at page address in `CW`.                   |
| `0x1A99`  | `STH [CW], CW`    | 2/4           |           | Stores 16-bit register `CW` at page address in `CW`.                  |
| `0x1A9A`  | `STH [CW], CH`    | 2/3           |           | Stores 8-bit register `CH` at page address in `CW`.                   |
| `0x1A9B`  | `STH [CW], CL`    | 2/3           |           | Stores 8-bit register `CL` at page address in `CW`.                   |
| `0x1A9C`  | `STH [CW], D`     | 2/6           |           | Stores 32-bit register `D` at page address in `CW`.                   |
| `0x1A9D`  | `STH [CW], DW`    | 2/4           |           | Stores 16-bit register `DW` at page address in `CW`.                  |
| `0x1A9E`  | `STH [CW], DH`    | 2/3           |           | Stores 8-bit register `DH` at page address in `CW`.                   |
| `0x1A9F`  | `STH [CW], DL`    | 2/3           |           | Stores 8-bit register `DL` at page address in `CW`.                   |
| `0x1AD0`  | `STH [DW], A`     | 2/6           |           | Stores 32-bit register `A` at page address in `DW`.                   |
| `0x1AD1`  | `STH [DW], AW`    | 2/4           |           | Stores 16-bit register `AW` at page address in `DW`.                  |
| `0x1AD2`  | `STH [DW], AH`    | 2/3           |           | Stores 8-bit register `AH` at page address in `DW`.                   |
| `0x1AD3`  | `STH [DW], AL`    | 2/3           |           | Stores 8-bit register `AL` at page address in `DW`.                   |
| `0x1AD4`  | `STH [DW], B`     | 2/6           |           | Stores 32-bit register `B` at page address in `DW`.                   |
| `0x1AD5`  | `STH [DW], BW`    | 2/4           |           | Stores 16-bit register `BW` at page address in `DW`.                  |
| `0x1AD6`  | `STH [DW], BH`    | 2/3           |           | Stores 8-bit register `BH` at page address in `DW`.                   |
| `0x1AD7`  | `STH [DW], BL`    | 2/3           |           | Stores 8-bit register `BL` at page address in `DW`.                   |
| `0x1AD8`  | `STH [DW], C`     | 2/6           |           | Stores 32-bit register `C` at page address in `DW`.                   |
| `0x1AD9`  | `STH [DW], CW`    | 2/4           |           | Stores 16-bit register `CW` at page address in `DW`.                  |
| `0x1ADA`  | `STH [DW], CH`    | 2/3           |           | Stores 8-bit register `CH` at page address in `DW`.                   |
| `0x1ADB`  | `STH [DW], CL`    | 2/3           |           | Stores 8-bit register `CL` at page address in `DW`.                   |
| `0x1ADC`  | `STH [DW], D`     | 2/6           |           | Stores 32-bit register `D` at page address in `DW`.                   |
| `0x1ADD`  | `STH [DW], DW`    | 2/4           |           | Stores 16-bit register `DW` at page address in `DW`.                  |
| `0x1ADE`  | `STH [DW], DH`    | 2/3           |           | Stores 8-bit register `DH` at page address in `DW`.                   |
| `0x1ADF`  | `STH [DW], DL`    | 2/3           |           | Stores 8-bit register `DL` at page address in `DW`.                   |
| `0x1B02`  | `STP [ADDR8], AH` | 3/3           |           | Stores 8-bit register `AH` at 8-bit page address.                     |
| `0x1B03`  | `STP [ADDR8], AL` | 3/3           |           | Stores 8-bit register `AL` at 8-bit page address.                     |
| `0x1B06`  | `STP [ADDR8], BH` | 3/3           |           | Stores 8-bit register `BH` at 8-bit page address.                     |
| `0x1B07`  | `STP [ADDR8], BL` | 3/3           |           | Stores 8-bit register `BL` at 8-bit page address.                     |
| `0x1B0A`  | `STP [ADDR8], CH` | 3/3           |           | Stores 8-bit register `CH` at 8-bit page address.                     |
| `0x1B0B`  | `STP [ADDR8], CL` | 3/3           |           | Stores 8-bit register `CL` at 8-bit page address.                     |
| `0x1B0E`  | `STP [ADDR8], DH` | 3/3           |           | Stores 8-bit register `DH` at 8-bit page address.                     |
| `0x1B0F`  | `STP [ADDR8], DL` | 3/3           |           | Stores 8-bit register `DL` at 8-bit page address.                     |
| `0x1C22`  | `STP [AH], AH`    | 2/3           |           | Stores 8-bit register `AH` at page address in `AH`.                   |
| `0x1C23`  | `STP [AH], AL`    | 2/3           |           | Stores 8-bit register `AL` at page address in `AH`.                   |
| `0x1C26`  | `STP [AH], BH`    | 2/3           |           | Stores 8-bit register `BH` at page address in `AH`.                   |
| `0x1C27`  | `STP [AH], BL`    | 2/3           |           | Stores 8-bit register `BL` at page address in `AH`.                   |
| `0x1C2A`  | `STP [AH], CH`    | 2/3           |           | Stores 8-bit register `CH` at page address in `AH`.                   |
| `0x1C2B`  | `STP [AH], CL`    | 2/3           |           | Stores 8-bit register `CL` at page address in `AH`.                   |
| `0x1C2E`  | `STP [AH], DH`    | 2/3           |           | Stores 8-bit register `DH` at page address in `AH`.                   |
| `0x1C2F`  | `STP [AH], DL`    | 2/3           |           | Stores 8-bit register `DL` at page address in `AH`.                   |
| `0x1C32`  | `STP [AL], AH`    | 2/3           |           | Stores 8-bit register `AH` at page address in `AL`.                   |
| `0x1C33`  | `STP [AL], AL`    | 2/3           |           | Stores 8-bit register `AL` at page address in `AL`.                   |
| `0x1C36`  | `STP [AL], BH`    | 2/3           |           | Stores 8-bit register `BH` at page address in `AL`.                   |
| `0x1C37`  | `STP [AL], BL`    | 2/3           |           | Stores 8-bit register `BL` at page address in `AL`.                   |
| `0x1C3A`  | `STP [AL], CH`    | 2/3           |           | Stores 8-bit register `CH` at page address in `AL`.                   |
| `0x1C3B`  | `STP [AL], CL`    | 2/3           |           | Stores 8-bit register `CL` at page address in `AL`.                   |
| `0x1C3E`  | `STP [AL], DH`    | 2/3           |           | Stores 8-bit register `DH` at page address in `AL`.                   |
| `0x1C3F`  | `STP [AL], DL`    | 2/3           |           | Stores 8-bit register `DL` at page address in `AL`.                   |
| `0x1C62`  | `STP [BH], AH`    | 2/3           |           | Stores 8-bit register `AH` at page address in `BH`.                   |
| `0x1C63`  | `STP [BH], AL`    | 2/3           |           | Stores 8-bit register `AL` at page address in `BH`.                   |
| `0x1C66`  | `STP [BH], BH`    | 2/3           |           | Stores 8-bit register `BH` at page address in `BH`.                   |
| `0x1C67`  | `STP [BH], BL`    | 2/3           |           | Stores 8-bit register `BL` at page address in `BH`.                   |
| `0x1C6A`  | `STP [BH], CH`    | 2/3           |           | Stores 8-bit register `CH` at page address in `BH`.                   |
| `0x1C6B`  | `STP [BH], CL`    | 2/3           |           | Stores 8-bit register `CL` at page address in `BH`.                   |
| `0x1C6E`  | `STP [BH], DH`    | 2/3           |           | Stores 8-bit register `DH` at page address in `BH`.                   |
| `0x1C6F`  | `STP [BH], DL`    | 2/3           |           | Stores 8-bit register `DL` at page address in `BH`.                   |
| `0x1C72`  | `STP [BL], AH`    | 2/3           |           | Stores 8-bit register `AH` at page address in `BL`.                   |
| `0x1C73`  | `STP [BL], AL`    | 2/3           |           | Stores 8-bit register `AL` at page address in `BL`.                   |
| `0x1C76`  | `STP [BL], BH`    | 2/3           |           | Stores 8-bit register `BH` at page address in `BL`.                   |
| `0x1C77`  | `STP [BL], BL`    | 2/3           |           | Stores 8-bit register `BL` at page address in `BL`.                   |
| `0x1C7A`  | `STP [BL], CH`    | 2/3           |           | Stores 8-bit register `CH` at page address in `BL`.                   |
| `0x1C7B`  | `STP [BL], CL`    | 2/3           |           | Stores 8-bit register `CL` at page address in `BL`.                   |
| `0x1C7E`  | `STP [BL], DH`    | 2/3           |           | Stores 8-bit register `DH` at page address in `BL`.                   |
| `0x1C7F`  | `STP [BL], DL`    | 2/3           |           | Stores 8-bit register `DL` at page address in `BL`.                   |
| `0x1CA2`  | `STP [CH], AH`    | 2/3           |           | Stores 8-bit register `AH` at page address in `CH`.                   |
| `0x1CA3`  | `STP [CH], AL`    | 2/3           |           | Stores 8-bit register `AL` at page address in `CH`.                   |
| `0x1CA6`  | `STP [CH], BH`    | 2/3           |           | Stores 8-bit register `BH` at page address in `CH`.                   |
| `0x1CA7`  | `STP [CH], BL`    | 2/3           |           | Stores 8-bit register `BL` at page address in `CH`.                   |
| `0x1CAA`  | `STP [CH], CH`    | 2/3           |           | Stores 8-bit register `CH` at page address in `CH`.                   |
| `0x1CAB`  | `STP [CH], CL`    | 2/3           |           | Stores 8-bit register `CL` at page address in `CH`.                   |
| `0x1CAE`  | `STP [CH], DH`    | 2/3           |           | Stores 8-bit register `DH` at page address in `CH`.                   |
| `0x1CAF`  | `STP [CH], DL`    | 2/3           |           | Stores 8-bit register `DL` at page address in `CH`.                   |
| `0x1CB2`  | `STP [CL], AH`    | 2/3           |           | Stores 8-bit register `AH` at page address in `CL`.                   |
| `0x1CB3`  | `STP [CL], AL`    | 2/3           |           | Stores 8-bit register `AL` at page address in `CL`.                   |
| `0x1CB6`  | `STP [CL], BH`    | 2/3           |           | Stores 8-bit register `BH` at page address in `CL`.                   |
| `0x1CB7`  | `STP [CL], BL`    | 2/3           |           | Stores 8-bit register `BL` at page address in `CL`.                   |
| `0x1CBA`  | `STP [CL], CH`    | 2/3           |           | Stores 8-bit register `CH` at page address in `CL`.                   |
| `0x1CBB`  | `STP [CL], CL`    | 2/3           |           | Stores 8-bit register `CL` at page address in `CL`.                   |
| `0x1CBE`  | `STP [CL], DH`    | 2/3           |           | Stores 8-bit register `DH` at page address in `CL`.                   |
| `0x1CBF`  | `STP [CL], DL`    | 2/3           |           | Stores 8-bit register `DL` at page address in `CL`.                   |
| `0x1CE2`  | `STP [DH], AH`    | 2/3           |           | Stores 8-bit register `AH` at page address in `DH`.                   |
| `0x1CE3`  | `STP [DH], AL`    | 2/3           |           | Stores 8-bit register `AL` at page address in `DH`.                   |
| `0x1CE6`  | `STP [DH], BH`    | 2/3           |           | Stores 8-bit register `BH` at page address in `DH`.                   |
| `0x1CE7`  | `STP [DH], BL`    | 2/3           |           | Stores 8-bit register `BL` at page address in `DH`.                   |
| `0x1CEA`  | `STP [DH], CH`    | 2/3           |           | Stores 8-bit register `CH` at page address in `DH`.                   |
| `0x1CEB`  | `STP [DH], CL`    | 2/3           |           | Stores 8-bit register `CL` at page address in `DH`.                   |
| `0x1CEE`  | `STP [DH], DH`    | 2/3           |           | Stores 8-bit register `DH` at page address in `DH`.                   |
| `0x1CEF`  | `STP [DH], DL`    | 2/3           |           | Stores 8-bit register `DL` at page address in `DH`.                   |
| `0x1CF2`  | `STP [DL], AH`    | 2/3           |           | Stores 8-bit register `AH` at page address in `DL`.                   |
| `0x1CF3`  | `STP [DL], AL`    | 2/3           |           | Stores 8-bit register `AL` at page address in `DL`.                   |
| `0x1CF6`  | `STP [DL], BH`    | 2/3           |           | Stores 8-bit register `BH` at page address in `DL`.                   |
| `0x1CF7`  | `STP [DL], BL`    | 2/3           |           | Stores 8-bit register `BL` at page address in `DL`.                   |
| `0x1CFA`  | `STP [DL], CH`    | 2/3           |           | Stores 8-bit register `CH` at page address in `DL`.                   |
| `0x1CFB`  | `STP [DL], CL`    | 2/3           |           | Stores 8-bit register `CL` at page address in `DL`.                   |
| `0x1CFE`  | `STP [DL], DH`    | 2/3           |           | Stores 8-bit register `DH` at page address in `DL`.                   |
| `0x1CFF`  | `STP [DL], DL`    | 2/3           |           | Stores 8-bit register `DL` at page address in `DL`.                   |
| `0x1D00`  | `MV A, A`         | 2/2           |           | Moves 32-bit register `A` into `A`.                                   |
| `0x1D01`  | `MV A, AW`        | 2/2           |           | Moves 16-bit register `AW` into 32-bit register `A`.                  |
| `0x1D02`  | `MV A, AH`        | 2/2           |           | Moves 8-bit register `AH` into 32-bit register `A`.                   |
| `0x1D03`  | `MV A, AL`        | 2/2           |           | Moves 8-bit register `AL` into 32-bit register `A`.                   |
| `0x1D04`  | `MV A, B`         | 2/2           |           | Moves 32-bit register `B` into `A`.                                   |
| `0x1D05`  | `MV A, BW`        | 2/2           |           | Moves 16-bit register `BW` into 32-bit register `A`.                  |
| `0x1D06`  | `MV A, BH`        | 2/2           |           | Moves 8-bit register `BH` into 32-bit register `A`.                   |
| `0x1D07`  | `MV A, BL`        | 2/2           |           | Moves 8-bit register `BL` into 32-bit register `A`.                   |
| `0x1D08`  | `MV A, C`         | 2/2           |           | Moves 32-bit register `C` into `A`.                                   |
| `0x1D09`  | `MV A, CW`        | 2/2           |           | Moves 16-bit register `CW` into 32-bit register `A`.                  |
| `0x1D0A`  | `MV A, CH`        | 2/2           |           | Moves 8-bit register `CH` into 32-bit register `A`.                   |
| `0x1D0B`  | `MV A, CL`        | 2/2           |           | Moves 8-bit register `CL` into 32-bit register `A`.                   |
| `0x1D0C`  | `MV A, D`         | 2/2           |           | Moves 32-bit register `D` into `A`.                                   |
| `0x1D0D`  | `MV A, DW`        | 2/2           |           | Moves 16-bit register `DW` into 32-bit register `A`.                  |
| `0x1D0E`  | `MV A, DH`        | 2/2           |           | Moves 8-bit register `DH` into 32-bit register `A`.                   |
| `0x1D0F`  | `MV A, DL`        | 2/2           |           | Moves 8-bit register `DL` into 32-bit register `A`.                   |
| `0x1D10`  | `MV AW, A`        | 2/2           |           | Moves 32-bit register `A` into 16-bit register `AW`.                  |
| `0x1D11`  | `MV AW, AW`       | 2/2           |           | Moves 16-bit register `AW` into `AW`.                                 |
| `0x1D12`  | `MV AW, AH`       | 2/2           |           | Moves 8-bit register `AH` into 16-bit register `AW`.                  |
| `0x1D13`  | `MV AW, AL`       | 2/2           |           | Moves 8-bit register `AL` into 16-bit register `AW`.                  |
| `0x1D14`  | `MV AW, B`        | 2/2           |           | Moves 32-bit register `B` into 16-bit register `AW`.                  |
| `0x1D15`  | `MV AW, BW`       | 2/2           |           | Moves 16-bit register `BW` into `AW`.                                 |
| `0x1D16`  | `MV AW, BH`       | 2/2           |           | Moves 8-bit register `BH` into 16-bit register `AW`.                  |
| `0x1D17`  | `MV AW, BL`       | 2/2           |           | Moves 8-bit register `BL` into 16-bit register `AW`.                  |
| `0x1D18`  | `MV AW, C`        | 2/2           |           | Moves 32-bit register `C` into 16-bit register `AW`.                  |
| `0x1D19`  | `MV AW, CW`       | 2/2           |           | Moves 16-bit register `CW` into `AW`.                                 |
| `0x1D1A`  | `MV AW, CH`       | 2/2           |           | Moves 8-bit register `CH` into 16-bit register `AW`.                  |
| `0x1D1B`  | `MV AW, CL`       | 2/2           |           | Moves 8-bit register `CL` into 16-bit register `AW`.                  |
| `0x1D1C`  | `MV AW, D`        | 2/2           |           | Moves 32-bit register `D` into 16-bit register `AW`.                  |
| `0x1D1D`  | `MV AW, DW`       | 2/2           |           | Moves 16-bit register `DW` into `AW`.                                 |
| `0x1D1E`  | `MV AW, DH`       | 2/2           |           | Moves 8-bit register `DH` into 16-bit register `AW`.                  |
| `0x1D1F`  | `MV AW, DL`       | 2/2           |           | Moves 8-bit register `DL` into 16-bit register `AW`.                  |
| `0x1D20`  | `MV AH, A`        | 2/2           |           | Moves 32-bit register `A` into 8-bit register `AH`.                   |
| `0x1D21`  | `MV AH, AW`       | 2/2           |           | Moves 16-bit register `AW` into 8-bit register `AH`.                  |
| `0x1D22`  | `MV AH, AH`       | 2/2           |           | Moves 8-bit register `AH` into `AH`.                                  |
| `0x1D23`  | `MV AH, AL`       | 2/2           |           | Moves 8-bit register `AL` into `AH`.                                  |
| `0x1D24`  | `MV AH, B`        | 2/2           |           | Moves 32-bit register `B` into 8-bit register `AH`.                   |
| `0x1D25`  | `MV AH, BW`       | 2/2           |           | Moves 16-bit register `BW` into 8-bit register `AH`.                  |
| `0x1D26`  | `MV AH, BH`       | 2/2           |           | Moves 8-bit register `BH` into `AH`.                                  |
| `0x1D27`  | `MV AH, BL`       | 2/2           |           | Moves 8-bit register `BL` into `AH`.                                  |
| `0x1D28`  | `MV AH, C`        | 2/2           |           | Moves 32-bit register `C` into 8-bit register `AH`.                   |
| `0x1D29`  | `MV AH, CW`       | 2/2           |           | Moves 16-bit register `CW` into 8-bit register `AH`.                  |
| `0x1D2A`  | `MV AH, CH`       | 2/2           |           | Moves 8-bit register `CH` into `AH`.                                  |
| `0x1D2B`  | `MV AH, CL`       | 2/2           |           | Moves 8-bit register `CL` into `AH`.                                  |
| `0x1D2C`  | `MV AH, D`        | 2/2           |           | Moves 32-bit register `D` into 8-bit register `AH`.                   |
| `0x1D2D`  | `MV AH, DW`       | 2/2           |           | Moves 16-bit register `DW` into 8-bit register `AH`.                  |
| `0x1D2E`  | `MV AH, DH`       | 2/2           |           | Moves 8-bit register `DH` into `AH`.                                  |
| `0x1D2F`  | `MV AH, DL`       | 2/2           |           | Moves 8-bit register `DL` into `AH`.                                  |
| `0x1D30`  | `MV AL, A`        | 2/2           |           | Moves 32-bit register `A` into 8-bit register `AL`.                   |
| `0x1D31`  | `MV AL, AW`       | 2/2           |           | Moves 16-bit register `AW` into 8-bit register `AL`.                  |
| `0x1D32`  | `MV AL, AH`       | 2/2           |           | Moves 8-bit register `AH` into `AL`.                                  |
| `0x1D33`  | `MV AL, AL`       | 2/2           |           | Moves 8-bit register `AL` into `AL`.                                  |
| `0x1D34`  | `MV AL, B`        | 2/2           |           | Moves 32-bit register `B` into 8-bit register `AL`.                   |
| `0x1D35`  | `MV AL, BW`       | 2/2           |           | Moves 16-bit register `BW` into 8-bit register `AL`.                  |
| `0x1D36`  | `MV AL, BH`       | 2/2           |           | Moves 8-bit register `BH` into `AL`.                                  |
| `0x1D37`  | `MV AL, BL`       | 2/2           |           | Moves 8-bit register `BL` into `AL`.                                  |
| `0x1D38`  | `MV AL, C`        | 2/2           |           | Moves 32-bit register `C` into 8-bit register `AL`.                   |
| `0x1D39`  | `MV AL, CW`       | 2/2           |           | Moves 16-bit register `CW` into 8-bit register `AL`.                  |
| `0x1D3A`  | `MV AL, CH`       | 2/2           |           | Moves 8-bit register `CH` into `AL`.                                  |
| `0x1D3B`  | `MV AL, CL`       | 2/2           |           | Moves 8-bit register `CL` into `AL`.                                  |
| `0x1D3C`  | `MV AL, D`        | 2/2           |           | Moves 32-bit register `D` into 8-bit register `AL`.                   |
| `0x1D3D`  | `MV AL, DW`       | 2/2           |           | Moves 16-bit register `DW` into 8-bit register `AL`.                  |
| `0x1D3E`  | `MV AL, DH`       | 2/2           |           | Moves 8-bit register `DH` into `AL`.                                  |
| `0x1D3F`  | `MV AL, DL`       | 2/2           |           | Moves 8-bit register `DL` into `AL`.                                  |
| `0x1D40`  | `MV B, A`         | 2/2           |           | Moves 32-bit register `A` into `B`.                                   |
| `0x1D41`  | `MV B, AW`        | 2/2           |           | Moves 16-bit register `AW` into 32-bit register `B`.                  |
| `0x1D42`  | `MV B, AH`        | 2/2           |           | Moves 8-bit register `AH` into 32-bit register `B`.                   |
| `0x1D43`  | `MV B, AL`        | 2/2           |           | Moves 8-bit register `AL` into 32-bit register `B`.                   |
| `0x1D44`  | `MV B, B`         | 2/2           |           | Moves 32-bit register `B` into `B`.                                   |
| `0x1D45`  | `MV B, BW`        | 2/2           |           | Moves 16-bit register `BW` into 32-bit register `B`.                  |
| `0x1D46`  | `MV B, BH`        | 2/2           |           | Moves 8-bit register `BH` into 32-bit register `B`.                   |
| `0x1D47`  | `MV B, BL`        | 2/2           |           | Moves 8-bit register `BL` into 32-bit register `B`.                   |
| `0x1D48`  | `MV B, C`         | 2/2           |           | Moves 32-bit register `C` into `B`.                                   |
| `0x1D49`  | `MV B, CW`        | 2/2           |           | Moves 16-bit register `CW` into 32-bit register `B`.                  |
| `0x1D4A`  | `MV B, CH`        | 2/2           |           | Moves 8-bit register `CH` into 32-bit register `B`.                   |
| `0x1D4B`  | `MV B, CL`        | 2/2           |           | Moves 8-bit register `CL` into 32-bit register `B`.                   |
| `0x1D4C`  | `MV B, D`         | 2/2           |           | Moves 32-bit register `D` into `B`.                                   |
| `0x1D4D`  | `MV B, DW`        | 2/2           |           | Moves 16-bit register `DW` into 32-bit register `B`.                  |
| `0x1D4E`  | `MV B, DH`        | 2/2           |           | Moves 8-bit register `DH` into 32-bit register `B`.                   |
| `0x1D4F`  | `MV B, DL`        | 2/2           |           | Moves 8-bit register `DL` into 32-bit register `B`.                   |
| `0x1D50`  | `MV BW, A`        | 2/2           |           | Moves 32-bit register `A` into 16-bit register `BW`.                  |
| `0x1D51`  | `MV BW, AW`       | 2/2           |           | Moves 16-bit register `AW` into `BW`.                                 |
| `0x1D52`  | `MV BW, AH`       | 2/2           |           | Moves 8-bit register `AH` into 16-bit register `BW`.                  |
| `0x1D53`  | `MV BW, AL`       | 2/2           |           | Moves 8-bit register `AL` into 16-bit register `BW`.                  |
| `0x1D54`  | `MV BW, B`        | 2/2           |           | Moves 32-bit register `B` into 16-bit register `BW`.                  |
| `0x1D55`  | `MV BW, BW`       | 2/2           |           | Moves 16-bit register `BW` into `BW`.                                 |
| `0x1D56`  | `MV BW, BH`       | 2/2           |           | Moves 8-bit register `BH` into 16-bit register `BW`.                  |
| `0x1D57`  | `MV BW, BL`       | 2/2           |           | Moves 8-bit register `BL` into 16-bit register `BW`.                  |
| `0x1D58`  | `MV BW, C`        | 2/2           |           | Moves 32-bit register `C` into 16-bit register `BW`.                  |
| `0x1D59`  | `MV BW, CW`       | 2/2           |           | Moves 16-bit register `CW` into `BW`.                                 |
| `0x1D5A`  | `MV BW, CH`       | 2/2           |           | Moves 8-bit register `CH` into 16-bit register `BW`.                  |
| `0x1D5B`  | `MV BW, CL`       | 2/2           |           | Moves 8-bit register `CL` into 16-bit register `BW`.                  |
| `0x1D5C`  | `MV BW, D`        | 2/2           |           | Moves 32-bit register `D` into 16-bit register `BW`.                  |
| `0x1D5D`  | `MV BW, DW`       | 2/2           |           | Moves 16-bit register `DW` into `BW`.                                 |
| `0x1D5E`  | `MV BW, DH`       | 2/2           |           | Moves 8-bit register `DH` into 16-bit register `BW`.                  |
| `0x1D5F`  | `MV BW, DL`       | 2/2           |           | Moves 8-bit register `DL` into 16-bit register `BW`.                  |
| `0x1D60`  | `MV BH, A`        | 2/2           |           | Moves 32-bit register `A` into 8-bit register `BH`.                   |
| `0x1D61`  | `MV BH, AW`       | 2/2           |           | Moves 16-bit register `AW` into 8-bit register `BH`.                  |
| `0x1D62`  | `MV BH, AH`       | 2/2           |           | Moves 8-bit register `AH` into `BH`.                                  |
| `0x1D63`  | `MV BH, AL`       | 2/2           |           | Moves 8-bit register `AL` into `BH`.                                  |
| `0x1D64`  | `MV BH, B`        | 2/2           |           | Moves 32-bit register `B` into 8-bit register `BH`.                   |
| `0x1D65`  | `MV BH, BW`       | 2/2           |           | Moves 16-bit register `BW` into 8-bit register `BH`.                  |
| `0x1D66`  | `MV BH, BH`       | 2/2           |           | Moves 8-bit register `BH` into `BH`.                                  |
| `0x1D67`  | `MV BH, BL`       | 2/2           |           | Moves 8-bit register `BL` into `BH`.                                  |
| `0x1D68`  | `MV BH, C`        | 2/2           |           | Moves 32-bit register `C` into 8-bit register `BH`.                   |
| `0x1D69`  | `MV BH, CW`       | 2/2           |           | Moves 16-bit register `CW` into 8-bit register `BH`.                  |
| `0x1D6A`  | `MV BH, CH`       | 2/2           |           | Moves 8-bit register `CH` into `BH`.                                  |
| `0x1D6B`  | `MV BH, CL`       | 2/2           |           | Moves 8-bit register `CL` into `BH`.                                  |
| `0x1D6C`  | `MV BH, D`        | 2/2           |           | Moves 32-bit register `D` into 8-bit register `BH`.                   |
| `0x1D6D`  | `MV BH, DW`       | 2/2           |           | Moves 16-bit register `DW` into 8-bit register `BH`.                  |
| `0x1D6E`  | `MV BH, DH`       | 2/2           |           | Moves 8-bit register `DH` into `BH`.                                  |
| `0x1D6F`  | `MV BH, DL`       | 2/2           |           | Moves 8-bit register `DL` into `BH`.                                  |
| `0x1D70`  | `MV BL, A`        | 2/2           |           | Moves 32-bit register `A` into 8-bit register `BL`.                   |
| `0x1D71`  | `MV BL, AW`       | 2/2           |           | Moves 16-bit register `AW` into 8-bit register `BL`.                  |
| `0x1D72`  | `MV BL, AH`       | 2/2           |           | Moves 8-bit register `AH` into `BL`.                                  |
| `0x1D73`  | `MV BL, AL`       | 2/2           |           | Moves 8-bit register `AL` into `BL`.                                  |
| `0x1D74`  | `MV BL, B`        | 2/2           |           | Moves 32-bit register `B` into 8-bit register `BL`.                   |
| `0x1D75`  | `MV BL, BW`       | 2/2           |           | Moves 16-bit register `BW` into 8-bit register `BL`.                  |
| `0x1D76`  | `MV BL, BH`       | 2/2           |           | Moves 8-bit register `BH` into `BL`.                                  |
| `0x1D77`  | `MV BL, BL`       | 2/2           |           | Moves 8-bit register `BL` into `BL`.                                  |
| `0x1D78`  | `MV BL, C`        | 2/2           |           | Moves 32-bit register `C` into 8-bit register `BL`.                   |
| `0x1D79`  | `MV BL, CW`       | 2/2           |           | Moves 16-bit register `CW` into 8-bit register `BL`.                  |
| `0x1D7A`  | `MV BL, CH`       | 2/2           |           | Moves 8-bit register `CH` into `BL`.                                  |
| `0x1D7B`  | `MV BL, CL`       | 2/2           |           | Moves 8-bit register `CL` into `BL`.                                  |
| `0x1D7C`  | `MV BL, D`        | 2/2           |           | Moves 32-bit register `D` into 8-bit register `BL`.                   |
| `0x1D7D`  | `MV BL, DW`       | 2/2           |           | Moves 16-bit register `DW` into 8-bit register `BL`.                  |
| `0x1D7E`  | `MV BL, DH`       | 2/2           |           | Moves 8-bit register `DH` into `BL`.                                  |
| `0x1D7F`  | `MV BL, DL`       | 2/2           |           | Moves 8-bit register `DL` into `BL`.                                  |
| `0x1D80`  | `MV C, A`         | 2/2           |           | Moves 32-bit register `A` into `C`.                                   |
| `0x1D81`  | `MV C, AW`        | 2/2           |           | Moves 16-bit register `AW` into 32-bit register `C`.                  |
| `0x1D82`  | `MV C, AH`        | 2/2           |           | Moves 8-bit register `AH` into 32-bit register `C`.                   |
| `0x1D83`  | `MV C, AL`        | 2/2           |           | Moves 8-bit register `AL` into 32-bit register `C`.                   |
| `0x1D84`  | `MV C, B`         | 2/2           |           | Moves 32-bit register `B` into `C`.                                   |
| `0x1D85`  | `MV C, BW`        | 2/2           |           | Moves 16-bit register `BW` into 32-bit register `C`.                  |
| `0x1D86`  | `MV C, BH`        | 2/2           |           | Moves 8-bit register `BH` into 32-bit register `C`.                   |
| `0x1D87`  | `MV C, BL`        | 2/2           |           | Moves 8-bit register `BL` into 32-bit register `C`.                   |
| `0x1D88`  | `MV C, C`         | 2/2           |           | Moves 32-bit register `C` into `C`.                                   |
| `0x1D89`  | `MV C, CW`        | 2/2           |           | Moves 16-bit register `CW` into 32-bit register `C`.                  |
| `0x1D8A`  | `MV C, CH`        | 2/2           |           | Moves 8-bit register `CH` into 32-bit register `C`.                   |
| `0x1D8B`  | `MV C, CL`        | 2/2           |           | Moves 8-bit register `CL` into 32-bit register `C`.                   |
| `0x1D8C`  | `MV C, D`         | 2/2           |           | Moves 32-bit register `D` into `C`.                                   |
| `0x1D8D`  | `MV C, DW`        | 2/2           |           | Moves 16-bit register `DW` into 32-bit register `C`.                  |
| `0x1D8E`  | `MV C, DH`        | 2/2           |           | Moves 8-bit register `DH` into 32-bit register `C`.                   |
| `0x1D8F`  | `MV C, DL`        | 2/2           |           | Moves 8-bit register `DL` into 32-bit register `C`.                   |
| `0x1D90`  | `MV CW, A`        | 2/2           |           | Moves 32-bit register `A` into 16-bit register `CW`.                  |
| `0x1D91`  | `MV CW, AW`       | 2/2           |           | Moves 16-bit register `AW` into `CW`.                                 |
| `0x1D92`  | `MV CW, AH`       | 2/2           |           | Moves 8-bit register `AH` into 16-bit register `CW`.                  |
| `0x1D93`  | `MV CW, AL`       | 2/2           |           | Moves 8-bit register `AL` into 16-bit register `CW`.                  |
| `0x1D94`  | `MV CW, B`        | 2/2           |           | Moves 32-bit register `B` into 16-bit register `CW`.                  |
| `0x1D95`  | `MV CW, BW`       | 2/2           |           | Moves 16-bit register `BW` into `CW`.                                 |
| `0x1D96`  | `MV CW, BH`       | 2/2           |           | Moves 8-bit register `BH` into 16-bit register `CW`.                  |
| `0x1D97`  | `MV CW, BL`       | 2/2           |           | Moves 8-bit register `BL` into 16-bit register `CW`.                  |
| `0x1D98`  | `MV CW, C`        | 2/2           |           | Moves 32-bit register `C` into 16-bit register `CW`.                  |
| `0x1D99`  | `MV CW, CW`       | 2/2           |           | Moves 16-bit register `CW` into `CW`.                                 |
| `0x1D9A`  | `MV CW, CH`       | 2/2           |           | Moves 8-bit register `CH` into 16-bit register `CW`.                  |
| `0x1D9B`  | `MV CW, CL`       | 2/2           |           | Moves 8-bit register `CL` into 16-bit register `CW`.                  |
| `0x1D9C`  | `MV CW, D`        | 2/2           |           | Moves 32-bit register `D` into 16-bit register `CW`.                  |
| `0x1D9D`  | `MV CW, DW`       | 2/2           |           | Moves 16-bit register `DW` into `CW`.                                 |
| `0x1D9E`  | `MV CW, DH`       | 2/2           |           | Moves 8-bit register `DH` into 16-bit register `CW`.                  |
| `0x1D9F`  | `MV CW, DL`       | 2/2           |           | Moves 8-bit register `DL` into 16-bit register `CW`.                  |
| `0x1DA0`  | `MV CH, A`        | 2/2           |           | Moves 32-bit register `A` into 8-bit register `CH`.                   |
| `0x1DA1`  | `MV CH, AW`       | 2/2           |           | Moves 16-bit register `AW` into 8-bit register `CH`.                  |
| `0x1DA2`  | `MV CH, AH`       | 2/2           |           | Moves 8-bit register `AH` into `CH`.                                  |
| `0x1DA3`  | `MV CH, AL`       | 2/2           |           | Moves 8-bit register `AL` into `CH`.                                  |
| `0x1DA4`  | `MV CH, B`        | 2/2           |           | Moves 32-bit register `B` into 8-bit register `CH`.                   |
| `0x1DA5`  | `MV CH, BW`       | 2/2           |           | Moves 16-bit register `BW` into 8-bit register `CH`.                  |
| `0x1DA6`  | `MV CH, BH`       | 2/2           |           | Moves 8-bit register `BH` into `CH`.                                  |
| `0x1DA7`  | `MV CH, BL`       | 2/2           |           | Moves 8-bit register `BL` into `CH`.                                  |
| `0x1DA8`  | `MV CH, C`        | 2/2           |           | Moves 32-bit register `C` into 8-bit register `CH`.                   |
| `0x1DA9`  | `MV CH, CW`       | 2/2           |           | Moves 16-bit register `CW` into 8-bit register `CH`.                  |
| `0x1DAA`  | `MV CH, CH`       | 2/2           |           | Moves 8-bit register `CH` into `CH`.                                  |
| `0x1DAB`  | `MV CH, CL`       | 2/2           |           | Moves 8-bit register `CL` into `CH`.                                  |
| `0x1DAC`  | `MV CH, D`        | 2/2           |           | Moves 32-bit register `D` into 8-bit register `CH`.                   |
| `0x1DAD`  | `MV CH, DW`       | 2/2           |           | Moves 16-bit register `DW` into 8-bit register `CH`.                  |
| `0x1DAE`  | `MV CH, DH`       | 2/2           |           | Moves 8-bit register `DH` into `CH`.                                  |
| `0x1DAF`  | `MV CH, DL`       | 2/2           |           | Moves 8-bit register `DL` into `CH`.                                  |
| `0x1DB0`  | `MV CL, A`        | 2/2           |           | Moves 32-bit register `A` into 8-bit register `CL`.                   |
| `0x1DB1`  | `MV CL, AW`       | 2/2           |           | Moves 16-bit register `AW` into 8-bit register `CL`.                  |
| `0x1DB2`  | `MV CL, AH`       | 2/2           |           | Moves 8-bit register `AH` into `CL`.                                  |
| `0x1DB3`  | `MV CL, AL`       | 2/2           |           | Moves 8-bit register `AL` into `CL`.                                  |
| `0x1DB4`  | `MV CL, B`        | 2/2           |           | Moves 32-bit register `B` into 8-bit register `CL`.                   |
| `0x1DB5`  | `MV CL, BW`       | 2/2           |           | Moves 16-bit register `BW` into 8-bit register `CL`.                  |
| `0x1DB6`  | `MV CL, BH`       | 2/2           |           | Moves 8-bit register `BH` into `CL`.                                  |
| `0x1DB7`  | `MV CL, BL`       | 2/2           |           | Moves 8-bit register `BL` into `CL`.                                  |
| `0x1DB8`  | `MV CL, C`        | 2/2           |           | Moves 32-bit register `C` into 8-bit register `CL`.                   |
| `0x1DB9`  | `MV CL, CW`       | 2/2           |           | Moves 16-bit register `CW` into 8-bit register `CL`.                  |
| `0x1DBA`  | `MV CL, CH`       | 2/2           |           | Moves 8-bit register `CH` into `CL`.                                  |
| `0x1DBB`  | `MV CL, CL`       | 2/2           |           | Moves 8-bit register `CL` into `CL`.                                  |
| `0x1DBC`  | `MV CL, D`        | 2/2           |           | Moves 32-bit register `D` into 8-bit register `CL`.                   |
| `0x1DBD`  | `MV CL, DW`       | 2/2           |           | Moves 16-bit register `DW` into 8-bit register `CL`.                  |
| `0x1DBE`  | `MV CL, DH`       | 2/2           |           | Moves 8-bit register `DH` into `CL`.                                  |
| `0x1DBF`  | `MV CL, DL`       | 2/2           |           | Moves 8-bit register `DL` into `CL`.                                  |
| `0x1DC0`  | `MV D, A`         | 2/2           |           | Moves 32-bit register `A` into `D`.                                   |
| `0x1DC1`  | `MV D, AW`        | 2/2           |           | Moves 16-bit register `AW` into 32-bit register `D`.                  |
| `0x1DC2`  | `MV D, AH`        | 2/2           |           | Moves 8-bit register `AH` into 32-bit register `D`.                   |
| `0x1DC3`  | `MV D, AL`        | 2/2           |           | Moves 8-bit register `AL` into 32-bit register `D`.                   |
| `0x1DC4`  | `MV D, B`         | 2/2           |           | Moves 32-bit register `B` into `D`.                                   |
| `0x1DC5`  | `MV D, BW`        | 2/2           |           | Moves 16-bit register `BW` into 32-bit register `D`.                  |
| `0x1DC6`  | `MV D, BH`        | 2/2           |           | Moves 8-bit register `BH` into 32-bit register `D`.                   |
| `0x1DC7`  | `MV D, BL`        | 2/2           |           | Moves 8-bit register `BL` into 32-bit register `D`.                   |
| `0x1DC8`  | `MV D, C`         | 2/2           |           | Moves 32-bit register `C` into `D`.                                   |
| `0x1DC9`  | `MV D, CW`        | 2/2           |           | Moves 16-bit register `CW` into 32-bit register `D`.                  |
| `0x1DCA`  | `MV D, CH`        | 2/2           |           | Moves 8-bit register `CH` into 32-bit register `D`.                   |
| `0x1DCB`  | `MV D, CL`        | 2/2           |           | Moves 8-bit register `CL` into 32-bit register `D`.                   |
| `0x1DCC`  | `MV D, D`         | 2/2           |           | Moves 32-bit register `D` into `D`.                                   |
| `0x1DCD`  | `MV D, DW`        | 2/2           |           | Moves 16-bit register `DW` into 32-bit register `D`.                  |
| `0x1DCE`  | `MV D, DH`        | 2/2           |           | Moves 8-bit register `DH` into 32-bit register `D`.                   |
| `0x1DCF`  | `MV D, DL`        | 2/2           |           | Moves 8-bit register `DL` into 32-bit register `D`.                   |
| `0x1DD0`  | `MV DW, A`        | 2/2           |           | Moves 32-bit register `A` into 16-bit register `DW`.                  |
| `0x1DD1`  | `MV DW, AW`       | 2/2           |           | Moves 16-bit register `AW` into `DW`.                                 |
| `0x1DD2`  | `MV DW, AH`       | 2/2           |           | Moves 8-bit register `AH` into 16-bit register `DW`.                  |
| `0x1DD3`  | `MV DW, AL`       | 2/2           |           | Moves 8-bit register `AL` into 16-bit register `DW`.                  |
| `0x1DD4`  | `MV DW, B`        | 2/2           |           | Moves 32-bit register `B` into 16-bit register `DW`.                  |
| `0x1DD5`  | `MV DW, BW`       | 2/2           |           | Moves 16-bit register `BW` into `DW`.                                 |
| `0x1DD6`  | `MV DW, BH`       | 2/2           |           | Moves 8-bit register `BH` into 16-bit register `DW`.                  |
| `0x1DD7`  | `MV DW, BL`       | 2/2           |           | Moves 8-bit register `BL` into 16-bit register `DW`.                  |
| `0x1DD8`  | `MV DW, C`        | 2/2           |           | Moves 32-bit register `C` into 16-bit register `DW`.                  |
| `0x1DD9`  | `MV DW, CW`       | 2/2           |           | Moves 16-bit register `CW` into `DW`.                                 |
| `0x1DDA`  | `MV DW, CH`       | 2/2           |           | Moves 8-bit register `CH` into 16-bit register `DW`.                  |
| `0x1DDB`  | `MV DW, CL`       | 2/2           |           | Moves 8-bit register `CL` into 16-bit register `DW`.                  |
| `0x1DDC`  | `MV DW, D`        | 2/2           |           | Moves 32-bit register `D` into 16-bit register `DW`.                  |
| `0x1DDD`  | `MV DW, DW`       | 2/2           |           | Moves 16-bit register `DW` into `DW`.                                 |
| `0x1DDE`  | `MV DW, DH`       | 2/2           |           | Moves 8-bit register `DH` into 16-bit register `DW`.                  |
| `0x1DDF`  | `MV DW, DL`       | 2/2           |           | Moves 8-bit register `DL` into 16-bit register `DW`.                  |
| `0x1DE0`  | `MV DH, A`        | 2/2           |           | Moves 32-bit register `A` into 8-bit register `DH`.                   |
| `0x1DE1`  | `MV DH, AW`       | 2/2           |           | Moves 16-bit register `AW` into 8-bit register `DH`.                  |
| `0x1DE2`  | `MV DH, AH`       | 2/2           |           | Moves 8-bit register `AH` into `DH`.                                  |
| `0x1DE3`  | `MV DH, AL`       | 2/2           |           | Moves 8-bit register `AL` into `DH`.                                  |
| `0x1DE4`  | `MV DH, B`        | 2/2           |           | Moves 32-bit register `B` into 8-bit register `DH`.                   |
| `0x1DE5`  | `MV DH, BW`       | 2/2           |           | Moves 16-bit register `BW` into 8-bit register `DH`.                  |
| `0x1DE6`  | `MV DH, BH`       | 2/2           |           | Moves 8-bit register `BH` into `DH`.                                  |
| `0x1DE7`  | `MV DH, BL`       | 2/2           |           | Moves 8-bit register `BL` into `DH`.                                  |
| `0x1DE8`  | `MV DH, C`        | 2/2           |           | Moves 32-bit register `C` into 8-bit register `DH`.                   |
| `0x1DE9`  | `MV DH, CW`       | 2/2           |           | Moves 16-bit register `CW` into 8-bit register `DH`.                  |
| `0x1DEA`  | `MV DH, CH`       | 2/2           |           | Moves 8-bit register `CH` into `DH`.                                  |
| `0x1DEB`  | `MV DH, CL`       | 2/2           |           | Moves 8-bit register `CL` into `DH`.                                  |
| `0x1DEC`  | `MV DH, D`        | 2/2           |           | Moves 32-bit register `D` into 8-bit register `DH`.                   |
| `0x1DED`  | `MV DH, DW`       | 2/2           |           | Moves 16-bit register `DW` into 8-bit register `DH`.                  |
| `0x1DEE`  | `MV DH, DH`       | 2/2           |           | Moves 8-bit register `DH` into `DH`.                                  |
| `0x1DEF`  | `MV DH, DL`       | 2/2           |           | Moves 8-bit register `DL` into `DH`.                                  |
| `0x1DF0`  | `MV DL, A`        | 2/2           |           | Moves 32-bit register `A` into 8-bit register `DL`.                   |
| `0x1DF1`  | `MV DL, AW`       | 2/2           |           | Moves 16-bit register `AW` into 8-bit register `DL`.                  |
| `0x1DF2`  | `MV DL, AH`       | 2/2           |           | Moves 8-bit register `AH` into `DL`.                                  |
| `0x1DF3`  | `MV DL, AL`       | 2/2           |           | Moves 8-bit register `AL` into `DL`.                                  |
| `0x1DF4`  | `MV DL, B`        | 2/2           |           | Moves 32-bit register `B` into 8-bit register `DL`.                   |
| `0x1DF5`  | `MV DL, BW`       | 2/2           |           | Moves 16-bit register `BW` into 8-bit register `DL`.                  |
| `0x1DF6`  | `MV DL, BH`       | 2/2           |           | Moves 8-bit register `BH` into `DL`.                                  |
| `0x1DF7`  | `MV DL, BL`       | 2/2           |           | Moves 8-bit register `BL` into `DL`.                                  |
| `0x1DF8`  | `MV DL, C`        | 2/2           |           | Moves 32-bit register `C` into 8-bit register `DL`.                   |
| `0x1DF9`  | `MV DL, CW`       | 2/2           |           | Moves 16-bit register `CW` into 8-bit register `DL`.                  |
| `0x1DFA`  | `MV DL, CH`       | 2/2           |           | Moves 8-bit register `CH` into `DL`.                                  |
| `0x1DFB`  | `MV DL, CL`       | 2/2           |           | Moves 8-bit register `CL` into `DL`.                                  |
| `0x1DFC`  | `MV DL, D`        | 2/2           |           | Moves 32-bit register `D` into 8-bit register `DL`.                   |
| `0x1DFD`  | `MV DL, DW`       | 2/2           |           | Moves 16-bit register `DW` into 8-bit register `DL`.                  |
| `0x1DFE`  | `MV DL, DH`       | 2/2           |           | Moves 8-bit register `DH` into `DL`.                                  |
| `0x1DFF`  | `MV DL, DL`       | 2/2           |           | Moves 8-bit register `DL` into `DL`.                                  |
| `0x1E00`  | `POP A`           | 2/7           |           | Pops a 32-bit value from the stack into `A`.                          |
| `0x1E40`  | `POP B`           | 2/7           |           | Pops a 32-bit value from the stack into `B`.                          |
| `0x1E80`  | `POP C`           | 2/7           |           | Pops a 32-bit value from the stack into `C`.                          |
| `0x1EC0`  | `POP D`           | 2/7           |           | Pops a 32-bit value from the stack into `D`.                          |
| `0x1F00`  | `PUSH A`          | 2/7           |           | Pushes 32-bit register `A` onto the stack.                            |
| `0x1F04`  | `PUSH B`          | 2/7           |           | Pushes 32-bit register `B` onto the stack.                            |
| `0x1F08`  | `PUSH C`          | 2/7           |           | Pushes 32-bit register `C` onto the stack.                            |
| `0x1F0C`  | `PUSH D`          | 2/7           |           | Pushes 32-bit register `D` onto the stack.                            |
| `0x2000`  | `JMP NC, IMM32`   | 6/7           |           | Unconditionally jumps to 32-bit immediate address.                    |
| `0x2010`  | `JMP ZC, IMM32`   | 6/6 (+1)      |           | Jumps to 32-bit immediate address if Zero flag is clear.              |
| `0x2020`  | `JMP CC, IMM32`   | 6/6 (+1)      |           | Jumps to 32-bit immediate address if Carry flag is clear.             |
| `0x2030`  | `JMP PC, IMM32`   | 6/6 (+1)      |           | Jumps to 32-bit immediate address if Parity flag is clear.            |
| `0x2040`  | `JMP SC, IMM32`   | 6/6 (+1)      |           | Jumps to 32-bit immediate address if Sign flag is clear.              |
| `0x2090`  | `JMP ZS, IMM32`   | 6/6 (+1)      |           | Jumps to 32-bit immediate address if Zero flag is set.                |
| `0x20A0`  | `JMP CS, IMM32`   | 6/6 (+1)      |           | Jumps to 32-bit immediate address if Carry flag is set.               |
| `0x20B0`  | `JMP PS, IMM32`   | 6/6 (+1)      |           | Jumps to 32-bit immediate address if Parity flag is set.              |
| `0x20C0`  | `JMP SS, IMM32`   | 6/6 (+1)      |           | Jumps to 32-bit immediate address if Sign flag is set.                |
| `0x2100`  | `JMP NC, A`       | 2/3           |           | Unconditionally jumps to address in `A`.                              |
| `0x2110`  | `JMP ZC, A`       | 2/2 (+1)      |           | Jumps to address in `A` if Zero flag is clear.                        |
| `0x2120`  | `JMP CC, A`       | 2/2 (+1)      |           | Jumps to address in `A` if Carry flag is clear.                       |
| `0x2130`  | `JMP PC, A`       | 2/2 (+1)      |           | Jumps to address in `A` if Parity flag is clear.                      |
| `0x2140`  | `JMP SC, A`       | 2/2 (+1)      |           | Jumps to address in `A` if Sign flag is clear.                        |
| `0x2190`  | `JMP ZS, A`       | 2/2 (+1)      |           | Jumps to address in `A` if Zero flag is set.                          |
| `0x21A0`  | `JMP CS, A`       | 2/2 (+1)      |           | Jumps to address in `A` if Carry flag is set.                         |
| `0x21B0`  | `JMP PS, A`       | 2/2 (+1)      |           | Jumps to address in `A` if Parity flag is set.                        |
| `0x21C0`  | `JMP SS, A`       | 2/2 (+1)      |           | Jumps to address in `A` if Sign flag is set.                          |
| `0x2104`  | `JMP NC, B`       | 2/3           |           | Unconditionally jumps to address in `B`.                              |
| `0x2114`  | `JMP ZC, B`       | 2/2 (+1)      |           | Jumps to address in `B` if Zero flag is clear.                        |
| `0x2124`  | `JMP CC, B`       | 2/2 (+1)      |           | Jumps to address in `B` if Carry flag is clear.                       |
| `0x2134`  | `JMP PC, B`       | 2/2 (+1)      |           | Jumps to address in `B` if Parity flag is clear.                      |
| `0x2144`  | `JMP SC, B`       | 2/2 (+1)      |           | Jumps to address in `B` if Sign flag is clear.                        |
| `0x2194`  | `JMP ZS, B`       | 2/2 (+1)      |           | Jumps to address in `B` if Zero flag is set.                          |
| `0x21A4`  | `JMP CS, B`       | 2/2 (+1)      |           | Jumps to address in `B` if Carry flag is set.                         |
| `0x21B4`  | `JMP PS, B`       | 2/2 (+1)      |           | Jumps to address in `B` if Parity flag is set.                        |
| `0x21C4`  | `JMP SS, B`       | 2/2 (+1)      |           | Jumps to address in `B` if Sign flag is set.                          |
| `0x2108`  | `JMP NC, C`       | 2/3           |           | Unconditionally jumps to address in `C`.                              |
| `0x2118`  | `JMP ZC, C`       | 2/2 (+1)      |           | Jumps to address in `C` if Zero flag is clear.                        |
| `0x2128`  | `JMP CC, C`       | 2/2 (+1)      |           | Jumps to address in `C` if Carry flag is clear.                       |
| `0x2138`  | `JMP PC, C`       | 2/2 (+1)      |           | Jumps to address in `C` if Parity flag is clear.                      |
| `0x2148`  | `JMP SC, C`       | 2/2 (+1)      |           | Jumps to address in `C` if Sign flag is clear.                        |
| `0x2198`  | `JMP ZS, C`       | 2/2 (+1)      |           | Jumps to address in `C` if Zero flag is set.                          |
| `0x21A8`  | `JMP CS, C`       | 2/2 (+1)      |           | Jumps to address in `C` if Carry flag is set.                         |
| `0x21B8`  | `JMP PS, C`       | 2/2 (+1)      |           | Jumps to address in `C` if Parity flag is set.                        |
| `0x21C8`  | `JMP SS, C`       | 2/2 (+1)      |           | Jumps to address in `C` if Sign flag is set.                          |
| `0x210C`  | `JMP NC, D`       | 2/3           |           | Unconditionally jumps to address in `D`.                              |
| `0x211C`  | `JMP ZC, D`       | 2/2 (+1)      |           | Jumps to address in `D` if Zero flag is clear.                        |
| `0x212C`  | `JMP CC, D`       | 2/2 (+1)      |           | Jumps to address in `D` if Carry flag is clear.                       |
| `0x213C`  | `JMP PC, D`       | 2/2 (+1)      |           | Jumps to address in `D` if Parity flag is clear.                      |
| `0x214C`  | `JMP SC, D`       | 2/2 (+1)      |           | Jumps to address in `D` if Sign flag is clear.                        |
| `0x219C`  | `JMP ZS, D`       | 2/2 (+1)      |           | Jumps to address in `D` if Zero flag is set.                          |
| `0x21AC`  | `JMP CS, D`       | 2/2 (+1)      |           | Jumps to address in `D` if Carry flag is set.                         |
| `0x21BC`  | `JMP PS, D`       | 2/2 (+1)      |           | Jumps to address in `D` if Parity flag is set.                        |
| `0x21CC`  | `JMP SS, D`       | 2/2 (+1)      |           | Jumps to address in `D` if Sign flag is set.                          |
| `0x2200`  | `JPB NC, SIMM16`  | 4/5           |           | Unconditionally moves PC by signed 16-bit offset.                     |
| `0x2210`  | `JPB ZC, SIMM16`  | 4/4 (+1)      |           | Moves PC by signed 16-bit offset if Zero flag is clear.               |
| `0x2220`  | `JPB CC, SIMM16`  | 4/4 (+1)      |           | Moves PC by signed 16-bit offset if Carry flag is clear.              |
| `0x2230`  | `JPB PC, SIMM16`  | 4/4 (+1)      |           | Moves PC by signed 16-bit offset if Parity flag is clear.             |
| `0x2240`  | `JPB SC, SIMM16`  | 4/4 (+1)      |           | Moves PC by signed 16-bit offset if Sign flag is clear.               |
| `0x2290`  | `JPB ZS, SIMM16`  | 4/4 (+1)      |           | Moves PC by signed 16-bit offset if Zero flag is set.                 |
| `0x22A0`  | `JPB CS, SIMM16`  | 4/4 (+1)      |           | Moves PC by signed 16-bit offset if Carry flag is set.                |
| `0x22B0`  | `JPB PS, SIMM16`  | 4/4 (+1)      |           | Moves PC by signed 16-bit offset if Parity flag is set.               |
| `0x22C0`  | `JPB SS, SIMM16`  | 4/4 (+1)      |           | Moves PC by signed 16-bit offset if Sign flag is set.                 |
| `0x2300`  | `CALL NC, IMM32`  | 6/12          |           | Unconditionally calls subroutine at 32-bit immediate address.         |
| `0x2310`  | `CALL ZC, IMM32`  | 6/6 (+6)      |           | Calls subroutine at 32-bit immediate address if Zero flag is clear.   |
| `0x2320`  | `CALL CC, IMM32`  | 6/6 (+6)      |           | Calls subroutine at 32-bit immediate address if Carry flag is clear.  |
| `0x2330`  | `CALL PC, IMM32`  | 6/6 (+6)      |           | Calls subroutine at 32-bit immediate address if Parity flag is clear. |
| `0x2340`  | `CALL SC, IMM32`  | 6/6 (+6)      |           | Calls subroutine at 32-bit immediate address if Sign flag is clear.   |
| `0x2390`  | `CALL ZS, IMM32`  | 6/6 (+6)      |           | Calls subroutine at 32-bit immediate address if Zero flag is set.     |
| `0x23A0`  | `CALL CS, IMM32`  | 6/6 (+6)      |           | Calls subroutine at 32-bit immediate address if Carry flag is set.    |
| `0x23B0`  | `CALL PS, IMM32`  | 6/6 (+6)      |           | Calls subroutine at 32-bit immediate address if Parity flag is set.   |
| `0x23C0`  | `CALL SS, IMM32`  | 6/6 (+6)      |           | Calls subroutine at 32-bit immediate address if Sign flag is set.     |
| `0x2400`  | `INT 0`           | 2/8           |           | Calls interrupt vector 0.                                             |
| `0x2401`  | `INT 1`           | 2/8           |           | Calls interrupt vector 1.                                             |
| `0x2402`  | `INT 2`           | 2/8           |           | Calls interrupt vector 2.                                             |
| `0x2403`  | `INT 3`           | 2/8           |           | Calls interrupt vector 3.                                             |
| `0x2404`  | `INT 4`           | 2/8           |           | Calls interrupt vector 4.                                             |
| `0x2405`  | `INT 5`           | 2/8           |           | Calls interrupt vector 5.                                             |
| `0x2406`  | `INT 6`           | 2/8           |           | Calls interrupt vector 6.                                             |
| `0x2407`  | `INT 7`           | 2/8           |           | Calls interrupt vector 7.                                             |
| `0x2408`  | `INT 8`           | 2/8           |           | Calls interrupt vector 8.                                             |
| `0x2409`  | `INT 9`           | 2/8           |           | Calls interrupt vector 9.                                             |
| `0x240A`  | `INT 10`          | 2/8           |           | Calls interrupt vector 10.                                            |
| `0x240B`  | `INT 11`          | 2/8           |           | Calls interrupt vector 11.                                            |
| `0x240C`  | `INT 12`          | 2/8           |           | Calls interrupt vector 12.                                            |
| `0x240D`  | `INT 13`          | 2/8           |           | Calls interrupt vector 13.                                            |
| `0x240E`  | `INT 14`          | 2/8           |           | Calls interrupt vector 14.                                            |
| `0x240F`  | `INT 15`          | 2/8           |           | Calls interrupt vector 15.                                            |
| `0x2410`  | `INT 16`          | 2/8           |           | Calls interrupt vector 16.                                            |
| `0x2411`  | `INT 17`          | 2/8           |           | Calls interrupt vector 17.                                            |
| `0x2412`  | `INT 18`          | 2/8           |           | Calls interrupt vector 18.                                            |
| `0x2413`  | `INT 19`          | 2/8           |           | Calls interrupt vector 19.                                            |
| `0x2414`  | `INT 20`          | 2/8           |           | Calls interrupt vector 20.                                            |
| `0x2415`  | `INT 21`          | 2/8           |           | Calls interrupt vector 21.                                            |
| `0x2416`  | `INT 22`          | 2/8           |           | Calls interrupt vector 22.                                            |
| `0x2417`  | `INT 23`          | 2/8           |           | Calls interrupt vector 23.                                            |
| `0x2418`  | `INT 24`          | 2/8           |           | Calls interrupt vector 24.                                            |
| `0x2419`  | `INT 25`          | 2/8           |           | Calls interrupt vector 25.                                            |
| `0x241A`  | `INT 26`          | 2/8           |           | Calls interrupt vector 26.                                            |
| `0x241B`  | `INT 27`          | 2/8           |           | Calls interrupt vector 27.                                            |
| `0x241C`  | `INT 28`          | 2/8           |           | Calls interrupt vector 28.                                            |
| `0x241D`  | `INT 29`          | 2/8           |           | Calls interrupt vector 29.                                            |
| `0x241E`  | `INT 30`          | 2/8           |           | Calls interrupt vector 30.                                            |
| `0x241F`  | `INT 31`          | 2/8           |           | Calls interrupt vector 31.                                            |
| `0x2500`  | `RET NC`          | 2/8           |           | Unconditionally returns from subroutine.                              |
| `0x2510`  | `RET ZC`          | 2/2 (+6)      |           | Returns from subroutine if Zero flag is clear.                        |
| `0x2520`  | `RET CC`          | 2/2 (+6)      |           | Returns from subroutine if Carry flag is clear.                       |
| `0x2530`  | `RET PC`          | 2/2 (+6)      |           | Returns from subroutine if Parity flag is clear.                      |
| `0x2540`  | `RET SC`          | 2/2 (+6)      |           | Returns from subroutine if Sign flag is clear.                        |
| `0x2590`  | `RET ZS`          | 2/2 (+6)      |           | Returns from subroutine if Zero flag is set.                          |
| `0x25A0`  | `RET CS`          | 2/2 (+6)      |           | Returns from subroutine if Carry flag is set.                         |
| `0x25B0`  | `RET PS`          | 2/2 (+6)      |           | Returns from subroutine if Parity flag is set.                        |
| `0x25C0`  | `RET SS`          | 2/2 (+6)      |           | Returns from subroutine if Sign flag is set.                          |
| `0x2600`  | `RETI`            | 2/8           |           | Enables interrupts, then returns from subroutine.                     |
| `0x3000`  | `INC A`           | 2/2           | `?0?--?`  | Increments 32-bit register `A` by one.                                |
| `0x3010`  | `INC AW`          | 2/2           | `?0?--?`  | Increments 16-bit register `AW` by one.                               |
| `0x3020`  | `INC AH`          | 2/2           | `?0?--?`  | Increments 8-bit register `AH` by one.                                |
| `0x3030`  | `INC AL`          | 2/2           | `?0?--?`  | Increments 8-bit register `AL` by one.                                |
| `0x3040`  | `INC B`           | 2/2           | `?0?--?`  | Increments 32-bit register `B` by one.                                |
| `0x3050`  | `INC BW`          | 2/2           | `?0?--?`  | Increments 16-bit register `BW` by one.                               |
| `0x3060`  | `INC BH`          | 2/2           | `?0?--?`  | Increments 8-bit register `BH` by one.                                |
| `0x3070`  | `INC BL`          | 2/2           | `?0?--?`  | Increments 8-bit register `BL` by one.                                |
| `0x3080`  | `INC C`           | 2/2           | `?0?--?`  | Increments 32-bit register `C` by one.                                |
| `0x3090`  | `INC CW`          | 2/2           | `?0?--?`  | Increments 16-bit register `CW` by one.                               |
| `0x30A0`  | `INC CH`          | 2/2           | `?0?--?`  | Increments 8-bit register `CH` by one.                                |
| `0x30B0`  | `INC CL`          | 2/2           | `?0?--?`  | Increments 8-bit register `CL` by one.                                |
| `0x30C0`  | `INC D`           | 2/2           | `?0?--?`  | Increments 32-bit register `D` by one.                                |
| `0x30D0`  | `INC DW`          | 2/2           | `?0?--?`  | Increments 16-bit register `DW` by one.                               |
| `0x30E0`  | `INC DH`          | 2/2           | `?0?--?`  | Increments 8-bit register `DH` by one.                                |
| `0x30F0`  | `INC DL`          | 2/2           | `?0?--?`  | Increments 8-bit register `DL` by one.                                |
| `0x3100`  | `INC [A]`         | 2/4           | `?0?--?`  | Increments byte at address in `A` by one.                             |
| `0x3140`  | `INC [B]`         | 2/4           | `?0?--?`  | Increments byte at address in `B` by one.                             |
| `0x3180`  | `INC [C]`         | 2/4           | `?0?--?`  | Increments byte at address in `C` by one.                             |
| `0x31C0`  | `INC [D]`         | 2/4           | `?0?--?`  | Increments byte at address in `D` by one.                             |
| `0x3200`  | `DEC A`           | 2/2           | `?1?--?`  | Decrements 32-bit register `A` by one.                                |
| `0x3210`  | `DEC AW`          | 2/2           | `?1?--?`  | Decrements 16-bit register `AW` by one.                               |
| `0x3220`  | `DEC AH`          | 2/2           | `?1?--?`  | Decrements 8-bit register `AH` by one.                                |
| `0x3230`  | `DEC AL`          | 2/2           | `?1?--?`  | Decrements 8-bit register `AL` by one.                                |
| `0x3240`  | `DEC B`           | 2/2           | `?1?--?`  | Decrements 32-bit register `B` by one.                                |
| `0x3250`  | `DEC BW`          | 2/2           | `?1?--?`  | Decrements 16-bit register `BW` by one.                               |
| `0x3260`  | `DEC BH`          | 2/2           | `?1?--?`  | Decrements 8-bit register `BH` by one.                                |
| `0x3270`  | `DEC BL`          | 2/2           | `?1?--?`  | Decrements 8-bit register `BL` by one.                                |
| `0x3280`  | `DEC C`           | 2/2           | `?1?--?`  | Decrements 32-bit register `C` by one.                                |
| `0x3290`  | `DEC CW`          | 2/2           | `?1?--?`  | Decrements 16-bit register `CW` by one.                               |
| `0x32A0`  | `DEC CH`          | 2/2           | `?1?--?`  | Decrements 8-bit register `CH` by one.                                |
| `0x32B0`  | `DEC CL`          | 2/2           | `?1?--?`  | Decrements 8-bit register `CL` by one.                                |
| `0x32C0`  | `DEC D`           | 2/2           | `?1?--?`  | Decrements 32-bit register `D` by one.                                |
| `0x32D0`  | `DEC DW`          | 2/2           | `?1?--?`  | Decrements 16-bit register `DW` by one.                               |
| `0x32E0`  | `DEC DH`          | 2/2           | `?1?--?`  | Decrements 8-bit register `DH` by one.                                |
| `0x32F0`  | `DEC DL`          | 2/2           | `?1?--?`  | Decrements 8-bit register `DL` by one.                                |
| `0x3300`  | `DEC [A]`         | 2/4           | `?1?--?`  | Decrements byte at address in `A` by one.                             |
| `0x3340`  | `DEC [B]`         | 2/4           | `?1?--?`  | Decrements byte at address in `B` by one.                             |
| `0x3380`  | `DEC [C]`         | 2/4           | `?1?--?`  | Decrements byte at address in `C` by one.                             |
| `0x33C0`  | `DEC [D]`         | 2/4           | `?1?--?`  | Decrements byte at address in `D` by one.                             |
| `0x3400`  | `ADD A, IMM32`    | 6/6           | `?0??0?`  | Adds 32-bit immediate value to `A`.                                   |
| `0x3410`  | `ADD AW, IMM16`   | 4/4           | `?0??0?`  | Adds 16-bit immediate value to `AW`.                                  |
| `0x3420`  | `ADD AH, IMM8`    | 2/2           | `?0??0?`  | Adds 8-bit immediate value to `AH`.                                   |
| `0x3430`  | `ADD AL, IMM8`    | 2/2           | `?0??0?`  | Adds 8-bit immediate value to `AL`.                                   |
| `0x3500`  | `ADD A, [ADDR32]` | 6/10          | `?0??0?`  | Adds value at 32-bit address to `A`.                                  |
| `0x3510`  | `ADD AW, [ADDR32]`| 6/8           | `?0??0?`  | Adds value at 32-bit address to `AW`.                                 |
| `0x3520`  | `ADD AH, [ADDR32]`| 6/7           | `?0??0?`  | Adds value at 32-bit address to `AH`.                                 |
| `0x3530`  | `ADD AL, [ADDR32]`| 6/7           | `?0??0?`  | Adds value at 32-bit address to `AL`.                                 |
| `0x3600`  | `ADD A, [A]`      | 2/6           | `?0??0?`  | Adds value at address in `A` to `A`.                                  |
| `0x3610`  | `ADD AW, [A]`     | 2/4           | `?0??0?`  | Adds value at address in `A` to `AW`.                                 |
| `0x3620`  | `ADD AH, [A]`     | 2/3           | `?0??0?`  | Adds value at address in `A` to `AH`.                                 |
| `0x3630`  | `ADD AL, [A]`     | 2/3           | `?0??0?`  | Adds value at address in `A` to `AL`.                                 |
| `0x3604`  | `ADD A, [B]`      | 2/6           | `?0??0?`  | Adds value at address in `B` to `A`.                                  |
| `0x3614`  | `ADD AW, [B]`     | 2/4           | `?0??0?`  | Adds value at address in `B` to `AW`.                                 |
| `0x3624`  | `ADD AH, [B]`     | 2/3           | `?0??0?`  | Adds value at address in `B` to `AH`.                                 |
| `0x3634`  | `ADD AL, [B]`     | 2/3           | `?0??0?`  | Adds value at address in `B` to `AL`.                                 |
| `0x3608`  | `ADD A, [C]`      | 2/6           | `?0??0?`  | Adds value at address in `C` to `A`.                                  |
| `0x3618`  | `ADD AW, [C]`     | 2/4           | `?0??0?`  | Adds value at address in `C` to `AW`.                                 |
| `0x3628`  | `ADD AH, [C]`     | 2/3           | `?0??0?`  | Adds value at address in `C` to `AH`.                                 |
| `0x3638`  | `ADD AL, [C]`     | 2/3           | `?0??0?`  | Adds value at address in `C` to `AL`.                                 |
| `0x360C`  | `ADD A, [D]`      | 2/6           | `?0??0?`  | Adds value at address in `D` to `A`.                                  |
| `0x361C`  | `ADD AW, [D]`     | 2/4           | `?0??0?`  | Adds value at address in `D` to `AW`.                                 |
| `0x362C`  | `ADD AH, [D]`     | 2/3           | `?0??0?`  | Adds value at address in `D` to `AH`.                                 |
| `0x363C`  | `ADD AL, [D]`     | 2/3           | `?0??0?`  | Adds value at address in `D` to `AL`.                                 |
| `0x3700`  | `ADC A, IMM32`    | 6/6           | `?0??0?`  | Adds 32-bit immediate value and Carry flag to `A`.                    |
| `0x3710`  | `ADC AW, IMM16`   | 4/4           | `?0??0?`  | Adds 16-bit immediate value and Carry flag to `AW`.                   |
| `0x3720`  | `ADC AH, IMM8`    | 2/2           | `?0??0?`  | Adds 8-bit immediate value and Carry flag to `AH`.                    |
| `0x3730`  | `ADC AL, IMM8`    | 2/2           | `?0??0?`  | Adds 8-bit immediate value and Carry flag to `AL`.                    |
| `0x3800`  | `ADC A, [ADDR32]` | 6/10          | `?0??0?`  | Adds value at 32-bit address and Carry flag to `A`.                   |
| `0x3810`  | `ADC AW, [ADDR32]`| 6/8           | `?0??0?`  | Adds value at 32-bit address and Carry flag to `AW`.                  |
| `0x3820`  | `ADC AH, [ADDR32]`| 6/7           | `?0??0?`  | Adds value at 32-bit address and Carry flag to `AH`.                  |
| `0x3830`  | `ADC AL, [ADDR32]`| 6/7           | `?0??0?`  | Adds value at 32-bit address and Carry flag to `AL`.                  |
| `0x3900`  | `ADC A, [A]`      | 2/6           | `?0??0?`  | Adds value at address in `A` and Carry flag to `A`.                   |
| `0x3910`  | `ADC AW, [A]`     | 2/4           | `?0??0?`  | Adds value at address in `A` and Carry flag to `AW`.                  |
| `0x3920`  | `ADC AH, [A]`     | 2/3           | `?0??0?`  | Adds value at address in `A` and Carry flag to `AH`.                  |
| `0x3930`  | `ADC AL, [A]`     | 2/3           | `?0??0?`  | Adds value at address in `A` and Carry flag to `AL`.                  |
| `0x3904`  | `ADC A, [B]`      | 2/6           | `?0??0?`  | Adds value at address in `B` and Carry flag to `A`.                   |
| `0x3914`  | `ADC AW, [B]`     | 2/4           | `?0??0?`  | Adds value at address in `B` and Carry flag to `AW`.                  |
| `0x3924`  | `ADC AH, [B]`     | 2/3           | `?0??0?`  | Adds value at address in `B` and Carry flag to `AH`.                  |
| `0x3934`  | `ADC AL, [B]`     | 2/3           | `?0??0?`  | Adds value at address in `B` and Carry flag to `AL`.                  |
| `0x3908`  | `ADC A, [C]`      | 2/6           | `?0??0?`  | Adds value at address in `C` and Carry flag to `A`.                   |
| `0x3918`  | `ADC AW, [C]`     | 2/4           | `?0??0?`  | Adds value at address in `C` and Carry flag to `AW`.                  |
| `0x3928`  | `ADC AH, [C]`     | 2/3           | `?0??0?`  | Adds value at address in `C` and Carry flag to `AH`.                  |
| `0x3938`  | `ADC AL, [C]`     | 2/3           | `?0??0?`  | Adds value at address in `C` and Carry flag to `AL`.                  |
| `0x390C`  | `ADC A, [D]`      | 2/6           | `?0??0?`  | Adds value at address in `D` and Carry flag to `A`.                   |
| `0x391C`  | `ADC AW, [D]`     | 2/4           | `?0??0?`  | Adds value at address in `D` and Carry flag to `AW`.                  |
| `0x392C`  | `ADC AH, [D]`     | 2/3           | `?0??0?`  | Adds value at address in `D` and Carry flag to `AH`.                  |
| `0x393C`  | `ADC AL, [D]`     | 2/3           | `?0??0?`  | Adds value at address in `D` and Carry flag to `AL`.                  |
| `0x3A00`  | `SUB A, IMM32`    | 6/6           | `?1????`  | Subtracts 32-bit immediate value from `A`.                            |
| `0x3A10`  | `SUB AW, IMM16`   | 4/4           | `?1????`  | Subtracts 16-bit immediate value from `AW`.                           |
| `0x3A20`  | `SUB AH, IMM8`    | 2/2           | `?1????`  | Subtracts 8-bit immediate value from `AH`.                            |
| `0x3A30`  | `SUB AL, IMM8`    | 2/2           | `?1????`  | Subtracts 8-bit immediate value from `AL`.                            |
| `0x3B00`  | `SUB A, [ADDR32]` | 6/10          | `?1????`  | Subtracts value at 32-bit address from `A`.                           |
| `0x3B10`  | `SUB AW, [ADDR32]`| 6/8           | `?1????`  | Subtracts value at 32-bit address from `AW`.                          |
| `0x3B20`  | `SUB AH, [ADDR32]`| 6/7           | `?1????`  | Subtracts value at 32-bit address from `AH`.                          |
| `0x3B30`  | `SUB AL, [ADDR32]`| 6/7           | `?1????`  | Subtracts value at 32-bit address from `AL`.                          |
| `0x3C00`  | `SUB A, [A]`      | 2/6           | `?1????`  | Subtracts value at address in `A` from `A`.                           |
| `0x3C10`  | `SUB AW, [A]`     | 2/4           | `?1????`  | Subtracts value at address in `A` from `AW`.                          |
| `0x3C20`  | `SUB AH, [A]`     | 2/3           | `?1????`  | Subtracts value at address in `A` from `AH`.                          |
| `0x3C30`  | `SUB AL, [A]`     | 2/3           | `?1????`  | Subtracts value at address in `A` from `AL`.                          |
| `0x3C04`  | `SUB A, [B]`      | 2/6           | `?1????`  | Subtracts value at address in `B` from `A`.                           |
| `0x3C14`  | `SUB AW, [B]`     | 2/4           | `?1????`  | Subtracts value at address in `B` from `AW`.                          |
| `0x3C24`  | `SUB AH, [B]`     | 2/3           | `?1????`  | Subtracts value at address in `B` from `AH`.                          |
| `0x3C34`  | `SUB AL, [B]`     | 2/3           | `?1????`  | Subtracts value at address in `B` from `AL`.                          |
| `0x3C08`  | `SUB A, [C]`      | 2/6           | `?1????`  | Subtracts value at address in `C` from `A`.                           |
| `0x3C18`  | `SUB AW, [C]`     | 2/4           | `?1????`  | Subtracts value at address in `C` from `AW`.                          |
| `0x3C28`  | `SUB AH, [C]`     | 2/3           | `?1????`  | Subtracts value at address in `C` from `AH`.                          |
| `0x3C38`  | `SUB AL, [C]`     | 2/3           | `?1????`  | Subtracts value at address in `C` from `AL`.                          |
| `0x3C0C`  | `SUB A, [D]`      | 2/6           | `?1????`  | Subtracts value at address in `D` from `A`.                           |
| `0x3C1C`  | `SUB AW, [D]`     | 2/4           | `?1????`  | Subtracts value at address in `D` from `AW`.                          |
| `0x3C2C`  | `SUB AH, [D]`     | 2/3           | `?1????`  | Subtracts value at address in `D` from `AH`.                          |
| `0x3C3C`  | `SUB AL, [D]`     | 2/3           | `?1????`  | Subtracts value at address in `D` from `AL`.                          |
| `0x3D00`  | `SBC A, IMM32`    | 6/6           | `?1????`  | Subtracts 32-bit immediate value and Carry flag from `A`.             |
| `0x3D10`  | `SBC AW, IMM16`   | 4/4           | `?1????`  | Subtracts 16-bit immediate value and Carry flag from `AW`.            |
| `0x3D20`  | `SBC AH, IMM8`    | 2/2           | `?1????`  | Subtracts 8-bit immediate value and Carry flag from `AH`.             |
| `0x3D30`  | `SBC AL, IMM8`    | 2/2           | `?1????`  | Subtracts 8-bit immediate value and Carry flag from `AL`.             |
| `0x3E00`  | `SBC A, [ADDR32]` | 6/10          | `?1????`  | Subtracts value at 32-bit address and Carry flag from `A`.            |
| `0x3E10`  | `SBC AW, [ADDR32]`| 6/8           | `?1????`  | Subtracts value at 32-bit address and Carry flag from `AW`.           |
| `0x3E20`  | `SBC AH, [ADDR32]`| 6/7           | `?1????`  | Subtracts value at 32-bit address and Carry flag from `AH`.           |
| `0x3E30`  | `SBC AL, [ADDR32]`| 6/7           | `?1????`  | Subtracts value at 32-bit address and Carry flag from `AL`.           |
| `0x3F00`  | `SBC A, [A]`      | 2/6           | `?1????`  | Subtracts value at address in `A` and Carry flag from `A`.            |
| `0x3F10`  | `SBC AW, [A]`     | 2/4           | `?1????`  | Subtracts value at address in `A` and Carry flag from `AW`.           |
| `0x3F20`  | `SBC AH, [A]`     | 2/3           | `?1????`  | Subtracts value at address in `A` and Carry flag from `AH`.           |
| `0x3F30`  | `SBC AL, [A]`     | 2/3           | `?1????`  | Subtracts value at address in `A` and Carry flag from `AL`.           |
| `0x3F04`  | `SBC A, [B]`      | 2/6           | `?1????`  | Subtracts value at address in `B` and Carry flag from `A`.            |
| `0x3F14`  | `SBC AW, [B]`     | 2/4           | `?1????`  | Subtracts value at address in `B` and Carry flag from `AW`.           |
| `0x3F24`  | `SBC AH, [B]`     | 2/3           | `?1????`  | Subtracts value at address in `B` and Carry flag from `AH`.           |
| `0x3F34`  | `SBC AL, [B]`     | 2/3           | `?1????`  | Subtracts value at address in `B` and Carry flag from `AL`.           |
| `0x3F08`  | `SBC A, [C]`      | 2/6           | `?1????`  | Subtracts value at address in `C` and Carry flag from `A`.            |
| `0x3F18`  | `SBC AW, [C]`     | 2/4           | `?1????`  | Subtracts value at address in `C` and Carry flag from `AW`.           |
| `0x3F28`  | `SBC AH, [C]`     | 2/3           | `?1????`  | Subtracts value at address in `C` and Carry flag from `AH`.           |
| `0x3F38`  | `SBC AL, [C]`     | 2/3           | `?1????`  | Subtracts value at address in `C` and Carry flag from `AL`.           |
| `0x3F0C`  | `SBC A, [D]`      | 2/6           | `?1????`  | Subtracts value at address in `D` and Carry flag from `A`.            |
| `0x3F1C`  | `SBC AW, [D]`     | 2/4           | `?1????`  | Subtracts value at address in `D` and Carry flag from `AW`.           |
| `0x3F2C`  | `SBC AH, [D]`     | 2/3           | `?1????`  | Subtracts value at address in `D` and Carry flag from `AH`.           |
| `0x3F3C`  | `SBC AL, [D]`     | 2/3           | `?1????`  | Subtracts value at address in `D` and Carry flag from `AL`.           |
| `0x4000`  | `AND A, IMM32`    | 6/6           | `?0100?`  | Bitwise ANDs 32-bit immediate value with `A`.                         |
| `0x4010`  | `AND AW, IMM16`   | 4/4           | `?0100?`  | Bitwise ANDs 16-bit immediate value with `AW`.                        |
| `0x4020`  | `AND AH, IMM8`    | 2/2           | `?0100?`  | Bitwise ANDs 8-bit immediate value with `AH`.                         |
| `0x4030`  | `AND AL, IMM8`    | 2/2           | `?0100?`  | Bitwise ANDs 8-bit immediate value with `AL`.                         |
| `0x4100`  | `AND A, [ADDR32]` | 6/10          | `?0100?`  | Bitwise ANDs value at 32-bit address with `A`.                        |
| `0x4110`  | `AND AW, [ADDR32]`| 6/8           | `?0100?`  | Bitwise ANDs value at 32-bit address with `AW`.                       |
| `0x4120`  | `AND AH, [ADDR32]`| 6/7           | `?0100?`  | Bitwise ANDs value at 32-bit address with `AH`.                       |
| `0x4130`  | `AND AL, [ADDR32]`| 6/7           | `?0100?`  | Bitwise ANDs value at 32-bit address with `AL`.                       |
| `0x4200`  | `AND A, [A]`      | 2/6           | `?0100?`  | Bitwise ANDs value at address in `A` with `A`.                        |
| `0x4210`  | `AND AW, [A]`     | 2/4           | `?0100?`  | Bitwise ANDs value at address in `A` with `AW`.                       |
| `0x4220`  | `AND AH, [A]`     | 2/3           | `?0100?`  | Bitwise ANDs value at address in `A` with `AH`.                       |
| `0x4230`  | `AND AL, [A]`     | 2/3           | `?0100?`  | Bitwise ANDs value at address in `A` with `AL`.                       |
| `0x4204`  | `AND A, [B]`      | 2/6           | `?0100?`  | Bitwise ANDs value at address in `B` with `A`.                        |
| `0x4214`  | `AND AW, [B]`     | 2/4           | `?0100?`  | Bitwise ANDs value at address in `B` with `AW`.                       |
| `0x4224`  | `AND AH, [B]`     | 2/3           | `?0100?`  | Bitwise ANDs value at address in `B` with `AH`.                       |
| `0x4234`  | `AND AL, [B]`     | 2/3           | `?0100?`  | Bitwise ANDs value at address in `B` with `AL`.                       |
| `0x4208`  | `AND A, [C]`      | 2/6           | `?0100?`  | Bitwise ANDs value at address in `C` with `A`.                        |
| `0x4218`  | `AND AW, [C]`     | 2/4           | `?0100?`  | Bitwise ANDs value at address in `C` with `AW`.                       |
| `0x4228`  | `AND AH, [C]`     | 2/3           | `?0100?`  | Bitwise ANDs value at address in `C` with `AH`.                       |
| `0x4238`  | `AND AL, [C]`     | 2/3           | `?0100?`  | Bitwise ANDs value at address in `C` with `AL`.                       |
| `0x420C`  | `AND A, [D]`      | 2/6           | `?0100?`  | Bitwise ANDs value at address in `D` with `A`.                        |
| `0x421C`  | `AND AW, [D]`     | 2/4           | `?0100?`  | Bitwise ANDs value at address in `D` with `AW`.                       |
| `0x422C`  | `AND AH, [D]`     | 2/3           | `?0100?`  | Bitwise ANDs value at address in `D` with `AH`.                       |
| `0x423C`  | `AND AL, [D]`     | 2/3           | `?0100?`  | Bitwise ANDs value at address in `D` with `AL`.                       |
| `0x4300`  | `OR A, IMM32`     | 6/6           | `?0000?`  | Bitwise ORs 32-bit immediate value with `A`.                          |
| `0x4310`  | `OR AW, IMM16`    | 4/4           | `?0000?`  | Bitwise ORs 16-bit immediate value with `AW`.                         |
| `0x4320`  | `OR AH, IMM8`     | 2/2           | `?0000?`  | Bitwise ORs 8-bit immediate value with `AH`.                          |
| `0x4330`  | `OR AL, IMM8`     | 2/2           | `?0000?`  | Bitwise ORs 8-bit immediate value with `AL`.                          |
| `0x4400`  | `OR A, [ADDR32]`  | 6/10          | `?0000?`  | Bitwise ORs value at 32-bit address with `A`.                         |
| `0x4410`  | `OR AW, [ADDR32]` | 6/8           | `?0000?`  | Bitwise ORs value at 32-bit address with `AW`.                        |
| `0x4420`  | `OR AH, [ADDR32]` | 6/7           | `?0000?`  | Bitwise ORs value at 32-bit address with `AH`.                        |
| `0x4430`  | `OR AL, [ADDR32]` | 6/7           | `?0000?`  | Bitwise ORs value at 32-bit address with `AL`.                        |
| `0x4500`  | `OR A, [A]`       | 2/6           | `?0000?`  | Bitwise ORs value at address in `A` with `A`.                         |
| `0x4510`  | `OR AW, [A]`      | 2/4           | `?0000?`  | Bitwise ORs value at address in `A` with `AW`.                        |
| `0x4520`  | `OR AH, [A]`      | 2/3           | `?0000?`  | Bitwise ORs value at address in `A` with `AH`.                        |
| `0x4530`  | `OR AL, [A]`      | 2/3           | `?0000?`  | Bitwise ORs value at address in `A` with `AL`.                        |
| `0x4504`  | `OR A, [B]`       | 2/6           | `?0000?`  | Bitwise ORs value at address in `B` with `A`.                         |
| `0x4514`  | `OR AW, [B]`      | 2/4           | `?0000?`  | Bitwise ORs value at address in `B` with `AW`.                        |
| `0x4524`  | `OR AH, [B]`      | 2/3           | `?0000?`  | Bitwise ORs value at address in `B` with `AH`.                        |
| `0x4534`  | `OR AL, [B]`      | 2/3           | `?0000?`  | Bitwise ORs value at address in `B` with `AL`.                        |
| `0x4508`  | `OR A, [C]`       | 2/6           | `?0000?`  | Bitwise ORs value at address in `C` with `A`.                         |
| `0x4518`  | `OR AW, [C]`      | 2/4           | `?0000?`  | Bitwise ORs value at address in `C` with `AW`.                        |
| `0x4528`  | `OR AH, [C]`      | 2/3           | `?0000?`  | Bitwise ORs value at address in `C` with `AH`.                        |
| `0x4538`  | `OR AL, [C]`      | 2/3           | `?0000?`  | Bitwise ORs value at address in `C` with `AL`.                        |
| `0x450C`  | `OR A, [D]`       | 2/6           | `?0000?`  | Bitwise ORs value at address in `D` with `A`.                         |
| `0x451C`  | `OR AW, [D]`      | 2/4           | `?0000?`  | Bitwise ORs value at address in `D` with `AW`.                        |
| `0x452C`  | `OR AH, [D]`      | 2/3           | `?0000?`  | Bitwise ORs value at address in `D` with `AH`.                        |
| `0x453C`  | `OR AL, [D]`      | 2/3           | `?0000?`  | Bitwise ORs value at address in `D` with `AL`.                        |
| `0x4600`  | `XOR A, IMM32`    | 6/6           | `?0000?`  | Bitwise XORs 32-bit immediate value with `A`.                         |
| `0x4610`  | `XOR AW, IMM16`   | 4/4           | `?0000?`  | Bitwise XORs 16-bit immediate value with `AW`.                        |
| `0x4620`  | `XOR AH, IMM8`    | 2/2           | `?0000?`  | Bitwise XORs 8-bit immediate value with `AH`.                         |
| `0x4630`  | `XOR AL, IMM8`    | 2/2           | `?0000?`  | Bitwise XORs 8-bit immediate value with `AL`.                         |
| `0x4700`  | `XOR A, [ADDR32]` | 6/10          | `?0000?`  | Bitwise XORs value at 32-bit address with `A`.                        |
| `0x4710`  | `XOR AW, [ADDR32]`| 6/8           | `?0000?`  | Bitwise XORs value at 32-bit address with `AW`.                       |
| `0x4720`  | `XOR AH, [ADDR32]`| 6/7           | `?0000?`  | Bitwise XORs value at 32-bit address with `AH`.                       |
| `0x4730`  | `XOR AL, [ADDR32]`| 6/7           | `?0000?`  | Bitwise XORs value at 32-bit address with `AL`.                       |
| `0x4800`  | `XOR A, [A]`      | 2/6           | `?0000?`  | Bitwise XORs value at address in `A` with `A`.                        |
| `0x4810`  | `XOR AW, [A]`     | 2/4           | `?0000?`  | Bitwise XORs value at address in `A` with `AW`.                       |
| `0x4820`  | `XOR AH, [A]`     | 2/3           | `?0000?`  | Bitwise XORs value at address in `A` with `AH`.                       |
| `0x4830`  | `XOR AL, [A]`     | 2/3           | `?0000?`  | Bitwise XORs value at address in `A` with `AL`.                       |
| `0x4804`  | `XOR A, [B]`      | 2/6           | `?0000?`  | Bitwise XORs value at address in `B` with `A`.                        |
| `0x4814`  | `XOR AW, [B]`     | 2/4           | `?0000?`  | Bitwise XORs value at address in `B` with `AW`.                       |
| `0x4824`  | `XOR AH, [B]`     | 2/3           | `?0000?`  | Bitwise XORs value at address in `B` with `AH`.                       |
| `0x4834`  | `XOR AL, [B]`     | 2/3           | `?0000?`  | Bitwise XORs value at address in `B` with `AL`.                       |
| `0x4808`  | `XOR A, [C]`      | 2/6           | `?0000?`  | Bitwise XORs value at address in `C` with `A`.                        |
| `0x4818`  | `XOR AW, [C]`     | 2/4           | `?0000?`  | Bitwise XORs value at address in `C` with `AW`.                       |
| `0x4828`  | `XOR AH, [C]`     | 2/3           | `?0000?`  | Bitwise XORs value at address in `C` with `AH`.                       |
| `0x4838`  | `XOR AL, [C]`     | 2/3           | `?0000?`  | Bitwise XORs value at address in `C` with `AL`.                       |
| `0x480C`  | `XOR A, [D]`      | 2/6           | `?0000?`  | Bitwise XORs value at address in `D` with `A`.                        |
| `0x481C`  | `XOR AW, [D]`     | 2/4           | `?0000?`  | Bitwise XORs value at address in `D` with `AW`.                       |
| `0x482C`  | `XOR AH, [D]`     | 2/3           | `?0000?`  | Bitwise XORs value at address in `D` with `AH`.                       |
| `0x483C`  | `XOR AL, [D]`     | 2/3           | `?0000?`  | Bitwise XORs value at address in `D` with `AL`.                       |
| `0x4C00`  | `NOT A`           | 2/2           | `-11---`  | Bitwise NOTs `A`.                                                     |
| `0x4C10`  | `NOT AW`          | 2/2           | `-11---`  | Bitwise NOTs `AW`.                                                    |
| `0x4C20`  | `NOT AH`          | 2/2           | `-11---`  | Bitwise NOTs `AH`.                                                    |
| `0x4C30`  | `NOT AL`          | 2/2           | `-11---`  | Bitwise NOTs `AL`.                                                    |
| `0x4C40`  | `NOT B`           | 2/2           | `-11---`  | Bitwise NOTs `B`.                                                     |
| `0x4C50`  | `NOT BW`          | 2/2           | `-11---`  | Bitwise NOTs `BW`.                                                    |
| `0x4C60`  | `NOT BH`          | 2/2           | `-11---`  | Bitwise NOTs `BH`.                                                    |
| `0x4C70`  | `NOT BL`          | 2/2           | `-11---`  | Bitwise NOTs `BL`.                                                    |
| `0x4C80`  | `NOT C`           | 2/2           | `-11---`  | Bitwise NOTs `C`.                                                     |
| `0x4C90`  | `NOT CW`          | 2/2           | `-11---`  | Bitwise NOTs `CW`.                                                    |
| `0x4CA0`  | `NOT CH`          | 2/2           | `-11---`  | Bitwise NOTs `CH`.                                                    |
| `0x4CB0`  | `NOT CL`          | 2/2           | `-11---`  | Bitwise NOTs `CL`.                                                    |
| `0x4CC0`  | `NOT D`           | 2/2           | `-11---`  | Bitwise NOTs `D`.                                                     |
| `0x4CD0`  | `NOT DW`          | 2/2           | `-11---`  | Bitwise NOTs `DW`.                                                    |
| `0x4CE0`  | `NOT DH`          | 2/2           | `-11---`  | Bitwise NOTs `DH`.                                                    |
| `0x4CF0`  | `NOT DL`          | 2/2           | `-11---`  | Bitwise NOTs `DL`.                                                    |
| `0x4D00`  | `NOT [A]`         | 2/4           | `-11---`  | Bitwise NOTs byte at address in `A`.                                  |
| `0x4D10`  | `NOT [B]`         | 2/4           | `-11---`  | Bitwise NOTs byte at address in `B`.                                  |
| `0x4D20`  | `NOT [C]`         | 2/4           | `-11---`  | Bitwise NOTs byte at address in `C`.                                  |
| `0x4D30`  | `NOT [D]`         | 2/4           | `-11---`  | Bitwise NOTs byte at address in `D`.                                  |
| `0x5000`  | `CMP A, IMM32`    | 6/6           | `?1???-`  | Compares `A` with 32-bit immediate value.                             |
| `0x5010`  | `CMP AW, IMM16`   | 4/4           | `?1???-`  | Compares `AW` with 16-bit immediate value.                            |
| `0x5020`  | `CMP AH, IMM8`    | 2/2           | `?1???-`  | Compares `AH` with 8-bit immediate value.                             |
| `0x5030`  | `CMP AL, IMM8`    | 2/2           | `?1???-`  | Compares `AL` with 8-bit immediate value.                             |
| `0x5100`  | `CMP A, [ADDR32]` | 6/10          | `?1???-`  | Compares `A` with value at 32-bit address.                            |
| `0x5110`  | `CMP AW, [ADDR32]`| 6/8           | `?1???-`  | Compares `AW` with value at 32-bit address.                           |
| `0x5120`  | `CMP AH, [ADDR32]`| 6/7           | `?1???-`  | Compares `AH` with value at 32-bit address.                           |
| `0x5130`  | `CMP AL, [ADDR32]`| 6/7           | `?1???-`  | Compares `AL` with value at 32-bit address.                           |
| `0x5200`  | `CMP A, [A]`      | 2/6           | `?1???-`  | Compares `A` with value at address in `A`.                            |
| `0x5210`  | `CMP AW, [A]`     | 2/4           | `?1???-`  | Compares `AW` with value at address in `A`.                           |
| `0x5220`  | `CMP AH, [A]`     | 2/3           | `?1???-`  | Compares `AH` with value at address in `A`.                           |
| `0x5230`  | `CMP AL, [A]`     | 2/3           | `?1???-`  | Compares `AL` with value at address in `A`.                           |
| `0x5204`  | `CMP A, [B]`      | 2/6           | `?1???-`  | Compares `A` with value at address in `B`.                            |
| `0x5214`  | `CMP AW, [B]`     | 2/4           | `?1???-`  | Compares `AW` with value at address in `B`.                           |
| `0x5224`  | `CMP AH, [B]`     | 2/3           | `?1???-`  | Compares `AH` with value at address in `B`.                           |
| `0x5234`  | `CMP AL, [B]`     | 2/3           | `?1???-`  | Compares `AL` with value at address in `B`.                           |
| `0x5208`  | `CMP A, [C]`      | 2/6           | `?1???-`  | Compares `A` with value at address in `C`.                            |
| `0x5218`  | `CMP AW, [C]`     | 2/4           | `?1???-`  | Compares `AW` with value at address in `C`.                           |
| `0x5228`  | `CMP AH, [C]`     | 2/3           | `?1???-`  | Compares `AH` with value at address in `C`.                           |
| `0x5238`  | `CMP AL, [C]`     | 2/3           | `?1???-`  | Compares `AL` with value at address in `C`.                           |
| `0x520C`  | `CMP A, [D]`      | 2/6           | `?1???-`  | Compares `A` with value at address in `D`.                            |
| `0x521C`  | `CMP AW, [D]`     | 2/4           | `?1???-`  | Compares `AW` with value at address in `D`.                           |
| `0x522C`  | `CMP AH, [D]`     | 2/3           | `?1???-`  | Compares `AH` with value at address in `D`.                           |
| `0x523C`  | `CMP AL, [D]`     | 2/3           | `?1???-`  | Compares `AL` with value at address in `D`.                           |
| `0x6000`  | `SLA A`           | 2/2           | `?00*-?`  | Shifts `A` left arithmetically by 1 bit.                              |
| `0x6010`  | `SLA AW`          | 2/2           | `?00*-?`  | Shifts `AW` left arithmetically by 1 bit.                             |
| `0x6020`  | `SLA AH`          | 2/2           | `?00*-?`  | Shifts `AH` left arithmetically by 1 bit.                             |
| `0x6030`  | `SLA AL`          | 2/2           | `?00*-?`  | Shifts `AL` left arithmetically by 1 bit.                             |
| `0x6040`  | `SLA B`           | 2/2           | `?00*-?`  | Shifts `B` left arithmetically by 1 bit.                              |
| `0x6050`  | `SLA BW`          | 2/2           | `?00*-?`  | Shifts `BW` left arithmetically by 1 bit.                             |
| `0x6060`  | `SLA BH`          | 2/2           | `?00*-?`  | Shifts `BH` left arithmetically by 1 bit.                             |
| `0x6070`  | `SLA BL`          | 2/2           | `?00*-?`  | Shifts `BL` left arithmetically by 1 bit.                             |
| `0x6080`  | `SLA C`           | 2/2           | `?00*-?`  | Shifts `C` left arithmetically by 1 bit.                              |
| `0x6090`  | `SLA CW`          | 2/2           | `?00*-?`  | Shifts `CW` left arithmetically by 1 bit.                             |
| `0x60A0`  | `SLA CH`          | 2/2           | `?00*-?`  | Shifts `CH` left arithmetically by 1 bit.                             |
| `0x60B0`  | `SLA CL`          | 2/2           | `?00*-?`  | Shifts `CL` left arithmetically by 1 bit.                             |
| `0x60C0`  | `SLA D`           | 2/2           | `?00*-?`  | Shifts `D` left arithmetically by 1 bit.                              |
| `0x60D0`  | `SLA DW`          | 2/2           | `?00*-?`  | Shifts `DW` left arithmetically by 1 bit.                             |
| `0x60E0`  | `SLA DH`          | 2/2           | `?00*-?`  | Shifts `DH` left arithmetically by 1 bit.                             |
| `0x60F0`  | `SLA DL`          | 2/2           | `?00*-?`  | Shifts `DL` left arithmetically by 1 bit.                             |
| `0x6100`  | `SLA [A]`         | 2/4           | `?00*-?`  | Shifts byte at address in `A` left arithmetically by 1 bit.           |
| `0x6140`  | `SLA [B]`         | 2/4           | `?00*-?`  | Shifts byte at address in `B` left arithmetically by 1 bit.           |
| `0x6180`  | `SLA [C]`         | 2/4           | `?00*-?`  | Shifts byte at address in `C` left arithmetically by 1 bit.           |
| `0x61C0`  | `SLA [D]`         | 2/4           | `?00*-?`  | Shifts byte at address in `D` left arithmetically by 1 bit.           |
| `0x6200`  | `SRA A`           | 2/2           | `?00*-?`  | Shifts `A` right arithmetically by 1 bit.                             |
| `0x6210`  | `SRA AW`          | 2/2           | `?00*-?`  | Shifts `AW` right arithmetically by 1 bit.                            |
| `0x6220`  | `SRA AH`          | 2/2           | `?00*-?`  | Shifts `AH` right arithmetically by 1 bit.                            |
| `0x6230`  | `SRA AL`          | 2/2           | `?00*-?`  | Shifts `AL` right arithmetically by 1 bit.                            |
| `0x6240`  | `SRA B`           | 2/2           | `?00*-?`  | Shifts `B` right arithmetically by 1 bit.                             |
| `0x6250`  | `SRA BW`          | 2/2           | `?00*-?`  | Shifts `BW` right arithmetically by 1 bit.                            |
| `0x6260`  | `SRA BH`          | 2/2           | `?00*-?`  | Shifts `BH` right arithmetically by 1 bit.                            |
| `0x6270`  | `SRA BL`          | 2/2           | `?00*-?`  | Shifts `BL` right arithmetically by 1 bit.                            |
| `0x6280`  | `SRA C`           | 2/2           | `?00*-?`  | Shifts `C` right arithmetically by 1 bit.                             |
| `0x6290`  | `SRA CW`          | 2/2           | `?00*-?`  | Shifts `CW` right arithmetically by 1 bit.                            |
| `0x62A0`  | `SRA CH`          | 2/2           | `?00*-?`  | Shifts `CH` right arithmetically by 1 bit.                            |
| `0x62B0`  | `SRA CL`          | 2/2           | `?00*-?`  | Shifts `CL` right arithmetically by 1 bit.                            |
| `0x62C0`  | `SRA D`           | 2/2           | `?00*-?`  | Shifts `D` right arithmetically by 1 bit.                             |
| `0x62D0`  | `SRA DW`          | 2/2           | `?00*-?`  | Shifts `DW` right arithmetically by 1 bit.                            |
| `0x62E0`  | `SRA DH`          | 2/2           | `?00*-?`  | Shifts `DH` right arithmetically by 1 bit.                            |
| `0x62F0`  | `SRA DL`          | 2/2           | `?00*-?`  | Shifts `DL` right arithmetically by 1 bit.                            |
| `0x6300`  | `SRA [A]`         | 2/4           | `?00*-?`  | Shifts byte at address in `A` right arithmetically by 1 bit.          |
| `0x6340`  | `SRA [B]`         | 2/4           | `?00*-?`  | Shifts byte at address in `B` right arithmetically by 1 bit.          |
| `0x6380`  | `SRA [C]`         | 2/4           | `?00*-?`  | Shifts byte at address in `C` right arithmetically by 1 bit.          |
| `0x63C0`  | `SRA [D]`         | 2/4           | `?00*-?`  | Shifts byte at address in `D` right arithmetically by 1 bit.          |
| `0x6400`  | `SRL A`           | 2/2           | `?00*-?`  | Shifts `A` right logically by 1 bit.                                  |
| `0x6410`  | `SRL AW`          | 2/2           | `?00*-?`  | Shifts `AW` right logically by 1 bit.                                 |
| `0x6420`  | `SRL AH`          | 2/2           | `?00*-?`  | Shifts `AH` right logically by 1 bit.                                 |
| `0x6430`  | `SRL AL`          | 2/2           | `?00*-?`  | Shifts `AL` right logically by 1 bit.                                 |
| `0x6440`  | `SRL B`           | 2/2           | `?00*-?`  | Shifts `B` right logically by 1 bit.                                  |
| `0x6450`  | `SRL BW`          | 2/2           | `?00*-?`  | Shifts `BW` right logically by 1 bit.                                 |
| `0x6460`  | `SRL BH`          | 2/2           | `?00*-?`  | Shifts `BH` right logically by 1 bit.                                 |
| `0x6470`  | `SRL BL`          | 2/2           | `?00*-?`  | Shifts `BL` right logically by 1 bit.                                 |
| `0x6480`  | `SRL C`           | 2/2           | `?00*-?`  | Shifts `C` right logically by 1 bit.                                  |
| `0x6490`  | `SRL CW`          | 2/2           | `?00*-?`  | Shifts `CW` right logically by 1 bit.                                 |
| `0x64A0`  | `SRL CH`          | 2/2           | `?00*-?`  | Shifts `CH` right logically by 1 bit.                                 |
| `0x64B0`  | `SRL CL`          | 2/2           | `?00*-?`  | Shifts `CL` right logically by 1 bit.                                 |
| `0x64C0`  | `SRL D`           | 2/2           | `?00*-?`  | Shifts `D` right logically by 1 bit.                                  |
| `0x64D0`  | `SRL DW`          | 2/2           | `?00*-?`  | Shifts `DW` right logically by 1 bit.                                 |
| `0x64E0`  | `SRL DH`          | 2/2           | `?00*-?`  | Shifts `DH` right logically by 1 bit.                                 |
| `0x64F0`  | `SRL DL`          | 2/2           | `?00*-?`  | Shifts `DL` right logically by 1 bit.                                 |
| `0x6500`  | `SRL [A]`         | 2/4           | `?00*-?`  | Shifts byte at address in `A` right logically by 1 bit.               |
| `0x6540`  | `SRL [B]`         | 2/4           | `?00*-?`  | Shifts byte at address in `B` right logically by 1 bit.               |
| `0x6580`  | `SRL [C]`         | 2/4           | `?00*-?`  | Shifts byte at address in `C` right logically by 1 bit.               |
| `0x65C0`  | `SRL [D]`         | 2/4           | `?00*-?`  | Shifts byte at address in `D` right logically by 1 bit.               |
| `0x6600`  | `RL A`            | 2/2           | `?00*-?`  | Rotates `A` left through the Carry flag.                              |
| `0x6610`  | `RL AW`           | 2/2           | `?00*-?`  | Rotates `AW` left through the Carry flag.                             |
| `0x6620`  | `RL AH`           | 2/2           | `?00*-?`  | Rotates `AH` left through the Carry flag.                             |
| `0x6630`  | `RL AL`           | 2/2           | `?00*-?`  | Rotates `AL` left through the Carry flag.                             |
| `0x6640`  | `RL B`            | 2/2           | `?00*-?`  | Rotates `B` left through the Carry flag.                              |
| `0x6650`  | `RL BW`           | 2/2           | `?00*-?`  | Rotates `BW` left through the Carry flag.                             |
| `0x6660`  | `RL BH`           | 2/2           | `?00*-?`  | Rotates `BH` left through the Carry flag.                             |
| `0x6670`  | `RL BL`           | 2/2           | `?00*-?`  | Rotates `BL` left through the Carry flag.                             |
| `0x6680`  | `RL C`            | 2/2           | `?00*-?`  | Rotates `C` left through the Carry flag.                              |
| `0x6690`  | `RL CW`           | 2/2           | `?00*-?`  | Rotates `CW` left through the Carry flag.                             |
| `0x66A0`  | `RL CH`           | 2/2           | `?00*-?`  | Rotates `CH` left through the Carry flag.                             |
| `0x66B0`  | `RL CL`           | 2/2           | `?00*-?`  | Rotates `CL` left through the Carry flag.                             |
| `0x66C0`  | `RL D`            | 2/2           | `?00*-?`  | Rotates `D` left through the Carry flag.                              |
| `0x66D0`  | `RL DW`           | 2/2           | `?00*-?`  | Rotates `DW` left through the Carry flag.                             |
| `0x66E0`  | `RL DH`           | 2/2           | `?00*-?`  | Rotates `DH` left through the Carry flag.                             |
| `0x66F0`  | `RL DL`           | 2/2           | `?00*-?`  | Rotates `DL` left through the Carry flag.                             |
| `0x6700`  | `RL [A]`          | 2/4           | `?00*-?`  | Rotates byte at address in `A` left through the Carry flag.           |
| `0x6740`  | `RL [B]`          | 2/4           | `?00*-?`  | Rotates byte at address in `B` left through the Carry flag.           |
| `0x6780`  | `RL [C]`          | 2/4           | `?00*-?`  | Rotates byte at address in `C` left through the Carry flag.           |
| `0x67C0`  | `RL [D]`          | 2/4           | `?00*-?`  | Rotates byte at address in `D` left through the Carry flag.           |
| `0x6800`  | `RLC A`           | 2/2           | `?00*-?`  | Rotates `A` left (circular rotation).                                 |
| `0x6810`  | `RLC AW`          | 2/2           | `?00*-?`  | Rotates `AW` left (circular rotation).                                |
| `0x6820`  | `RLC AH`          | 2/2           | `?00*-?`  | Rotates `AH` left (circular rotation).                                |
| `0x6830`  | `RLC AL`          | 2/2           | `?00*-?`  | Rotates `AL` left (circular rotation).                                |
| `0x6840`  | `RLC B`           | 2/2           | `?00*-?`  | Rotates `B` left (circular rotation).                                 |
| `0x6850`  | `RLC BW`          | 2/2           | `?00*-?`  | Rotates `BW` left (circular rotation).                                |
| `0x6860`  | `RLC BH`          | 2/2           | `?00*-?`  | Rotates `BH` left (circular rotation).                                |
| `0x6870`  | `RLC BL`          | 2/2           | `?00*-?`  | Rotates `BL` left (circular rotation).                                |
| `0x6880`  | `RLC C`           | 2/2           | `?00*-?`  | Rotates `C` left (circular rotation).                                 |
| `0x6890`  | `RLC CW`          | 2/2           | `?00*-?`  | Rotates `CW` left (circular rotation).                                |
| `0x68A0`  | `RLC CH`          | 2/2           | `?00*-?`  | Rotates `CH` left (circular rotation).                                |
| `0x68B0`  | `RLC CL`          | 2/2           | `?00*-?`  | Rotates `CL` left (circular rotation).                                |
| `0x68C0`  | `RLC D`           | 2/2           | `?00*-?`  | Rotates `D` left (circular rotation).                                 |
| `0x68D0`  | `RLC DW`          | 2/2           | `?00*-?`  | Rotates `DW` left (circular rotation).                                |
| `0x68E0`  | `RLC DH`          | 2/2           | `?00*-?`  | Rotates `DH` left (circular rotation).                                |
| `0x68F0`  | `RLC DL`          | 2/2           | `?00*-?`  | Rotates `DL` left (circular rotation).                                |
| `0x6900`  | `RLC [A]`         | 2/4           | `?00*-?`  | Rotates byte at address in `A` left (circular rotation).              |
| `0x6940`  | `RLC [B]`         | 2/4           | `?00*-?`  | Rotates byte at address in `B` left (circular rotation).              |
| `0x6980`  | `RLC [C]`         | 2/4           | `?00*-?`  | Rotates byte at address in `C` left (circular rotation).              |
| `0x69C0`  | `RLC [D]`         | 2/4           | `?00*-?`  | Rotates byte at address in `D` left (circular rotation).              |
| `0x6A00`  | `RR A`            | 2/2           | `?00*-?`  | Rotates `A` right through the Carry flag.                             |
| `0x6A10`  | `RR AW`           | 2/2           | `?00*-?`  | Rotates `AW` right through the Carry flag.                            |
| `0x6A20`  | `RR AH`           | 2/2           | `?00*-?`  | Rotates `AH` right through the Carry flag.                            |
| `0x6A30`  | `RR AL`           | 2/2           | `?00*-?`  | Rotates `AL` right through the Carry flag.                            |
| `0x6A40`  | `RR B`            | 2/2           | `?00*-?`  | Rotates `B` right through the Carry flag.                             |
| `0x6A50`  | `RR BW`           | 2/2           | `?00*-?`  | Rotates `BW` right through the Carry flag.                            |
| `0x6A60`  | `RR BH`           | 2/2           | `?00*-?`  | Rotates `BH` right through the Carry flag.                            |
| `0x6A70`  | `RR BL`           | 2/2           | `?00*-?`  | Rotates `BL` right through the Carry flag.                            |
| `0x6A80`  | `RR C`            | 2/2           | `?00*-?`  | Rotates `C` right through the Carry flag.                             |
| `0x6A90`  | `RR CW`           | 2/2           | `?00*-?`  | Rotates `CW` right through the Carry flag.                            |
| `0x6AA0`  | `RR CH`           | 2/2           | `?00*-?`  | Rotates `CH` right through the Carry flag.                            |
| `0x6AB0`  | `RR CL`           | 2/2           | `?00*-?`  | Rotates `CL` right through the Carry flag.                            |
| `0x6AC0`  | `RR D`            | 2/2           | `?00*-?`  | Rotates `D` right through the Carry flag.                             |
| `0x6AD0`  | `RR DW`           | 2/2           | `?00*-?`  | Rotates `DW` right through the Carry flag.                            |
| `0x6AE0`  | `RR DH`           | 2/2           | `?00*-?`  | Rotates `DH` right through the Carry flag.                            |
| `0x6AF0`  | `RR DL`           | 2/2           | `?00*-?`  | Rotates `DL` right through the Carry flag.                            |
| `0x6B00`  | `RR [A]`          | 2/4           | `?00*-?`  | Rotates byte at address in `A` right through the Carry flag.          |
| `0x6B40`  | `RR [B]`          | 2/4           | `?00*-?`  | Rotates byte at address in `B` right through the Carry flag.          |
| `0x6B80`  | `RR [C]`          | 2/4           | `?00*-?`  | Rotates byte at address in `C` right through the Carry flag.          |
| `0x6BC0`  | `RR [D]`          | 2/4           | `?00*-?`  | Rotates byte at address in `D` right through the Carry flag.          |
| `0x6C00`  | `RRC A`           | 2/2           | `?00*-?`  | Rotates `A` right (circular rotation).                                |
| `0x6C10`  | `RRC AW`          | 2/2           | `?00*-?`  | Rotates `AW` right (circular rotation).                               |
| `0x6C20`  | `RRC AH`          | 2/2           | `?00*-?`  | Rotates `AH` right (circular rotation).                               |
| `0x6C30`  | `RRC AL`          | 2/2           | `?00*-?`  | Rotates `AL` right (circular rotation).                               |
| `0x6C40`  | `RRC B`           | 2/2           | `?00*-?`  | Rotates `B` right (circular rotation).                                |
| `0x6C50`  | `RRC BW`          | 2/2           | `?00*-?`  | Rotates `BW` right (circular rotation).                               |
| `0x6C60`  | `RRC BH`          | 2/2           | `?00*-?`  | Rotates `BH` right (circular rotation).                               |
| `0x6C70`  | `RRC BL`          | 2/2           | `?00*-?`  | Rotates `BL` right (circular rotation).                               |
| `0x6C80`  | `RRC C`           | 2/2           | `?00*-?`  | Rotates `C` right (circular rotation).                                |
| `0x6C90`  | `RRC CW`          | 2/2           | `?00*-?`  | Rotates `CW` right (circular rotation).                               |
| `0x6CA0`  | `RRC CH`          | 2/2           | `?00*-?`  | Rotates `CH` right (circular rotation).                               |
| `0x6CB0`  | `RRC CL`          | 2/2           | `?00*-?`  | Rotates `CL` right (circular rotation).                               |
| `0x6CC0`  | `RRC D`           | 2/2           | `?00*-?`  | Rotates `D` right (circular rotation).                                |
| `0x6CD0`  | `RRC DW`          | 2/2           | `?00*-?`  | Rotates `DW` right (circular rotation).                               |
| `0x6CE0`  | `RRC DH`          | 2/2           | `?00*-?`  | Rotates `DH` right (circular rotation).                               |
| `0x6CF0`  | `RRC DL`          | 2/2           | `?00*-?`  | Rotates `DL` right (circular rotation).                               |
| `0x6D00`  | `RRC [A]`         | 2/4           | `?00*-?`  | Rotates byte at address in `A` right (circular rotation).             |
| `0x6D40`  | `RRC [B]`         | 2/4           | `?00*-?`  | Rotates byte at address in `B` right (circular rotation).             |
| `0x6D80`  | `RRC [C]`         | 2/4           | `?00*-?`  | Rotates byte at address in `C` right (circular rotation).             |
| `0x6DC0`  | `RRC [D]`         | 2/4           | `?00*-?`  | Rotates byte at address in `D` right (circular rotation).             |
| `0x7000`  | `BIT XD, A`       | 3/3           | `*01---`  | Tests bit `XD` of `A`.                                                |
| `0x7010`  | `BIT XD, AW`      | 3/3           | `*01---`  | Tests bit `XD` of `AW`.                                               |
| `0x7020`  | `BIT XD, AH`      | 3/3           | `*01---`  | Tests bit `XD` of `AH`.                                               |
| `0x7030`  | `BIT XD, AL`      | 3/3           | `*01---`  | Tests bit `XD` of `AL`.                                               |
| `0x7040`  | `BIT XD, B`       | 3/3           | `*01---`  | Tests bit `XD` of `B`.                                                |
| `0x7050`  | `BIT XD, BW`      | 3/3           | `*01---`  | Tests bit `XD` of `BW`.                                               |
| `0x7060`  | `BIT XD, BH`      | 3/3           | `*01---`  | Tests bit `XD` of `BH`.                                               |
| `0x7070`  | `BIT XD, BL`      | 3/3           | `*01---`  | Tests bit `XD` of `BL`.                                               |
| `0x7080`  | `BIT XD, C`       | 3/3           | `*01---`  | Tests bit `XD` of `C`.                                                |
| `0x7090`  | `BIT XD, CW`      | 3/3           | `*01---`  | Tests bit `XD` of `CW`.                                               |
| `0x70A0`  | `BIT XD, CH`      | 3/3           | `*01---`  | Tests bit `XD` of `CH`.                                               |
| `0x70B0`  | `BIT XD, CL`      | 3/3           | `*01---`  | Tests bit `XD` of `CL`.                                               |
| `0x70C0`  | `BIT XD, D`       | 3/3           | `*01---`  | Tests bit `XD` of `D`.                                                |
| `0x70D0`  | `BIT XD, DW`      | 3/3           | `*01---`  | Tests bit `XD` of `DW`.                                               |
| `0x70E0`  | `BIT XD, DH`      | 3/3           | `*01---`  | Tests bit `XD` of `DH`.                                               |
| `0x70F0`  | `BIT XD, DL`      | 3/3           | `*01---`  | Tests bit `XD` of `DL`.                                               |
| `0x7100`  | `BIT XD, [A]`     | 3/4           | `*01---`  | Tests bit `XD` of byte at address in `A`.                             |
| `0x7140`  | `BIT XD, [B]`     | 3/4           | `*01---`  | Tests bit `XD` of byte at address in `B`.                             |
| `0x7180`  | `BIT XD, [C]`     | 3/4           | `*01---`  | Tests bit `XD` of byte at address in `C`.                             |
| `0x71C0`  | `BIT XD, [D]`     | 3/4           | `*01---`  | Tests bit `XD` of byte at address in `D`.                             |
| `0x7200`  | `TOG XD, A`       | 3/3           |           | Toggles bit `XD` of `A`.                                              |
| `0x7210`  | `TOG XD, AW`      | 3/3           |           | Toggles bit `XD` of `AW`.                                             |
| `0x7220`  | `TOG XD, AH`      | 3/3           |           | Toggles bit `XD` of `AH`.                                             |
| `0x7230`  | `TOG XD, AL`      | 3/3           |           | Toggles bit `XD` of `AL`.                                             |
| `0x7240`  | `TOG XD, B`       | 3/3           |           | Toggles bit `XD` of `B`.                                              |
| `0x7250`  | `TOG XD, BW`      | 3/3           |           | Toggles bit `XD` of `BW`.                                             |
| `0x7260`  | `TOG XD, BH`      | 3/3           |           | Toggles bit `XD` of `BH`.                                             |
| `0x7270`  | `TOG XD, BL`      | 3/3           |           | Toggles bit `XD` of `BL`.                                             |
| `0x7280`  | `TOG XD, C`       | 3/3           |           | Toggles bit `XD` of `C`.                                              |
| `0x7290`  | `TOG XD, CW`      | 3/3           |           | Toggles bit `XD` of `CW`.                                             |
| `0x72A0`  | `TOG XD, CH`      | 3/3           |           | Toggles bit `XD` of `CH`.                                             |
| `0x72B0`  | `TOG XD, CL`      | 3/3           |           | Toggles bit `XD` of `CL`.                                             |
| `0x72C0`  | `TOG XD, D`       | 3/3           |           | Toggles bit `XD` of `D`.                                              |
| `0x72D0`  | `TOG XD, DW`      | 3/3           |           | Toggles bit `XD` of `DW`.                                             |
| `0x72E0`  | `TOG XD, DH`      | 3/3           |           | Toggles bit `XD` of `DH`.                                             |
| `0x72F0`  | `TOG XD, DL`      | 3/3           |           | Toggles bit `XD` of `DL`.                                             |
| `0x7300`  | `TOG XD, [A]`     | 3/5           |           | Toggles bit `XD` of byte at address in `A`.                           |
| `0x7340`  | `TOG XD, [B]`     | 3/5           |           | Toggles bit `XD` of byte at address in `B`.                           |
| `0x7380`  | `TOG XD, [C]`     | 3/5           |           | Toggles bit `XD` of byte at address in `C`.                           |
| `0x73C0`  | `TOG XD, [D]`     | 3/5           |           | Toggles bit `XD` of byte at address in `D`.                           |
| `0x7400`  | `SET XD, A`       | 3/3           |           | Sets bit `XD` of `A` to 1.                                            |
| `0x7410`  | `SET XD, AW`      | 3/3           |           | Sets bit `XD` of `AW` to 1.                                           |
| `0x7420`  | `SET XD, AH`      | 3/3           |           | Sets bit `XD` of `AH` to 1.                                           |
| `0x7430`  | `SET XD, AL`      | 3/3           |           | Sets bit `XD` of `AL` to 1.                                           |
| `0x7440`  | `SET XD, B`       | 3/3           |           | Sets bit `XD` of `B` to 1.                                            |
| `0x7450`  | `SET XD, BW`      | 3/3           |           | Sets bit `XD` of `BW` to 1.                                           |
| `0x7460`  | `SET XD, BH`      | 3/3           |           | Sets bit `XD` of `BH` to 1.                                           |
| `0x7470`  | `SET XD, BL`      | 3/3           |           | Sets bit `XD` of `BL` to 1.                                           |
| `0x7480`  | `SET XD, C`       | 3/3           |           | Sets bit `XD` of `C` to 1.                                            |
| `0x7490`  | `SET XD, CW`      | 3/3           |           | Sets bit `XD` of `CW` to 1.                                           |
| `0x74A0`  | `SET XD, CH`      | 3/3           |           | Sets bit `XD` of `CH` to 1.                                           |
| `0x74B0`  | `SET XD, CL`      | 3/3           |           | Sets bit `XD` of `CL` to 1.                                           |
| `0x74C0`  | `SET XD, D`       | 3/3           |           | Sets bit `XD` of `D` to 1.                                            |
| `0x74D0`  | `SET XD, DW`      | 3/3           |           | Sets bit `XD` of `DW` to 1.                                           |
| `0x74E0`  | `SET XD, DH`      | 3/3           |           | Sets bit `XD` of `DH` to 1.                                           |
| `0x74F0`  | `SET XD, DL`      | 3/3           |           | Sets bit `XD` of `DL` to 1.                                           |
| `0x7500`  | `SET XD, [A]`     | 3/5           |           | Sets bit `XD` of byte at address in `A` to 1.                         |
| `0x7540`  | `SET XD, [B]`     | 3/5           |           | Sets bit `XD` of byte at address in `B` to 1.                         |
| `0x7580`  | `SET XD, [C]`     | 3/5           |           | Sets bit `XD` of byte at address in `C` to 1.                         |
| `0x75C0`  | `SET XD, [D]`     | 3/5           |           | Sets bit `XD` of byte at address in `D` to 1.                         |
| `0x7600`  | `RES XD, A`       | 3/3           |           | Resets bit `XD` of `A` to 0.                                          |
| `0x7610`  | `RES XD, AW`      | 3/3           |           | Resets bit `XD` of `AW` to 0.                                         |
| `0x7620`  | `RES XD, AH`      | 3/3           |           | Resets bit `XD` of `AH` to 0.                                         |
| `0x7630`  | `RES XD, AL`      | 3/3           |           | Resets bit `XD` of `AL` to 0.                                         |
| `0x7640`  | `RES XD, B`       | 3/3           |           | Resets bit `XD` of `B` to 0.                                          |
| `0x7650`  | `RES XD, BW`      | 3/3           |           | Resets bit `XD` of `BW` to 0.                                         |
| `0x7660`  | `RES XD, BH`      | 3/3           |           | Resets bit `XD` of `BH` to 0.                                         |
| `0x7670`  | `RES XD, BL`      | 3/3           |           | Resets bit `XD` of `BL` to 0.                                         |
| `0x7680`  | `RES XD, C`       | 3/3           |           | Resets bit `XD` of `C` to 0.                                          |
| `0x7690`  | `RES XD, CW`      | 3/3           |           | Resets bit `XD` of `CW` to 0.                                         |
| `0x76A0`  | `RES XD, CH`      | 3/3           |           | Resets bit `XD` of `CH` to 0.                                         |
| `0x76B0`  | `RES XD, CL`      | 3/3           |           | Resets bit `XD` of `CL` to 0.                                         |
| `0x76C0`  | `RES XD, D`       | 3/3           |           | Resets bit `XD` of `D` to 0.                                          |
| `0x76D0`  | `RES XD, DW`      | 3/3           |           | Resets bit `XD` of `DW` to 0.                                         |
| `0x76E0`  | `RES XD, DH`      | 3/3           |           | Resets bit `XD` of `DH` to 0.                                         |
| `0x76F0`  | `RES XD, DL`      | 3/3           |           | Resets bit `XD` of `DL` to 0.                                         |
| `0x7700`  | `RES XD, [A]`     | 3/5           |           | Resets bit `XD` of byte at address in `A` to 0.                       |
| `0x7740`  | `RES XD, [B]`     | 3/5           |           | Resets bit `XD` of byte at address in `B` to 0.                       |
| `0x7780`  | `RES XD, [C]`     | 3/5           |           | Resets bit `XD` of byte at address in `C` to 0.                       |
| `0x77C0`  | `RES XD, [D]`     | 3/5           |           | Resets bit `XD` of byte at address in `D` to 0.                       |

## Notes

- `(+1)`: Consume one additional cycle if the execution condition is met.
- `(+6)`: Consume six additional cycles if the execution condition is met.
