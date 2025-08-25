/**
 * @file    tm32core_common.h
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025/08/19
 *
 * @brief   Common definitions and macros for the TM32 Core project.
 */

#ifndef TM32CORE_COMMON_H
#define TM32CORE_COMMON_H

/* Include Files **************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

/* Library Import/Export Macros ***********************************************/

#if !defined(TM32CORE_BUILD_STATIC)
    #if defined(TM32_WINDOWS)
        #ifdef TM32CORE_EXPORTING
            #define TM32CORE_API __declspec(dllexport)
        #else
            #define TM32CORE_API __declspec(dllimport)
        #endif
    #elif defined(TM32_LINUX) || defined(TM32_MACOS)
        #ifdef TM32CORE_EXPORTING
            #define TM32CORE_API __attribute__((visibility("default")))
        #else
            #define TM32CORE_API
        #endif
    #endif
#else
    #define TM32CORE_API
#endif

/* Constant Macros ************************************************************/

// Library Version
#define TM32CORE_VERSION_MAJOR 0x01
#define TM32CORE_VERSION_MINOR 0x00
#define TM32CORE_VERSION_PATCH 0x0000
#define TM32CORE_VERSION_COMBINED ( \
    (TM32CORE_VERSION_MAJOR << 24) | \
    (TM32CORE_VERSION_MINOR << 16) | \
    TM32CORE_VERSION_PATCH \
)

/* Function Macros ************************************************************/

// Logging Macros
#define TM32Core_LogInfo(fmt, ...) \
    fprintf(stdout, "[%s | INFO] " fmt "\n", __func__, ##__VA_ARGS__)
#define TM32Core_LogWarn(fmt, ...) \
    fprintf(stderr, "[%s | WARN] " fmt "\n", __func__, ##__VA_ARGS__)
#define TM32Core_LogError(fmt, ...) \
    fprintf(stderr, "[%s | ERROR] " fmt "\n", __func__, ##__VA_ARGS__)
#define TM32Core_LogErrno(fmt, ...) \
    fprintf(stderr, "[%s | ERROR] " fmt ": %s\n", __func__, ##__VA_ARGS__, strerror(errno))
#if defined(TM_DEBUG)
    #define TM32Core_LogDebug(fmt, ...) \
        fprintf(stdout, "[%s | DEBUG] " fmt "\n", __func__, ##__VA_ARGS__)
#else
    #define TM32Core_LogDebug(fmt, ...)
#endif

// Memory Management Macros
#define TM32Core_Create(count, type) \
    ((type*) malloc((count) * sizeof(type)))
#define TM32Core_CreateZero(count, type) \
    ((type*) calloc((count), sizeof(type)))
#define TM32Core_Resize(ptr, count, type) \
    ((type*) realloc((ptr), (count) * sizeof(type)))
#define TM32Core_Destroy(ptr) \
    do { if (ptr != NULL) { free(ptr); ptr = NULL; } } while(0)

// Early Exit/Return Macros
#if defined(TM_DEBUG)
    #include <assert.h>
    #define TM32Core_HardAssert(condition) \
        assert(condition)
#else
    #define TM32Core_HardAssert(condition) ((void)0)
#endif
#define TM32Core_ReturnIf(clause) \
    do { \
        if ((clause)) { \
            TM32Core_LogError("Condition failed: %s", #clause); \
            return; \
        } \
    } while(0)
#define TM32Core_ReturnValueIf(clause, value) \
    do { \
        if ((clause)) { \
            TM32Core_LogError("Condition failed: %s", #clause); \
            return (value); \
        } \
    } while(0)
#define TM32Core_ReturnIfNull(ptr) \
    do { \
        if ((ptr) == NULL) { \
            TM32Core_LogError("Pointer '%s' is NULL.", #ptr); \
            return; \
        } \
    } while(0)
#define TM32Core_ReturnValueIfNull(ptr, value) \
    do { \
        if ((ptr) == NULL) { \
            TM32Core_LogError("Pointer '%s' is NULL.", #ptr); \
            return (value); \
        } \
    } while(0)

#endif // TM32CORE_COMMON_H
