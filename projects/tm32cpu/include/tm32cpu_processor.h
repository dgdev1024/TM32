/**
 * @file    tm32cpu_processor.h
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025/08/19
 *
 * @brief   Contains declarations for the TM32CPU Processor.
 */

#ifndef TM32CPU_PROCESSOR_H
#define TM32CPU_PROCESSOR_H

/* Include Files **************************************************************/

#include <tm32cpu_common.h>

/* Typedefs and Forward Declarations ******************************************/

/**
 * @brief   Contains the TM32CPU Processor structure.
 */
typedef struct TM32CPU_Processor TM32CPU_Processor;

/**
 * @brief   Defines a callback function invoked by the processor when reading
 *          a byte of data from the address bus to which it is connected.
 * 
 * @param   address     The absolute address being read from.
 * @param   data        Will contain the data read from the address.
 * @param   userData    The userdata pointer attached to the processor calling
 *                      this callback.
 *
 * @return  `true` if the bus was read from successfully;
 *          `false` if an error occurred.
 */
typedef bool (*TM32CPU_BusReadCallback) (
    uint32_t    address,
    uint8_t*    data,
    void*       userData
);

/**
 * @brief   Defines a callback function invoked by the processor when writing
 *          a byte of data to the address bus to which it is connected.
 * 
 * @param   address     The absolute address being written to.
 * @param   data        The data to write to the address.
 * @param   userData    The userdata pointer attached to the processor calling
 *                      this callback.
 *
 * @return  `true` if the bus was written to successfully;
 *          `false` if an error occurred.
 */
typedef bool (*TM32CPU_BusWriteCallback) (
    uint32_t    address,
    uint8_t     data,
    void*       userData
);

/**
 * @brief   Defines a callback function invoked by the processor when it
 *          consumes a CPU cycle.
 * 
 * A CPU cycle is a single clock tick of the processor. A cycle is consumed
 * every time a byte is read from or written to the address bus, or when a stack
 * or control-transfer instruction manipulates the program counter and/or the
 * stack pointer.
 * 
 * This callback can be used to implement cycle-accurate simulations of the
 * processor's behavior, as well as those of any other virtual hardware devices
 * connected to the processor's address bus.
 * 
 * @param   cycles      The number of cycles consumed.
 * @param   userData    The userdata pointer attached to the processor calling
 *                      this callback.
 * 
 * @return  `true` if the processor and its connected devices clocked without
 *          errors;
 *          `false` if an error occurred during the clocking process.
 */
typedef bool (*TM32CPU_ClockCallback) (
    uint32_t    cycles,
    void*       userData
);

/* Public Constants ***********************************************************/

// Memory Map Constants
#define TM32CPU_METADATA_START          0x00000000  /** @brief Start of the metadata region. */
#define TM32CPU_METADATA_END            0x00000FFF  /** @brief End of the metadata region. */
#define TM32CPU_INTERRUPT_START         0x00001000  /** @brief Start of the interrupt vector table. */
#define TM32CPU_INTERRUPT_END           0x00002FFF  /** @brief End of the interrupt vector table. */
#define TM32CPU_PROGRAM_START           0x00003000  /** @brief Start of the program memory region. */
#define TM32CPU_PROGRAM_END             0x7FFFFFFF  /** @brief End of the program memory region. */
#define TM32CPU_RAM_START               0x80000000  /** @brief Start of the RAM region. */
#define TM32CPU_RAM_END                 0xFFFDFFFF  /** @brief End of the RAM region. */
#define TM32CPU_STACK_START             0xFFFE0000  /** @brief Start of the stack region. */
#define TM32CPU_STACK_END               0xFFFEFFFF  /** @brief End of the stack region. */
#define TM32CPU_QRAM_START              0xFFFF0000  /** @brief Start of the quick RAM region. */
#define TM32CPU_QRAM_END                0xFFFFFFFF  /** @brief End of the quick RAM region. */

