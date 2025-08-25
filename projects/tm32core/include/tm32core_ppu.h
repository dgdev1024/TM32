/**
 * @file    tm32core_ppu.h
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025-08-25
 * 
 * @brief   Contains the opaque type and function prototypes for the TM32 Core
 *          pixel processing unit (PPU) component.
 */

#ifndef TM32CORE_PPU_H
#define TM32CORE_PPU_H

/* Include Files **************************************************************/

#include <tm32core_common.h>

/* Typedefs and Forward Declarations ******************************************/

/**
 * @brief   The opaque type representing the TM32 Core Bus interface.
 */
typedef struct TM32Core_Bus TM32Core_Bus;

/**
 * @brief   The opaque type representing the TM32 Core Pixel Processing Unit
 *          (PPU) component.
 */
typedef struct TM32Core_PPU TM32Core_PPU;

/* Public Function Prototypes *************************************************/

/**
 * @brief   Allocates and creates a new instance of the TM32 Core Pixel
 *          Processing Unit (PPU) component.
 * 
 * @param   bus     A pointer to the TM32 Core Bus interface to which this
 *                  PPU component will be connected.
 * 
 * @return  A pointer to the newly created TM32 Core PPU component;
 *          `NULL` on failure.
 */
TM32CORE_API TM32Core_PPU* TM32Core_CreatePPU (
    TM32Core_Bus*   bus
);

/**
 * @brief   Initializes (or resets) the given TM32 Core PPU component,
 *          resetting its state.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component to initialize.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_InitializePPU (
    TM32Core_PPU*   ppu
);

/**
 * @brief   Clocks the given TM32 Core PPU component.
 * 
 * This function is called every time the TM32 CPU attached to this PPU's bus
 * executes an instruction.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component to clock.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_ClockPPU (
    TM32Core_PPU*   ppu
);

/**
 * @brief   Destroys and deallocates the given TM32 Core PPU component.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component to destroy.
 */
TM32CORE_API void TM32Core_DestroyPPU (
    TM32Core_PPU*   ppu
);

#endif // TM32CORE_PPU_H
