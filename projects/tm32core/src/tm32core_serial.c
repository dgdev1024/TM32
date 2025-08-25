/**
 * @file    tm32core_serial.c
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025-08-25
 * 
 * @brief   Contains the implementation of the TM32 Core's Serial Communication
 *          Interface (SCI) component.
 */

/* Include Files **************************************************************/

#include <tm32core_bus.h>
#include <tm32core_serial.h>

/* Private Structures *********************************************************/

struct TM32Core_Serial
{
    TM32Core_Bus*   bus;        /** @brief The TM32 Bus to which this SCI is connected. */
};

/* Public Functions ***********************************************************/

TM32Core_Serial* TM32Core_CreateSerial (
    TM32Core_Bus*   bus
)
{
    TM32Core_ReturnValueIfNull(bus, NULL);

    TM32Core_Serial* serial = TM32Core_CreateZero(1, TM32Core_Serial);
    if (serial == NULL)
    {
        TM32Core_LogErrno("Could not allocate the TM32 Core SCI component");
        return NULL;
    }

    serial->bus = bus;

    if (!TM32Core_InitializeSerial(serial))
    {
        TM32Core_LogError("Could not initialize the TM32 Core SCI component");
        TM32Core_Destroy(serial);
        return NULL;
    }

    return serial;
}

bool TM32Core_InitializeSerial (
    TM32Core_Serial* serial
)
{
    TM32Core_ReturnValueIfNull(serial, false);

    return true;
}

bool TM32Core_ClockSerial (
    TM32Core_Serial* serial
)
{
    TM32Core_ReturnValueIfNull(serial, false);

    return true;
}

void TM32Core_DestroySerial (
    TM32Core_Serial* serial
)
{
    if (serial != NULL)
    {
        TM32Core_Destroy(serial);
    }
}