// Stack Constants
#define TM32CPU_STACK_LOWER             0xFFFE0003  /** @brief The stack is considered full if the stack pointer (`SP`) is less than or equal to this value. */
#define TM32CPU_STACK_UPPER             0xFFFEFFFF  /** @brief The stack is considered empty if the stack pointer (`SP`) is greater than or equal to this value. */

// Default Register Values
#define TM32CPU_INITIAL_PC              0x00003000  /** @brief Initial value of the Program Counter (`PC`) register. */
#define TM32CPU_INITIAL_SP              0xFFFEFFFF  /** @brief Initial value of the Stack Pointer (`SP`) register. */

// Other Constants
#define TM32CPU_INTERRUPT_VECTOR_SIZE   0x00000100  /** @brief Size of each interrupt vector in bytes. */

/* Public Enumerations ********************************************************/

/**
 * @brief   Enumerates the TM32CPU processor's general-purpose registers,
 *          including the full 32-bit registers and the partial 16- and 8-bit
 *          registers.
 */
typedef enum
{
    TM32CPU_RT_A    = 0b0000,   /** @brief `A` - 32-bit Accumulator */
    TM32CPU_RT_AW   = 0b0001,   /** @brief `AW` - 16-bit Accumulator */
    TM32CPU_RT_AH   = 0b0010,   /** @brief `AH` - 8-bit Accumulator High */
    TM32CPU_RT_AL   = 0b0011,   /** @brief `AL` - 8-bit Accumulator Low */
    TM32CPU_RT_B    = 0b0100,   /** @brief `B` - 32-bit General Purpose Register */
    TM32CPU_RT_BW   = 0b0101,   /** @brief `BW` - 16-bit General Purpose Register */
    TM32CPU_RT_BH   = 0b0110,   /** @brief `BH` - 8-bit General Purpose Register High */
    TM32CPU_RT_BL   = 0b0111,   /** @brief `BL` - 8-bit General Purpose Register Low */
    TM32CPU_RT_C    = 0b1000,   /** @brief `C` - 32-bit General Purpose Register */
    TM32CPU_RT_CW   = 0b1001,   /** @brief `CW` - 16-bit General Purpose Register */
    TM32CPU_RT_CH   = 0b1010,   /** @brief `CH` - 8-bit General Purpose Register High */
    TM32CPU_RT_CL   = 0b1011,   /** @brief `CL` - 8-bit General Purpose Register Low */
    TM32CPU_RT_D    = 0b1100,   /** @brief `D` - 32-bit General Purpose Register */
    TM32CPU_RT_DW   = 0b1101,   /** @brief `DW` - 16-bit General Purpose Register */
    TM32CPU_RT_DH   = 0b1110,   /** @brief `DH` - 8-bit General Purpose Register High */
    TM32CPU_RT_DL   = 0b1111,   /** @brief `DL` - 8-bit General Purpose Register Low */
} TM32CPU_RegisterType;

/**
 * @brief   Enumerates the TM32CPU processor's flags, which are used to
 *          indicate the status of the processor and its operations.
 */
typedef enum
{
    TM32CPU_FT_T = (1 << 0),   /** @brief `T` - Stop Flag */
    TM32CPU_FT_L = (1 << 1),   /** @brief `L` - Halt Flag */
    TM32CPU_FT_P = (1 << 2),   /** @brief `P` - Parity Flag */
    TM32CPU_FT_S = (1 << 3),   /** @brief `S` - Sign Flag */
    TM32CPU_FT_C = (1 << 4),   /** @brief `C` - Carry Flag */
    TM32CPU_FT_H = (1 << 5),   /** @brief `H` - Half-Carry Flag */
    TM32CPU_FT_N = (1 << 6),   /** @brief `N` - Negative/Subtraction Flag */
    TM32CPU_FT_Z = (1 << 7),   /** @brief `Z` - Zero Flag */
} TM32CPU_FlagType;

/**
 * @brief   Enumerates the execution conditions evaluated by the TM32CPU
 *          processor when executing the control-transfer instructions.
 */
