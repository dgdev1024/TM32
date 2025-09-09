#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tm32.h"
#include "tm32_assembler.h"
#include "tm32_linker.h"
#include "tm32_emulator.h"

void print_usage(const char *program_name) {
    printf("TM32 Virtual CPU Emulator\n");
    printf("Usage: %s <command> [options]\n\n", program_name);
    printf("Commands:\n");
    printf("  asm <input.s> <output.o>     - Assemble source file\n");
    printf("  link <obj1> [obj2...] <exe>  - Link object files\n");
    printf("  run <program>                - Run executable\n");
    printf("  test                         - Run built-in test\n");
    printf("\nExample:\n");
    printf("  %s asm hello.s hello.o\n", program_name);
    printf("  %s link hello.o hello.exe\n", program_name);
    printf("  %s run hello.exe\n", program_name);
}

int run_test() {
    printf("Running TM32 built-in test...\n");
    
    /* Create CPU */
    tm32_cpu_t *cpu = tm32_cpu_create();
    if (!cpu) {
        printf("Error: Failed to create CPU\n");
        return 1;
    }
    
    /* Test assembly */
    uint8_t *bytecode;
    size_t size;
    if (tm32_assemble_string("HALT", &bytecode, &size) == TM32_ASM_SUCCESS) {
        printf("✓ Assembler test passed\n");
        
        /* Load bytecode into CPU memory */
        memcpy(cpu->memory, bytecode, size);
        free(bytecode);
        
        /* Run the program */
        tm32_cpu_reset(cpu);
        cpu->running = true;
        
        printf("✓ Emulator test: ");
        tm32_emulator_debug_print_state(cpu);
        
        if (tm32_cpu_run(cpu) == 0) {
            printf("✓ CPU execution completed successfully\n");
        } else {
            printf("✗ CPU execution failed\n");
        }
    } else {
        printf("✗ Assembler test failed\n");
    }
    
    tm32_cpu_destroy(cpu);
    printf("✓ All tests completed\n");
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }
    
    if (strcmp(argv[1], "test") == 0) {
        return run_test();
    }
    
    if (strcmp(argv[1], "asm") == 0) {
        if (argc != 4) {
            printf("Error: asm command requires input and output files\n");
            print_usage(argv[0]);
            return 1;
        }
        return tm32_assemble_file(argv[2], argv[3]) == TM32_ASM_SUCCESS ? 0 : 1;
    }
    
    if (strcmp(argv[1], "link") == 0) {
        if (argc < 4) {
            printf("Error: link command requires at least one object file and output file\n");
            print_usage(argv[0]);
            return 1;
        }
        const char **obj_files = (const char**)&argv[2];
        int num_obj_files = argc - 3;
        const char *output_file = argv[argc - 1];
        
        return tm32_link_objects(obj_files, num_obj_files, output_file) == TM32_LINK_SUCCESS ? 0 : 1;
    }
    
    if (strcmp(argv[1], "run") == 0) {
        if (argc != 3) {
            printf("Error: run command requires a program file\n");
            print_usage(argv[0]);
            return 1;
        }
        
        tm32_cpu_t *cpu = tm32_cpu_create();
        if (!cpu) {
            printf("Error: Failed to create CPU\n");
            return 1;
        }
        
        int result = tm32_emulator_run_program(cpu, argv[2]);
        tm32_cpu_destroy(cpu);
        
        return result == TM32_EMU_SUCCESS ? 0 : 1;
    }
    
    printf("Error: Unknown command '%s'\n", argv[1]);
    print_usage(argv[0]);
    return 1;
}