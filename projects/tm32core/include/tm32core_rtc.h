/**
 * @file    tm32core_rtc.h
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025-08-25
 * 
 * @brief   Contains the opaque type and function prototypes for the TM32 Core's
 *          real-time clock (RTC) component.
 */

#ifndef TM32CORE_RTC_H
#define TM32CORE_RTC_H

/* Include Files **************************************************************/

#include <tm32core_common.h>

/* Typedefs and Forward Declarations ******************************************/

/**
 * @brief   The opaque type representing the TM32 Core Bus interface.
 */
typedef struct TM32Core_Bus TM32Core_Bus;

/**
 * @brief   The opaque type representing the TM32 Core Real-Time Clock (RTC)
 *          component.
 */
typedef struct TM32Core_RTC TM32Core_RTC;

/* Public Function Prototypes *************************************************/

/**
 * @brief   Allocates and creates a new instance of the TM32 Core Real-Time
 *          Clock (RTC) component.
 * 
 * @param   bus     A pointer to the TM32 Core Bus interface to which this
 *                  RTC component will be connected.
 * 
 * @return  A pointer to the newly created TM32 Core RTC component;
 *          `NULL` on failure.
 */
TM32CORE_API TM32Core_RTC* TM32Core_CreateRTC (
    TM32Core_Bus*   bus
);

/**
 * @brief   Initializes (or resets) the given TM32 Core RTC component,
 *          resetting its state.
 * 
 * @param   rtc     A pointer to the TM32 Core RTC component to initialize.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_InitializeRTC (
    TM32Core_RTC*   rtc
);

/**
 * @brief   Destroys and deallocates the given TM32 Core RTC component.
 * 
 * @param   rtc     A pointer to the TM32 Core RTC component to destroy.
 */
TM32CORE_API void TM32Core_DestroyRTC (
    TM32Core_RTC*   rtc
);

#endif // TM32CORE_RTC_H
