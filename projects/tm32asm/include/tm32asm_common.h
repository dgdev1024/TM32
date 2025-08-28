/**
 * @file    tm32asm_common.h
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025/08/26
 * 
 * @brief   Common includes and definitions for the TM32 Assembler Tool.
 */

#ifndef TM32ASM_COMMON_H
#define TM32ASM_COMMON_H

/* Include Files **************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>
#include <tm32cpu_common.h>

/* Constant Macros ************************************************************/

// Maybe Unused (Platform-Specific)
#if defined(__GNUC__) || defined(__clang__)
    #define TM32ASM_MAYBE_UNUSED __attribute__((unused))
#elif defined(_MSC_VER)
    #define TM32ASM_MAYBE_UNUSED __declspec(unused)
#else
    #define TM32ASM_MAYBE_UNUSED
#endif

// Application Version
#define TM32ASM_VERSION_MAJOR 0x01
#define TM32ASM_VERSION_MINOR 0x00
#define TM32ASM_VERSION_PATCH 0x0000
#define TM32ASM_VERSION_COMBINED ( \
    (TM32ASM_VERSION_MAJOR << 24) | \
    (TM32ASM_VERSION_MINOR << 16) | \
    TM32ASM_VERSION_PATCH \
)

/* Function Macros ************************************************************/

// Logging Macros
#define TM32ASM_LogInfo(fmt, ...) \
    fprintf(stdout, "[%s | INFO] " fmt "\n", __func__, ##__VA_ARGS__)
#define TM32ASM_LogWarn(fmt, ...) \
    fprintf(stderr, "[%s | WARN] " fmt "\n", __func__, ##__VA_ARGS__)
#define TM32ASM_LogError(fmt, ...) \
    fprintf(stderr, "[%s | ERROR] " fmt "\n", __func__, ##__VA_ARGS__)
#define TM32ASM_LogErrno(fmt, ...) \
    fprintf(stderr, "[%s | ERROR] " fmt ": %s\n", __func__, ##__VA_ARGS__, strerror(errno))
#if defined(TM32_DEBUG)
    #define TM32ASM_LogDebug(fmt, ...) \
        fprintf(stdout, "[%s | DEBUG] " fmt "\n", __func__, ##__VA_ARGS__)
#else
    #define TM32ASM_LogDebug(fmt, ...)
#endif

// Memory Management Macros
#define TM32ASM_Create(count, type) \
    ((type*) malloc((count) * sizeof(type)))
#define TM32ASM_CreateZero(count, type) \
    ((type*) calloc((count), sizeof(type)))
#define TM32ASM_Resize(ptr, count, type) \
    ((type*) realloc((ptr), (count) * sizeof(type)))
#define TM32ASM_Destroy(ptr) \
    do { if (ptr != NULL) { free(ptr); ptr = NULL; } } while(0)

// Early Exit/Return Macros
#if defined(TM32_DEBUG)
    #include <assert.h>
    #define TM32ASM_HardAssert(condition) \
        assert(condition)
#else
    #define TM32ASM_HardAssert(condition) ((void)0)
#endif
#define TM32ASM_ReturnIf(clause) \
    do { \
        if ((clause)) { \
            TM32ASM_LogError("Condition failed: %s", #clause); \
            return; \
        } \
    } while(0)
#define TM32ASM_ReturnValueIf(clause, value) \
    do { \
        if ((clause)) { \
            TM32ASM_LogError("Condition failed: %s", #clause); \
            return (value); \
        } \
    } while(0)
#define TM32ASM_ReturnIfNull(ptr) \
    do { \
        if ((ptr) == NULL) { \
            TM32ASM_LogError("Pointer '%s' is NULL.", #ptr); \
            return; \
        } \
    } while(0)
#define TM32ASM_ReturnValueIfNull(ptr, value) \
    do { \
        if ((ptr) == NULL) { \
            TM32ASM_LogError("Pointer '%s' is NULL.", #ptr); \
            return (value); \
        } \
    } while(0)

#endif // TM32ASM_COMMON_H
