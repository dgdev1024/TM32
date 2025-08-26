/**
 * @file    tm32core_joypad.c
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025-08-25
 * 
 * @brief   Implements the TM32 Core Joypad component structure and functions.
 */

/* Include Files **************************************************************/

#include <tm32core_bus.h>
#include <tm32core_joypad.h>

/* Private Structures *********************************************************/

struct TM32Core_Joypad
{
    TM32Core_Bus*   bus;            /** @brief The TM32 Bus to which this joypad is connected. */
    
    uint8_t         buttonStates;   /** @brief Current state of all 8 buttons (0 = pressed, 1 = not pressed). */
    
    /** @brief Union containing the joypad's control register (JOYP). */
    union
    {
        struct
        {
            uint8_t     buttons     : 4;    /** @brief Button state bits (read-only) */
            uint8_t     selectDir   : 1;    /** @brief Select direction buttons (0 = selected) */
            uint8_t     selectAction: 1;    /** @brief Select action buttons (0 = selected) */
            uint8_t                 : 2;    /** @brief Unused bits (always 1) */
        };
        
        uint8_t control;
    };
};

/* Public Functions ***********************************************************/

TM32Core_Joypad* TM32Core_CreateJoypad (
    TM32Core_Bus*   bus
)
{
    TM32Core_ReturnValueIfNull(bus, NULL);

    TM32Core_Joypad* joypad = TM32Core_CreateZero(1, TM32Core_Joypad);
    if (joypad == NULL)
    {
        TM32Core_LogErrno("Could not allocate the TM32 Core joypad component");
        return NULL;
    }

    joypad->bus = bus;

    if (!TM32Core_InitializeJoypad(joypad))
    {
        TM32Core_LogError("Could not initialize the TM32 Core joypad component");
        TM32Core_Destroy(joypad);
        return NULL;
    }

    return joypad;
}

bool TM32Core_InitializeJoypad (
    TM32Core_Joypad* joypad
)
{
    TM32Core_ReturnValueIfNull(joypad, false);

    // Initialize all buttons as not pressed (1 = not pressed)
    joypad->buttonStates = 0xFF;
    
    // Initialize control register - no buttons selected, unused bits set to 1
    joypad->control = 0xFF;
    
    return true;
}

void TM32Core_DestroyJoypad (
    TM32Core_Joypad* joypad
)
{
    if (joypad != NULL)
    {
        TM32Core_Destroy(joypad);
    }
}

uint8_t TM32Core_ReadJOYP (
    const TM32Core_Joypad*  joypad
)
{
    TM32Core_ReturnValueIfNull(joypad, 0xFF);
    
    uint8_t result = joypad->control;
    
    // Determine which buttons to read based on selection bits
    if (joypad->selectDir == 0)
    {
        // Direction buttons selected (bits 3-0 = Down, Up, Left, Right)
        result = (result & 0xF0) | ((joypad->buttonStates >> 0) & 0x0F);
    }
    else if (joypad->selectAction == 0)
    {
        // Action buttons selected (bits 3-0 = Start, Select, B, A)
        result = (result & 0xF0) | ((joypad->buttonStates >> 4) & 0x0F);
    }
    else
    {
        // No buttons selected, return all buttons as not pressed
        result = (result & 0xF0) | 0x0F;
    }
    
    // Ensure unused bits are always set to 1
    result |= 0xC0;
    
    return result;
}

bool TM32Core_WriteJOYP (
    TM32Core_Joypad*    joypad,
    uint8_t             value
)
{
    TM32Core_ReturnValueIfNull(joypad, false);
    TM32Core_ReturnValueIfNull(joypad->bus, false);
    
    // Only bits 5-4 are writable (button selection)
    joypad->selectDir = (value >> 4) & 0x01;
    joypad->selectAction = (value >> 5) & 0x01;
    
    // Unused bits are always 1
    joypad->control = (joypad->control & 0x0F) | (value & 0x30) | 0xC0;
    
    return true;
}

bool TM32Core_SetButtonState (
    TM32Core_Joypad*        joypad,
    TM32Core_JoypadButton   button,
    bool                    pressed
)
{
    TM32Core_ReturnValueIfNull(joypad, false);
    TM32Core_ReturnValueIfNull(joypad->bus, false);
    
    // Validate button parameter
    if (button > TM32CORE_JOYPAD_START)
    {
        TM32Core_LogError("Invalid button: %d", button);
        return false;
    }
    
    // Get the previous state for interrupt detection
    bool wasPressed = ((joypad->buttonStates >> button) & 0x01) == 0;
    
    // Update button state (0 = pressed, 1 = not pressed)
    if (pressed)
    {
        joypad->buttonStates &= ~(1 << button);
    }
    else
    {
        joypad->buttonStates |= (1 << button);
    }
    
    // If button was just pressed (transition from not pressed to pressed),
    // request a joypad interrupt
    if (!wasPressed && pressed)
    {
        return TM32CPU_RequestInterrupt(
            TM32Core_GetBusCPU(joypad->bus),
            TM32CORE_INT_JOYPAD
        );
    }
    
    return true;
}

bool TM32Core_GetButtonState (
    const TM32Core_Joypad*  joypad,
    TM32Core_JoypadButton   button
)
{
    TM32Core_ReturnValueIfNull(joypad, false);
    
    // Validate button parameter
    if (button > TM32CORE_JOYPAD_START)
    {
        TM32Core_LogError("Invalid button: %d", button);
        return false;
    }
    
    // Return true if button is pressed (bit is 0)
    return ((joypad->buttonStates >> button) & 0x01) == 0;
}
