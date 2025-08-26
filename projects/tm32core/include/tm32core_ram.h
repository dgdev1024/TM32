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

/* Constants ******************************************************************/

// RAM Memory Address Ranges
#define TM32CORE_WRAM_BASE_ADDRESS      0x80000000  /** @brief Working RAM base address */
#define TM32CORE_WRAM_END_ADDRESS       0x9FFFFFFF  /** @brief Working RAM end address */
#define TM32CORE_WRAM_MAX_SIZE          0x20000000  /** @brief Maximum WRAM size (512 MB) */

#define TM32CORE_STACK_BASE_ADDRESS     0xFFFE0000  /** @brief Stack memory base address */
#define TM32CORE_STACK_END_ADDRESS      0xFFFEFFFF  /** @brief Stack memory end address */
#define TM32CORE_STACK_SIZE             0x00010000  /** @brief Stack memory size (64 KB) */

#define TM32CORE_QRAM_BASE_ADDRESS      0xFFFF0000  /** @brief Quick RAM base address */
#define TM32CORE_QRAM_END_ADDRESS       0xFFFFEEFF  /** @brief Quick RAM end address (before port registers) */
#define TM32CORE_QRAM_SIZE              0x0000FF00  /** @brief Quick RAM size (65280 bytes) */

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
 * and before using it. The WRAM size is determined from the loaded program.
 * 
 * @param   ram         A pointer to the TM32 Core RAM module to initialize.
 * @param   wramSize    The size of Working RAM to allocate (based on program metadata).
 * 
 * @return  `true` if the RAM module was initialized successfully;
 *          `false` if there was an error (e.g., invalid RAM instance, allocation failure).
 */
TM32CORE_API bool TM32Core_InitializeRAM (
    TM32Core_RAM*   ram,
    uint32_t        wramSize
);

/**
 * @brief   Reads a byte from the RAM module.
 * 
 * @param   ram         A pointer to the TM32 Core RAM module.
 * @param   address     The memory address to read from.
 * @param   data        A pointer to store the read byte.
 * 
 * @return  `true` if the byte was read successfully;
 *          `false` if there was an error.
 */
TM32CORE_API bool TM32Core_ReadRAMByte (
    TM32Core_RAM*   ram,
    uint32_t        address,
    uint8_t*        data
);

/**
 * @brief   Writes a byte to the RAM module.
 * 
 * @param   ram         A pointer to the TM32 Core RAM module.
 * @param   address     The memory address to write to.
 * @param   data        The byte to write.
 * 
 * @return  `true` if the byte was written successfully;
 *          `false` if there was an error.
 */
TM32CORE_API bool TM32Core_WriteRAMByte (
    TM32Core_RAM*   ram,
    uint32_t        address,
    uint8_t         data
);

/**
 * @brief   Clears all RAM contents to zero.
 * 
 * @param   ram         A pointer to the TM32 Core RAM module.
 * 
 * @return  `true` if the RAM was cleared successfully;
 *          `false` if there was an error.
 */
TM32CORE_API bool TM32Core_ClearRAM (
    TM32Core_RAM*   ram
);

/**
 * @brief   Gets the allocated Working RAM size.
 * 
 * @param   ram         A pointer to the TM32 Core RAM module.
 * 
 * @return  The size of allocated Working RAM in bytes; 0 if not initialized.
 */
TM32CORE_API uint32_t TM32Core_GetWRAMSize (
    TM32Core_RAM*   ram
);

/**
 * @brief   Checks if the RAM module is initialized.
 * 
 * @param   ram         A pointer to the TM32 Core RAM module.
 * 
 * @return  `true` if the RAM module is initialized;
 *          `false` otherwise.
 */
TM32CORE_API bool TM32Core_IsRAMInitialized (
    TM32Core_RAM*   ram
);

/**
 * @brief   Reinitializes the RAM module with a new WRAM size.
 * 
 * This function is used to reallocate Working RAM when a program is loaded
 * that requires a different amount of WRAM than what was initially allocated.
 * 
 * @param   ram         A pointer to the TM32 Core RAM module to reinitialize.
 * @param   wramSize    The new size of Working RAM to allocate.
 * 
 * @return  `true` if the RAM module was reinitialized successfully;
 *          `false` if there was an error.
 */
TM32CORE_API bool TM32Core_ReinitializeRAM (
    TM32Core_RAM*   ram,
    uint32_t        wramSize
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
