#include <stdio.h>
#include <stdlib.h>
#include "tm32_emulator.h"

int tm32_emulator_load_program(tm32_cpu_t *cpu, const char *program_file) {
    if (!cpu || !program_file) {
        return TM32_EMU_ERROR;
    }
    
    FILE *file = fopen(program_file, "rb");
    if (!file) {
        fprintf(stderr, "Error: Cannot open program file '%s'\n", program_file);
        return TM32_EMU_ERROR;
    }
    
    /* Load program into memory starting at address 0 */
    size_t bytes_read = fread(cpu->memory, 1, TM32_MEMORY_SIZE, file);
    fclose(file);
    
    if (bytes_read == 0) {
        fprintf(stderr, "Error: No data read from program file\n");
        return TM32_EMU_ERROR;
    }
    
    printf("Loaded %zu bytes from '%s'\n", bytes_read, program_file);
    return TM32_EMU_SUCCESS;
}

int tm32_emulator_run_program(tm32_cpu_t *cpu, const char *program_file) {
    if (tm32_emulator_load_program(cpu, program_file) != TM32_EMU_SUCCESS) {
        return TM32_EMU_ERROR;
    }
    
    tm32_cpu_reset(cpu);
    cpu->running = true;
    
    printf("Starting TM32 emulation...\n");
    tm32_emulator_debug_print_state(cpu);
    
    return tm32_cpu_run(cpu);
}

void tm32_emulator_debug_print_state(tm32_cpu_t *cpu) {
    if (!cpu) return;
    
    printf("\n=== TM32 CPU State ===\n");
    printf("PC: 0x%08X  SP: 0x%08X  FLAGS: 0x%08X\n", 
           cpu->pc, cpu->sp, cpu->flags);
    
    printf("Registers:\n");
    for (int i = 0; i < TM32_NUM_REGISTERS; i += 4) {
        printf("R%02d-R%02d: ", i, i+3);
        for (int j = 0; j < 4 && (i+j) < TM32_NUM_REGISTERS; j++) {
            printf("0x%08X ", cpu->registers[i+j]);
        }
        printf("\n");
    }
    
    printf("Running: %s\n", cpu->running ? "YES" : "NO");
    printf("======================\n\n");
}