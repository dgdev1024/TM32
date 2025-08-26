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
    TM32Core_Bus*   bus;            /** @brief The TM32 Bus to which this SCI is connected. */
    
    uint8_t         data;           /** @brief Serial data register (SB). */
    uint8_t         transferBits;   /** @brief Number of bits transferred in current operation. */
    uint8_t         transferCycles; /** @brief Cycle counter for current transfer. */
    uint8_t         receivedData;   /** @brief Data received from external source. */
    bool            hasReceivedData; /** @brief Flag indicating if received data is available. */
    
    /** @brief Union containing the serial control register (SC). */
    union
    {
        struct
        {
            uint8_t     clockSelect : 1;    /** @brief Clock source (0 = external, 1 = internal) */
            uint8_t     clockSpeed  : 1;    /** @brief Clock speed (0 = normal, 1 = fast) */
            uint8_t                 : 5;    /** @brief Unused bits */
            uint8_t     transferStart : 1;  /** @brief Transfer in progress (1 = active) */
        };
        
        uint8_t control;
    };
    
    // Callback functions and user data
    TM32Core_SerialTransmitCallback transmitCallback;   /** @brief Callback for data transmission. */
    void*                           transmitUserData;   /** @brief User data for transmit callback. */
    TM32Core_SerialReceiveCallback  receiveCallback;    /** @brief Callback for data reception. */
    void*                           receiveUserData;    /** @brief User data for receive callback. */
    TM32Core_SerialClockCallback    clockCallback;      /** @brief Callback for external clock. */
    void*                           clockUserData;      /** @brief User data for clock callback. */
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
    
    // Initialize callbacks to NULL
    serial->transmitCallback = NULL;
    serial->transmitUserData = NULL;
    serial->receiveCallback = NULL;
    serial->receiveUserData = NULL;
    serial->clockCallback = NULL;
    serial->clockUserData = NULL;

    return serial;
}

bool TM32Core_InitializeSerial (
    TM32Core_Serial* serial
)
{
    TM32Core_ReturnValueIfNull(serial, false);

    // Initialize serial registers
    serial->data = 0x00;
    serial->control = 0x00;
    serial->transferBits = 0;
    serial->transferCycles = 0;
    serial->receivedData = 0x00;
    serial->hasReceivedData = false;

    // Serial callbacks and user datas are not reset here.

    return true;
}

