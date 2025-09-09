#ifndef TM32_EMULATOR_H
#define TM32_EMULATOR_H

#include "tm32.h"

/* Emulator return codes */
#define TM32_EMU_SUCCESS    0
#define TM32_EMU_ERROR     -1

/* Function prototypes */
int tm32_emulator_load_program(tm32_cpu_t *cpu, const char *program_file);
int tm32_emulator_run_program(tm32_cpu_t *cpu, const char *program_file);
void tm32_emulator_debug_print_state(tm32_cpu_t *cpu);

#endif /* TM32_EMULATOR_H */