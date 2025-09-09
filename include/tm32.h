#ifndef TM32_H
#define TM32_H

#include <stdint.h>
#include <stdbool.h>

/* TM32 Virtual CPU Architecture Constants */
#define TM32_WORD_SIZE      32
#define TM32_MEMORY_SIZE    (1024 * 1024)  /* 1MB */
#define TM32_NUM_REGISTERS  16
#define TM32_STACK_SIZE     (64 * 1024)    /* 64KB */

/* TM32 CPU State */
typedef struct {
    uint32_t registers[TM32_NUM_REGISTERS];
    uint32_t pc;        /* Program Counter */
    uint32_t sp;        /* Stack Pointer */
    uint32_t flags;     /* Status Flags */
    uint8_t *memory;    /* Main Memory */
    bool running;       /* CPU Running State */
} tm32_cpu_t;

/* Function prototypes */
tm32_cpu_t* tm32_cpu_create(void);
void tm32_cpu_destroy(tm32_cpu_t *cpu);
void tm32_cpu_reset(tm32_cpu_t *cpu);
int tm32_cpu_step(tm32_cpu_t *cpu);
int tm32_cpu_run(tm32_cpu_t *cpu);

#endif /* TM32_H */