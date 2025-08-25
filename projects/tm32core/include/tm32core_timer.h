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

/* Public Enumerations ********************************************************/

/**
 * @brief   Enumeration of the possible timer speeds.
 * 
 * These values correspond to the different clock rates at which the timer can
 * operate, as defined by the timer's control register settings.
 */
typedef enum
{
    TM32CORE_TS_4096HZ    = 0b00,
    TM32CORE_TS_262144HZ  = 0b01,
    TM32CORE_TS_65536HZ   = 0b10,
    TM32CORE_TS_16384HZ   = 0b11
} TM32Core_TimerSpeed;

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

/**
 * @brief   Reads the current value of the timer's DIV register.
 * 
 * This is the visible 8-bit register that reflects the upper 8 bits of the
 * internal 16-bit divider. It increments at a fixed rate and is unaffected by
 * the timer's control settings.
 * 
 * @param   timer   A pointer to the TM32 Core Timer component.
 * 
 * @return  The current value of the timer's DIV register.
 */
TM32CORE_API uint8_t TM32Core_ReadDIV (
    const TM32Core_Timer*   timer
);

/**
 * @brief   Reads the current value of the timer's TIMA register.
 * 
 * This is the timer's main 8-bit counter register. It increments based on the
 * timer's control settings and can trigger an interrupt when it overflows.
 * 
 * @param   timer   A pointer to the TM32 Core Timer component.
 * 
 * @return  The current value of the timer's TIMA register.
 */
TM32CORE_API uint8_t TM32Core_ReadTIMA (
    const TM32Core_Timer*   timer
);

/**
 * @brief   Reads the current value of the timer's TMA register.
 * 
 * This register holds the modulo value to which the TIMA register resets
 * after an overflow. It can be set by the program to control the timer's
 * behavior.
 * 
 * @param   timer   A pointer to the TM32 Core Timer component.
 * 
 * @return  The current value of the timer's TMA register.
 */
TM32CORE_API uint8_t TM32Core_ReadTMA (
    const TM32Core_Timer*   timer
);

/**
 * @brief   Reads the current value of the timer's TAC register.
 * 
 * This register controls the timer's operation, including enabling/disabling
 * the timer and setting its speed. It is used to configure how the timer
 * behaves.
 * 
 * @param   timer   A pointer to the TM32 Core Timer component.
 * 
 * @return  The current value of the timer's TAC register.
 */
TM32CORE_API uint8_t TM32Core_ReadTAC (
    const TM32Core_Timer*   timer
);

/**
 * @brief   Writes a value to the timer's DIV register.
 * 
 * Writing any value to this register resets the internal 16-bit divider to
 * zero. This is typically used to synchronize the timer with other events.
 * 
 * @param   timer   A pointer to the TM32 Core Timer component.
 * @param   value   The value to write to the DIV register (ignored).
 * 
 * @return  `true` if the write was successful; `false` if there was an error.
 */
TM32CORE_API bool TM32Core_WriteDIV (
    TM32Core_Timer* timer,
    uint8_t         value
);

/**
 * @brief   Writes a value to the timer's TIMA register.
 * 
 * This sets the current value of the timer's main counter. Writing to this
 * register can be used to adjust the timer's counting behavior.
 * 
 * @param   timer   A pointer to the TM32 Core Timer component.
 * @param   value   The value to write to the TIMA register.
 * 
 * @return  `true` if the write was successful; `false` if there was an error.
 */
TM32CORE_API bool TM32Core_WriteTIMA (
    TM32Core_Timer* timer,
    uint8_t         value
);

/**
 * @brief   Writes a value to the timer's TMA register.
 * 
 * This sets the modulo value for the timer. When the TIMA register overflows,
 * it will be reset to this value. This is used to control the timer's
 * overflow behavior.
 * 
 * @param   timer   A pointer to the TM32 Core Timer component.
 * @param   value   The value to write to the TMA register.
 * 
 * @return  `true` if the write was successful; `false` if there was an error.
 */
TM32CORE_API bool TM32Core_WriteTMA (
    TM32Core_Timer* timer,
    uint8_t         value
);

/**
 * @brief   Writes a value to the timer's TAC register.
 * 
 * This configures the timer's control settings, including enabling or
 * disabling the timer and setting its speed. The value written to this
 * register determines how the timer operates.
 * 
 * @param   timer   A pointer to the TM32 Core Timer component.
 * @param   value   The value to write to the TAC register.
 * 
 * @return  `true` if the write was successful; `false` if there was an error.
 */
TM32CORE_API bool TM32Core_WriteTAC (
    TM32Core_Timer* timer,
    uint8_t         value
);

#endif // TM32CORE_TIMER_H

