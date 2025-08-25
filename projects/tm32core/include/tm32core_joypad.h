/**
 * @file    tm32core_joypad.h
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025-08-25
 * 
 * @brief   Contains the opaque type and function prototypes for the TM32 Core
 *          joypad component.
 */

#ifndef TM32CORE_JOYPAD_H
#define TM32CORE_JOYPAD_H

/* Include Files **************************************************************/

#include <tm32core_common.h>

/* Typedefs and Forward Declarations ******************************************/

/**
 * @brief   The opaque type representing the TM32 Core Bus interface.
 */
typedef struct TM32Core_Bus TM32Core_Bus;

/**
 * @brief   The opaque type representing the TM32 Core Joypad component.
 */
typedef struct TM32Core_Joypad TM32Core_Joypad;

/* Public Function Prototypes *************************************************/

/**
 * @brief   Allocates and creates a new instance of the TM32 Core Joypad
 *          component.
 * 
 * @param   bus     A pointer to the TM32 Core Bus interface to which this
 *                  joypad component will be connected.
 * 
 * @return  A pointer to the newly created TM32 Core Joypad component;
 *          `NULL` on failure.
 */
TM32CORE_API TM32Core_Joypad* TM32Core_CreateJoypad (
    TM32Core_Bus*   bus
);

/**
 * @brief   Initializes (or resets) the given TM32 Core Joypad component,
 *          resetting its state.
 * 
 * @param   joypad  A pointer to the TM32 Core Joypad component to initialize.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_InitializeJoypad (
    TM32Core_Joypad* joypad
);

/**
 * @brief   Destroys and deallocates the given TM32 Core Joypad component.
 * 
 * @param   joypad  A pointer to the TM32 Core Joypad component to destroy.
 */
TM32CORE_API void TM32Core_DestroyJoypad (
    TM32Core_Joypad* joypad
);

#endif // TM32CORE_JOYPAD_H