bool TM32Core_ClockSerial (
    TM32Core_Serial* serial
)
{
    TM32Core_ReturnValueIfNull(serial, false);
    TM32Core_ReturnValueIfNull(serial->bus, false);

    // Handle external clock mode
    if (serial->transferStart && serial->clockSelect == 0)
    {
        // External clock mode - check for clock edges via callback
        if (serial->clockCallback != NULL)
        {
            if (serial->clockCallback(serial->clockUserData))
            {
                // Clock edge detected - transfer one bit
                serial->transferBits++;
                
                // On first bit, transmit data if callback is available
                if (serial->transferBits == 1 && serial->transmitCallback != NULL)
                {
                    serial->transmitCallback(serial->data, serial->transmitUserData);
                }
                
                // Shift in received data or dummy bits
                uint8_t receivedBit = 1; // Default to 1 if no data available
                
                // Check for injected data (used as complete byte on first bit)
                if (serial->hasReceivedData && serial->transferBits == 1)
                {
                    // Use injected data for this transfer - set the complete byte
                    serial->data = serial->receivedData;
                    serial->hasReceivedData = false;
                    
                    // Skip to transfer completion since we have the full byte
                    serial->transferBits = 8;
                }
                else
                {
                    // Normal bit-by-bit transfer
                    if (serial->receiveCallback != NULL)
                    {
                        uint8_t receivedByte;
                        if (serial->receiveCallback(&receivedByte, serial->receiveUserData))
                        {
                            // Extract the bit corresponding to current transfer position
                            receivedBit = (receivedByte >> (7 - (serial->transferBits - 1))) & 0x01;
                        }
                    }
                    
                    serial->data = (serial->data << 1) | receivedBit;
                }
                
                // Check if transfer is complete
                if (serial->transferBits >= 8)
                {
                    serial->transferStart = 0;
                    serial->transferBits = 0;
                    serial->transferCycles = 0;
                    
                    return TM32CPU_RequestInterrupt(
                        TM32Core_GetBusCPU(serial->bus),
                        TM32CORE_INT_SERIAL
                    );
                }
            }
        }
        return true;
    }

    // Handle internal clock mode
    if (serial->transferStart == 0 || serial->clockSelect == 0)
    {
        return true;
    }

    // Determine transfer speed - fast is 4x normal speed
    uint8_t cyclesPerBit = serial->clockSpeed ? 128 : 512; // Fast: 128, Normal: 512

    // Increment transfer cycle counter
    serial->transferCycles++;

    // Check if it's time to transfer the next bit
    if (serial->transferCycles >= cyclesPerBit)
    {
        serial->transferCycles = 0;
        serial->transferBits++;

        // On first bit, transmit data if callback is available
        if (serial->transferBits == 1 && serial->transmitCallback != NULL)
        {
            serial->transmitCallback(serial->data, serial->transmitUserData);
        }
        
        // Shift in received data or dummy bits
        uint8_t receivedBit = 1; // Default to 1 if no data available
        
        // Check for injected data (used as complete byte on first bit)
        if (serial->hasReceivedData && serial->transferBits == 1)
        {
            // Use injected data for this transfer - set the complete byte
            serial->data = serial->receivedData;
            serial->hasReceivedData = false;
            
            // Skip to transfer completion since we have the full byte
            serial->transferBits = 8;
        }
        else
        {
            // Normal bit-by-bit transfer
            if (serial->receiveCallback != NULL)
            {
                uint8_t receivedByte;
                if (serial->receiveCallback(&receivedByte, serial->receiveUserData))
                {
                    // Extract the bit corresponding to current transfer position
                    receivedBit = (receivedByte >> (7 - (serial->transferBits - 1))) & 0x01;
                }
            }
            
            serial->data = (serial->data << 1) | receivedBit;
        }

        // Check if transfer is complete (8 bits transferred)
        if (serial->transferBits >= 8)
        {
            // Transfer complete - clear transfer start bit and request interrupt
            serial->transferStart = 0;
            serial->transferBits = 0;
            serial->transferCycles = 0;

            // Request serial interrupt
            return TM32CPU_RequestInterrupt(
                TM32Core_GetBusCPU(serial->bus),
                TM32CORE_INT_SERIAL
            );
        }
    }

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

uint8_t TM32Core_ReadSB (
    const TM32Core_Serial*  serial
)
{
    TM32Core_ReturnValueIfNull(serial, 0x00);
    
    return serial->data;
}

uint8_t TM32Core_ReadSC (
    const TM32Core_Serial*  serial
)
{
    TM32Core_ReturnValueIfNull(serial, 0x00);
    
    // Return control register with unused bits set to 1
    return serial->control | 0x7C;
}

bool TM32Core_WriteSB (
    TM32Core_Serial*    serial,
    uint8_t             value
)
{
    TM32Core_ReturnValueIfNull(serial, false);
    
    serial->data = value;
    return true;
}

bool TM32Core_WriteSC (
    TM32Core_Serial*    serial,
    uint8_t             value
)
{
    TM32Core_ReturnValueIfNull(serial, false);
    TM32Core_ReturnValueIfNull(serial->bus, false);
    
    // Store the previous transfer start state for edge detection
    bool wasTransferring = serial->transferStart;
    
    // Update control register (only bits 7, 1, and 0 are writable)
    serial->control = value & 0x83;
    
    // If transfer start bit was just set (0 -> 1 transition)
    if (!wasTransferring && serial->transferStart)
    {
        // Start a new transfer
        serial->transferBits = 0;
        serial->transferCycles = 0;
    }
    
    return true;
}

bool TM32Core_SetSerialTransmitCallback (
    TM32Core_Serial*                serial,
    TM32Core_SerialTransmitCallback callback,
    void*                           userData
)
{
    TM32Core_ReturnValueIfNull(serial, false);
    
    serial->transmitCallback = callback;
    serial->transmitUserData = userData;
    
    return true;
}

bool TM32Core_SetSerialReceiveCallback (
    TM32Core_Serial*                serial,
    TM32Core_SerialReceiveCallback  callback,
    void*                           userData
)
{
    TM32Core_ReturnValueIfNull(serial, false);
    
    serial->receiveCallback = callback;
    serial->receiveUserData = userData;
    
    return true;
}

bool TM32Core_SetSerialClockCallback (
    TM32Core_Serial*                serial,
    TM32Core_SerialClockCallback    callback,
    void*                           userData
)
{
    TM32Core_ReturnValueIfNull(serial, false);
    
    serial->clockCallback = callback;
    serial->clockUserData = userData;
    
    return true;
}

bool TM32Core_InjectSerialData (
    TM32Core_Serial*    serial,
    uint8_t             data
)
{
    TM32Core_ReturnValueIfNull(serial, false);
    
    // Store the received data for later use
    serial->receivedData = data;
    serial->hasReceivedData = true;
    
    return true;
}
