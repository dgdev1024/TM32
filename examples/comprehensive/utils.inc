; Utility macros for testing

.macro INIT_REGISTERS
    LD A, 0x00000000
    LD B, 0x00000000
    LD C, 0x00000000
    LD D, 0x00000000
.endmacro

.macro CALCULATE_CHECKSUM
    ADD A, B
    ADD A, C
    ADD A, D
    AND A, 0x000000FF
.endmacro

.macro SAVE_STATE
    PUSH A
    PUSH B
    PUSH C
    PUSH D
.endmacro

.macro RESTORE_STATE
    POP D
    POP C
    POP B
    POP A
.endmacro