typedef enum
{
    TM32CPU_CT_NC = 0b0000, /** @brief `NC` - No Condition */
    TM32CPU_CT_ZC = 0b0001, /** @brief `ZC` - Zero Clear */
    TM32CPU_CT_ZS = 0b1001, /** @brief `ZS` - Zero Set */
    TM32CPU_CT_CC = 0b0010, /** @brief `CC` - Carry Clear */
    TM32CPU_CT_CS = 0b1010, /** @brief `CS` - Carry Set */
    TM32CPU_CT_PC = 0b0011, /** @brief `PC` - Parity Clear */
    TM32CPU_CT_PS = 0b1011, /** @brief `PS` - Parity Set */
    TM32CPU_CT_SC = 0b0100, /** @brief `SC` - Sign Clear */
    TM32CPU_CT_SS = 0b1100, /** @brief `SS` - Sign Set */
} TM32CPU_ConditionType;

/**
 * @brief   Enumerates error codes mapped to exceptions thrown by the TM32CPU.
 * 
 * The TM32 CPU can internally set the `EC` register as its way of throwing
 * exceptions. This enumeration maps the error codes to these exceptions. When
 * the `EC` register is set in this way, the processor will automatically call
 * interrupt vector 0 to handle the exception, regardless of interrupt flags.
 */
typedef enum
{
    TM32CPU_EC_OK = 0x00,           /** @brief No error - processor is running normally. */
    TM32CPU_EC_INVALID_OPCODE,      /** @brief Invalid opcode - the processor encountered an opcode that does not resolve to a valid instruction. */
    TM32CPU_EC_INVALID_ARGUMENT,    /** @brief Invalid argument - the processor attempted to execute an instruction with an invalid argument, such as an invalid register or immediate value. */
    TM32CPU_EC_INVALID_READ,        /** @brief Invalid read - the processor attempted to read from an invalid memory address while executing an instruction. */
    TM32CPU_EC_INVALID_WRITE,       /** @brief Invalid write - the processor attempted to write to an invalid memory address while executing an instruction. */
    TM32CPU_EC_INVALID_EXECUTE,     /** @brief Invalid execute - the processor attempted to execute memory at an address in non-executable memory. */
    TM32CPU_EC_STACK_OVERFLOW,      /** @brief Stack overflow - the processor attempted to push data onto the stack, but the stack pointer (`SP`) exceeded the stack's bounds. */
    TM32CPU_EC_STACK_UNDERFLOW,     /** @brief Stack underflow - the processor attempted to pop data from the stack, but the stack pointer (`SP`) was already at the bottom of the stack. */
    TM32CPU_EC_DIVIDE_BY_ZERO,      /** @brief Divide by zero - A processor instruction attempted a division by zero. */
    TM32CPU_EC_HARDWARE_ERROR,      /** @brief Hardware error - one of the hardware components connected to the processor's address bus reported an error during cycle consumption. */
} TM32CPU_ExceptionCode;

/* Public Function Prototypes *************************************************/

/**
 * @brief   Allocates and creates a new TM32CPU Processor instance.
 * 
 * @param   busReadCallback     Callback function for reading a byte from the
 *                              address bus.
 * @param   busWriteCallback    Callback function for writing a byte to the
 *                              address bus.
 * @param   clockCallback       Callback function for consuming CPU cycles.
 * @param   userData            User-defined data pointer.
 * 
 * @return  A pointer to the newly created TM32CPU Processor instance;
 *          `NULL` if an error occurred during creation.
 */
TM32CPU_API TM32CPU_Processor* TM32CPU_CreateProcessor (
    TM32CPU_BusReadCallback   busReadCallback,
    TM32CPU_BusWriteCallback  busWriteCallback,
    TM32CPU_ClockCallback     clockCallback,
    void*                     userData
);

