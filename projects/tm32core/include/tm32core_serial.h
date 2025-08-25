/**
 * @file    tm32core_serial.h
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025-08-25
 * 
 * @brief   Contains the opaque type and function prototypes for the TM32 Core's
 *          serial communication interface (SCI) component.
 */

#ifndef TM32CORE_SERIAL_H
#define TM32CORE_SERIAL_H

/* Include Files **************************************************************/

#include <tm32core_common.h>

/* Typedefs and Forward Declarations ******************************************/

/**
 * @brief   The opaque type representing the TM32 Core Bus interface.
 */
typedef struct TM32Core_Bus TM32Core_Bus;

/**
 * @brief   The opaque type representing the TM32 Core Serial Communication
 *          Interface (SCI) component.
 */
typedef struct TM32Core_Serial TM32Core_Serial;

/* Public Function Prototypes *************************************************/

/**
 * @brief   Allocates and creates a new instance of the TM32 Core Serial
 *          Communication Interface (SCI) component.
 * 
 * @param   bus     A pointer to the TM32 Core Bus interface to which this
 *                  SCI component will be connected.
 * 
 * @return  A pointer to the newly created TM32 Core SCI component;
 *          `NULL` on failure.
 */
TM32CORE_API TM32Core_Serial* TM32Core_CreateSerial (
    TM32Core_Bus*   bus
);

/**
 * @brief   Initializes (or resets) the given TM32 Core SCI component,
 *          resetting its state.
 * 
 * @param   serial  A pointer to the TM32 Core SCI component to initialize.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_InitializeSerial (
    TM32Core_Serial* serial
);

/**
 * @brief   Clocks the given TM32 Core SCI component.
 * 
 * This function is called every time the TM32 CPU attached to this SCI's bus
 * clocks. It handles the timing and state changes of the SCI component.
 * 
 * @param   serial  A pointer to the TM32 Core SCI component to clock.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_ClockSerial (
    TM32Core_Serial* serial
);

/**
 * @brief   Destroys and deallocates the given TM32 Core SCI component.
 * 
 * @param   serial  A pointer to the TM32 Core SCI component to destroy.
 */
TM32CORE_API void TM32Core_DestroySerial (
    TM32Core_Serial* serial
);

#endif // TM32CORE_SERIAL_H
