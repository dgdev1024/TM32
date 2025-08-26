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

/**
 * @brief   Clocks the TM32 Core RTC component, advancing time by one cycle.
 * 
 * @param   rtc     A pointer to the TM32 Core RTC component to clock.
 */
TM32CORE_API void TM32Core_ClockRTC (
    TM32Core_RTC*   rtc
);

/* RTC Register Access Functions **********************************************/

/**
 * @brief   Reads the RTCS register (RTC Seconds).
 * 
 * @param   rtc     A pointer to the TM32 Core RTC component.
 * 
 * @return  The current RTCS register value (0-59 seconds).
 */
TM32CORE_API uint8_t TM32Core_ReadRTCS (
    TM32Core_RTC*   rtc
);

/**
 * @brief   Reads the RTCM register (RTC Minutes).
 * 
 * @param   rtc     A pointer to the TM32 Core RTC component.
 * 
 * @return  The current RTCM register value (0-59 minutes).
 */
TM32CORE_API uint8_t TM32Core_ReadRTCM (
    TM32Core_RTC*   rtc
);

/**
 * @brief   Reads the RTCH register (RTC Hours).
 * 
 * @param   rtc     A pointer to the TM32 Core RTC component.
 * 
 * @return  The current RTCH register value (0-23 hours).
 */
TM32CORE_API uint8_t TM32Core_ReadRTCH (
    TM32Core_RTC*   rtc
);

/**
 * @brief   Reads the RTCD1 register (RTC Day High Byte).
 * 
 * @param   rtc     A pointer to the TM32 Core RTC component.
 * 
 * @return  The current RTCD1 register value (day counter bits 15-8).
 */
TM32CORE_API uint8_t TM32Core_ReadRTCD1 (
    TM32Core_RTC*   rtc
);

/**
 * @brief   Reads the RTCD0 register (RTC Day Low Byte).
 * 
 * @param   rtc     A pointer to the TM32 Core RTC component.
 * 
 * @return  The current RTCD0 register value (day counter bits 7-0).
 */
TM32CORE_API uint8_t TM32Core_ReadRTCD0 (
    TM32Core_RTC*   rtc
);

/**
 * @brief   Reads the RTCC register (RTC Control).
 * 
 * @param   rtc     A pointer to the TM32 Core RTC component.
 * 
 * @return  The current RTCC register value.
 */
TM32CORE_API uint8_t TM32Core_ReadRTCC (
    TM32Core_RTC*   rtc
);

/**
 * @brief   Writes to the RTCC register (RTC Control).
 * 
 * @param   rtc     A pointer to the TM32 Core RTC component.
 * @param   value   The value to write to the RTCC register.
 */
TM32CORE_API void TM32Core_WriteRTCC (
    TM32Core_RTC*   rtc,
    uint8_t         value
);

/**
 * @brief   Writes to the RTCL register (RTC Latch).
 * 
 * @param   rtc     A pointer to the TM32 Core RTC component.
 * @param   value   The value to write to the RTCL register (value ignored).
 */
TM32CORE_API void TM32Core_WriteRTCL (
    TM32Core_RTC*   rtc,
    uint8_t         value
);

#endif // TM32CORE_RTC_H