/**
 * @brief   Initializes or resets the TM32CPU Processor instance to its default
 *          state.
 * 
 * This function is called automatically when the processor is created, but can
 * also be called manually to reset the processor to its initial state.
 * 
 * @param   processor   Points to the TM32CPU processor instance.
 * 
 * @return  `true` if the processor was initialized successfully;
 *          `false` if an error occurred during initialization.
 */
TM32CPU_API bool TM32CPU_InitializeProcessor (
    TM32CPU_Processor* processor
);

/**
 * @brief   Destroys the TM32CPU Processor instance and frees its resources.
 * 
 * @param   processor   Points to the TM32CPU processor instance.
 */
TM32CPU_API void TM32CPU_DestroyProcessor (
    TM32CPU_Processor* processor
);

/**
 * @brief   Steps through one frame of execution of the TM32CPU Processor.
 * 
 * This function executes a single fetch-decode-execute cycle of the processor,
 * processing one instruction and consuming the necessary CPU cycles. If the
 * processor is in a halted state, this function will instead check if an
 * interrupt is pending and, if so, will clear the halt state.
 * 
 * @param   processor   Points to the TM32CPU processor instance.
 * 
 * @return  `true` if the processor executed a frame successfully;
 *          `false` if an error occurred during execution.
 */
TM32CPU_API bool TM32CPU_StepProcessor (
    TM32CPU_Processor* processor
);

/**
 * @brief   Wakes the TM32CPU Processor from STOP mode.
 * 
 * This function is used by virtual hardware components to wake the processor
 * from ultra-low power STOP mode when external wake-up events occur (such as
 * button presses, serial communication completion, or other hardware events).
 * 
 * Unlike HALT mode which can be woken by any interrupt, STOP mode requires
 * explicit external wake-up events, making this function essential for proper
 * Game Boy-inspired behavior.
 * 
 * If the processor is not currently in STOP mode (i.e., the Stop flag is not
 * set), this function does nothing and simply returns true.
 * 
 * @param   processor   Points to the TM32CPU processor instance.
 * 
 * @return  `true` if the processor was woken successfully or was not in a
 *          STOP state;
 *         `false` if an error occurred during the wake-up process.
 */
TM32CPU_API bool TM32CPU_WakeProcessor (
    TM32CPU_Processor* processor
);

/**
 * @brief   Consumes the given number of CPU cycles in the TM32CPU Processor.
 * 
 * The TM32CPU processor's private functions will call this function
 * every time it accesses the address bus, or manipulates the program counter
 * or stack pointer. However, certain hardware components attached to the
 * processor's address bus may also call this function to consume cycles.
 * 
 * @param   processor   Points to the TM32CPU processor instance.
 * @param   cycles      The number of CPU cycles to consume.
 * 
 * @return  `true` if the processor consumed the cycles successfully;
 *          `false` if an error occurred during the cycle consumption or
 *          hardware interaction.
 */
TM32CPU_API bool TM32CPU_ConsumeCycles (
    TM32CPU_Processor*  processor,
    uint32_t            cycles
);

/**
 * @brief   Reads one of the TM32CPU Processor's general-purpose registers.
 * 
 * @param   processor   Points to the TM32CPU processor instance.
 * @param   type        The type of the register to read.
 * 
 * @return  The value of the specified register if the type is valid;
 *          `0` if the type is invalid or an error occurred.
 * 
 * @see     @a `TM32CPU_RegisterType` for the list of available registers.
 */
TM32CPU_API uint32_t TM32CPU_ReadRegister (
    const TM32CPU_Processor*    processor,
    TM32CPU_RegisterType        type
);

/**
 * @brief   Writes a value to one of the TM32CPU Processor's general-purpose
 *          registers.
 * 
 * @param   processor   Points to the TM32CPU processor instance.
 * @param   type        The type of the register to write to.
 * @param   value       The value to write to the specified register.
 * 
 * @return  `true` if the register was written successfully;
 *          `false` if the type is invalid or an error occurred.
 * 
 * @see     @a `TM32CPU_RegisterType` for the list of available registers.
 */
