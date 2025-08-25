/**
 * @file    tm32core_apu.h
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025-08-25
 * 
 * @brief   Contains the opaque type and function prototypes for the TM32 Core
 *          audio processing unit (APU) component.
 */

#ifndef TM32CORE_APU_H
#define TM32CORE_APU_H

/* Include Files **************************************************************/

#include <tm32core_common.h>

/* Typedefs and Forward Declarations ******************************************/

/**
 * @brief   The opaque type representing the TM32 Core Bus interface.
 */
typedef struct TM32Core_Bus TM32Core_Bus;

/**
 * @brief   The opaque type representing the TM32 Core Audio Processing Unit
 *          (APU) component.
 */
typedef struct TM32Core_APU TM32Core_APU;

/* Public Function Prototypes *************************************************/

/**
 * @brief   Allocates and creates a new instance of the TM32 Core Audio
 *          Processing Unit (APU) component.
 * 
 * @param   bus     A pointer to the TM32 Core Bus interface to which this
 *                  APU component will be connected.
 * 
 * @return  A pointer to the newly created TM32 Core APU component;
 *          `NULL` on failure.
 */
TM32CORE_API TM32Core_APU* TM32Core_CreateAPU (
    TM32Core_Bus*   bus
);

/**
 * @brief   Initializes (or resets) the given TM32 Core APU component,
 *          resetting its state.
 * 
 * @param   apu     A pointer to the TM32 Core APU component to initialize.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_InitializeAPU (
    TM32Core_APU*   apu
);

/**
 * @brief   Clocks the given TM32 Core APU component.
 * 
 * This function is called every time the TM32 CPU attached to this APU's bus
 * executes a single instruction cycle.
 * 
 * @param   apu     A pointer to the TM32 Core APU component to clock.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_ClockAPU (
    TM32Core_APU*   apu
);

/**
 * @brief   Destroys and deallocates the given TM32 Core APU component.
 * 
 * @param   apu     A pointer to the TM32 Core APU component to destroy.
 */
TM32CORE_API void TM32Core_DestroyAPU (
    TM32Core_APU*   apu
);

#endif // TM32CORE_APU_H