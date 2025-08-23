/**
 * @file    tm32cpu_common.h
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025/08/19
 *
 * @brief   Common definitions and macros for the TM32 CPU project.
 */

#ifndef TM32CPU_COMMON_H
#define TM32CPU_COMMON_H

/* Include Files **************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

/* Library Import/Export Macros ***********************************************/

#if !defined(TM32CPU_BUILD_STATIC)
    #if defined(TM32_WINDOWS)
        #ifdef TM32CPU_EXPORTING
            #define TM32CPU_API __declspec(dllexport)
        #else
            #define TM32CPU_API __declspec(dllimport)
        #endif
    #elif defined(TM32_LINUX) || defined(TM32_MACOS)
        #ifdef TM32CPU_EXPORTING
            #define TM32CPU_API __attribute__((visibility("default")))
        #else
            #define TM32CPU_API
        #endif
    #endif
#else
    #define TM32CPU_API
#endif

/* Constant Macros ************************************************************/

// Library Version
#define TM32CPU_VERSION_MAJOR 0x01
#define TM32CPU_VERSION_MINOR 0x00
#define TM32CPU_VERSION_PATCH 0x0000
#define TM32CPU_VERSION_COMBINED ( \
    (TM32CPU_VERSION_MAJOR << 24) | \
    (TM32CPU_VERSION_MINOR << 16) | \
    TM32CPU_VERSION_PATCH \
)

/* Function Macros ************************************************************/

// Logging Macros
#define TM32CPU_LogInfo(fmt, ...) \
    fprintf(stdout, "[%s | INFO] " fmt "\n", __func__, ##__VA_ARGS__)
#define TM32CPU_LogWarn(fmt, ...) \
    fprintf(stderr, "[%s | WARN] " fmt "\n", __func__, ##__VA_ARGS__)
#define TM32CPU_LogError(fmt, ...) \
    fprintf(stderr, "[%s | ERROR] " fmt "\n", __func__, ##__VA_ARGS__)
#define TM32CPU_LogErrno(fmt, ...) \
    fprintf(stderr, "[%s | ERROR] " fmt ": %s\n", __func__, ##__VA_ARGS__, strerror(errno))
#if defined(TM_DEBUG)
    #define TM32CPU_LogDebug(fmt, ...) \
        fprintf(stdout, "[%s | DEBUG] " fmt "\n", __func__, ##__VA_ARGS__)
#else
    #define TM32CPU_LogDebug(fmt, ...)
#endif

// Memory Management Macros
#define TM32CPU_Create(count, type) \
    ((type*) malloc((count) * sizeof(type)))
#define TM32CPU_CreateZero(count, type) \
    ((type*) calloc((count), sizeof(type)))
#define TM32CPU_Resize(ptr, count, type) \
    ((type*) realloc((ptr), (count) * sizeof(type)))
#define TM32CPU_Destroy(ptr) \
    do { if (ptr != NULL) { free(ptr); ptr = NULL; } } while(0)

// Early Exit/Return Macros
#if defined(TM_DEBUG)
    #include <assert.h>
    #define TM32CPU_HardAssert(condition) \
        assert(condition)
#else
    #define TM32CPU_HardAssert(condition) ((void)0)
#endif
#define TM32CPU_ReturnIf(clause) \
    do { \
        if ((clause)) { \
            TM32CPU_LogError("Condition failed: %s", #clause); \
            return; \
        } \
    } while(0)
#define TM32CPU_ReturnValueIf(clause, value) \
    do { \
        if ((clause)) { \
            TM32CPU_LogError("Condition failed: %s", #clause); \
            return (value); \
        } \
    } while(0)
#define TM32CPU_ReturnIfNull(ptr) \
    do { \
        if ((ptr) == NULL) { \
            TM32CPU_LogError("Pointer '%s' is NULL.", #ptr); \
            return; \
        } \
    } while(0)
#define TM32CPU_ReturnValueIfNull(ptr, value) \
    do { \
        if ((ptr) == NULL) { \
            TM32CPU_LogError("Pointer '%s' is NULL.", #ptr); \
            return (value); \
        } \
    } while(0)

#endif // TM32CPU_COMMON_H
