/**
 * @file    tm32core_timer.h
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025-08-25
 * 
 * @brief   Contains the opaque type and function prototypes for the TM32 Core
 *          timer component.
 */

#ifndef TM32CORE_TIMER_H
#define TM32CORE_TIMER_H

/* Include Files **************************************************************/

#include <tm32core_common.h>

/* Typedefs and Forward Declarations ******************************************/

/**
 * @brief   The opaque type representing the TM32 Core Bus interface.
 */
typedef struct TM32Core_Bus TM32Core_Bus;

/**
 * @brief   The opaque type representing the TM32 Core Timer component.
 */
typedef struct TM32Core_Timer TM32Core_Timer;

/* Public Function Prototypes *************************************************/

/**
 * @brief   Allocates and creates a new instance of the TM32 Core Timer
 *          component.
 * 
 * @param   bus     A pointer to the TM32 Core Bus interface to which this
 *                  timer component will be connected.
 * 
 * @return  A pointer to the newly created TM32 Core Timer component;
 *          `NULL` on failure.
 */
TM32CORE_API TM32Core_Timer* TM32Core_CreateTimer (
    TM32Core_Bus*   bus
);

/**
 * @brief   Initializes (or resets) the given TM32 Core Timer component,
 *          resetting its state.
 * 
 * @param   timer   A pointer to the TM32 Core Timer component to initialize.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_InitializeTimer (
    TM32Core_Timer* timer
);

/**
 * @brief   Clocks the given TM32 Core Timer component.
 * 
 * This function is called every time the TM32 CPU attached to this timer's bus
 * consumes a CPU cycle, causing the hardware connected to its bus to be
 * clocked.
 * 
 * @param   timer   A pointer to the TM32 Core Timer component to clock.
 * 
 * @return  `true` if the timer was successfully clocked;
 *          `false` if there was an error.
 */
TM32CORE_API bool TM32Core_ClockTimer (
    TM32Core_Timer* timer
);

/**
 * @brief   Destroys and deallocates the given TM32 Core Timer component.
 * 
 * @param   timer   A pointer to the TM32 Core Timer component to destroy.
 */
TM32CORE_API void TM32Core_DestroyTimer (
    TM32Core_Timer* timer
);

#endif // TM32CORE_TIMER_H