TM32CPU_API bool TM32CPU_WriteRegister (
    TM32CPU_Processor*      processor,
    TM32CPU_RegisterType    type,
    uint32_t                value
);

/**
 * @brief   Checks if the TM32CPU Processor's flags match the specified mask.
 * 
 * This function is used to check if the processor's flags match a specific
 * condition, such as whether the zero flag is set or not.
 * 
 * @param   processor   Points to the TM32CPU processor instance.
 * @param   flagsMask   The mask of flags to check against the processor's flags.
 * 
 * @return  `true` if the processor's flags match the specified mask;
 *          `false` otherwise.
 * 
 * @see     @a `TM32CPU_FlagType` for the list of available flags.
 */
TM32CPU_API bool TM32CPU_CheckFlags (
    const TM32CPU_Processor*    processor,
    uint32_t                    flagsMask
);

/**
 * @brief   Sets the TM32CPU Processor's flags based on the specified mask.
 * 
 * This function is used to set or clear specific flags in the processor's
 * flags register. It can be used to manipulate the processor's state based on
 * the results of operations or conditions.
 * 
 * @param   processor   Points to the TM32CPU processor instance.
 * @param   flagsMask   The mask of flags to set in the processor's flags.
 * 
 * @see     @a `TM32CPU_FlagType` for the list of available flags.
 */
TM32CPU_API void TM32CPU_SetFlags (
    TM32CPU_Processor*          processor,
    uint32_t                    flagsMask
);

/**
 * @brief   Clears the TM32CPU Processor's flags based on the specified mask.
 * 
 * This function is used to clear specific flags in the processor's flags
 * register. It can be used to reset the processor's state based on the results
 * of operations or conditions.
 * 
 * @param   processor   Points to the TM32CPU processor instance.
 * @param   flagsMask   The mask of flags to clear in the processor's flags.
 * 
 * @see     @a `TM32CPU_FlagType` for the list of available flags.
 */
TM32CPU_API void TM32CPU_ClearFlags (
    TM32CPU_Processor*          processor,
    uint32_t                    flagsMask
);

/**
 * @brief   Toggles the TM32CPU Processor's flags based on the specified mask.
 * 
 * This function is used to toggle specific flags in the processor's flags
 * register. It can be used to change the state of the processor's flags based
 * on the results of operations or conditions.
 * 
 * @param   processor   Points to the TM32CPU processor instance.
 * @param   flagsMask   The mask of flags to toggle in the processor's flags.
 * 
 * @see     @a `TM32CPU_FlagType` for the list of available flags.
 */
TM32CPU_API void TM32CPU_ToggleFlags (
    TM32CPU_Processor*          processor,
    uint32_t                    flagsMask
);

/**
 * @brief   Changes the TM32CPU Processor's flags based on the specified mask.
 * 
 * This function is used to set or clear specific flags in the processor's
 * flags register based on a boolean value. If `set` is `true`, the flags are
 * set; if `false`, the flags are cleared.
 * 
 * @param   processor   Points to the TM32CPU processor instance.
 * @param   flagsMask   The mask of flags to change in the processor's flags.
 * @param   set         If `true`, sets the flags; if `false`, clears them.
 * 
 * @see     @a `TM32CPU_FlagType` for the list of available flags.
 */
TM32CPU_API void TM32CPU_ChangeFlags (
    TM32CPU_Processor*          processor,
    uint32_t                    flagsMask,
    bool                        set
);

/**
 * @brief   Requests an interrupt on the TM32CPU Processor.
 * 
 * This function sets the corresponding bit in the interrupt flags register
 * (`INTFLAGS`) to request an interrupt. The processor will handle the
 * interrupt during its next instruction cycle if interrupts are enabled and
 * the interrupt is not masked.
 * 
 * @param   processor       Points to the TM32CPU processor instance.
 * @param   interruptNumber The number of the interrupt to request (0-31).
 * 
 * @return  `true` if the interrupt was requested successfully;
 *          `false` if the processor is NULL or the interrupt number is invalid.
 */
