/**
 * @file    tm32core_ram.h
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025-08-25
 * 
 * @brief   Interface for the TM32 Core RAM module.
 */

#ifndef TM32CORE_RAM_H
#define TM32CORE_RAM_H

/* Include Files **************************************************************/

#include <tm32core_common.h>

/* Typedefs and Forward Declarations ******************************************/

/**
 * @brief   The opaque type representing the TM32 Core RAM module.
 */
typedef struct TM32Core_RAM TM32Core_RAM;

/* Public Function Prototypes *************************************************/

/**
 * @brief   Allocates and creates a new instance of the TM32 Core RAM module.
 * 
 * @return  A pointer to the newly created TM32 Core RAM module;
 *          `NULL` on failure.
 */
TM32CORE_API TM32Core_RAM* TM32Core_CreateRAM ();

/**
 * @brief   Initializes the TM32 Core RAM module.
 * 
 * This function sets up the RAM module, allocating necessary resources and
 * preparing it for use. It should be called after creating the RAM instance
 * and before using it.
 * 
 * @param   ram     A pointer to the TM32 Core RAM module to initialize.
 * 
 * @return  `true` if the RAM module was initialized successfully;
 *          `false` if there was an error (e.g., invalid RAM instance).
 */
TM32CORE_API bool TM32Core_InitializeRAM (
    TM32Core_RAM*  ram
);

/**
 * @brief   Destroys and deallocates the TM32 Core RAM module.
 * 
 * This function frees all resources associated with the RAM module and
 * invalidates the pointer. After calling this function, the pointer should
 * not be used unless it is reassigned to a new RAM instance.
 * 
 * @param   ram     A pointer to the TM32 Core RAM module to destroy.
 */
TM32CORE_API void TM32Core_DestroyRAM (
    TM32Core_RAM*  ram
);

#endif // TM32CORE_RAM_H
