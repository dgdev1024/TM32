/**
 * @file    tm32core_bus.h
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025-08-25
 * 
 * @brief   Implements the TM32 Core Timer component structure and functions.
 */

/* Include Files **************************************************************/

#include <tm32core_bus.h>
#include <tm32core_timer.h>

/* Private Structures *********************************************************/

struct TM32Core_Timer
{
    TM32Core_Bus*   bus;        /** @brief The TM32 Bus to which this timer is connected. */

    uint16_t        divider;    /** @brief The timer's full 16-bit divider value. */
    uint8_t         counter;    /** @brief The timer's current 8-bit counter value. */
    uint8_t         modulo;     /** @brief The timer's modulo value for resets. */

    /** @brief A union containing the timer's control register. */
    union
    {
        struct
        {
            uint8_t     enabled : 1;
            uint8_t     speed   : 2;
            uint8_t             : 5;
        };

        uint8_t control;
    };
};

/* Public Functions ***********************************************************/

TM32Core_Timer* TM32Core_CreateTimer (
    TM32Core_Bus*   bus
)
{
    TM32Core_ReturnValueIfNull(bus, NULL);

    TM32Core_Timer* timer = TM32Core_CreateZero(1, TM32Core_Timer);
    if (timer == NULL)
    {
        TM32Core_LogErrno("Could not allocate the TM32 Core timer component");
        return NULL;
    }

    timer->bus = bus;

    if (!TM32Core_InitializeTimer(timer))
    {
        TM32Core_LogError("Could not initialize the TM32 Core timer component");
        TM32Core_Destroy(timer);
        return NULL;
    }

    return timer;
}

bool TM32Core_InitializeTimer (
    TM32Core_Timer* timer
)
{
    TM32Core_ReturnValueIfNull(timer, false);

    timer->divider = 0;
    timer->counter = 0;
    timer->modulo  = 0;
    timer->control = 0;

    return true;
}

bool TM32Core_ClockTimer (
    TM32Core_Timer* timer
)
{
    TM32Core_ReturnValueIfNull(timer, false);
    TM32Core_ReturnValueIfNull(timer->bus, false);

    // Increment the internal 16-bit divider, storing the old value for edge
    // detection.
    uint16_t oldDivider = timer->divider++;

    // If the timer is not currently enabled, we are done.
    if (timer->enabled == false)
    {
        return true;
    }

    // Determine the bit position in the divider that corresponds to the
    // selected timer speed.
    uint8_t bitPosition = 9; // Default to 4096 Hz
    switch (timer->speed)
    {
        case 0: bitPosition = 9;  break; // 4096 Hz
        case 1: bitPosition = 3;  break; // 262144 Hz
        case 2: bitPosition = 5;  break; // 65536 Hz
        case 3: bitPosition = 7;  break; // 16384 Hz
    }

    // Check for a falling edge on the selected bit of the divider. If a falling
    // edge is detected, increment the timer's counter.
    bool oldBit = (oldDivider >> bitPosition) & 0x01;
    bool newBit = (timer->divider >> bitPosition) & 0x01;
    if (oldBit == true && newBit == false)
    {
        if (++timer->counter == 0x00)
        {
            // Timer overflow: reset counter to modulo and request an interrupt
            timer->counter = timer->modulo;
            return TM32CPU_RequestInterrupt(
                TM32Core_GetBusCPU(timer->bus),
                TM32CORE_INT_TIMER
            );
        }
    }

    return true;
}

void TM32Core_DestroyTimer (
    TM32Core_Timer* timer
)
{
    if (timer != NULL)
    {
        TM32Core_Destroy(timer);
    }
}

uint8_t TM32Core_ReadDIV (
    const TM32Core_Timer* timer
)
{
    TM32Core_ReturnValueIfNull(timer, 0);

    return ((timer->divider >> 8) & 0xFF);
}

uint8_t TM32Core_ReadTIMA (
    const TM32Core_Timer* timer
)
{
    TM32Core_ReturnValueIfNull(timer, 0);

    return timer->counter;
}

uint8_t TM32Core_ReadTMA (
    const TM32Core_Timer* timer
)
{
    TM32Core_ReturnValueIfNull(timer, 0);

    return timer->modulo;
}

uint8_t TM32Core_ReadTAC (
    const TM32Core_Timer* timer
)
{
    TM32Core_ReturnValueIfNull(timer, 0);

    return timer->control;
}

bool TM32Core_WriteDIV (
    TM32Core_Timer* timer,
    uint8_t         value
)
{
    TM32Core_ReturnValueIfNull(timer, false);

    // Writing any value to DIV resets the internal 16-bit divider to zero
    (void) value;
    timer->divider = 0;

    return true;
}

bool TM32Core_WriteTIMA (
    TM32Core_Timer* timer,
    uint8_t         value
)
{
    TM32Core_ReturnValueIfNull(timer, false);

    timer->counter = value;
    return true;
}

bool TM32Core_WriteTMA (
    TM32Core_Timer* timer,
    uint8_t         value
)
{
    TM32Core_ReturnValueIfNull(timer, false);

    timer->modulo = value;
    return true;
}

bool TM32Core_WriteTAC (
    TM32Core_Timer* timer,
    uint8_t         value
)
{
    TM32Core_ReturnValueIfNull(timer, false);

    timer->control = value & 0x07; // Only the lower 3 bits are used
    return true;
}
