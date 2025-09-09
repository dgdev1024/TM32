#include <stdio.h>
#include <stdlib.h>
#include "tm32_linker.h"

int tm32_link_objects(const char **object_files, int num_files, const char *output_file) {
    if (!object_files || num_files <= 0 || !output_file) {
        return TM32_LINK_ERROR;
    }
    
    FILE *out = fopen(output_file, "wb");
    if (!out) {
        fprintf(stderr, "Error: Cannot create output file '%s'\n", output_file);
        return TM32_LINK_ERROR;
    }
    
    /* Simple linker - just concatenate object files */
    for (int i = 0; i < num_files; i++) {
        FILE *in = fopen(object_files[i], "rb");
        if (!in) {
            fprintf(stderr, "Error: Cannot open object file '%s'\n", object_files[i]);
            fclose(out);
            return TM32_LINK_ERROR;
        }
        
        /* Copy file contents */
        int ch;
        while ((ch = fgetc(in)) != EOF) {
            fputc(ch, out);
        }
        
        fclose(in);
    }
    
    fclose(out);
    
    printf("Linking completed: %d object files -> %s\n", num_files, output_file);
    return TM32_LINK_SUCCESS;
}