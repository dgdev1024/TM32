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

/* Public Enumerations ********************************************************/

/**
 * @brief   Enumeration of joypad buttons.
 * 
 * These correspond to the standard Game Boy button layout with direction
 * buttons (D-pad) and action buttons.
 */
typedef enum
{
    TM32CORE_JOYPAD_RIGHT   = 0,    /** @brief Right direction button */
    TM32CORE_JOYPAD_LEFT    = 1,    /** @brief Left direction button */
    TM32CORE_JOYPAD_UP      = 2,    /** @brief Up direction button */
    TM32CORE_JOYPAD_DOWN    = 3,    /** @brief Down direction button */
    TM32CORE_JOYPAD_A       = 4,    /** @brief A action button */
    TM32CORE_JOYPAD_B       = 5,    /** @brief B action button */
    TM32CORE_JOYPAD_SELECT  = 6,    /** @brief Select action button */
    TM32CORE_JOYPAD_START   = 7     /** @brief Start action button */
} TM32Core_JoypadButton;

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

/**
 * @brief   Reads the current value of the joypad's JOYP register.
 * 
 * This register controls which button group is selected and returns the
 * current state of the selected buttons. Bits 5-4 control selection
 * (bit 5 for action buttons, bit 4 for direction buttons), and bits 3-0
 * return the button states (0 = pressed, 1 = not pressed).
 * 
 * @param   joypad  A pointer to the TM32 Core Joypad component.
 * 
 * @return  The current value of the joypad's JOYP register.
 */
TM32CORE_API uint8_t TM32Core_ReadJOYP (
    const TM32Core_Joypad*  joypad
);

/**
 * @brief   Writes a value to the joypad's JOYP register.
 * 
 * This is used to select which button group to read from. Only bits 5-4
 * are writable (bit 5 for action buttons, bit 4 for direction buttons).
 * Writing to other bits has no effect.
 * 
 * @param   joypad  A pointer to the TM32 Core Joypad component.
 * @param   value   The value to write to the JOYP register.
 * 
 * @return  `true` if the write was successful; `false` if there was an error.
 */
TM32CORE_API bool TM32Core_WriteJOYP (
    TM32Core_Joypad*    joypad,
    uint8_t             value
);

/**
 * @brief   Sets the state of a specific joypad button.
 * 
 * This function is typically called by the emulator frontend when user
 * input is detected. Setting a button to pressed may trigger a joypad
 * interrupt if the button was previously not pressed.
 * 
 * @param   joypad  A pointer to the TM32 Core Joypad component.
 * @param   button  The button to set the state for.
 * @param   pressed `true` if the button is pressed; `false` if released.
 * 
 * @return  `true` if the button state was successfully set;
 *          `false` if there was an error.
 */
TM32CORE_API bool TM32Core_SetButtonState (
    TM32Core_Joypad*        joypad,
    TM32Core_JoypadButton   button,
    bool                    pressed
);

/**
 * @brief   Gets the state of a specific joypad button.
 * 
 * @param   joypad  A pointer to the TM32 Core Joypad component.
 * @param   button  The button to get the state for.
 * 
 * @return  `true` if the button is currently pressed; `false` if not pressed
 *          or if there was an error.
 */
TM32CORE_API bool TM32Core_GetButtonState (
    const TM32Core_Joypad*  joypad,
    TM32Core_JoypadButton   button
);

#endif // TM32CORE_JOYPAD_H