TM32CPU_API bool TM32CPU_RequestInterrupt (
    TM32CPU_Processor*  processor,
    uint8_t             interruptNumber
);

/**
 * @brief   Cancels a previously requested interrupt on the TM32CPU Processor.
 * 
 * This function clears the corresponding bit in the interrupt flags register
 * (`INTFLAGS`) to cancel a previously requested interrupt. If the interrupt
 * was not previously requested, this function does nothing.
 * 
 * @param   processor       Points to the TM32CPU processor instance.
 * @param   interruptNumber The number of the interrupt to cancel (0-31).
 * 
 * @return  `true` if the interrupt was canceled successfully;
 *          `false` if the processor is NULL or the interrupt number is invalid.
 */
TM32CPU_API bool TM32CPU_CancelInterrupt (
    TM32CPU_Processor*  processor,
    uint8_t             interruptNumber
);

/**
 * @brief   Retrieves the current state of the TM32CPU Processor's program
 *          counter register (`PC`).
 * 
 * @param   processor   Points to the TM32CPU processor instance.
 * 
 * @return  The current value of the program counter register (`PC`).
 */
TM32CPU_API uint32_t TM32CPU_GetProgramCounter (
    const TM32CPU_Processor* processor
);

/**
 * @brief   Sets the TM32CPU Processor's program counter register (`PC`) to 
 *          the specified address.
 * 
 * This function allows external code to directly manipulate the program counter,
 * which is useful for implementing debuggers, bootloaders, or handling 
 * exceptions. The address is validated to ensure it points to executable memory.
 * 
 * @param   processor   Points to the TM32CPU processor instance.
 * @param   address     The new value for the program counter register (`PC`).
 * 
 * @return  `true` if the program counter was set successfully;
 *          `false` if the processor is NULL or the address is invalid.
 */
TM32CPU_API bool TM32CPU_SetProgramCounter (
    TM32CPU_Processor*  processor,
    uint32_t            address
);

/**
 * @brief   Retrieves the current state of the TM32CPU Processor's stack pointer
 *          register (`SP`).
 * 
 * @param   processor   Points to the TM32CPU processor instance.
 * 
 * @return  The current value of the stack pointer register (`SP`).
 */
TM32CPU_API uint32_t TM32CPU_GetStackPointer (
    const TM32CPU_Processor* processor
);

/**
 * @brief   Sets the TM32CPU Processor's stack pointer register (`SP`) to 
 *          the specified address.
 * 
 * This function allows external code to directly manipulate the stack pointer,
 * which is useful for implementing debuggers, context switchers, or handling 
 * stack-related exceptions. The address is validated to ensure it's within 
 * the valid stack memory range.
 * 
 * @param   processor   Points to the TM32CPU processor instance.
 * @param   address     The new value for the stack pointer register (`SP`).
 * 
 * @return  `true` if the stack pointer was set successfully;
 *          `false` if the processor is NULL or the address is invalid.
 */
TM32CPU_API bool TM32CPU_SetStackPointer (
    TM32CPU_Processor*  processor,
    uint32_t            address
);

/**
 * @brief   Retrieves the current state of the TM32CPU Processor's current
 *          instruction register (`CI`).
 * 
 * The current instruction register holds data on the instruction currently
 * being executed by the processor. The high byte of this register contains the
 * opcode of the instruction, while the low byte contains its parameters
 * (eg. registers, execution conditions, etc.). The high nibble of the low
 * byte contains the first parameter, while the low nibble contains the second
 * parameter.
 * 
 * @param   processor   Points to the TM32CPU processor instance.
 * 
 * @return  The current value of the instruction register (`CI`).
 */
TM32CPU_API uint16_t TM32CPU_GetCurrentInstruction (
    const TM32CPU_Processor* processor
);

