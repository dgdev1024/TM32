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

// Memory Map Constants
#define TM32CORE_METADATA_START             0x00000000
#define TM32CORE_METADATA_END               0x00000FFF
#define TM32CORE_INTERRUPT_START            0x00001000
#define TM32CORE_INTERRUPT_END              0x00002FFF
#define TM32CORE_PROGRAM_START              0x00003000
#define TM32CORE_PROGRAM_END                0x7FFFFFFF
#define TM32CORE_WORK_RAM_START             0x80000000
#define TM32CORE_WORK_RAM_END               0x9FFFFFFF
#define TM32CORE_SAVE_RAM_START             0xA0000000
#define TM32CORE_SAVE_RAM_END               0xBFFFFFFF
#define TM32CORE_VIDEO_RAM_START            0xC0008000
#define TM32CORE_VIDEO_RAM_END              0xC0009FFF
#define TM32CORE_OAM_START                  0xC000FE00
#define TM32CORE_OAM_END                    0xC000FE9F
#define TM32CORE_STACK_START                0xFFFE0000
#define TM32CORE_STACK_END                  0xFFFEFFFF
#define TM32CORE_QUICK_RAM_START            0xFFFF0000
#define TM32CORE_QUICK_RAM_END              0xFFFFFEFF
#define TM32CORE_PORT_START                 0xFFFFFF00
#define TM32CORE_PORT_END                   0xFFFFFFFF

// Interrupt Vector Constants
#define TM32CORE_INT_VBLANK                 1
#define TM32CORE_INT_LCD_STAT               2
#define TM32CORE_INT_TIMER                  3
#define TM32CORE_INT_SERIAL                 4
#define TM32CORE_INT_JOYPAD                 5
#define TM32CORE_INT_RTC                    6

// Port Constants
#define TM32CORE_PORT_JOYP                  0xFFFFFF00
#define TM32CORE_PORT_SB                    0xFFFFFF01
#define TM32CORE_PORT_SC                    0xFFFFFF02
#define TM32CORE_PORT_DIV                   0xFFFFFF03
#define TM32CORE_PORT_TIMA                  0xFFFFFF04
#define TM32CORE_PORT_TMA                   0xFFFFFF05
#define TM32CORE_PORT_TAC                   0xFFFFFF06
#define TM32CORE_PORT_NR10                  0xFFFFFF10  
#define TM32CORE_PORT_NR11                  0xFFFFFF11  
#define TM32CORE_PORT_NR12                  0xFFFFFF12  
#define TM32CORE_PORT_NR13                  0xFFFFFF13  
#define TM32CORE_PORT_NR14                  0xFFFFFF14  
#define TM32CORE_PORT_NR21                  0xFFFFFF16  
#define TM32CORE_PORT_NR22                  0xFFFFFF17  
#define TM32CORE_PORT_NR23                  0xFFFFFF18  
#define TM32CORE_PORT_NR24                  0xFFFFFF19  
#define TM32CORE_PORT_NR30                  0xFFFFFF1A  
#define TM32CORE_PORT_NR31                  0xFFFFFF1B  
#define TM32CORE_PORT_NR32                  0xFFFFFF1C  
#define TM32CORE_PORT_NR33                  0xFFFFFF1D  
#define TM32CORE_PORT_NR34                  0xFFFFFF1E  
#define TM32CORE_PORT_NR41                  0xFFFFFF20  
#define TM32CORE_PORT_NR42                  0xFFFFFF21  
#define TM32CORE_PORT_NR43                  0xFFFFFF22  
#define TM32CORE_PORT_NR44                  0xFFFFFF23  
#define TM32CORE_PORT_NR50                  0xFFFFFF24  
#define TM32CORE_PORT_NR51                  0xFFFFFF25  
#define TM32CORE_PORT_NR52                  0xFFFFFF26  
#define TM32CORE_PORT_LCDC                  0xFFFFFF40  
#define TM32CORE_PORT_STAT                  0xFFFFFF41  
#define TM32CORE_PORT_SCY                   0xFFFFFF42   
#define TM32CORE_PORT_SCX                   0xFFFFFF43   
#define TM32CORE_PORT_LY                    0xFFFFFF44    
#define TM32CORE_PORT_LYC                   0xFFFFFF45   
#define TM32CORE_PORT_DMA                   0xFFFFFF46   
#define TM32CORE_PORT_BGP                   0xFFFFFF47   
#define TM32CORE_PORT_OBP0                  0xFFFFFF48  
#define TM32CORE_PORT_OBP1                  0xFFFFFF49  
#define TM32CORE_PORT_WY                    0xFFFFFF4A    
#define TM32CORE_PORT_WX                    0xFFFFFF4B    
#define TM32CORE_PORT_KEY1                  0xFFFFFF4D  
#define TM32CORE_PORT_VBK                   0xFFFFFF4F   
#define TM32CORE_PORT_RP                    0xFFFFFF56    
#define TM32CORE_PORT_BGPI                  0xFFFFFF68  
#define TM32CORE_PORT_BGPD                  0xFFFFFF69  
#define TM32CORE_PORT_OBPI                  0xFFFFFF6A  
#define TM32CORE_PORT_OBPD                  0xFFFFFF6B  
#define TM32CORE_PORT_OPRI                  0xFFFFFF6C  
#define TM32CORE_PORT_PCM12                 0xFFFFFF76 
#define TM32CORE_PORT_PCM34                 0xFFFFFF77 
#define TM32CORE_PORT_RTCS                  0xFFFFFF80  
#define TM32CORE_PORT_RTCM                  0xFFFFFF81  
#define TM32CORE_PORT_RTCH                  0xFFFFFF82  
#define TM32CORE_PORT_RTCD1                 0xFFFFFF83 
#define TM32CORE_PORT_RTCD0                 0xFFFFFF84 
#define TM32CORE_PORT_RTCC                  0xFFFFFF85  
#define TM32CORE_PORT_RTCL                  0xFFFFFF87  
#define TM32CORE_PORT_ODMAS3                0xFFFFFFA0
#define TM32CORE_PORT_ODMAS2                0xFFFFFFA1
#define TM32CORE_PORT_ODMAS1                0xFFFFFFA2
#define TM32CORE_PORT_ODMA                  0xFFFFFFA3  
#define TM32CORE_PORT_HDMAS3                0xFFFFFFA4
#define TM32CORE_PORT_HDMAS2                0xFFFFFFA5
#define TM32CORE_PORT_HDMAS1                0xFFFFFFA6
#define TM32CORE_PORT_HDMAS0                0xFFFFFFA7
#define TM32CORE_PORT_HDMAD3                0xFFFFFFA8
#define TM32CORE_PORT_HDMAD2                0xFFFFFFA9
#define TM32CORE_PORT_HDMAD1                0xFFFFFFAA
#define TM32CORE_PORT_HDMAD0                0xFFFFFFAB
#define TM32CORE_PORT_HDMA                  0xFFFFFFAC  
#define TM32CORE_PORT_GRPM                  0xFFFFFFAD  
#define TM32CORE_PORT_IF3                   0xFFFFFFF8   
#define TM32CORE_PORT_IF2                   0xFFFFFFF9   
#define TM32CORE_PORT_IF1                   0xFFFFFFFA   
#define TM32CORE_PORT_IF0                   0xFFFFFFFB   
#define TM32CORE_PORT_IE3                   0xFFFFFFFC   
#define TM32CORE_PORT_IE2                   0xFFFFFFFD   
#define TM32CORE_PORT_IE1                   0xFFFFFFFE   
#define TM32CORE_PORT_IE0                   0xFFFFFFFF   

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
