#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tm32_assembler.h"

int tm32_assemble_file(const char *input_file, const char *output_file) {
    if (!input_file || !output_file) {
        return TM32_ASM_ERROR;
    }
    
    FILE *in = fopen(input_file, "r");
    if (!in) {
        fprintf(stderr, "Error: Cannot open input file '%s'\n", input_file);
        return TM32_ASM_ERROR;
    }
    
    FILE *out = fopen(output_file, "wb");
    if (!out) {
        fprintf(stderr, "Error: Cannot create output file '%s'\n", output_file);
        fclose(in);
        return TM32_ASM_ERROR;
    }
    
    char line[256];
    while (fgets(line, sizeof(line), in)) {
        /* Basic assembly - convert simple mnemonics to bytecode */
        if (strncmp(line, "NOP", 3) == 0) {
            uint32_t instruction = 0x00;
            fwrite(&instruction, sizeof(uint32_t), 1, out);
        } else if (strncmp(line, "HALT", 4) == 0) {
            uint32_t instruction = 0xFF;
            fwrite(&instruction, sizeof(uint32_t), 1, out);
        }
        /* Skip comments and empty lines */
    }
    
    fclose(in);
    fclose(out);
    
    printf("Assembly completed: %s -> %s\n", input_file, output_file);
    return TM32_ASM_SUCCESS;
}

int tm32_assemble_string(const char *assembly_code, uint8_t **bytecode, size_t *size) {
    if (!assembly_code || !bytecode || !size) {
        return TM32_ASM_ERROR;
    }
    
    /* Simple implementation for basic instructions */
    *size = 4; /* One instruction */
    *bytecode = malloc(*size);
    if (!*bytecode) {
        return TM32_ASM_ERROR;
    }
    
    if (strncmp(assembly_code, "NOP", 3) == 0) {
        *(uint32_t*)*bytecode = 0x00;
    } else if (strncmp(assembly_code, "HALT", 4) == 0) {
        *(uint32_t*)*bytecode = 0xFF;
    } else {
        free(*bytecode);
        *bytecode = NULL;
        *size = 0;
        return TM32_ASM_ERROR;
    }
    
    return TM32_ASM_SUCCESS;
}