/**
 * @brief   Retrieves the current state of the TM32CPU Processor's interrupt
 *          enable register (`INTENABLE`).
 * 
 * @param   processor   Points to the TM32CPU processor instance.
 * 
 * @return  The current value of the interrupt enable register (`INTENABLE`).
 */
TM32CPU_API uint32_t TM32CPU_GetInterruptEnable (
    const TM32CPU_Processor* processor
);

/**
 * @brief   Sets the TM32CPU Processor's interrupt enable register
 *          (`INTENABLE`) to the specified value.
 * 
 * This function allows external code to enable or disable specific interrupts
 * by setting or clearing bits in the interrupt enable register. Each bit in
 * this register corresponds to a specific interrupt; if a bit is set, the
 * corresponding interrupt is enabled, and if it is cleared, the interrupt is
 * disabled.
 * 
 * @param   processor   Points to the TM32CPU processor instance.
 * @param   intEnable   The new value for the interrupt enable register
 *                      (`INTENABLE`).
 */
TM32CPU_API void TM32CPU_SetInterruptEnable (
    TM32CPU_Processor*  processor,
    uint32_t            intEnable
);

/**
 * @brief   Retrieves the current state of the TM32CPU Processor's interrupt
 *          flags register (`INTFLAGS`).
 * 
 * @param   processor   Points to the TM32CPU processor instance.
 * 
 * @return  The current value of the interrupt flags register (`INTFLAGS`).
 */
TM32CPU_API uint32_t TM32CPU_GetInterruptFlags (
    const TM32CPU_Processor* processor
);

/**
 * @brief   Sets the TM32CPU Processor's interrupt flags register
 *          (`INTFLAGS`) to the specified value.
 * 
 * This function allows external code to set or clear specific interrupt flags
 * by setting or clearing bits in the interrupt flags register. Each bit in
 * this register corresponds to a specific interrupt; if a bit is set, the
 * corresponding interrupt is requested, and if it is cleared, the interrupt
 * request is canceled.
 * 
 * @param   processor   Points to the TM32CPU processor instance.
 * @param   intFlags    The new value for the interrupt flags register
 *                      (`INTFLAGS`).
 */
TM32CPU_API void TM32CPU_SetInterruptFlags (
    TM32CPU_Processor*  processor,
    uint32_t            intFlags
);

/**
 * @brief   Retrieves the current state of the TM32CPU Processor's interrupt
 *          master enable flag (`IME`).
 * 
 * This flag indicates whether the processor is currently enabled to handle
 * interrupts. If `true`, interrupts are enabled; if `false`, they are disabled.
 * 
 * If the `IME` flag is set to `false`, the processor will not respond to
 * interrupts, even if that individual interrupt is enabled in the interrupt
 * enable register (`INTENABLE`), and set in the interrupt flags register
 * (`INTFLAGS`).
 * 
 * @param   processor   Points to the TM32CPU processor instance.
 * 
 * @return  `true` if the interrupt master enable flag is set;
 *          `false` otherwise.
 */
TM32CPU_API bool TM32CPU_GetInterruptMasterEnable (
    const TM32CPU_Processor* processor
);

/**
 * @brief   Retrieves the current state of the TM32CPU Processor's error
 *          code register (`EC`).
 * 
 * @param   processor   Points to the TM32CPU processor instance.
 * 
 * @return  The current value of the error code register (`EC`).
 */
TM32CPU_API uint8_t TM32CPU_GetErrorCode (
    const TM32CPU_Processor* processor
);

/**
 * @brief   Sets the TM32CPU Processor's error code register (`EC`) to the
 *          specified error code.
 * 
 * Setting the error code via this function will not set the processor's
 * Stop flag (`T`) in the Flags register. This function is intended to be used
 * for debugging and testing purposes, allowing external code to set the
 * error code without affecting the processor's execution state.
 * 
 * @param   processor   Points to the TM32CPU processor instance.
 * @param   errorCode   The error code to set in the error code register (`EC`).
 */
TM32CPU_API void TM32CPU_SetErrorCode (
    TM32CPU_Processor*  processor,
    uint8_t             errorCode
);

