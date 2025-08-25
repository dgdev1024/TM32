/**
 * @file    tm32core_bus.h
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025-08-25
 * 
 * @brief   Contains the opaque type and function prototypes for the
 *          TM32Core bus interface.
 */

#ifndef TM32CORE_BUS_H
#define TM32CORE_BUS_H

/* Include Files **************************************************************/

#include <tm32cpu_processor.h>
#include <tm32core_program.h>
#include <tm32core_timer.h>
#include <tm32core_rtc.h>
#include <tm32core_serial.h>
#include <tm32core_joypad.h>
#include <tm32core_apu.h>
#include <tm32core_ppu.h>

/* Typedefs and Forward Declarations ******************************************/

/**
 * @brief   The opaque type representing the TM32 Core Bus interface.
 */
typedef struct TM32Core_Bus TM32Core_Bus;

/* Public Function Prototypes *************************************************/

/**
 * @brief   Allocates and creates a new instance of the TM32 Core Bus interface.
 * 
 * @return  A pointer to the newly created TM32 Core Bus interface;
 *          `NULL` on failure.
 */
TM32CORE_API TM32Core_Bus* TM32Core_CreateBus ();

/**
 * @brief   Initializes (or resets) the given TM32 Core Bus interface, resetting
 *          the state of all of its connected components.
 * 
 * @param   bus     A pointer to the TM32 Core Bus interface to initialize.
 * 
 * @return  `true` if the bus and its connected components were successfully
 *          initialized or reset;
 *          `false` if there was an error.
 */
TM32CORE_API bool TM32Core_InitializeBus (
    TM32Core_Bus*   bus
);

/**
 * @brief   Destroys and deallocates the given TM32 Core Bus interface, and its
 *          connected components.
 * 
 * @param   bus     A pointer to the TM32 Core Bus interface to destroy.
 */
TM32CORE_API void TM32Core_DestroyBus (
    TM32Core_Bus*   bus
);

/**
 * @brief   Retrieves the TM32 CPU connected to the given TM32 Core Bus interface.
 * 
 * @param   bus     A pointer to the TM32 Core Bus interface.
 * 
 * @return  A pointer to the TM32 CPU connected to the bus;
 *          `NULL` if the bus is `NULL` or if no CPU is connected.
 */
TM32CORE_API TM32CPU_Processor* TM32Core_GetBusCPU (
    TM32Core_Bus*   bus
);

/**
 * @brief   Retrieves the TM32 Core ROM program connected to the given TM32 Core 
 *          Bus interface.
 * 
 * @param   bus     A pointer to the TM32 Core Bus interface.
 * 
 * @return  A pointer to the TM32 Core ROM program connected to the bus;
 *          `NULL` if the bus is `NULL` or if no program is connected.
 */
TM32CORE_API TM32Core_Program* TM32Core_GetBusProgramInterface (
    TM32Core_Bus*   bus
);

/**
 * @brief   Retrieves the TM32 Timer connected to the given TM32 Core Bus interface.
 * 
 * @param   bus     A pointer to the TM32 Core Bus interface.
 * 
 * @return  A pointer to the TM32 Timer connected to the bus;
 *          `NULL` if the bus is `NULL` or if no Timer is connected.
 */
TM32CORE_API TM32Core_Timer* TM32Core_GetBusTimer (
    TM32Core_Bus*   bus
);

/**
 * @brief   Retrieves the TM32 Real-Time Clock connected to the given TM32 Core Bus interface.
 * 
 * @param   bus     A pointer to the TM32 Core Bus interface.
 * 
 * @return  A pointer to the TM32 Real-Time Clock connected to the bus;
 *          `NULL` if the bus is `NULL` or if no RTC is connected.
 */
TM32CORE_API TM32Core_RTC* TM32Core_GetBusRTC (
    TM32Core_Bus*   bus
);

/**
 * @brief   Retrieves the TM32 Serial interface connected to the given TM32 Core Bus interface.
 * 
 * @param   bus     A pointer to the TM32 Core Bus interface.
 * 
 * @return  A pointer to the TM32 Serial interface connected to the bus;
 *          `NULL` if the bus is `NULL` or if no Serial interface is connected.
 */
TM32CORE_API TM32Core_Serial* TM32Core_GetBusSerial (
    TM32Core_Bus*   bus
);

/**
 * @brief   Retrieves the TM32 Joypad interface connected to the given TM32 Core Bus interface.
 * 
 * @param   bus     A pointer to the TM32 Core Bus interface.
 * 
 * @return  A pointer to the TM32 Joypad interface connected to the bus;
 *          `NULL` if the bus is `NULL` or if no Joypad interface is connected.
 */
TM32CORE_API TM32Core_Joypad* TM32Core_GetBusJoypad (
    TM32Core_Bus*   bus
);

/**
 * @brief   Retrieves the TM32 Audio Processing Unit connected to the given TM32 Core Bus interface.
 * 
 * @param   bus     A pointer to the TM32 Core Bus interface.
 * 
 * @return  A pointer to the TM32 Audio Processing Unit connected to the bus;
 *          `NULL` if the bus is `NULL` or if no APU is connected.
 */
TM32CORE_API TM32Core_APU* TM32Core_GetBusAPU (
    TM32Core_Bus*   bus
);

/**
 * @brief   Retrieves the TM32 Picture Processing Unit connected to the given TM32 Core Bus interface.
 * 
 * @param   bus     A pointer to the TM32 Core Bus interface.
 * 
 * @return  A pointer to the TM32 Picture Processing Unit connected to the bus;
 *          `NULL` if the bus is `NULL` or if no PPU is connected.
 */
TM32CORE_API TM32Core_PPU* TM32Core_GetBusPPU (
    TM32Core_Bus*   bus
);

#endif // TM32CORE_BUS_H