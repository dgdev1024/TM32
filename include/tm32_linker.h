#ifndef TM32_LINKER_H
#define TM32_LINKER_H

#include <stdint.h>

/* Linker return codes */
#define TM32_LINK_SUCCESS   0
#define TM32_LINK_ERROR    -1

/* Function prototypes */
int tm32_link_objects(const char **object_files, int num_files, const char *output_file);

#endif /* TM32_LINKER_H */