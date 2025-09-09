#ifndef TM32_ASSEMBLER_H
#define TM32_ASSEMBLER_H

#include <stdint.h>

/* Assembler return codes */
#define TM32_ASM_SUCCESS    0
#define TM32_ASM_ERROR     -1

/* Function prototypes */
int tm32_assemble_file(const char *input_file, const char *output_file);
int tm32_assemble_string(const char *assembly_code, uint8_t **bytecode, size_t *size);

#endif /* TM32_ASSEMBLER_H */