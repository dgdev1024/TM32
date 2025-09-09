#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tm32.h"

tm32_cpu_t* tm32_cpu_create(void) {
    tm32_cpu_t *cpu = malloc(sizeof(tm32_cpu_t));
    if (!cpu) {
        return NULL;
    }
    
    cpu->memory = malloc(TM32_MEMORY_SIZE);
    if (!cpu->memory) {
        free(cpu);
        return NULL;
    }
    
    tm32_cpu_reset(cpu);
    return cpu;
}

void tm32_cpu_destroy(tm32_cpu_t *cpu) {
    if (cpu) {
        if (cpu->memory) {
            free(cpu->memory);
        }
        free(cpu);
    }
}

void tm32_cpu_reset(tm32_cpu_t *cpu) {
    if (!cpu) return;
    
    memset(cpu->registers, 0, sizeof(cpu->registers));
    cpu->pc = 0;
    cpu->sp = TM32_MEMORY_SIZE - TM32_STACK_SIZE;
    cpu->flags = 0;
    cpu->running = false;
    
    if (cpu->memory) {
        memset(cpu->memory, 0, TM32_MEMORY_SIZE);
    }
}

int tm32_cpu_step(tm32_cpu_t *cpu) {
    if (!cpu || !cpu->running) {
        return -1;
    }
    
    /* Basic instruction fetch and execute cycle */
    uint32_t instruction = *(uint32_t*)(cpu->memory + cpu->pc);
    
    /* For now, just implement a simple NOP and HALT */
    switch (instruction & 0xFF) {
        case 0x00: /* NOP */
            cpu->pc += 4;
            break;
        case 0xFF: /* HALT */
            cpu->running = false;
            break;
        default:
            /* Unknown instruction */
            cpu->running = false;
            return -1;
    }
    
    return 0;
}

int tm32_cpu_run(tm32_cpu_t *cpu) {
    if (!cpu) return -1;
    
    cpu->running = true;
    while (cpu->running) {
        if (tm32_cpu_step(cpu) != 0) {
            break;
        }
    }
    
    return 0;
}