/**
 * @brief   Checks if the TM32CPU Processor is currently in a HALT state.
 * 
 * The HALT state is a low-power mode where the processor stops executing
 * instructions and waits for an interrupt to resume. This is different from
 * the STOP state, which requires an explicit wake-up event to resume.
 * 
 * The HALT state is exited when an interrupt is requested, regardless of
 * whether or not its bit in the interrupt enable register (`INTENABLE`)
 * is set, or whether or not the interrupt master enable flag (`IME`) is set.
 * 
 * @param   processor   Points to the TM32CPU processor instance.
 * 
 * @return  `true` if the processor is in a HALT state;
 *          `false` otherwise.
 */
TM32CPU_API bool TM32CPU_IsHalted (
    const TM32CPU_Processor* processor
);

/**
 * @brief   Checks if the TM32CPU Processor is currently in a STOP state.
 * 
 * The STOP state is an ultra-low power mode where the processor halts all
 * operations and waits for an explicit, external wake-up event to resume.
 * This is different from the HALT state, which can be exited by any interrupt.
 * 
 * @param   processor   Points to the TM32CPU processor instance.
 * 
 * @return  `true` if the processor is in a STOP state;
 *          `false` otherwise.
 */
TM32CPU_API bool TM32CPU_IsStopped (
    const TM32CPU_Processor* processor
);

/**
 * @brief   Retrieves a human-readable string describing the current error
 *          state of the TM32CPU Processor.
 * 
 * This function converts the current error code in the processor's error code
 * register (`EC`) into a human-readable string, which can be useful for
 * debugging and logging purposes.
 * 
 * @param   processor       Points to the TM32CPU processor instance.
 * @param   string          A pointer to a character array where the error
 *                          string will be stored.
 * @param   stringLength    The length of the character array pointed to by
 *                          `string`, not including the null terminator.
 * 
 * @return  `true` if the error string was retrieved successfully;
 *          `false` if an error occurred (e.g., invalid parameters).
 * 
 * @note    The size of `string` should be at least 256 bytes to ensure that
 *          the entire error message can be stored.
 */
TM32CPU_API bool TM32CPU_GetErrorString (
    const TM32CPU_Processor*    processor,
    char*                       string,
    size_t                      stringLength
);

/**
 * @brief   Retrieves the current cycle count of the TM32CPU Processor.
 * 
 * The cycle count represents the total number of CPU cycles consumed by the
 * processor since it was initialized or reset. This can be useful for
 * performance monitoring or debugging purposes.
 * 
 * @param   processor   Points to the TM32CPU processor instance.
 * 
 * @return  The current cycle count of the processor.
 */
TM32CPU_API uint64_t TM32CPU_GetCycleCount (
    const TM32CPU_Processor* processor
);

/**
 * @brief   Retrieves the user data pointer attached to the TM32CPU Processor.
 * 
 * This function allows external code to access the user-defined data pointer
 * that was set when the processor was created. This can be useful for passing
 * additional context or state information to the processor's callbacks.
 * 
 * @param   processor   Points to the TM32CPU processor instance.
 * 
 * @return  The user data pointer attached to the processor; `NULL` if no user
 *          data was set.
 */
TM32CPU_API void* TM32CPU_GetUserData (
    const TM32CPU_Processor* processor
);

/**
 * @brief   Sets the user data pointer for the TM32CPU Processor.
 * 
 * This function allows external code to set a user-defined data pointer that
 * can be accessed by the processor's callbacks. This can be useful for passing
 * additional context or state information to the processor.
 * 
 * @param   processor   Points to the TM32CPU processor instance.
 * @param   userData    The user data pointer to set for the processor.
 * 
 * @return  `true` if a valid processor context was provided;
 *          `false` otherwise.
 */
TM32CPU_API bool TM32CPU_SetUserData (
    TM32CPU_Processor*  processor,
    void*               userData
);

#endif // TM32CPU_PROCESSOR_H
