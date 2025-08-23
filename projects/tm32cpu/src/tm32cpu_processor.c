/**
 * @file    tm32cpu_processor.c
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025/08/19
 * 
 * @brief   Contains the implementation of the TM32CPU Processor.
 */

/* Include Files **************************************************************/

#include <tm32cpu_processor.h>

/* Private Structures *********************************************************/

struct TM32CPU_Processor
{

    // Callback Functions
    TM32CPU_BusReadCallback   busReadCallback;
    TM32CPU_BusWriteCallback  busWriteCallback;
    TM32CPU_ClockCallback     clockCallback;

    // User Data
    void*       userData;           /** @brief User-defined data pointer */

    // General Purpose Registers
    uint32_t    a;                  /** @brief `A` - Accumulator Register */
    uint32_t    b;                  /** @brief `B` - General Purpose Register B */
    uint32_t    c;                  /** @brief `C` - General Purpose Register C */
    uint32_t    d;                  /** @brief `D` - General Purpose Register D */

    // Special Purpose Registers
    uint32_t    pc;                 /** @brief `PC` - Program Counter */
    uint32_t    ip;                 /** @brief `IP` - Instruction Pointer */
    uint32_t    sp;                 /** @brief `SP` - Stack Pointer */
    uint32_t    da;                 /** @brief `DA` - Destination Address */
    uint32_t    sa;                 /** @brief `SA` - Source Address */
    uint32_t    sd;                 /** @brief `SD` - Source Data */
    uint32_t    flags;              /** @brief `FLAGS` - Processor Flags */
    uint32_t    intflags;           /** @brief `INTFLAGS` - Interrupt Flags */
    uint32_t    intenable;          /** @brief `INTENABLE` - Interrupt Enable */
    uint16_t    ci;                 /** @brief `CI` - Current Instruction */
    uint8_t     ec;                 /** @brief `EC` - Exception Code */
    uint8_t     xd;                 /** @brief `XD` - Extra Data */
    bool        ime;                /** @brief `IME` - Interrupt Master Enable */

    // Internal State
    uint64_t    cycleCount;         /** @brief Total CPU cycles consumed */
    uint32_t    errorAddress;       /** @brief Address of the last read/write to cause an error. */
    uint8_t     itype;              /** @brief Current Instruction Type */
    uint8_t     iparam1;            /** @brief Current Instruction Parameter 1 */
    uint8_t     iparam2;            /** @brief Current Instruction Parameter 2 */
    bool        ei;                 /** @brief Enable IME at end of frame? */

};

/* Private Function Prototypes ************************************************/

// Error Handling Functions
static bool TM32CPU_RaiseException (TM32CPU_Processor* processor, uint8_t errorCode);

// Bounds Checking Functions
static bool TM32CPU_CheckReadable (TM32CPU_Processor* processor, uint32_t address, uint32_t readSize);
static bool TM32CPU_CheckWritable (TM32CPU_Processor* processor, uint32_t address, uint32_t writeSize);
static bool TM32CPU_CheckExecutable (TM32CPU_Processor* processor, uint32_t address);

// Parity Functions
static bool TM32CPU_CheckParity (TM32CPU_Processor* processor, uint32_t value);

// Memory Access Functions
static bool TM32CPU_ReadByte (TM32CPU_Processor* processor, uint32_t address, uint8_t* data);
static bool TM32CPU_ReadWord (TM32CPU_Processor* processor, uint32_t address, uint16_t* data);
static bool TM32CPU_ReadDoubleWord (TM32CPU_Processor* processor, uint32_t address, uint32_t* data);
static bool TM32CPU_WriteByte (TM32CPU_Processor* processor, uint32_t address, uint8_t data);
static bool TM32CPU_WriteWord (TM32CPU_Processor* processor, uint32_t address, uint16_t data);
static bool TM32CPU_WriteDoubleWord (TM32CPU_Processor* processor, uint32_t address, uint32_t data);

// Stack Operation Functions
static bool TM32CPU_Push (TM32CPU_Processor* processor, uint32_t value);
static bool TM32CPU_Pop (TM32CPU_Processor* processor, uint32_t* value);

// Control Transfer Functions
static bool TM32CPU_CheckCondition (TM32CPU_Processor* processor, TM32CPU_ConditionType condition);
static bool TM32CPU_MoveProgramCounter (TM32CPU_Processor* processor, uint32_t address);
static bool TM32CPU_ServiceInterrupt (TM32CPU_Processor* processor);
static bool TM32CPU_AdvanceProgramCounter (TM32CPU_Processor* processor, uint32_t cycles);

// Data Fetching Functions
static bool TM32CPU_Fetch_REG_NULL (TM32CPU_Processor* processor);
static bool TM32CPU_Fetch_REG_REG (TM32CPU_Processor* processor);
static bool TM32CPU_Fetch_REG_IMM (TM32CPU_Processor* processor);
static bool TM32CPU_Fetch_REG_ADDR32 (TM32CPU_Processor* processor);
static bool TM32CPU_Fetch_REG_ADDR16 (TM32CPU_Processor* processor);
static bool TM32CPU_Fetch_REG_ADDR8 (TM32CPU_Processor* processor);
static bool TM32CPU_Fetch_REG_REGPTR32 (TM32CPU_Processor* processor);
static bool TM32CPU_Fetch_REG_REGPTR16 (TM32CPU_Processor* processor);
static bool TM32CPU_Fetch_REG_REGPTR8 (TM32CPU_Processor* processor);
static bool TM32CPU_Fetch_ADDR32_REG (TM32CPU_Processor* processor);
static bool TM32CPU_Fetch_ADDR16_REG (TM32CPU_Processor* processor);
static bool TM32CPU_Fetch_ADDR8_REG (TM32CPU_Processor* processor);
static bool TM32CPU_Fetch_REGPTR32_NULL (TM32CPU_Processor* processor);
static bool TM32CPU_Fetch_REGPTR32_REG (TM32CPU_Processor* processor);
static bool TM32CPU_Fetch_REGPTR16_REG (TM32CPU_Processor* processor);
static bool TM32CPU_Fetch_REGPTR8_REG (TM32CPU_Processor* processor);
static bool TM32CPU_Fetch_NULL_IMM32 (TM32CPU_Processor* processor);
static bool TM32CPU_Fetch_NULL_IMM16 (TM32CPU_Processor* processor);
static bool TM32CPU_Fetch_XD_REG_NULL (TM32CPU_Processor* processor);
static bool TM32CPU_Fetch_XD_REGPTR32_NULL (TM32CPU_Processor* processor);

// Instruction Execution Functions
static bool TM32CPU_Execute_NOP (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_STOP (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_HALT (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_SEC (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_CEC (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_DI (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_EI (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_DAA (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_SCF (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_CCF (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_FLG (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_STF (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_CLF (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_LD (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_ST (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_MV (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_POP (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_PUSH (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_JMP (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_JPB (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_CALL (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_INT (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_RET (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_RETI (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_INC (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_DEC (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_ADD (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_ADC (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_SUB (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_SBC (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_AND (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_OR (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_XOR (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_NOT (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_CMP (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_SLA (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_SRA (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_SRL (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_RL (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_RLC (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_RR (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_RRC (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_BIT (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_TOG (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_SET (TM32CPU_Processor* processor);
static bool TM32CPU_Execute_RES (TM32CPU_Processor* processor);

/* Private Functions - Error Handling *****************************************/

bool TM32CPU_RaiseException (
    TM32CPU_Processor*  processor,
    uint8_t             errorCode
)
{
    // Set the error code
    processor->ec = errorCode;
    
    // If this is not the OK status, call interrupt vector 0 to handle the exception
    if (errorCode != TM32CPU_EC_OK)
    {
        // Push the current program counter onto the stack
        if (TM32CPU_Push(processor, processor->pc) == false)
        {
            // If an additional error occurs during the exception-handling
            // process, this is a "double fault" situtation. In this case,
            // a program exit is needed.
            char message[256] = { 0 };
            TM32CPU_GetErrorString(processor, message, sizeof(message) - 1);

            TM32CPU_LogError("DOUBLE FAULT: '%s'!\n", message);
            exit(EXIT_FAILURE);
        }
        
        // Move program counter to interrupt vector 0 (exception handler)
        processor->pc = TM32CPU_INTERRUPT_START;
        
        // Clear the Halt flag and disable IME (like normal interrupt handling)
        TM32CPU_ClearFlags(processor, TM32CPU_FT_L);
        processor->ime = false;
    }
    
    return (processor->ec == TM32CPU_EC_OK);
}

/* Private Functions - Bounds Checking ****************************************/

bool TM32CPU_CheckReadable (
    TM32CPU_Processor*  processor,
    uint32_t            address,
    uint32_t            readSize
)
{
    // Start and end addresses of the read operation...
    uint64_t    startAddress = (uint64_t) address,
                endAddress   = (uint64_t) address + (uint64_t) readSize - 1;

    // Check if the read operation is within the valid memory ranges:
    // - Interrupt Start to RAM End: 0x00001000 to 0xFFFDFFFF
    // - Quick RAM Region: 0xFFFF0000 to 0xFFFFFFFF
    if (
        (startAddress >= TM32CPU_INTERRUPT_START && endAddress <= TM32CPU_RAM_END) ||
        (startAddress >= TM32CPU_QRAM_START && endAddress <= TM32CPU_QRAM_END)
    )
    {
        return true;
    }

    processor->errorAddress = address;  // Store the address that caused the error
    return TM32CPU_RaiseException(processor, TM32CPU_EC_INVALID_READ);
}

bool TM32CPU_CheckWritable (
    TM32CPU_Processor*  processor,
    uint32_t            address,
    uint32_t            writeSize
)
{
    // Start and end addresses of the write operation...
    uint64_t    startAddress = (uint64_t) address,
                endAddress   = (uint64_t) address + (uint64_t) writeSize - 1;

    // Check if the write operation is within the valid memory ranges:
    // - RAM Region: 0x80000000 to 0xFFFDFFFF
    // - Quick RAM Region: 0xFFFF0000 to 0xFFFFFFFF
    if (
        (startAddress >= TM32CPU_RAM_START && endAddress <= TM32CPU_RAM_END) ||
        (startAddress >= TM32CPU_QRAM_START && endAddress <= TM32CPU_QRAM_END)
    )
    {
        return true;
    }

    return TM32CPU_RaiseException(processor, TM32CPU_EC_INVALID_WRITE);
}

bool TM32CPU_CheckExecutable (
    TM32CPU_Processor*  processor,
    uint32_t            address
)
{
    // Start and end addresses of the executable region...
    uint64_t    startAddress = (uint64_t) address,
                endAddress   = (uint64_t) address + 1;  // Instruction opcodes are two bytes.

    // Check if the address is within the valid executable memory range:
    // - Interrupt Start to Program End: 0x00001000 to 0x7FFFFFFF
    if (
        startAddress >= TM32CPU_INTERRUPT_START && endAddress <= TM32CPU_PROGRAM_END
    )
    {
        return true;
    }

    processor->errorAddress = address;  // Store the address that caused the error
    return TM32CPU_RaiseException(processor, TM32CPU_EC_INVALID_EXECUTE);
}

/* Private Functions - Parity *************************************************/

bool TM32CPU_CheckParity (
    TM32CPU_Processor*  processor,
    uint32_t            value
)
{
    // Count the number of set bits in the value
    uint32_t count = 0;
    for (uint32_t i = 0; i < 32; i++)
    {
        if ((value >> i) & 1)
        {
            count++;
        }
    }

    // Check if the count is even or odd
    bool isEven = (count % 2 == 0);

    // Change the Parity Flag (P) based on the parity of the value
    TM32CPU_ChangeFlags(processor, TM32CPU_FT_P, isEven);

    return isEven;
}

/* Private Functions - Memory Access ******************************************/

bool TM32CPU_ReadByte (
    TM32CPU_Processor*  processor,
    uint32_t            address,
    uint8_t*            data
)
{
    uint8_t byte0 = 0;

    if (processor->busReadCallback(address, &byte0, processor->userData) == false)
    {
        processor->errorAddress = address;  
        return TM32CPU_RaiseException(processor, TM32CPU_EC_INVALID_READ);
    }

    *data = byte0;

    return TM32CPU_ConsumeCycles(processor, 1);
}

bool TM32CPU_ReadWord (
    TM32CPU_Processor*  processor,
    uint32_t            address,
    uint16_t*           data
)
{
    uint8_t byte0 = 0, 
            byte1 = 0;

    if (
        processor->busReadCallback(address,     &byte0, processor->userData) == false ||
        processor->busReadCallback(address + 1, &byte1, processor->userData) == false
    )
    {
        processor->errorAddress = address;  // Store the address that caused the error
        return TM32CPU_RaiseException(processor, TM32CPU_EC_INVALID_READ);
    }

    *data =
        ((uint16_t) byte1 <<  8) |
        ((uint16_t) byte0 <<  0);

    return TM32CPU_ConsumeCycles(processor, 2);
}

bool TM32CPU_ReadDoubleWord (
    TM32CPU_Processor*  processor,
    uint32_t            address,
    uint32_t*           data
)
{
    uint8_t byte0 = 0, 
            byte1 = 0, 
            byte2 = 0, 
            byte3 = 0;

    if (
        processor->busReadCallback(address,     &byte0, processor->userData) == false ||
        processor->busReadCallback(address + 1, &byte1, processor->userData) == false ||
        processor->busReadCallback(address + 2, &byte2, processor->userData) == false ||
        processor->busReadCallback(address + 3, &byte3, processor->userData) == false
    )
    {
        processor->errorAddress = address;  // Store the address that caused the error
        return TM32CPU_RaiseException(processor, TM32CPU_EC_INVALID_READ);
    }

    *data =
        ((uint32_t) byte3 << 24) |
        ((uint32_t) byte2 << 16) |
        ((uint32_t) byte1 <<  8) |
        ((uint32_t) byte0 <<  0);

    return TM32CPU_ConsumeCycles(processor, 4);
}

bool TM32CPU_WriteByte (
    TM32CPU_Processor*  processor,
    uint32_t            address,
    uint8_t             data
)
{
    if (processor->busWriteCallback(address, data, processor->userData) == false)
    {
        processor->errorAddress = address;  // Store the address that caused the error
        return TM32CPU_RaiseException(processor, TM32CPU_EC_INVALID_WRITE);
    }

    return TM32CPU_ConsumeCycles(processor, 1);
}

bool TM32CPU_WriteWord (
    TM32CPU_Processor*  processor,
    uint32_t            address,
    uint16_t            data
)
{
    uint8_t byte0 = (data >> 0) & 0xFF;
    uint8_t byte1 = (data >> 8) & 0xFF;

    if (
        processor->busWriteCallback(address,     byte0, processor->userData) == false ||
        processor->busWriteCallback(address + 1, byte1, processor->userData) == false
    )
    {
        processor->errorAddress = address;  // Store the address that caused the error
        return TM32CPU_RaiseException(processor, TM32CPU_EC_INVALID_WRITE);
    }

    return TM32CPU_ConsumeCycles(processor, 2);
}

bool TM32CPU_WriteDoubleWord (
    TM32CPU_Processor*  processor,
    uint32_t            address,
    uint32_t            data
)
{
    uint8_t byte0 = (data >>  0) & 0xFF;
    uint8_t byte1 = (data >>  8) & 0xFF;
    uint8_t byte2 = (data >> 16) & 0xFF;
    uint8_t byte3 = (data >> 24) & 0xFF;

    if (
        processor->busWriteCallback(address,     byte0, processor->userData) == false ||
        processor->busWriteCallback(address + 1, byte1, processor->userData) == false ||
        processor->busWriteCallback(address + 2, byte2, processor->userData) == false ||
        processor->busWriteCallback(address + 3, byte3, processor->userData) == false
    )
    {
        processor->errorAddress = address;  // Store the address that caused the error
        return TM32CPU_RaiseException(processor, TM32CPU_EC_INVALID_WRITE);
    }

    return TM32CPU_ConsumeCycles(processor, 4);
}

/* Private Functions - Stack Operations ***************************************/

bool TM32CPU_Push (
    TM32CPU_Processor*  processor,
    uint32_t            value
)
{
    // Check if the stack is full
    if (processor->sp <= TM32CPU_STACK_LOWER)
    {
        return TM32CPU_RaiseException(processor, TM32CPU_EC_STACK_OVERFLOW);
    }
    
    // Decrement the stack pointer.
    processor->sp -= 4;

    // Consume one cycle for the stack manipulation, then write the value
    // to the stack.
    return
        TM32CPU_ConsumeCycles(processor, 1) &&
        TM32CPU_WriteDoubleWord(processor, processor->sp, value);
}

bool TM32CPU_Pop (
    TM32CPU_Processor*  processor,
    uint32_t*           value
)
{
    // Check if the stack is empty
    if (processor->sp >= TM32CPU_STACK_UPPER)
    {
        return TM32CPU_RaiseException(processor, TM32CPU_EC_STACK_UNDERFLOW);
    }

    // Read the value from the stack.
    if (TM32CPU_ReadDoubleWord(processor, processor->sp, value) == false)
    {
        return false;
    }

    // Increment the stack pointer, then consume one cycle.
    processor->sp += 4;
    return TM32CPU_ConsumeCycles(processor, 1);
}

/* Private Functions - Control Transfer ***************************************/

bool TM32CPU_CheckCondition (
    TM32CPU_Processor*  processor,
    TM32CPU_ConditionType condition
)
{
    // Check the condition based on the processor's flags
    switch (condition)
    {
        case TM32CPU_CT_NC:  
            return true;
        case TM32CPU_CT_ZC:  
            return (processor->flags & TM32CPU_FT_Z) == 0;
        case TM32CPU_CT_ZS:  
            return (processor->flags & TM32CPU_FT_Z) != 0;
        case TM32CPU_CT_CC:  
            return (processor->flags & TM32CPU_FT_C) == 0;
        case TM32CPU_CT_CS:  
            return (processor->flags & TM32CPU_FT_C) != 0;
        case TM32CPU_CT_PC:  
            return (processor->flags & TM32CPU_FT_P) == 0;
        case TM32CPU_CT_PS:  
            return (processor->flags & TM32CPU_FT_P) != 0;
        case TM32CPU_CT_SC:  
            return (processor->flags & TM32CPU_FT_S) == 0;
        case TM32CPU_CT_SS:  
            return (processor->flags & TM32CPU_FT_S) != 0;
        default:             
            return false; // Invalid condition
    }
}

bool TM32CPU_MoveProgramCounter (
    TM32CPU_Processor*  processor,
    uint32_t            address
)
{
    // Move the program counter to the specified address, then consume one cycle.
    processor->pc = address;
    return TM32CPU_ConsumeCycles(processor, 1);
}

bool TM32CPU_ServiceInterrupt (
    TM32CPU_Processor*  processor
)
{
    for (uint8_t i = 0; i < 32; ++i)
    {
        if (
            (processor->intflags    & (1 << i)) != 0 &&
            (processor->intenable   & (1 << i)) != 0
        )
        {
            // Acknowledge the interrupt
            processor->intflags &= ~(1 << i);
            processor->ime = false;
            TM32CPU_ClearFlags(processor, TM32CPU_FT_L);

            // Push the current program counter onto the stack, then move to the
            // interrupt vector.
            return
                TM32CPU_Push(processor, processor->pc) &&
                TM32CPU_MoveProgramCounter(processor,
                    TM32CPU_INTERRUPT_START + (i * TM32CPU_INTERRUPT_VECTOR_SIZE));
        }
    }
    
    return true;
}

bool TM32CPU_AdvanceProgramCounter (
    TM32CPU_Processor*  processor,
    uint32_t            cycles
)
{
    processor->pc += cycles;
    return true;
}

/* Private Functions - Data Fetching ******************************************/

/*
    The `TM32CPU_Fetch_*` function names are an exception to the TM32 workspace's
    code style. These function follow a specific naming convention to
    indicate their purpose and the types of operands they handle.

    Notation: `TM32CPU_Fetch_<XD>_<DEST>_<SRC>`

    Where:
        - <XD>, if present, indicates that the `XD` extra data register will be
            written to and used in the instruction execution.
        - <DEST> is the destination operand type. This could be a register or
            memory address.
        - <SRC> is the source operand type. This could be a register, immediate
            value, or memory address.

    Legend:
        - `<DEST>` can be:
            - `REG`: General-purpose register. This could be one of the full 32-bit
                registers (`A`, `B`, `C`, `D`), or a 16-bit or 8-bit partial register.
            - `ADDR32`: 32-bit memory address.
            - `ADDR16`: 16-bit relative memory address.
            - `ADDR8`: 8-bit relative memory address.
            - `REGPTR32`: Address pointed to by a 32-bit register's value.
            - `REGPTR16`: Relative address pointed to by a 16-bit register's value.
            - `REGPTR8`: Relative address pointed to by an 8-bit register's value.
            - `NULL`: Destination is not used.
        - `<SRC>` can be:
            - `REG`: General-purpose register.
            - `IMM`: Immediate 8-, 16-, or 32-bit value.
                - If `<DEST>` is a register, the size of the immediate value
                  is determined by the register size:
                    - 8-bit for 8-bit registers.
                    - 16-bit for 16-bit registers.
                    - 32-bit for 32-bit registers.
            - `ADDR32`: 32-bit memory address.
            - `ADDR16`: 16-bit relative memory address.
            - `ADDR8`: 8-bit relative memory address.
            - `REGPTR32`: Address pointed to by a 32-bit register's value.
            - `REGPTR16`: Relative address pointed to by a 16-bit register's value.
            - `REGPTR8`: Relative address pointed to by an 8-bit register's value.
            - `NULL`: Source is not used.
        - `ADDR16` and `REGPTR16` are relative to the absolute address `0xFFFF0000`.
        - `ADDR8` and `REGPTR8` are relative to the absolute address `0xFFFFFF00`.
            
*/

bool TM32CPU_Fetch_REG_NULL (TM32CPU_Processor* processor)
{
    // Param 1: Destination Register; Param 2: Nothing
    //
    // This is typically used for instructions that operate on a single register
    // (like INC, DEC, NOT, etc.) where the register is both source and destination.
    //
    // Load the value of the destination register into the `SD` register.
    processor->sd = TM32CPU_ReadRegister(processor, processor->iparam1);
    return true;
}

bool TM32CPU_Fetch_REG_REG (TM32CPU_Processor* processor)
{
    // Param 1: Destination Register; Param 2: Source Register
    //
    // Load the value of the source register into the `SD` register.
    processor->sd = TM32CPU_ReadRegister(processor, processor->iparam2);
    return true;
}

bool TM32CPU_Fetch_REG_IMM (TM32CPU_Processor* processor)
{
    // Param 1: Destination Register; Param 2: Immediate Value
    //
    // Determine the size of the immediate value based on the destination register
    // size, then read the appropriate number of bytes from the program counter
    // into the `SD` register.
    //
    // For instructions with a source or destination register parameter, the size
    // of the register can be determined by the lower two bits of the parameter:
    // - 0b00: Full 32-bit register
    // - 0b01: Partial 16-bit register
    // - 0b10 or 0b11: Partial 8-bit register
    switch (processor->iparam1 & 0b11)
    {
    case 0b00:
        return 
            TM32CPU_ReadDoubleWord(processor, processor->pc, &processor->sd) &&
            TM32CPU_AdvanceProgramCounter(processor, 4);
    case 0b01:
        return 
            TM32CPU_ReadWord(processor, processor->pc, (uint16_t*) &processor->sd) &&
            TM32CPU_AdvanceProgramCounter(processor, 2);
    default:
        return 
            TM32CPU_ReadByte(processor, processor->pc, (uint8_t*) &processor->sd) &&
            TM32CPU_AdvanceProgramCounter(processor, 1);
    }
}

bool TM32CPU_Fetch_REG_ADDR32 (TM32CPU_Processor* processor)
{
    // Param 1: Destination Register; Param 2: 32-bit Source Memory Address
    //
    // First, read the 32-bit memory address from the program counter into the
    // `SA` register.
    if (TM32CPU_ReadDoubleWord(processor, processor->pc, &processor->sa) == false)
    {
        return false;
    }

    // Advance the program counter past the 32-bit address
    if (TM32CPU_AdvanceProgramCounter(processor, 4) == false)
    {
        return false;
    }

    // Based on the size of the destination register:
    // - Determine if the source address range is within valid readable memory.
    // - Read the appropriate number of bytes from the memory address into the
    //   `SD` register.
    switch (processor->iparam1 & 0b11)
    {
    case 0b00: // Full 32-bit register
        return
            TM32CPU_CheckReadable(processor, processor->sa, 4) &&
            TM32CPU_ReadDoubleWord(processor, processor->sa, &processor->sd);
    case 0b01: // Partial 16-bit register
        return
            TM32CPU_CheckReadable(processor, processor->sa, 2) &&
            TM32CPU_ReadWord(processor, processor->sa, (uint16_t*) &processor->sd);
    default: // Partial 8-bit register
        return
            TM32CPU_CheckReadable(processor, processor->sa, 1) &&
            TM32CPU_ReadByte(processor, processor->sa, (uint8_t*) &processor->sd);
    }
}

bool TM32CPU_Fetch_REG_ADDR16 (TM32CPU_Processor* processor)
{
    // Param 1: Destination Register; Param 2: 16-bit Source Memory Address
    //
    // First, read the 16-bit memory address from the program counter into the
    // `SA` register.
    uint16_t relativeAddress = 0;
    if (
        TM32CPU_ReadWord(processor, processor->pc, &relativeAddress) == false ||
        TM32CPU_AdvanceProgramCounter(processor, 2) == false
    )
    {
        return false;
    }

    // 16-bit relative addresses are relative to the absolute address
    // `0xFFFF0000`. Therefore, we need to add this base address to
    // the relative address to get the absolute address.
    processor->sa = 0xFFFF0000 + relativeAddress;

    // Based on the size of the destination register:
    // - Determine if the source address range is within valid readable memory.
    // - Read the appropriate number of bytes from the memory address into the
    //   `SD` register.
    switch (processor->iparam1 & 0b11)
    {
    case 0b00: // Full 32-bit register
        return
            TM32CPU_CheckReadable(processor, processor->sa, 4) &&
            TM32CPU_ReadDoubleWord(processor, processor->sa, &processor->sd);
    case 0b01: // Partial 16-bit register
        return
            TM32CPU_CheckReadable(processor, processor->sa, 2) &&
            TM32CPU_ReadWord(processor, processor->sa, (uint16_t*) &processor->sd);
    default: // Partial 8-bit register
        return
            TM32CPU_CheckReadable(processor, processor->sa, 1) &&
            TM32CPU_ReadByte(processor, processor->sa, (uint8_t*) &processor->sd);
    }
}

bool TM32CPU_Fetch_REG_ADDR8 (TM32CPU_Processor* processor)
{
    // Param 1: Destination Register; Param 2: 8-bit Source Memory Address
    //
    // First, read the 8-bit memory address from the program counter into the
    // `SA` register.
    uint8_t relativeAddress = 0;
    if (
        TM32CPU_ReadByte(processor, processor->pc, &relativeAddress) == false ||
        TM32CPU_AdvanceProgramCounter(processor, 1) == false
    )
    {
        return false;
    }

    // 8-bit relative addresses are relative to the absolute address
    // `0xFFFFFF00`. Therefore, we need to add this base address to
    // the relative address to get the absolute address.
    processor->sa = 0xFFFFFF00 + relativeAddress;

    // Based on the size of the destination register:
    // - Determine if the source address range is within valid readable memory.
    // - Read the appropriate number of bytes from the memory address into the
    //   `SD` register.
    switch (processor->iparam1 & 0b11)
    {
    case 0b00: // Full 32-bit register
        return
            TM32CPU_CheckReadable(processor, processor->sa, 4) &&
            TM32CPU_ReadDoubleWord(processor, processor->sa, &processor->sd);
    case 0b01: // Partial 16-bit register
        return
            TM32CPU_CheckReadable(processor, processor->sa, 2) &&
            TM32CPU_ReadWord(processor, processor->sa, (uint16_t*) &processor->sd);
    default: // Partial 8-bit register
        return
            TM32CPU_CheckReadable(processor, processor->sa, 1) &&
            TM32CPU_ReadByte(processor, processor->sa, (uint8_t*) &processor->sd);
    }
}

bool TM32CPU_Fetch_REG_REGPTR32 (TM32CPU_Processor* processor)
{
    // Param 1: Destination Register; Param 2: 32-bit Source Register Pointer
    //
    // First, make sure the source register is a valid 32-bit register.
    if ((processor->iparam2 & 0b11) != 0b00)
    {
        return TM32CPU_RaiseException(processor, TM32CPU_EC_INVALID_ARGUMENT);
    }

    // Read the value of the source register into the `SA` register.
    processor->sa = TM32CPU_ReadRegister(processor, processor->iparam2);

    // Based on the size of the destination register:
    // - Determine if the source address range is within valid readable memory.
    // - Read the appropriate number of bytes from the memory address into the
    //   `SD` register.
    switch (processor->iparam1 & 0b11)
    {
    case 0b00: // Full 32-bit register
        return
            TM32CPU_CheckReadable(processor, processor->sa, 4) &&
            TM32CPU_ReadDoubleWord(processor, processor->sa, &processor->sd);
    case 0b01: // Partial 16-bit register
        return
            TM32CPU_CheckReadable(processor, processor->sa, 2) &&
            TM32CPU_ReadWord(processor, processor->sa, (uint16_t*) &processor->sd);
    default: // Partial 8-bit register
        return
            TM32CPU_CheckReadable(processor, processor->sa, 1) &&
            TM32CPU_ReadByte(processor, processor->sa, (uint8_t*) &processor->sd);
    }
}

bool TM32CPU_Fetch_REG_REGPTR16 (TM32CPU_Processor* processor)
{
    // Param 1: Destination Register; Param 2: 16-bit Source Register Pointer
    //
    // First, make sure the source register is a valid 16-bit register.
    if ((processor->iparam2 & 0b11) != 0b01)
    {
        return TM32CPU_RaiseException(processor, TM32CPU_EC_INVALID_ARGUMENT);
    }

    // Read the value of the source register into the `SA` register.
    //
    // 16-bit relative addresses are relative to the absolute address
    // `0xFFFF0000`. Therefore, we need to add this base address to
    // the relative address to get the absolute address.
    processor->sa = 0xFFFF0000 + TM32CPU_ReadRegister(processor, processor->iparam2);

    // Based on the size of the destination register:
    // - Determine if the source address range is within valid readable memory.
    // - Read the appropriate number of bytes from the memory address into the
    //   `SD` register.
    switch (processor->iparam1 & 0b11)
    {
    case 0b00: // Full 32-bit register
        return
            TM32CPU_CheckReadable(processor, processor->sa, 4) &&
            TM32CPU_ReadDoubleWord(processor, processor->sa, &processor->sd);
    case 0b01: // Partial 16-bit register
        return
            TM32CPU_CheckReadable(processor, processor->sa, 2) &&
            TM32CPU_ReadWord(processor, processor->sa, (uint16_t*) &processor->sd);
    default: // Partial 8-bit register
        return
            TM32CPU_CheckReadable(processor, processor->sa, 1) &&
            TM32CPU_ReadByte(processor, processor->sa, (uint8_t*) &processor->sd);
    }
}

bool TM32CPU_Fetch_REG_REGPTR8 (TM32CPU_Processor* processor)
{
    // Param 1: Destination Register; Param 2: 8-bit Source Register Pointer
    //
    // First, make sure the source register is a valid 8-bit register.
    if ((processor->iparam2 & 0b11) != 0b10 && (processor->iparam2 & 0b11) != 0b11)
    {
        return TM32CPU_RaiseException(processor, TM32CPU_EC_INVALID_ARGUMENT);
    }

    // Read the value of the source register into the `SA` register.
    //
    // 8-bit relative addresses are relative to the absolute address
    // `0xFFFFFF00`. Therefore, we need to add this base address to
    // the relative address to get the absolute address.
    processor->sa = 0xFFFFFF00 + TM32CPU_ReadRegister(processor, processor->iparam2);

    // Based on the size of the destination register:
    // - Determine if the source address range is within valid readable memory.
    // - Read the appropriate number of bytes from the memory address into the
    //   `SD` register.
    switch (processor->iparam1 & 0b11)
    {
    case 0b00: // Full 32-bit register
        return
            TM32CPU_CheckReadable(processor, processor->sa, 4) &&
            TM32CPU_ReadDoubleWord(processor, processor->sa, &processor->sd);
    case 0b01: // Partial 16-bit register
        return
            TM32CPU_CheckReadable(processor, processor->sa, 2) &&
            TM32CPU_ReadWord(processor, processor->sa, (uint16_t*) &processor->sd);
    default: // Partial 8-bit register
        return
            TM32CPU_CheckReadable(processor, processor->sa, 1) &&
            TM32CPU_ReadByte(processor, processor->sa, (uint8_t*) &processor->sd);
    }
}

bool TM32CPU_Fetch_ADDR32_REG (TM32CPU_Processor* processor)
{
    // Param 1: 32-bit Destination Memory Address; Param 2: Source Register
    //
    // First, read the 32-bit memory address from the program counter into the
    // `DA` register.
    if (
        TM32CPU_ReadDoubleWord(processor, processor->pc, &processor->da) == false ||
        TM32CPU_AdvanceProgramCounter(processor, 4) == false
    )
    {
        return false;
    }
    
    // Based on the size of the source register:
    // - Determine if the destination address range is within valid writable memory.
    switch (processor->iparam2 & 0b11)
    {
    case 0b00: // Full 32-bit register
        if (TM32CPU_CheckWritable(processor, processor->da, 4) == false)
            { return false; }
        break;
    case 0b01: // Partial 16-bit register
        if (TM32CPU_CheckWritable(processor, processor->da, 2) == false)
            { return false; }
        break;
    default: // Partial 8-bit register
        if (TM32CPU_CheckWritable(processor, processor->da, 1) == false)
            { return false; }
        break;
    }

    // Read the source register into the `SD` register.
    processor->sd = TM32CPU_ReadRegister(processor, processor->iparam2);
    return true;
}

bool TM32CPU_Fetch_ADDR16_REG (TM32CPU_Processor* processor)
{
    // Param 1: 16-bit Memory Address; Param 2: Source Register
    //
    // First, read the 16-bit memory address from the program counter into the
    // `DA` register.
    uint16_t relativeAddress = 0;
    if (
        TM32CPU_ReadWord(processor, processor->pc, &relativeAddress) == false ||
        TM32CPU_AdvanceProgramCounter(processor, 2) == false
    )
    {
        return false;
    }

    // 16-bit relative addresses are relative to the absolute address
    // `0xFFFF0000`. Therefore, we need to add this base address to
    // the relative address to get the absolute address.
    processor->da = 0xFFFF0000 + relativeAddress;

    // Based on the size of the source register:
    // - Determine if the destination address range is within valid writable memory.
    switch (processor->iparam2 & 0b11)
    {
    case 0b00: // Full 32-bit register
        if (TM32CPU_CheckWritable(processor, processor->da, 4) == false)
            { return false; }
        break;
    case 0b01: // Partial 16-bit register
        if (TM32CPU_CheckWritable(processor, processor->da, 2) == false)
            { return false; }
        break;
    default: // Partial 8-bit register
        if (TM32CPU_CheckWritable(processor, processor->da, 1) == false)
            { return false; }
        break;
    }

    // Read the source register into the `SD` register.
    processor->sd = TM32CPU_ReadRegister(processor, processor->iparam2);
    return true;
}

bool TM32CPU_Fetch_ADDR8_REG (TM32CPU_Processor* processor)
{
    // Param 1: 8-bit Memory Address; Param 2: Source Register
    //
    // First, read the 8-bit memory address from the program counter into the
    // `DA` register.
    uint8_t relativeAddress = 0;
    if (
        TM32CPU_ReadByte(processor, processor->pc, &relativeAddress) == false ||
        TM32CPU_AdvanceProgramCounter(processor, 1) == false
    )
    {
        return false;
    }

    // 8-bit relative addresses are relative to the absolute address
    // `0xFFFFFF00`. Therefore, we need to add this base address to
    // the relative address to get the absolute address.
    processor->da = 0xFFFFFF00 + relativeAddress;

    // Based on the size of the source register:
    // - Determine if the destination address range is within valid writable memory.
    switch (processor->iparam2 & 0b11)
    {
    case 0b00: // Full 32-bit register
        if (TM32CPU_CheckWritable(processor, processor->da, 4) == false)
            { return false; }
        break;
    case 0b01: // Partial 16-bit register
        if (TM32CPU_CheckWritable(processor, processor->da, 2) == false)
            { return false; }
        break;
    default: // Partial 8-bit register
        if (TM32CPU_CheckWritable(processor, processor->da, 1) == false)
            { return false; }
        break;
    }

    // Read the source register into the `SD` register.
    processor->sd = TM32CPU_ReadRegister(processor, processor->iparam2);
    return true;
}

bool TM32CPU_Fetch_REGPTR32_NULL (TM32CPU_Processor* processor)
{
    // Param 1: 32-bit Register Pointer; Param 2: Nothing
    //
    // First, make sure the register is a valid 32-bit register.
    if ((processor->iparam1 & 0b11) != 0b00)
    {
        return TM32CPU_RaiseException(processor, TM32CPU_EC_INVALID_ARGUMENT);
    }

    // Read the value of the source register into the `DA` register.
    processor->da = TM32CPU_ReadRegister(processor, processor->iparam1);

    // It's important to note that, for instructions with this fetching mode,
    // the register pointer parameter is both a source AND destination address.

    // Check if the address is within valid writable memory, then read the byte
    // at that address into the `SD` register.
    return
        TM32CPU_CheckWritable(processor, processor->da, 1) &&
        TM32CPU_ReadByte(processor, processor->da, (uint8_t*) &processor->sd);
}

bool TM32CPU_Fetch_REGPTR32_REG (TM32CPU_Processor* processor)
{
    // Param 1: 32-bit Destination Register Pointer; Param 2: Source Register
    //
    // First, make sure the destination register is a valid 32-bit register.
    if ((processor->iparam1 & 0b11) != 0b00)
    {
        return TM32CPU_RaiseException(processor, TM32CPU_EC_INVALID_ARGUMENT);
    }

    // Read the value of the destination register into the `DA` register.
    processor->da = TM32CPU_ReadRegister(processor, processor->iparam1);

    // Based on the size of the source register:
    // - Determine if the destination address range is within valid writable memory.
    switch (processor->iparam2 & 0b11)
    {
    case 0b00: // Full 32-bit register
        if (TM32CPU_CheckWritable(processor, processor->da, 4) == false)
            { return false; }
        break;
    case 0b01: // Partial 16-bit register
        if (TM32CPU_CheckWritable(processor, processor->da, 2) == false)
            { return false; }
        break;
    default: // Partial 8-bit register
        if (TM32CPU_CheckWritable(processor, processor->da, 1) == false)
            { return false; }
        break;
    }

    // Read the source register into the `SD` register.
    processor->sd = TM32CPU_ReadRegister(processor, processor->iparam2);
    return true;
}

bool TM32CPU_Fetch_REGPTR16_REG (TM32CPU_Processor* processor)
{
    // Param 1: 16-bit Destination Register Pointer; Param 2: Source Register
    //
    // First, make sure the destination register is a valid 16-bit register.
    if ((processor->iparam1 & 0b11) != 0b01)
    {
        return TM32CPU_RaiseException(processor, TM32CPU_EC_INVALID_ARGUMENT);
    }

    // Read the value of the destination register into the `DA` register, then
    // convert it to an absolute address by adding the base address `0xFFFF0000`.
    processor->da = 0xFFFF0000 + TM32CPU_ReadRegister(processor, processor->iparam1);

    // Based on the size of the source register:
    // - Determine if the destination address range is within valid writable memory.
    switch (processor->iparam2 & 0b11)
    {
    case 0b00: // Full 32-bit register
        if (TM32CPU_CheckWritable(processor, processor->da, 4) == false)
            { return false; }
        break;
    case 0b01: // Partial 16-bit register
        if (TM32CPU_CheckWritable(processor, processor->da, 2) == false)
            { return false; }
        break;
    default: // Partial 8-bit register
        if (TM32CPU_CheckWritable(processor, processor->da, 1) == false)
            { return false; }
        break;
    }

    // Read the source register into the `SD` register.
    processor->sd = TM32CPU_ReadRegister(processor, processor->iparam2);
    return true;
}

bool TM32CPU_Fetch_REGPTR8_REG (TM32CPU_Processor* processor)
{
    // Param 1: 8-bit Destination Register Pointer; Param 2: Source Register
    //
    // First, make sure the destination register is a valid 8-bit register.
    if ((processor->iparam1 & 0b11) != 0b10 && (processor->iparam1 & 0b11) != 0b11)
    {
        return TM32CPU_RaiseException(processor, TM32CPU_EC_INVALID_ARGUMENT);
    }

    // Read the value of the destination register into the `DA` register, then
    // convert it to an absolute address by adding the base address `0xFFFFFF00`.
    processor->da = 0xFFFFFF00 + TM32CPU_ReadRegister(processor, processor->iparam1);

    // Based on the size of the source register:
    // - Determine if the destination address range is within valid writable memory.
    switch (processor->iparam2 & 0b11)
    {
    case 0b00: // Full 32-bit register
        if (TM32CPU_CheckWritable(processor, processor->da, 4) == false)
            { return false; }
        break;
    case 0b01: // Partial 16-bit register
        if (TM32CPU_CheckWritable(processor, processor->da, 2) == false)
            { return false; }
        break;
    default: // Partial 8-bit register
        if (TM32CPU_CheckWritable(processor, processor->da, 1) == false)
            { return false; }
        break;
    }

    // Read the source register into the `SD` register.
    processor->sd = TM32CPU_ReadRegister(processor, processor->iparam2);
    return true;
}

bool TM32CPU_Fetch_NULL_IMM32 (TM32CPU_Processor* processor)
{
    // Param 1: Nothing; Param 2: 32-bit Immediate Value
    //
    // Read the 32-bit immediate value from the program counter into the `SD`
    // register.
    return
        TM32CPU_ReadDoubleWord(processor, processor->pc, &processor->sd) &&
        TM32CPU_AdvanceProgramCounter(processor, 4);
}

bool TM32CPU_Fetch_NULL_IMM16 (TM32CPU_Processor* processor)
{
    // Param 1: Nothing; Param 2: 16-bit Immediate Value
    //
    // Read the 16-bit immediate value from the program counter into the `SD`
    // register.
    return
        TM32CPU_ReadWord(processor, processor->pc, (uint16_t*) &processor->sd) &&
        TM32CPU_AdvanceProgramCounter(processor, 2);
}

bool TM32CPU_Fetch_XD_REG_NULL (TM32CPU_Processor* processor)
{
    // XD Register; Param 1: Destination Register; Param 2: Nothing
    //
    // Read an immediate byte from the program counter into the `XD` register.
    if (
        TM32CPU_ReadByte(processor, processor->pc, &processor->xd) == false ||
        TM32CPU_AdvanceProgramCounter(processor, 1) == false
    )
    {
        return false;
    }

    // Load the value of the destination register into the `SD` register.
    processor->sd = TM32CPU_ReadRegister(processor, processor->iparam1);
    return true;
}

bool TM32CPU_Fetch_XD_REGPTR32_NULL (TM32CPU_Processor* processor)
{
    // XD Register; Param 1: 32-bit Destination Register Pointer; Param 2: Nothing
    //
    // First, make sure the destination register is a valid 32-bit register.
    if ((processor->iparam1 & 0b11) != 0b00)
    {
        return TM32CPU_RaiseException(processor, TM32CPU_EC_INVALID_ARGUMENT);
    }

    // Read an immediate byte from the program counter into the `XD` register.
    if (
        TM32CPU_ReadByte(processor, processor->pc, &processor->xd) == false ||
        TM32CPU_AdvanceProgramCounter(processor, 1) == false
    )
    {
        return false;
    }

    // Read the value of the destination register into the `DA` register.
    processor->da = TM32CPU_ReadRegister(processor, processor->iparam1);

    // Check if the address is within valid writable memory, then read the byte
    // at that address into the `SD` register.
    return
        TM32CPU_CheckWritable(processor, processor->da, 1) &&
        TM32CPU_ReadByte(processor, processor->da, (uint8_t*) &processor->sd);
}

/* Private Functions - Instruction Execution **********************************/

/*
    The `TM32CPU_Execute_*` function names are an exception to the TM32 workspace's
    code style. These function follow a specific naming convention to indicate
    the type of instruction they execute.

    Notation: `TM32CPU_Execute_<INSTRUCTION>`

    Where:
        - `<INSTRUCTION>` is the name of the instruction being executed.
*/

bool TM32CPU_Execute_NOP (
    TM32CPU_Processor* processor
)
{
    // 0x0000 NOP
    //
    // Does nothing.

    return true;
}

bool TM32CPU_Execute_STOP (
    TM32CPU_Processor* processor
)
{
    // 0x0100 STOP
    //
    // Enters ultra-low power standby mode. Can only be woken by external 
    // wake-up events or reset. This is Game Boy-inspired behavior.
    TM32CPU_SetFlags(processor, TM32CPU_FT_T);
    return true;
}

bool TM32CPU_Execute_HALT (
    TM32CPU_Processor* processor
)
{
    // 0x0200 HALT
    //
    // Halts the processor until an interrupt occurs.
    TM32CPU_SetFlags(processor, TM32CPU_FT_L);
    return true;
}

bool TM32CPU_Execute_SEC (
    TM32CPU_Processor* processor
)
{
    // 0x03XX SEC XX
    //
    // Sets the error code register to `XX`.
    processor->ec = processor->ci & 0xFF;
    return true;
}

bool TM32CPU_Execute_CEC (
    TM32CPU_Processor* processor
)
{
    // 0x0400 CEC
    //
    // Clears the error code register.
    processor->ec = TM32CPU_EC_OK;
    return true;
}

bool TM32CPU_Execute_DI (
    TM32CPU_Processor* processor
)
{
    // 0x0500 DI
    //
    // Disables interrupts.
    processor->ime = false;
    return true;
}

bool TM32CPU_Execute_EI (
    TM32CPU_Processor* processor
)
{
    // 0x0600 EI
    //
    // Enables interrupts.
    processor->ei = true;
    return true;
}

bool TM32CPU_Execute_DAA (
    TM32CPU_Processor* processor
)
{
    // 0x0700 DAA
    //
    // Decimal Adjusts Byte Register `AL`.
    
    // Get the carry, half-carry and subtract flags.
    bool carryFlag = TM32CPU_CheckFlags(processor, TM32CPU_FT_C);
    bool halfCarryFlag = TM32CPU_CheckFlags(processor, TM32CPU_FT_H);
    bool subtractFlag = TM32CPU_CheckFlags(processor, TM32CPU_FT_N);

    // Get the value of the `AL` byte register. Keep track of how it needs to be
    // adjusted.
    uint8_t al = TM32CPU_ReadRegister(processor, TM32CPU_RT_AL);
    uint8_t adjustment = 0, result = 0;

    // Check the low nibble. If it is greater than 0x9 (represented by a letter,
    // rather than a decimal), or if the half-carry flag is set, add 0x6 to it 
    // to decimal-adjust that nibble.
    if (halfCarryFlag == true || (al & 0x0F) > 0x09)
    {
        adjustment += 0x06;
    }

    // Check the high nibble. If it is greater than 0x9, or if the carry flag
    // is set, add 0x60 to it to decimal-adjust that nibble. Also, set or clear
    // the carry flag accordingly.
    if (carryFlag == true || (al & 0xF0) > 0x90)
    {
        adjustment += 0x60;
        carryFlag = true;
    }
    else
    {
        carryFlag = false;
    }

    // Add or subtract the adjustment value from the `AL` register, depending
    // on the subtract flag.
    result = (subtractFlag == true) ?
        (al - adjustment) : 
        (al + adjustment);

    // Write the result back to the `AL` register.
    TM32CPU_WriteRegister(processor, TM32CPU_RT_AL, result);

    // Set the flags accordingly.
    TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, (result == 0));
    TM32CPU_ClearFlags(processor, TM32CPU_FT_H);
    TM32CPU_ChangeFlags(processor, TM32CPU_FT_C, carryFlag);
    TM32CPU_CheckParity(processor, result);

    return true;
}

bool TM32CPU_Execute_SCF (
    TM32CPU_Processor* processor
)
{
    // 0x0800 SCF
    //
    // Sets the carry flag.
    TM32CPU_SetFlags(processor, TM32CPU_FT_C);
    TM32CPU_ClearFlags(processor, TM32CPU_FT_H | TM32CPU_FT_N);
    return true;
}

bool TM32CPU_Execute_CCF (
    TM32CPU_Processor* processor
)
{
    // 0x0900 CCF
    //
    // Complements the carry flag.
    bool carryFlag = TM32CPU_CheckFlags(processor, TM32CPU_FT_C);

    TM32CPU_ChangeFlags(processor, TM32CPU_FT_C, !carryFlag);
    TM32CPU_ClearFlags(processor, TM32CPU_FT_H | TM32CPU_FT_N);
    return true;
}

bool TM32CPU_Execute_FLG (
    TM32CPU_Processor* processor
)
{
    // 0x0AX0 FLG X
    //
    // Checks to see if bit (16 + `X`) of the flags register is set.
    uint8_t bitIndex = 16 + processor->iparam1;
    TM32CPU_HardAssert(bitIndex >= 16 && bitIndex <= 31);

    // Set the Zero flag if the bit is cleared, clear it otherwise.
    bool bitSet = (processor->flags & (1 << bitIndex)) != 0;
    TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, !bitSet);

    return true;
}

bool TM32CPU_Execute_STF (
    TM32CPU_Processor* processor
)
{
    // 0x0BX0 STF X
    //
    // Sets bit (16 + `X`) of the flags register.
    uint8_t bitIndex = 16 + processor->iparam1;
    TM32CPU_HardAssert(bitIndex >= 16 && bitIndex <= 31);

    // Set the bit in the flags register.
    processor->flags |= (1 << bitIndex);
    return true;
}

bool TM32CPU_Execute_CLF (
    TM32CPU_Processor* processor
)
{
    // 0x0CX0 CLF X
    //
    // Clears bit (16 + `X`) of the flags register.
    uint8_t bitIndex = 16 + processor->iparam1;
    TM32CPU_HardAssert(bitIndex >= 16 && bitIndex <= 31);

    // Clear the bit in the flags register.
    processor->flags &= ~(1 << bitIndex);
    return true;
}

bool TM32CPU_Execute_LD (
    TM32CPU_Processor* processor
)
{
    // 0x10X0 LD X, IMM
    // 0x11X0 LD X, [ADDR32]
    // 0x12XY LD X, [Y32]
    // 0x13X0 LDH X, [ADDR16]
    // 0x14XY LDH X, [Y16]
    // 0x15X0 LDP X, [ADDR8]
    // 0x16XY LDP X, [Y8]
    //
    // Loads a source value into a destination register. The appropriate fetch
    // function will store the source value in the `SD` register.

    TM32CPU_WriteRegister(processor, processor->iparam1, processor->sd);
    return true;
}

bool TM32CPU_Execute_ST (
    TM32CPU_Processor* processor
)
{
    // 0x170Y ST [ADDR32], Y
    // 0x18XY ST [X32], Y
    // 0x190Y STH [ADDR16], Y
    // 0x1AXY STH [X16], Y
    // 0x1B0Y STP [ADDR8], Y
    // 0x1CXY STP [X8], Y
    //
    // Stores a value from a source register into a destination memory address.
    // The appropriate fetch function will store the destination address in the
    // `DA` register and the source value in the `SD` register, as well as
    // checking if the address is writable.

    // Depending on the size of the source register, write the appropriate
    // number of bytes to the destination address.
    switch (processor->iparam2 & 0b11)
    {
    case 0b00: // Full 32-bit register
        return
            TM32CPU_CheckWritable(processor, processor->da, 4) &&
            TM32CPU_WriteDoubleWord(processor, processor->da, processor->sd);
    case 0b01: // Partial 16-bit register
        return
            TM32CPU_CheckWritable(processor, processor->da, 2) &&
            TM32CPU_WriteWord(processor, processor->da, processor->sd & 0xFFFF);
    default: // Partial 8-bit register
        return
            TM32CPU_CheckWritable(processor, processor->da, 1) &&
            TM32CPU_WriteByte(processor, processor->da, processor->sd & 0xFF);
    }
}

bool TM32CPU_Execute_MV (
    TM32CPU_Processor* processor
)
{
    // 0x1DXY MV X, Y
    //
    // Copies (moves) the value of a source register into a destination
    // register. The source value is already stored in the `SD` register.
    TM32CPU_WriteRegister(processor, processor->iparam1, processor->sd);
    return true;
}

bool TM32CPU_Execute_POP (
    TM32CPU_Processor* processor
)
{
    // 0x1EX0 POP X32
    //
    // Pops a value from the top of the stack, and stores it in a 32-bit
    // destination register. The stack pointer is decremented by 4 bytes.
    //
    // There is no fetch function for this instruction, as the stack and stack
    // pointer are used here.

    // First, make sure the destination register is a valid 32-bit register.
    if ((processor->iparam1 & 0b11) != 0b00)
    {
        return TM32CPU_RaiseException(processor, TM32CPU_EC_INVALID_ARGUMENT);
    }

    // Pop the value from the stack into the `SD` register.
    if (TM32CPU_Pop(processor, &processor->sd) == false)
    {
        return false;
    }

    // Write the value from the `SD` register into the destination register.
    TM32CPU_WriteRegister(processor, processor->iparam1, processor->sd);
    return true;
}

bool TM32CPU_Execute_PUSH (
    TM32CPU_Processor* processor
)
{
    // 0x1F0Y PUSH Y32
    //
    // Pushes a value from a source 32-bit register onto the stack, and
    // increments the stack pointer by 4 bytes.
    //
    // There is no fetch function for this instruction, as the stack and stack
    // pointer are used here.

    // First, make sure the source register is a valid 32-bit register.
    if ((processor->iparam2 & 0b11) != 0b00)
    {
        return TM32CPU_RaiseException(processor, TM32CPU_EC_INVALID_ARGUMENT);
    }

    // Read the value of the source register into the `SD` register.
    processor->sd = TM32CPU_ReadRegister(processor, processor->iparam2);

    // Push the value from the `SD` register onto the stack.
    return TM32CPU_Push(processor, processor->sd);
}

bool TM32CPU_Execute_JMP (
    TM32CPU_Processor* processor
)
{
    // 0x20Z0 JMP Z, IMM32
    // 0x21ZX JMP Z, X32
    //
    // If execution condition `Z` is met, moves the program counter to the
    // specified immediate address. The target address is already stored in
    // the `SD` register.

    if (TM32CPU_CheckCondition(processor, processor->iparam1) == true)
    {
        // Set the program counter to the target address.
        processor->pc = processor->sd;

        // Manipulations of the program counter by the control-transfer
        // instructions result in a cycle being consumed.
        return TM32CPU_ConsumeCycles(processor, 1);
    }

    return true;
}

bool TM32CPU_Execute_JPB (
    TM32CPU_Processor* processor
)
{
    // 0x22Z0 JPB Z, SIMM16
    //
    // If execution condition `Z` is met, moves the program counter by an
    // offset specified by the signed immediate value. The offset is already
    // stored in the `SD` register.

    if (TM32CPU_CheckCondition(processor, processor->iparam1) == true)
    {
        processor->pc += (int16_t) processor->sd;
        return TM32CPU_ConsumeCycles(processor, 1);
    }

    return true;
}

bool TM32CPU_Execute_CALL (
    TM32CPU_Processor* processor
)
{
    // 0x23Z0 CALL Z, IMM32
    //
    // If execution condition `Z` is met, calls a subroutine at the specified
    // immediate address. The target address is already stored in the `SD`
    // register.

    if (TM32CPU_CheckCondition(processor, processor->iparam1) == true)
    {
        // Push the current program counter onto the stack.
        if (TM32CPU_Push(processor, processor->pc) == false)
        {
            return false;
        }

        // Set the program counter to the target address.
        processor->pc = processor->sd;

        // Manipulations of the program counter by the control-transfer
        // instructions result in a cycle being consumed.
        return TM32CPU_ConsumeCycles(processor, 1);
    }

    return true;
}

bool TM32CPU_Execute_INT (
    TM32CPU_Processor* processor
)
{
    // 0x24XX INT XX
    //
    // Explicitly calls one of the TM32 CPU's 32 interrupt handlers. `XX` must
    // be a value between 0 and 31, inclusive. This will call the interrupt
    // handler at the address `0xFFFF0000 + (XX * 4)`, regardless of its bits
    // in the `intflags` and `intenable` registers, and the `ime` flag.

    // The target interrupt vector's index is stored in the low byte of the
    // `CI` register.
    uint8_t vector = processor->ci & 0xFF;
    if (vector > 31)
    {
        return TM32CPU_RaiseException(processor, TM32CPU_EC_INVALID_ARGUMENT);
    }

    // Push the current program counter onto the stack.
    if (TM32CPU_Push(processor, processor->pc) == false)
    {
        return false;
    }

    // Set the program counter to the interrupt vector's address.
    processor->pc = (TM32CPU_INTERRUPT_START +
        (vector * TM32CPU_INTERRUPT_VECTOR_SIZE));

    return TM32CPU_ConsumeCycles(processor, 1);
}

bool TM32CPU_Execute_RET (
    TM32CPU_Processor* processor
)
{
    // 0x25Z0 RET Z
    //
    // If execution condition `Z` is met, returns from a subroutine by popping
    // the top value from the stack into the program counter.

    if (TM32CPU_CheckCondition(processor, processor->iparam1) == true)
    {
        return TM32CPU_Pop(processor, &processor->pc) &&
               TM32CPU_ConsumeCycles(processor, 1);
    }

    return true;
}

bool TM32CPU_Execute_RETI (
    TM32CPU_Processor* processor
)
{
    // 0x2600 RETI
    //
    // Enables interrupts and returns from an interrupt handler by popping
    // the top value from the stack into the program counter.
    //
    // This instruction is similar to `RET`, but it also enables interrupts
    // after returning from the interrupt handler. Unlike `EI`, this instruction
    // immediately enables interrupts, without waiting for the end of the
    // current frame.

    if (
        TM32CPU_Pop(processor, &processor->pc) == false ||
        TM32CPU_ConsumeCycles(processor, 1) == false
    )
    {
        return false;
    }
    
    processor->ime = true; // Enable interrupts.
    processor->ei = false; // Clear the EI flag.
    return true;
}

bool TM32CPU_Execute_INC (
    TM32CPU_Processor* processor
)
{
    // 0x30X0 INC X
    // 0x31X0 INC [X32]
    //
    // Increments the source value by one. The value to be incremented is
    // currently stored in the `SD` register, and the destination is determined
    // by whether or not the `DA` register is non-zero:
    // - If `DA` is zero, then the destination is a source register (0x30X0).
    // - Otherwise, the destination is a byte in memory at the address
    //   specified by the `DA` register (0x31X0).

    // Increment the value in the `SD` register, and store the result.
    uint32_t result = processor->sd + 1;

    // If the `DA` register is set, then a byte in memory is being written to.
    if (processor->da != 0)
    {
        // Set flags as if a byte register was being affected.
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, (result & 0xFF) == 0);
        TM32CPU_ClearFlags(processor, TM32CPU_FT_N);
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_H, (result & 0xF) == 0);
        TM32CPU_CheckParity(processor, result & 0xFF);

        // Write the result to the memory address specified by the `DA` register.
        return TM32CPU_WriteByte(processor, processor->da, result & 0xFF);
    }

    // Otherwise, the destination is a source register.
    TM32CPU_WriteRegister(processor, processor->iparam1, result);

    // Set flags according to the size of the source register.
    TM32CPU_ClearFlags(processor, TM32CPU_FT_N);
    switch (processor->iparam1 & 0b11)
    {
    case 0b00: // Full 32-bit register
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, (result == 0));
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_H, (result & 0xFFFFFFF) == 0);
        TM32CPU_CheckParity(processor, result);
        break;
    case 0b01: // Partial 16-bit register
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, (result & 0xFFFF) == 0);
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_H, (result & 0xFFF) == 0);
        TM32CPU_CheckParity(processor, result & 0xFFFF);
        break;
    default: // Partial 8-bit register
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, (result & 0xFF) == 0);
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_H, (result & 0xF) == 0);
        TM32CPU_CheckParity(processor, result & 0xFF);
        break;
    }

    return true;
}

bool TM32CPU_Execute_DEC (
    TM32CPU_Processor* processor
)
{
    // 0x32X0 DEC X
    // 0x33X0 DEC [X32]
    //
    // Decrements the source value by one. The value to be decremented is
    // currently stored in the `SD` register, and the destination is determined
    // by whether or not the `DA` register is non-zero:
    // - If `DA` is zero, then the destination is a source register (0x32X0).
    // - Otherwise, the destination is a byte in memory at the address
    //   specified by the `DA` register (0x33X0).

    // Decrement the value in the `SD` register, and store the result.
    uint32_t result = processor->sd - 1;

    // If the `DA` register is set, then a byte in memory is being written to.
    if (processor->da != 0)
    {
        // Set flags as if a byte register was being affected.
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, (result & 0xFF) == 0);
        TM32CPU_SetFlags(processor, TM32CPU_FT_N);
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_H, (result & 0xF) == 0xF);
        TM32CPU_CheckParity(processor, result & 0xFF);

        // Write the result to the memory address specified by the `DA` register.
        return TM32CPU_WriteByte(processor, processor->da, result & 0xFF);
    }

    // Otherwise, the destination is a source register.
    TM32CPU_WriteRegister(processor, processor->iparam1, result);

    // Set flags according to the size of the source register.
    TM32CPU_SetFlags(processor, TM32CPU_FT_N);
    switch (processor->iparam1 & 0b11)
    {
    case 0b00: // Full 32-bit register
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, (result == 0));
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_H, (result & 0xFFFFFFF) == 0xFFFFFFF);
        TM32CPU_CheckParity(processor, result);
        break;
    case 0b01: // Partial 16-bit register
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, (result & 0xFFFF) == 0);
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_H, (result & 0xFFF) == 0xFFF);
        TM32CPU_CheckParity(processor, result & 0xFFFF);
        break;
    default: // Partial 8-bit register
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, (result & 0xFF) == 0);
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_H, (result & 0xF) == 0xF);
        TM32CPU_CheckParity(processor, result & 0xFF);
        break;
    }

    return true;
}

bool TM32CPU_Execute_ADD (
    TM32CPU_Processor* processor
)
{
    // 0x34X0 ADD X, IMM
    // 0x35X0 ADD X, [ADDR32]
    // 0x36XY ADD X, [Y32]
    //
    // Adds a source value to a destination accumulator register. The source
    // value is currently stored in the `SD` register.

    // Get the lefthand and righthand values.
    uint32_t left  = TM32CPU_ReadRegister(processor, processor->iparam1);
    uint32_t right = processor->sd;

    // Add the two values together, and store the result.
    //
    // If the `ADC` instruction is calling this function, then the `xd` register
    // will contain the state of the carry flag, which is added to the
    // righthand value.
    uint64_t result = left + right + processor->xd;

    // Write the result back to the destination register.
    TM32CPU_WriteRegister(processor, processor->iparam1, result);

    // Set the flags according to the size of the destination register. Setting
    // the Half-Carry flag will require an additional addition of all but the
    // most significant nibbles of the left and right values.
    TM32CPU_ClearFlags(processor, TM32CPU_FT_N | TM32CPU_FT_S);
    switch (processor->iparam1 & 0b11)
    {
    case 0b00: // Full 32-bit register
        uint32_t half = (left & 0x0FFFFFFF) + (right & 0x0FFFFFFF) + processor->xd;
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, (result & 0xFFFFFFFF) == 0);
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_H, (half & 0x0FFFFFFF) > 0x0FFFFFFF);
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_C, result > 0xFFFFFFFF);
        TM32CPU_CheckParity(processor, result & 0xFFFFFFFF);
        break;
    case 0b01: // Partial 16-bit register
        half = (left & 0x0FFF) + (right & 0x0FFF) + processor->xd;
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, (result & 0xFFFF) == 0);
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_H, (half & 0x0FFF) > 0x0FFF);
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_C, result > 0xFFFF);
        TM32CPU_CheckParity(processor, result & 0xFFFF);
        break;
    default: // Partial 8-bit register
        half = (left & 0x0F) + (right & 0x0F) + processor->xd;
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, (result & 0xFF) == 0);
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_H, (half & 0x0F) > 0x0F);
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_C, result > 0xFF);
        TM32CPU_CheckParity(processor, result & 0xFF);
        break;
    }

    return true;
}

bool TM32CPU_Execute_ADC (
    TM32CPU_Processor* processor
)
{
    // 0x37X0 ADC X, IMM
    // 0x38X0 ADC X, [ADDR32]
    // 0x39XY ADC X, [Y32]
    //
    // Adds a source value and the carry flag to a destination accumulator
    // register. The source value is currently stored in the `SD` register.

    // This is the same as `ADD`, but with the carry flag added to the
    // righthand value if set.
    processor->xd = TM32CPU_CheckFlags(processor, TM32CPU_FT_C) ? 1 : 0;
    return TM32CPU_Execute_ADD(processor);
}

bool TM32CPU_Execute_SUB (
    TM32CPU_Processor* processor
)
{
    // 0x3AX0 SUB X, IMM
    // 0x3BX0 SUB X, [ADDR32]
    // 0x3CXY SUB X, [Y32]
    //
    // Subtracts a source value from a destination accumulator register. The
    // source value is currently stored in the `SD` register.

    // Get the lefthand and righthand values.
    uint32_t left  = TM32CPU_ReadRegister(processor, processor->iparam1);
    uint32_t right = processor->sd;

    // Subtract the two values, and store the result. Store the result in
    // in a signed integer to account for negative values.
    //
    // If the `SBC` instruction is calling this function, then the `xd` register
    // will contain the state of the carry flag, which is subtracted from the
    // righthand value.
    int64_t result = left - right - processor->xd;

    // Write the result back to the destination register.
    TM32CPU_WriteRegister(processor, processor->iparam1, result & 0xFFFFFFFF);

    // Set the flags according to the size of the destination register. Setting
    // the Half-Carry flag will require an additional subtraction of all but the
    // most significant nibbles of the left and right values.
    TM32CPU_SetFlags(processor, TM32CPU_FT_N);
    switch (processor->iparam1 & 0b11)
    {
    case 0b00: // Full 32-bit register
        int32_t half = (left & 0x0FFFFFFF) - (right & 0x0FFFFFFF) - processor->xd;
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, (result & 0xFFFFFFFF) == 0);
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_H, (half & 0x0FFFFFFF) < 0x0);
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_C | TM32CPU_FT_S, result < 0);
        TM32CPU_CheckParity(processor, result & 0xFFFFFFFF);
        break;
    case 0b01: // Partial 16-bit register
        half = (left & 0x0FFF) - (right & 0x0FFF) - processor->xd;
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, (result & 0xFFFF) == 0);
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_H, (half & 0x0FFF) < 0x0);
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_C | TM32CPU_FT_S, result < 0);
        TM32CPU_CheckParity(processor, result & 0xFFFF);
        break;
    default: // Partial 8-bit register
        half = (left & 0x0F) - (right & 0x0F) - processor->xd;
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, (result & 0xFF) == 0);
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_H, (half & 0x0F) < 0x0);
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_C | TM32CPU_FT_S, result < 0);
        TM32CPU_CheckParity(processor, result & 0xFF);
        break;
    }

    return true;
}

bool TM32CPU_Execute_SBC (
    TM32CPU_Processor* processor
)
{
    // 0x3DX0 SBC X, IMM
    // 0x3EX0 SBC X, [ADDR32]
    // 0x3FXY SBC X, [Y32]
    //
    // Subtracts a source value and the carry flag from a destination accumulator
    // register. The source value is currently stored in the `SD` register.

    // This is the same as `SUB`, but with the carry flag subtracted from the
    // righthand value if set.
    processor->xd = TM32CPU_CheckFlags(processor, TM32CPU_FT_C) ? 1 : 0;
    return TM32CPU_Execute_SUB(processor);
}

bool TM32CPU_Execute_AND (
    TM32CPU_Processor* processor
)
{
    // 0x40X0 AND X, IMM
    // 0x41X0 AND X, [ADDR32]
    // 0x42XY AND X, [Y32]
    //
    // Performs bitwise AND between a destination register and a source value.
    // The source value is currently stored in the `SD` register.

    // Get the lefthand and righthand values.
    uint32_t left  = TM32CPU_ReadRegister(processor, processor->iparam1);
    uint32_t right = processor->sd;

    // Perform the AND operation.
    uint32_t result = left & right;

    // Write the result back to the destination register.
    TM32CPU_WriteRegister(processor, processor->iparam1, result);

    // Set the flags according to the size of the destination register.
    // AND clears the Carry and Half-Carry flags, sets the Negative flag, and
    // sets the Zero, Sign, and Parity flags based on the result.
    TM32CPU_ClearFlags(processor, TM32CPU_FT_C | TM32CPU_FT_H);
    TM32CPU_SetFlags(processor, TM32CPU_FT_N);
    switch (processor->iparam1 & 0b11)
    {
    case 0b00: // Full 32-bit register
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, result == 0);
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_S, (result & 0x80000000) != 0);
        TM32CPU_CheckParity(processor, result);
        break;
    case 0b01: // Partial 16-bit register
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, (result & 0xFFFF) == 0);
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_S, (result & 0x8000) != 0);
        TM32CPU_CheckParity(processor, result & 0xFFFF);
        break;
    default: // Partial 8-bit register
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, (result & 0xFF) == 0);
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_S, (result & 0x80) != 0);
        TM32CPU_CheckParity(processor, result & 0xFF);
        break;
    }

    return true;
}

bool TM32CPU_Execute_OR (
    TM32CPU_Processor* processor
)
{
    // 0x43X0 OR X, IMM
    // 0x44X0 OR X, [ADDR32]
    // 0x45XY OR X, [Y32]
    //
    // Performs bitwise OR between a destination register and a source value.
    // The source value is currently stored in the `SD` register.

    // Get the lefthand and righthand values.
    uint32_t left  = TM32CPU_ReadRegister(processor, processor->iparam1);
    uint32_t right = processor->sd;

    // Perform the OR operation.
    uint32_t result = left | right;

    // Write the result back to the destination register.
    TM32CPU_WriteRegister(processor, processor->iparam1, result);

    // Set the flags according to the size of the destination register.
    // OR clears the Negative, Half-Carry, and Carry flags, and sets the Zero,
    // Sign, and Parity flags based on the result.
    TM32CPU_ClearFlags(processor, TM32CPU_FT_N | TM32CPU_FT_H | TM32CPU_FT_C);
    switch (processor->iparam1 & 0b11)
    {
    case 0b00: // Full 32-bit register
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, result == 0);
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_S, (result & 0x80000000) != 0);
        TM32CPU_CheckParity(processor, result);
        break;
    case 0b01: // Partial 16-bit register
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, (result & 0xFFFF) == 0);
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_S, (result & 0x8000) != 0);
        TM32CPU_CheckParity(processor, result & 0xFFFF);
        break;
    default: // Partial 8-bit register
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, (result & 0xFF) == 0);
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_S, (result & 0x80) != 0);
        TM32CPU_CheckParity(processor, result & 0xFF);
        break;
    }

    return true;
}

bool TM32CPU_Execute_XOR (
    TM32CPU_Processor* processor
)
{
    // 0x46X0 XOR X, IMM
    // 0x47X0 XOR X, [ADDR32]
    // 0x48XY XOR X, [Y32]
    //
    // Performs bitwise XOR between a destination register and a source value.
    // The source value is currently stored in the `SD` register.

    // Get the lefthand and righthand values.
    uint32_t left  = TM32CPU_ReadRegister(processor, processor->iparam1);
    uint32_t right = processor->sd;

    // Perform the XOR operation.
    uint32_t result = left ^ right;

    // Write the result back to the destination register.
    TM32CPU_WriteRegister(processor, processor->iparam1, result);

    // Set the flags according to the size of the destination register.
    // XOR clears the Negative, Half-Carry, and Carry flags, and sets the Zero,
    // Sign, and Parity flags based on the result.
    TM32CPU_ClearFlags(processor, TM32CPU_FT_N | TM32CPU_FT_H | TM32CPU_FT_C);
    switch (processor->iparam1 & 0b11)
    {
    case 0b00: // Full 32-bit register
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, result == 0);
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_S, (result & 0x80000000) != 0);
        TM32CPU_CheckParity(processor, result);
        break;
    case 0b01: // Partial 16-bit register
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, (result & 0xFFFF) == 0);
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_S, (result & 0x8000) != 0);
        TM32CPU_CheckParity(processor, result & 0xFFFF);
        break;
    default: // Partial 8-bit register
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, (result & 0xFF) == 0);
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_S, (result & 0x80) != 0);
        TM32CPU_CheckParity(processor, result & 0xFF);
        break;
    }

    return true;
}

bool TM32CPU_Execute_NOT (
    TM32CPU_Processor* processor
)
{
    // 0x4CX0 NOT X
    // 0x4DX0 NOT [X32]
    //
    // Performs bitwise NOT (complement) on a value. The value to be complemented 
    // is currently stored in the `SD` register, and the destination is determined
    // by whether or not the `DA` register is non-zero:
    // - If `DA` is zero, then the destination is a source register (0x4CX0).
    // - Otherwise, the destination is a byte in memory at the address
    //   specified by the `DA` register (0x4DX0).

    // Perform the NOT operation on the value in the `SD` register.
    uint32_t result = ~processor->sd;

    // If the `DA` register is set, then a byte in memory is being written to.
    if (processor->da != 0)
    {
        // Write only the low byte to memory
        return TM32CPU_WriteByte(processor, processor->da, result & 0xFF);
    }

    // Otherwise, the destination is a source register.
    TM32CPU_WriteRegister(processor, processor->iparam1, result);

    // Set the flags according to the NOT instruction.
    // NOT sets the Carry and Negative flags, and leaves all other flags
    // unaffected.
    TM32CPU_SetFlags(processor, TM32CPU_FT_C | TM32CPU_FT_N);

    return true;
}

bool TM32CPU_Execute_CMP (
    TM32CPU_Processor* processor
)
{
    // 0x50X0 CMP X, IMM
    // 0x51X0 CMP X, [ADDR32]
    // 0x52XY CMP X, [Y32]
    //
    // Compares a register value with a source value by performing subtraction
    // without storing the result. Only the flags are affected.
    // The source value is currently stored in the `SD` register.

    // Get the lefthand and righthand values.
    uint32_t left  = TM32CPU_ReadRegister(processor, processor->iparam1);
    uint32_t right = processor->sd;

    // Subtract the two values, and store the result in a signed integer
    // to account for negative values. We don't store this result back to 
    // any register - this is compare, not subtract.
    int64_t result = left - right;

    // Set the flags according to the size of the destination register. Setting
    // the Half-Carry flag will require an additional subtraction of all but the
    // most significant nibbles of the left and right values.
    // CMP sets the Carry flag and affects the Zero, Negative, Half-Carry, and
    // Sign flags based on the comparison result.
    TM32CPU_SetFlags(processor, TM32CPU_FT_C);
    switch (processor->iparam1 & 0b11)
    {
    case 0b00: // Full 32-bit register
        {
            int32_t half = (left & 0x0FFFFFFF) - (right & 0x0FFFFFFF);
            TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, (result & 0xFFFFFFFF) == 0);
            TM32CPU_ChangeFlags(processor, TM32CPU_FT_H, (half & 0x0FFFFFFF) < 0x0);
            TM32CPU_ChangeFlags(processor, TM32CPU_FT_N, result < 0);
            TM32CPU_ChangeFlags(processor, TM32CPU_FT_S, (result & 0x80000000) != 0);
        }
        break;
    case 0b01: // Partial 16-bit register
        {
            int32_t half = (left & 0x0FFF) - (right & 0x0FFF);
            TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, (result & 0xFFFF) == 0);
            TM32CPU_ChangeFlags(processor, TM32CPU_FT_H, (half & 0x0FFF) < 0x0);
            TM32CPU_ChangeFlags(processor, TM32CPU_FT_N, result < 0);
            TM32CPU_ChangeFlags(processor, TM32CPU_FT_S, (result & 0x8000) != 0);
        }
        break;
    default: // Partial 8-bit register
        {
            int32_t half = (left & 0x0F) - (right & 0x0F);
            TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, (result & 0xFF) == 0);
            TM32CPU_ChangeFlags(processor, TM32CPU_FT_H, (half & 0x0F) < 0x0);
            TM32CPU_ChangeFlags(processor, TM32CPU_FT_N, result < 0);
            TM32CPU_ChangeFlags(processor, TM32CPU_FT_S, (result & 0x80) != 0);
        }
        break;
    }

    return true;
}

bool TM32CPU_Execute_SLA (
    TM32CPU_Processor* processor
)
{
    // 0x60X0 SLA X
    // 0x61X0 SLA [X32]
    //
    // Shifts left arithmetically (SLA). The value to be shifted is currently 
    // stored in the `SD` register, and the destination is determined by whether 
    // or not the `DA` register is non-zero:
    // - If `DA` is zero, then the destination is a source register (0x60X0).
    // - Otherwise, the destination is a byte in memory at the address
    //   specified by the `DA` register (0x61X0).
    //
    // SLA sets the Carry flag to the leftmost bit before the shift, 
    // then sets the rightmost bit to zero after the shift.

    uint32_t value = processor->sd;
    uint32_t result = 0;
    bool carryOut = false;

    // Determine the bit width and perform the shift
    if (processor->da != 0)
    {
        // Memory operation - always 8-bit
        carryOut = (value & 0x80) != 0;
        result = (value << 1) & 0xFF;
        if (TM32CPU_WriteByte(processor, processor->da, result) == false)
        {
            return false;
        }
    }
    else
    {
        // Register operation - check register size
        switch (processor->iparam1 & 0b11)
        {
        case 0b00: // Full 32-bit register
            carryOut = (value & 0x80000000) != 0;
            result = value << 1;
            break;
        case 0b01: // Partial 16-bit register
            carryOut = (value & 0x8000) != 0;
            result = (value << 1) & 0xFFFF;
            break;
        default: // Partial 8-bit register
            carryOut = (value & 0x80) != 0;
            result = (value << 1) & 0xFF;
            break;
        }
        TM32CPU_WriteRegister(processor, processor->iparam1, result);
    }

    // Set flags: Clear N and H, set C to shifted-out bit, set Z based on result
    TM32CPU_ClearFlags(processor, TM32CPU_FT_N | TM32CPU_FT_H);
    TM32CPU_ChangeFlags(processor, TM32CPU_FT_C, carryOut);
    
    // Set other flags based on register/memory size
    if (processor->da != 0 || (processor->iparam1 & 0b11) >= 0b10)
    {
        // 8-bit operation
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, (result & 0xFF) == 0);
        TM32CPU_CheckParity(processor, result & 0xFF);
    }
    else if ((processor->iparam1 & 0b11) == 0b01)
    {
        // 16-bit operation
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, (result & 0xFFFF) == 0);
        TM32CPU_CheckParity(processor, result & 0xFFFF);
    }
    else
    {
        // 32-bit operation
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, result == 0);
        TM32CPU_CheckParity(processor, result);
    }

    return true;
}

bool TM32CPU_Execute_SRA (
    TM32CPU_Processor* processor
)
{
    // 0x62X0 SRA X
    // 0x63X0 SRA [X32]
    //
    // Shifts right arithmetically (SRA). The value to be shifted is currently 
    // stored in the `SD` register, and the destination is determined by whether 
    // or not the `DA` register is non-zero.
    //
    // SRA sets the Carry flag to the rightmost bit before the shift and 
    // leaves the leftmost bit unchanged (sign extension).

    uint32_t value = processor->sd;
    uint32_t result = 0;
    bool carryOut = false;

    // Determine the bit width and perform the shift
    if (processor->da != 0)
    {
        // Memory operation - always 8-bit
        carryOut = (value & 0x01) != 0;
        // Sign extend from bit 7
        if (value & 0x80)
        {
            result = ((value >> 1) | 0x80) & 0xFF;
        }
        else
        {
            result = (value >> 1) & 0xFF;
        }
        if (TM32CPU_WriteByte(processor, processor->da, result) == false)
        {
            return false;
        }
    }
    else
    {
        // Register operation - check register size
        switch (processor->iparam1 & 0b11)
        {
        case 0b00: // Full 32-bit register
            carryOut = (value & 0x01) != 0;
            // Sign extend from bit 31
            if (value & 0x80000000)
            {
                result = (value >> 1) | 0x80000000;
            }
            else
            {
                result = value >> 1;
            }
            break;
        case 0b01: // Partial 16-bit register
            carryOut = (value & 0x01) != 0;
            // Sign extend from bit 15
            if (value & 0x8000)
            {
                result = ((value >> 1) | 0x8000) & 0xFFFF;
            }
            else
            {
                result = (value >> 1) & 0xFFFF;
            }
            break;
        default: // Partial 8-bit register
            carryOut = (value & 0x01) != 0;
            // Sign extend from bit 7
            if (value & 0x80)
            {
                result = ((value >> 1) | 0x80) & 0xFF;
            }
            else
            {
                result = (value >> 1) & 0xFF;
            }
            break;
        }
        TM32CPU_WriteRegister(processor, processor->iparam1, result);
    }

    // Set flags: Clear N and H, set C to shifted-out bit, set Z based on result
    TM32CPU_ClearFlags(processor, TM32CPU_FT_N | TM32CPU_FT_H);
    TM32CPU_ChangeFlags(processor, TM32CPU_FT_C, carryOut);
    
    // Set other flags based on register/memory size
    if (processor->da != 0 || (processor->iparam1 & 0b11) >= 0b10)
    {
        // 8-bit operation
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, (result & 0xFF) == 0);
        TM32CPU_CheckParity(processor, result & 0xFF);
    }
    else if ((processor->iparam1 & 0b11) == 0b01)
    {
        // 16-bit operation
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, (result & 0xFFFF) == 0);
        TM32CPU_CheckParity(processor, result & 0xFFFF);
    }
    else
    {
        // 32-bit operation
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, result == 0);
        TM32CPU_CheckParity(processor, result);
    }

    return true;
}

bool TM32CPU_Execute_SRL (
    TM32CPU_Processor* processor
)
{
    // 0x64X0 SRL X
    // 0x65X0 SRL [X32]
    //
    // Shifts right logically (SRL). The value to be shifted is currently 
    // stored in the `SD` register, and the destination is determined by whether 
    // or not the `DA` register is non-zero.
    //
    // SRL sets the Carry flag to the rightmost bit before the shift, 
    // then sets the leftmost bit to zero after the shift.

    uint32_t value = processor->sd;
    uint32_t result = 0;
    bool carryOut = false;

    // Determine the bit width and perform the shift
    if (processor->da != 0)
    {
        // Memory operation - always 8-bit
        carryOut = (value & 0x01) != 0;
        result = (value >> 1) & 0x7F;  // Clear the MSB
        if (TM32CPU_WriteByte(processor, processor->da, result) == false)
        {
            return false;
        }
    }
    else
    {
        // Register operation - check register size
        switch (processor->iparam1 & 0b11)
        {
        case 0b00: // Full 32-bit register
            carryOut = (value & 0x01) != 0;
            result = value >> 1;  // Logical shift automatically clears MSB
            break;
        case 0b01: // Partial 16-bit register
            carryOut = (value & 0x01) != 0;
            result = (value >> 1) & 0x7FFF;  // Clear the MSB and mask to 16 bits
            break;
        default: // Partial 8-bit register
            carryOut = (value & 0x01) != 0;
            result = (value >> 1) & 0x7F;  // Clear the MSB and mask to 8 bits
            break;
        }
        TM32CPU_WriteRegister(processor, processor->iparam1, result);
    }

    // Set flags: Clear N and H, set C to shifted-out bit, set Z based on result
    TM32CPU_ClearFlags(processor, TM32CPU_FT_N | TM32CPU_FT_H);
    TM32CPU_ChangeFlags(processor, TM32CPU_FT_C, carryOut);
    
    // Set other flags based on register/memory size
    if (processor->da != 0 || (processor->iparam1 & 0b11) >= 0b10)
    {
        // 8-bit operation
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, (result & 0xFF) == 0);
        TM32CPU_CheckParity(processor, result & 0xFF);
    }
    else if ((processor->iparam1 & 0b11) == 0b01)
    {
        // 16-bit operation
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, (result & 0xFFFF) == 0);
        TM32CPU_CheckParity(processor, result & 0xFFFF);
    }
    else
    {
        // 32-bit operation
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, result == 0);
        TM32CPU_CheckParity(processor, result);
    }

    return true;
}

bool TM32CPU_Execute_RL (
    TM32CPU_Processor* processor
)
{
    // 0x66X0 RL X
    // 0x67X0 RL [X32]
    //
    // Rotates left through the Carry flag (RL). The value to be rotated is 
    // currently stored in the `SD` register, and the destination is determined 
    // by whether or not the `DA` register is non-zero.
    //
    // RL sets the Carry flag to the leftmost bit before the rotate, then sets 
    // the rightmost bit to the old state of the Carry flag after the rotate.

    uint32_t value = processor->sd;
    uint32_t result = 0;
    bool oldCarry = TM32CPU_CheckFlags(processor, TM32CPU_FT_C);
    bool newCarry = false;

    // Determine the bit width and perform the rotation
    if (processor->da != 0)
    {
        // Memory operation - always 8-bit
        newCarry = (value & 0x80) != 0;
        result = ((value << 1) | (oldCarry ? 1 : 0)) & 0xFF;
        if (TM32CPU_WriteByte(processor, processor->da, result) == false)
        {
            return false;
        }
    }
    else
    {
        // Register operation - check register size
        switch (processor->iparam1 & 0b11)
        {
        case 0b00: // Full 32-bit register
            newCarry = (value & 0x80000000) != 0;
            result = (value << 1) | (oldCarry ? 1 : 0);
            break;
        case 0b01: // Partial 16-bit register
            newCarry = (value & 0x8000) != 0;
            result = ((value << 1) | (oldCarry ? 1 : 0)) & 0xFFFF;
            break;
        default: // Partial 8-bit register
            newCarry = (value & 0x80) != 0;
            result = ((value << 1) | (oldCarry ? 1 : 0)) & 0xFF;
            break;
        }
        TM32CPU_WriteRegister(processor, processor->iparam1, result);
    }

    // Set flags: Clear N and H, set C to rotated-out bit, set Z based on result
    TM32CPU_ClearFlags(processor, TM32CPU_FT_N | TM32CPU_FT_H);
    TM32CPU_ChangeFlags(processor, TM32CPU_FT_C, newCarry);
    
    // Set other flags based on register/memory size
    if (processor->da != 0 || (processor->iparam1 & 0b11) >= 0b10)
    {
        // 8-bit operation
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, (result & 0xFF) == 0);
        TM32CPU_CheckParity(processor, result & 0xFF);
    }
    else if ((processor->iparam1 & 0b11) == 0b01)
    {
        // 16-bit operation
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, (result & 0xFFFF) == 0);
        TM32CPU_CheckParity(processor, result & 0xFFFF);
    }
    else
    {
        // 32-bit operation
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, result == 0);
        TM32CPU_CheckParity(processor, result);
    }

    return true;
}

bool TM32CPU_Execute_RLC (
    TM32CPU_Processor* processor
)
{
    // 0x68X0 RLC X
    // 0x69X0 RLC [X32]
    //
    // Rotates left (RLC). The value to be rotated is currently stored in the 
    // `SD` register, and the destination is determined by whether or not the 
    // `DA` register is non-zero.
    //
    // RLC sets the Carry flag to the leftmost bit before the rotate, then sets 
    // the rightmost bit to the old state of the leftmost bit after the rotate.

    uint32_t value = processor->sd;
    uint32_t result = 0;
    bool carryOut = false;

    // Determine the bit width and perform the rotation
    if (processor->da != 0)
    {
        // Memory operation - always 8-bit
        carryOut = (value & 0x80) != 0;
        result = ((value << 1) | (carryOut ? 1 : 0)) & 0xFF;
        if (TM32CPU_WriteByte(processor, processor->da, result) == false)
        {
            return false;
        }
    }
    else
    {
        // Register operation - check register size
        switch (processor->iparam1 & 0b11)
        {
        case 0b00: // Full 32-bit register
            carryOut = (value & 0x80000000) != 0;
            result = (value << 1) | (carryOut ? 1 : 0);
            break;
        case 0b01: // Partial 16-bit register
            carryOut = (value & 0x8000) != 0;
            result = ((value << 1) | (carryOut ? 1 : 0)) & 0xFFFF;
            break;
        default: // Partial 8-bit register
            carryOut = (value & 0x80) != 0;
            result = ((value << 1) | (carryOut ? 1 : 0)) & 0xFF;
            break;
        }
        TM32CPU_WriteRegister(processor, processor->iparam1, result);
    }

    // Set flags: Clear N and H, set C to rotated-out bit, set Z based on result
    TM32CPU_ClearFlags(processor, TM32CPU_FT_N | TM32CPU_FT_H);
    TM32CPU_ChangeFlags(processor, TM32CPU_FT_C, carryOut);
    
    // Set other flags based on register/memory size
    if (processor->da != 0 || (processor->iparam1 & 0b11) >= 0b10)
    {
        // 8-bit operation
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, (result & 0xFF) == 0);
        TM32CPU_CheckParity(processor, result & 0xFF);
    }
    else if ((processor->iparam1 & 0b11) == 0b01)
    {
        // 16-bit operation
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, (result & 0xFFFF) == 0);
        TM32CPU_CheckParity(processor, result & 0xFFFF);
    }
    else
    {
        // 32-bit operation
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, result == 0);
        TM32CPU_CheckParity(processor, result);
    }

    return true;
}

bool TM32CPU_Execute_RR (
    TM32CPU_Processor* processor
)
{
    // 0x6AX0 RR X
    // 0x6BX0 RR [X32]
    //
    // Rotates right through the Carry flag (RR). The value to be rotated is 
    // currently stored in the `SD` register, and the destination is determined 
    // by whether or not the `DA` register is non-zero.
    //
    // RR sets the Carry flag to the rightmost bit before the rotate, then sets 
    // the leftmost bit to the old state of the Carry flag after the rotate.

    uint32_t value = processor->sd;
    uint32_t result = 0;
    bool oldCarry = TM32CPU_CheckFlags(processor, TM32CPU_FT_C);
    bool newCarry = false;

    // Determine the bit width and perform the rotation
    if (processor->da != 0)
    {
        // Memory operation - always 8-bit
        newCarry = (value & 0x01) != 0;
        result = ((value >> 1) | (oldCarry ? 0x80 : 0)) & 0xFF;
        if (TM32CPU_WriteByte(processor, processor->da, result) == false)
        {
            return false;
        }
    }
    else
    {
        // Register operation - check register size
        switch (processor->iparam1 & 0b11)
        {
        case 0b00: // Full 32-bit register
            newCarry = (value & 0x01) != 0;
            result = (value >> 1) | (oldCarry ? 0x80000000 : 0);
            break;
        case 0b01: // Partial 16-bit register
            newCarry = (value & 0x01) != 0;
            result = ((value >> 1) | (oldCarry ? 0x8000 : 0)) & 0xFFFF;
            break;
        default: // Partial 8-bit register
            newCarry = (value & 0x01) != 0;
            result = ((value >> 1) | (oldCarry ? 0x80 : 0)) & 0xFF;
            break;
        }
        TM32CPU_WriteRegister(processor, processor->iparam1, result);
    }

    // Set flags: Clear N and H, set C to rotated-out bit, set Z based on result
    TM32CPU_ClearFlags(processor, TM32CPU_FT_N | TM32CPU_FT_H);
    TM32CPU_ChangeFlags(processor, TM32CPU_FT_C, newCarry);
    
    // Set other flags based on register/memory size
    if (processor->da != 0 || (processor->iparam1 & 0b11) >= 0b10)
    {
        // 8-bit operation
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, (result & 0xFF) == 0);
        TM32CPU_CheckParity(processor, result & 0xFF);
    }
    else if ((processor->iparam1 & 0b11) == 0b01)
    {
        // 16-bit operation
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, (result & 0xFFFF) == 0);
        TM32CPU_CheckParity(processor, result & 0xFFFF);
    }
    else
    {
        // 32-bit operation
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, result == 0);
        TM32CPU_CheckParity(processor, result);
    }

    return true;
}

bool TM32CPU_Execute_RRC (
    TM32CPU_Processor* processor
)
{
    // 0x6CX0 RRC X
    // 0x6DX0 RRC [X32]
    //
    // Rotates right (RRC). The value to be rotated is currently stored in the 
    // `SD` register, and the destination is determined by whether or not the 
    // `DA` register is non-zero.
    //
    // RRC sets the Carry flag to the rightmost bit before the rotate, then sets 
    // the leftmost bit to the old state of the rightmost bit after the rotate.

    uint32_t value = processor->sd;
    uint32_t result = 0;
    bool carryOut = false;

    // Determine the bit width and perform the rotation
    if (processor->da != 0)
    {
        // Memory operation - always 8-bit
        carryOut = (value & 0x01) != 0;
        result = ((value >> 1) | (carryOut ? 0x80 : 0)) & 0xFF;
        if (TM32CPU_WriteByte(processor, processor->da, result) == false)
        {
            return false;
        }
    }
    else
    {
        // Register operation - check register size
        switch (processor->iparam1 & 0b11)
        {
        case 0b00: // Full 32-bit register
            carryOut = (value & 0x01) != 0;
            result = (value >> 1) | (carryOut ? 0x80000000 : 0);
            break;
        case 0b01: // Partial 16-bit register
            carryOut = (value & 0x01) != 0;
            result = ((value >> 1) | (carryOut ? 0x8000 : 0)) & 0xFFFF;
            break;
        default: // Partial 8-bit register
            carryOut = (value & 0x01) != 0;
            result = ((value >> 1) | (carryOut ? 0x80 : 0)) & 0xFF;
            break;
        }
        TM32CPU_WriteRegister(processor, processor->iparam1, result);
    }

    // Set flags: Clear N and H, set C to rotated-out bit, set Z based on result
    TM32CPU_ClearFlags(processor, TM32CPU_FT_N | TM32CPU_FT_H);
    TM32CPU_ChangeFlags(processor, TM32CPU_FT_C, carryOut);
    
    // Set other flags based on register/memory size
    if (processor->da != 0 || (processor->iparam1 & 0b11) >= 0b10)
    {
        // 8-bit operation
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, (result & 0xFF) == 0);
        TM32CPU_CheckParity(processor, result & 0xFF);
    }
    else if ((processor->iparam1 & 0b11) == 0b01)
    {
        // 16-bit operation
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, (result & 0xFFFF) == 0);
        TM32CPU_CheckParity(processor, result & 0xFFFF);
    }
    else
    {
        // 32-bit operation
        TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, result == 0);
        TM32CPU_CheckParity(processor, result);
    }

    return true;
}

bool TM32CPU_Execute_BIT (
    TM32CPU_Processor* processor
)
{
    // 0x70X0 BIT XD, X
    // 0x71X0 BIT XD, [X32]
    //
    // Tests bit XD of a value. The bit number is stored in the `XD` register,
    // and the value to test is currently stored in the `SD` register.
    // The destination is determined by whether or not the `DA` register is 
    // non-zero:
    // - If `DA` is zero, then testing a register bit (0x70X0).
    // - Otherwise, testing a memory bit (0x71X0).
    //
    // BIT sets the Zero flag if bit XD is clear, and affects other flags.

    uint32_t value = processor->sd;
    uint8_t bitNumber = processor->xd;
    uint32_t maxBits = 32;
    bool bitSet = false;

    // Determine the bit width and validate bit number
    if (processor->da != 0)
    {
        // Memory operation - always 8-bit
        maxBits = 8;
    }
    else
    {
        // Register operation - check register size
        switch (processor->iparam1 & 0b11)
        {
        case 0b00: // Full 32-bit register
            maxBits = 32;
            break;
        case 0b01: // Partial 16-bit register
            maxBits = 16;
            break;
        default: // Partial 8-bit register
            maxBits = 8;
            break;
        }
    }

    // Validate bit number
    if (bitNumber >= maxBits)
    {
        return TM32CPU_RaiseException(processor, TM32CPU_EC_INVALID_ARGUMENT);
    }

    // Test the bit
    bitSet = (value & (1 << bitNumber)) != 0;

    // Set the flags according to the BIT instruction.
    // BIT clears the Carry flag, sets the Negative flag, sets the Zero flag
    // if the tested bit is clear, and leaves other flags unaffected.
    TM32CPU_ClearFlags(processor, TM32CPU_FT_C);
    TM32CPU_SetFlags(processor, TM32CPU_FT_N);
    TM32CPU_ChangeFlags(processor, TM32CPU_FT_Z, !bitSet);

    return true;
}

bool TM32CPU_Execute_TOG (
    TM32CPU_Processor* processor
)
{
    // 0x72X0 TOG XD, X
    // 0x73X0 TOG XD, [X32]
    //
    // Toggles (complements) bit XD of a value. The bit number is stored in the 
    // `XD` register, and the value to toggle is currently stored in the `SD` 
    // register. The destination is determined by whether or not the `DA` 
    // register is non-zero:
    // - If `DA` is zero, then toggling a register bit (0x72X0).
    // - Otherwise, toggling a memory bit (0x73X0).

    uint32_t value = processor->sd;
    uint8_t bitNumber = processor->xd;
    uint32_t maxBits = 32;
    uint32_t result = 0;

    // Determine the bit width and validate bit number
    if (processor->da != 0)
    {
        // Memory operation - always 8-bit
        maxBits = 8;
        if (bitNumber >= maxBits)
        {
            return TM32CPU_RaiseException(processor, TM32CPU_EC_INVALID_ARGUMENT);
        }
        
        // Toggle the bit
        result = value ^ (1 << bitNumber);
        
        // Write back to memory
        return TM32CPU_WriteByte(processor, processor->da, result & 0xFF);
    }
    else
    {
        // Register operation - check register size
        switch (processor->iparam1 & 0b11)
        {
        case 0b00: // Full 32-bit register
            maxBits = 32;
            break;
        case 0b01: // Partial 16-bit register
            maxBits = 16;
            break;
        default: // Partial 8-bit register
            maxBits = 8;
            break;
        }

        // Validate bit number
        if (bitNumber >= maxBits)
        {
            return TM32CPU_RaiseException(processor, TM32CPU_EC_INVALID_ARGUMENT);
        }

        // Toggle the bit
        result = value ^ (1 << bitNumber);

        // Write back to register
        TM32CPU_WriteRegister(processor, processor->iparam1, result);
    }

    return true;
}

bool TM32CPU_Execute_SET (
    TM32CPU_Processor* processor
)
{
    // 0x74X0 SET XD, X
    // 0x75X0 SET XD, [X32]
    //
    // Sets bit XD of a value to 1. The bit number is stored in the `XD` 
    // register, and the value to modify is currently stored in the `SD` 
    // register. The destination is determined by whether or not the `DA` 
    // register is non-zero:
    // - If `DA` is zero, then setting a register bit (0x74X0).
    // - Otherwise, setting a memory bit (0x75X0).

    uint32_t value = processor->sd;
    uint8_t bitNumber = processor->xd;
    uint32_t maxBits = 32;
    uint32_t result = 0;

    // Determine the bit width and validate bit number
    if (processor->da != 0)
    {
        // Memory operation - always 8-bit
        maxBits = 8;
        if (bitNumber >= maxBits)
        {
            return TM32CPU_RaiseException(processor, TM32CPU_EC_INVALID_ARGUMENT);
        }
        
        // Set the bit
        result = value | (1 << bitNumber);
        
        // Write back to memory
        return TM32CPU_WriteByte(processor, processor->da, result & 0xFF);
    }
    else
    {
        // Register operation - check register size
        switch (processor->iparam1 & 0b11)
        {
        case 0b00: // Full 32-bit register
            maxBits = 32;
            break;
        case 0b01: // Partial 16-bit register
            maxBits = 16;
            break;
        default: // Partial 8-bit register
            maxBits = 8;
            break;
        }

        // Validate bit number
        if (bitNumber >= maxBits)
        {
            return TM32CPU_RaiseException(processor, TM32CPU_EC_INVALID_ARGUMENT);
        }

        // Set the bit
        result = value | (1 << bitNumber);

        // Write back to register
        TM32CPU_WriteRegister(processor, processor->iparam1, result);
    }

    return true;
}

bool TM32CPU_Execute_RES (
    TM32CPU_Processor* processor
)
{
    // 0x76X0 RES XD, X
    // 0x77X0 RES XD, [X32]
    //
    // Resets (clears) bit XD of a value to 0. The bit number is stored in the 
    // `XD` register, and the value to modify is currently stored in the `SD` 
    // register. The destination is determined by whether or not the `DA` 
    // register is non-zero:
    // - If `DA` is zero, then clearing a register bit (0x76X0).
    // - Otherwise, clearing a memory bit (0x77X0).

    uint32_t value = processor->sd;
    uint8_t bitNumber = processor->xd;
    uint32_t maxBits = 32;
    uint32_t result = 0;

    // Determine the bit width and validate bit number
    if (processor->da != 0)
    {
        // Memory operation - always 8-bit
        maxBits = 8;
        if (bitNumber >= maxBits)
        {
            return TM32CPU_RaiseException(processor, TM32CPU_EC_INVALID_ARGUMENT);
        }
        
        // Clear the bit
        result = value & ~(1 << bitNumber);
        
        // Write back to memory
        return TM32CPU_WriteByte(processor, processor->da, result & 0xFF);
    }
    else
    {
        // Register operation - check register size
        switch (processor->iparam1 & 0b11)
        {
        case 0b00: // Full 32-bit register
            maxBits = 32;
            break;
        case 0b01: // Partial 16-bit register
            maxBits = 16;
            break;
        default: // Partial 8-bit register
            maxBits = 8;
            break;
        }

        // Validate bit number
        if (bitNumber >= maxBits)
        {
            return TM32CPU_RaiseException(processor, TM32CPU_EC_INVALID_ARGUMENT);
        }

        // Clear the bit
        result = value & ~(1 << bitNumber);

        // Write back to register
        TM32CPU_WriteRegister(processor, processor->iparam1, result);
    }

    return true;
}

/* Public Functions ***********************************************************/

TM32CPU_Processor* TM32CPU_CreateProcessor (
    TM32CPU_BusReadCallback   busReadCallback,
    TM32CPU_BusWriteCallback  busWriteCallback,
    TM32CPU_ClockCallback     clockCallback,
    void*                     userData
)
{
    TM32CPU_ReturnValueIfNull(busReadCallback, NULL);
    TM32CPU_ReturnValueIfNull(busWriteCallback, NULL);
    TM32CPU_ReturnValueIfNull(clockCallback, NULL);

    TM32CPU_Processor* processor = TM32CPU_Create(1, TM32CPU_Processor);
    if (processor == NULL)
    {
        TM32CPU_LogErrno("Failed to allocate memory for processor");
        return NULL;
    }

    processor->busReadCallback   = busReadCallback;
    processor->busWriteCallback  = busWriteCallback;
    processor->clockCallback     = clockCallback;

    TM32CPU_InitializeProcessor(processor);
    return processor;
}

bool TM32CPU_InitializeProcessor (
    TM32CPU_Processor* processor
)
{
    TM32CPU_ReturnValueIfNull(processor, false);
    
    // Initialize General Purpose Registers
    processor->a = 0;
    processor->b = 0;
    processor->c = 0;
    processor->d = 0;

    // Initialize Special Purpose Registers
    processor->pc = TM32CPU_INITIAL_PC;
    processor->ip = 0;
    processor->sp = TM32CPU_INITIAL_SP;
    processor->da = 0;
    processor->sa = 0;
    processor->sd = 0;
    processor->flags = 0;
    processor->intflags = 0;
    processor->intenable = 0;
    processor->ci = 0;
    processor->ec = TM32CPU_EC_OK;
    processor->xd = 0;
    processor->ime = false;

    // Initialize Internal State
    processor->cycleCount = 0;
    processor->itype = 0;
    processor->iparam1 = 0;
    processor->iparam2 = 0;
    processor->ei = false;

    return true;
}

void TM32CPU_DestroyProcessor (
    TM32CPU_Processor* processor
)
{
    if (processor != NULL)
    {
        TM32CPU_Destroy(processor);
    }
}

bool TM32CPU_StepProcessor (
    TM32CPU_Processor* processor
)
{
    TM32CPU_ReturnValueIfNull(processor, false);

    // Do not step if the processor is in a STOP state.
    if (TM32CPU_CheckFlags(processor, TM32CPU_FT_S) == true)
    {
        // If the `EC` register indicates an exception, then return false.
        if (processor->ec != TM32CPU_EC_OK)
        {
            return false;
        }

        // Otherwise, consume one cycle and return the result.
        return TM32CPU_ConsumeCycles(processor, 1);
    }

    // The step will work differently based on the processor's halt flag.
    if (TM32CPU_CheckFlags(processor, TM32CPU_FT_L) == true)
    {
        // If the processor is halted, check if an interrupt is pending. If so,
        // then clear the halt state.
        if (processor->intflags != 0)
        {
            TM32CPU_ClearFlags(processor, TM32CPU_FT_L);
        }
    }
    else
    {
        // Clear the outputs from the previous instruction.
        processor->sa = 0;
        processor->sd = 0;
        processor->da = 0;
        processor->xd = 0;
        processor->ci = 0;
        processor->itype = 0;
        processor->iparam1 = 0;
        processor->iparam2 = 0;

        // Set `IP` to the current `PC`, ensure that address is in executable
        // memory bounds, then read the instruction word from memory.
        processor->ip = processor->pc;
        if (
            TM32CPU_CheckExecutable(processor, processor->ip) == false ||
            TM32CPU_ReadWord(processor, processor->ip, &processor->ci) == false
        )
        {
            return false;
        }

        // Increment the program counter by 2. Incrementing the program counter
        // as part of the instruction fetch does not consume any cycles.
        processor->pc += 2;

        // Decode the instruction type and parameters.
        processor->itype    = (processor->ci >> 8) & 0xFF;
        processor->iparam1  = (processor->ci >> 4) & 0xF;
        processor->iparam2  = (processor->ci >> 0) & 0xF;

        // Perform necessary fetching operations based on the instruction's
        // type and parameters, then execute the instruction.
        bool ok = false;
        switch (processor->itype)
        {
            // General Instructions (0x00 - 0x0C)
            case 0x00: // NOP
                ok = TM32CPU_Execute_NOP(processor);
                break;
            case 0x01: // STOP
                ok = TM32CPU_Execute_STOP(processor);
                break;
            case 0x02: // HALT
                ok = TM32CPU_Execute_HALT(processor);
                break;
            case 0x03: // SEC XX
                ok = TM32CPU_Execute_SEC(processor);
                break;
            case 0x04: // CEC
                ok = TM32CPU_Execute_CEC(processor);
                break;
            case 0x05: // DI
                ok = TM32CPU_Execute_DI(processor);
                break;
            case 0x06: // EI
                ok = TM32CPU_Execute_EI(processor);
                break;
            case 0x07: // DAA
                ok = TM32CPU_Execute_DAA(processor);
                break;
            case 0x08: // SCF
                ok = TM32CPU_Execute_SCF(processor);
                break;
            case 0x09: // CCF
                ok = TM32CPU_Execute_CCF(processor);
                break;
            case 0x0A: // FLG X
                ok = TM32CPU_Execute_FLG(processor);
                break;
            case 0x0B: // STF X
                ok = TM32CPU_Execute_STF(processor);
                break;
            case 0x0C: // CLF X
                ok = TM32CPU_Execute_CLF(processor);
                break;

            // Data Transfer Instructions (0x10 - 0x1F)
            case 0x10: // LD X, IMM
                ok = TM32CPU_Fetch_REG_IMM(processor) && TM32CPU_Execute_LD(processor);
                break;
            case 0x11: // LD X, [ADDR32]
                ok = TM32CPU_Fetch_REG_ADDR32(processor) && TM32CPU_Execute_LD(processor);
                break;
            case 0x12: // LD X, [Y32]
                ok = TM32CPU_Fetch_REG_REGPTR32(processor) && TM32CPU_Execute_LD(processor);
                break;
            case 0x13: // LDH X, [ADDR16]
                ok = TM32CPU_Fetch_REG_ADDR16(processor) && TM32CPU_Execute_LD(processor);
                break;
            case 0x14: // LDH X, [Y16]
                ok = TM32CPU_Fetch_REG_REGPTR16(processor) && TM32CPU_Execute_LD(processor);
                break;
            case 0x15: // LDP X8, [ADDR8]
                ok = TM32CPU_Fetch_REG_ADDR8(processor) && TM32CPU_Execute_LD(processor);
                break;
            case 0x16: // LDP X8, [Y8]
                ok = TM32CPU_Fetch_REG_REGPTR8(processor) && TM32CPU_Execute_LD(processor);
                break;
            case 0x17: // ST [ADDR32], Y
                ok = TM32CPU_Fetch_ADDR32_REG(processor) && TM32CPU_Execute_ST(processor);
                break;
            case 0x18: // ST [X32], Y
                ok = TM32CPU_Fetch_REGPTR32_REG(processor) && TM32CPU_Execute_ST(processor);
                break;
            case 0x19: // STH [ADDR16], Y
                ok = TM32CPU_Fetch_ADDR16_REG(processor) && TM32CPU_Execute_ST(processor);
                break;
            case 0x1A: // STH [X16], Y
                ok = TM32CPU_Fetch_REGPTR16_REG(processor) && TM32CPU_Execute_ST(processor);
                break;
            case 0x1B: // STP [ADDR8], Y8
                ok = TM32CPU_Fetch_ADDR8_REG(processor) && TM32CPU_Execute_ST(processor);
                break;
            case 0x1C: // STP [X8], Y8
                ok = TM32CPU_Fetch_REGPTR8_REG(processor) && TM32CPU_Execute_ST(processor);
                break;
            case 0x1D: // MV X, Y
                ok = TM32CPU_Fetch_REG_REG(processor) && TM32CPU_Execute_MV(processor);
                break;
            case 0x1E: // POP X32
                ok = TM32CPU_Execute_POP(processor);
                break;
            case 0x1F: // PUSH Y32
                ok = TM32CPU_Execute_PUSH(processor);
                break;

            // Control Transfer Instructions (0x20 - 0x26)
            case 0x20: // JMP Z, IMM32
                ok = TM32CPU_Fetch_NULL_IMM32(processor) && TM32CPU_Execute_JMP(processor);
                break;
            case 0x21: // JMP Z, X32
                ok = TM32CPU_Fetch_REG_NULL(processor) && TM32CPU_Execute_JMP(processor);
                break;
            case 0x22: // JPB Z, SIMM16
                ok = TM32CPU_Fetch_NULL_IMM16(processor) && TM32CPU_Execute_JPB(processor);
                break;
            case 0x23: // CALL Z, IMM32
                ok = TM32CPU_Fetch_NULL_IMM32(processor) && TM32CPU_Execute_CALL(processor);
                break;
            case 0x24: // INT XX
                ok = TM32CPU_Execute_INT(processor);
                break;
            case 0x25: // RET Z
                ok = TM32CPU_Execute_RET(processor);
                break;
            case 0x26: // RETI
                ok = TM32CPU_Execute_RETI(processor);
                break;

            // Arithmetic Instructions (0x30 - 0x3F)
            case 0x30: // INC X
                ok = TM32CPU_Fetch_REG_NULL(processor) && TM32CPU_Execute_INC(processor);
                break;
            case 0x31: // INC [X32]
                ok = TM32CPU_Fetch_REGPTR32_NULL(processor) && TM32CPU_Execute_INC(processor);
                break;
            case 0x32: // DEC X
                ok = TM32CPU_Fetch_REG_NULL(processor) && TM32CPU_Execute_DEC(processor);
                break;
            case 0x33: // DEC [X32]
                ok = TM32CPU_Fetch_REGPTR32_NULL(processor) && TM32CPU_Execute_DEC(processor);
                break;
            case 0x34: // ADD X, IMM
                ok = TM32CPU_Fetch_REG_IMM(processor) && TM32CPU_Execute_ADD(processor);
                break;
            case 0x35: // ADD X, [ADDR32]
                ok = TM32CPU_Fetch_REG_ADDR32(processor) && TM32CPU_Execute_ADD(processor);
                break;
            case 0x36: // ADD X, [Y32]
                ok = TM32CPU_Fetch_REG_REGPTR32(processor) && TM32CPU_Execute_ADD(processor);
                break;
            case 0x37: // ADC X, IMM
                ok = TM32CPU_Fetch_REG_IMM(processor) && TM32CPU_Execute_ADC(processor);
                break;
            case 0x38: // ADC X, [ADDR32]
                ok = TM32CPU_Fetch_REG_ADDR32(processor) && TM32CPU_Execute_ADC(processor);
                break;
            case 0x39: // ADC X, [Y32]
                ok = TM32CPU_Fetch_REG_REGPTR32(processor) && TM32CPU_Execute_ADC(processor);
                break;
            case 0x3A: // SUB X, IMM
                ok = TM32CPU_Fetch_REG_IMM(processor) && TM32CPU_Execute_SUB(processor);
                break;
            case 0x3B: // SUB X, [ADDR32]
                ok = TM32CPU_Fetch_REG_ADDR32(processor) && TM32CPU_Execute_SUB(processor);
                break;
            case 0x3C: // SUB X, [Y32]
                ok = TM32CPU_Fetch_REG_REGPTR32(processor) && TM32CPU_Execute_SUB(processor);
                break;
            case 0x3D: // SBC X, IMM
                ok = TM32CPU_Fetch_REG_IMM(processor) && TM32CPU_Execute_SBC(processor);
                break;
            case 0x3E: // SBC X, [ADDR32]
                ok = TM32CPU_Fetch_REG_ADDR32(processor) && TM32CPU_Execute_SBC(processor);
                break;
            case 0x3F: // SBC X, [Y32]
                ok = TM32CPU_Fetch_REG_REGPTR32(processor) && TM32CPU_Execute_SBC(processor);
                break;

            // Bitwise Instructions (0x40 - 0x4D)
            case 0x40: // AND X, IMM
                ok = TM32CPU_Fetch_REG_IMM(processor) && TM32CPU_Execute_AND(processor);
                break;
            case 0x41: // AND X, [ADDR32]
                ok = TM32CPU_Fetch_REG_ADDR32(processor) && TM32CPU_Execute_AND(processor);
                break;
            case 0x42: // AND X, [Y32]
                ok = TM32CPU_Fetch_REG_REGPTR32(processor) && TM32CPU_Execute_AND(processor);
                break;
            case 0x43: // OR X, IMM
                ok = TM32CPU_Fetch_REG_IMM(processor) && TM32CPU_Execute_OR(processor);
                break;
            case 0x44: // OR X, [ADDR32]
                ok = TM32CPU_Fetch_REG_ADDR32(processor) && TM32CPU_Execute_OR(processor);
                break;
            case 0x45: // OR X, [Y32]
                ok = TM32CPU_Fetch_REG_REGPTR32(processor) && TM32CPU_Execute_OR(processor);
                break;
            case 0x46: // XOR X, IMM
                ok = TM32CPU_Fetch_REG_IMM(processor) && TM32CPU_Execute_XOR(processor);
                break;
            case 0x47: // XOR X, [ADDR32]
                ok = TM32CPU_Fetch_REG_ADDR32(processor) && TM32CPU_Execute_XOR(processor);
                break;
            case 0x48: // XOR X, [Y32]
                ok = TM32CPU_Fetch_REG_REGPTR32(processor) && TM32CPU_Execute_XOR(processor);
                break;
            case 0x4C: // NOT X
                ok = TM32CPU_Fetch_REG_NULL(processor) && TM32CPU_Execute_NOT(processor);
                break;
            case 0x4D: // NOT [X32]
                ok = TM32CPU_Fetch_REGPTR32_NULL(processor) && TM32CPU_Execute_NOT(processor);
                break;

            // Comparison Instructions (0x50 - 0x52)
            case 0x50: // CMP X, IMM
                ok = TM32CPU_Fetch_REG_IMM(processor) && TM32CPU_Execute_CMP(processor);
                break;
            case 0x51: // CMP X, [ADDR32]
                ok = TM32CPU_Fetch_REG_ADDR32(processor) && TM32CPU_Execute_CMP(processor);
                break;
            case 0x52: // CMP X, [Y32]
                ok = TM32CPU_Fetch_REG_REGPTR32(processor) && TM32CPU_Execute_CMP(processor);
                break;

            // Shift and Rotate Instructions (0x60 - 0x6D)
            case 0x60: // SLA X
                ok = TM32CPU_Fetch_REG_NULL(processor) && TM32CPU_Execute_SLA(processor);
                break;
            case 0x61: // SLA [X32]
                ok = TM32CPU_Fetch_REGPTR32_NULL(processor) && TM32CPU_Execute_SLA(processor);
                break;
            case 0x62: // SRA X
                ok = TM32CPU_Fetch_REG_NULL(processor) && TM32CPU_Execute_SRA(processor);
                break;
            case 0x63: // SRA [X32]
                ok = TM32CPU_Fetch_REGPTR32_NULL(processor) && TM32CPU_Execute_SRA(processor);
                break;
            case 0x64: // SRL X
                ok = TM32CPU_Fetch_REG_NULL(processor) && TM32CPU_Execute_SRL(processor);
                break;
            case 0x65: // SRL [X32]
                ok = TM32CPU_Fetch_REGPTR32_NULL(processor) && TM32CPU_Execute_SRL(processor);
                break;
            case 0x66: // RL X
                ok = TM32CPU_Fetch_REG_NULL(processor) && TM32CPU_Execute_RL(processor);
                break;
            case 0x67: // RL [X32]
                ok = TM32CPU_Fetch_REGPTR32_NULL(processor) && TM32CPU_Execute_RL(processor);
                break;
            case 0x68: // RLC X
                ok = TM32CPU_Fetch_REG_NULL(processor) && TM32CPU_Execute_RLC(processor);
                break;
            case 0x69: // RLC [X32]
                ok = TM32CPU_Fetch_REGPTR32_NULL(processor) && TM32CPU_Execute_RLC(processor);
                break;
            case 0x6A: // RR X
                ok = TM32CPU_Fetch_REG_NULL(processor) && TM32CPU_Execute_RR(processor);
                break;
            case 0x6B: // RR [X32]
                ok = TM32CPU_Fetch_REGPTR32_NULL(processor) && TM32CPU_Execute_RR(processor);
                break;
            case 0x6C: // RRC X
                ok = TM32CPU_Fetch_REG_NULL(processor) && TM32CPU_Execute_RRC(processor);
                break;
            case 0x6D: // RRC [X32]
                ok = TM32CPU_Fetch_REGPTR32_NULL(processor) && TM32CPU_Execute_RRC(processor);
                break;

            // Bit Testing and Manipulation Instructions (0x70 - 0x77)
            case 0x70: // BIT XD, X
                ok = TM32CPU_Fetch_XD_REG_NULL(processor) && TM32CPU_Execute_BIT(processor);
                break;
            case 0x71: // BIT XD, [X32]
                ok = TM32CPU_Fetch_XD_REGPTR32_NULL(processor) && TM32CPU_Execute_BIT(processor);
                break;
            case 0x72: // TOG XD, X
                ok = TM32CPU_Fetch_XD_REG_NULL(processor) && TM32CPU_Execute_TOG(processor);
                break;
            case 0x73: // TOG XD, [X32]
                ok = TM32CPU_Fetch_XD_REGPTR32_NULL(processor) && TM32CPU_Execute_TOG(processor);
                break;
            case 0x74: // SET XD, X
                ok = TM32CPU_Fetch_XD_REG_NULL(processor) && TM32CPU_Execute_SET(processor);
                break;
            case 0x75: // SET XD, [X32]
                ok = TM32CPU_Fetch_XD_REGPTR32_NULL(processor) && TM32CPU_Execute_SET(processor);
                break;
            case 0x76: // RES XD, X
                ok = TM32CPU_Fetch_XD_REG_NULL(processor) && TM32CPU_Execute_RES(processor);
                break;
            case 0x77: // RES XD, [X32]
                ok = TM32CPU_Fetch_XD_REGPTR32_NULL(processor) && TM32CPU_Execute_RES(processor);
                break;

            default:
                return TM32CPU_RaiseException(processor, TM32CPU_EC_INVALID_OPCODE);
        }

        if (ok == false)
        {
            return false;
        }
    }

    // At this point, if the IME is enabled, service a pending interrupt.
    if (processor->ime == true)
    {
        if (TM32CPU_ServiceInterrupt(processor) == false)
        {
            return false;
        }
    }

    // At this point, if the `EI` flag is set, enable the IME, then clear the
    // `EI` flag.
    if (processor->ei == true)
    {
        processor->ime = true;
        processor->ei = false;
    }

    return true;
}

bool TM32CPU_WakeProcessor (
    TM32CPU_Processor* processor
)
{
    TM32CPU_ReturnValueIfNull(processor, false);

    // Make sure the processor is in a STOP state.
    if (TM32CPU_CheckFlags(processor, TM32CPU_FT_S) == true)
    {
        // The processor can only be woken up from a STOP state if the `EC`
        // register indicates no exception.
        if (processor->ec != TM32CPU_EC_OK)
        {
            return false;
        }

        // Clear the STOP flag and consume a few cycles to simulate the wake-up
        // time.
        TM32CPU_ClearFlags(processor, TM32CPU_FT_S);
        return TM32CPU_ConsumeCycles(processor, 4);
    }

    return true;
}

bool TM32CPU_ConsumeCycles (
    TM32CPU_Processor*  processor,
    uint32_t            cycles
)
{
    TM32CPU_ReturnValueIfNull(processor, false);
    TM32CPU_ReturnValueIf(cycles == 0, true);

    // Increment the cycle count
    processor->cycleCount += cycles;

    // Invoke the clock callback
    if (processor->clockCallback(cycles, processor->userData) == false)
    {
        return TM32CPU_RaiseException(processor, TM32CPU_EC_HARDWARE_ERROR);
    }

    return true;
}

uint32_t TM32CPU_ReadRegister (
    const TM32CPU_Processor*    processor,
    TM32CPU_RegisterType        type
)
{
    TM32CPU_ReturnValueIf(processor == NULL, 0);
    switch (type)
    {
        case TM32CPU_RT_A:
            return processor->a;
        case TM32CPU_RT_AW:
            return processor->a & 0x0000FFFF;
        case TM32CPU_RT_AH:
            return (processor->a >> 8) & 0x000000FF;
        case TM32CPU_RT_AL:
            return processor->a & 0x000000FF;
        case TM32CPU_RT_B:
            return processor->b;
        case TM32CPU_RT_BW:
            return processor->b & 0x0000FFFF;
        case TM32CPU_RT_BH:
            return (processor->b >> 8) & 0x000000FF;
        case TM32CPU_RT_BL:
            return processor->b & 0x000000FF;
        case TM32CPU_RT_C:
            return processor->c;
        case TM32CPU_RT_CW:
            return processor->c & 0x0000FFFF;
        case TM32CPU_RT_CH:
            return (processor->c >> 8) & 0x000000FF;
        case TM32CPU_RT_CL:
            return processor->c & 0x000000FF;
        case TM32CPU_RT_D:
            return processor->d;
        case TM32CPU_RT_DW:
            return processor->d & 0x0000FFFF;
        case TM32CPU_RT_DH:
            return (processor->d >> 8) & 0x000000FF;
        case TM32CPU_RT_DL:
            return processor->d & 0x000000FF;
        default:
            TM32CPU_LogError("Invalid register enumeration: %d.", type);
            return 0;
    }
}

bool TM32CPU_WriteRegister (
    TM32CPU_Processor*      processor,
    TM32CPU_RegisterType    type,
    uint32_t                value
)
{
    TM32CPU_ReturnValueIf(processor == NULL, false);
    switch (type)
    {
        case TM32CPU_RT_A:
            processor->a = value;
            break;
        case TM32CPU_RT_AW:
            processor->a = 
                (processor->a & 0xFFFF0000) | (value & 0x0000FFFF);
            break;
        case TM32CPU_RT_AH:
            processor->a =
                (processor->a & 0xFFFF00FF) | ((value & 0xFF) << 8);
            break;
        case TM32CPU_RT_AL:
            processor->a =
                (processor->a & 0xFFFFFF00) | (value & 0x000000FF);
            break;
        case TM32CPU_RT_B:
            processor->b = value;
            break;
        case TM32CPU_RT_BW:
            processor->b =
                (processor->b & 0xFFFF0000) | (value & 0x0000FFFF);
            break;
        case TM32CPU_RT_BH:
            processor->b =
                (processor->b & 0xFFFF00FF) | ((value & 0xFF) << 8);
            break;
        case TM32CPU_RT_BL:
            processor->b =
                (processor->b & 0xFFFFFF00) | (value & 0x000000FF);
            break;
        case TM32CPU_RT_C:
            processor->c = value;
            break;
        case TM32CPU_RT_CW:
            processor->c =
                (processor->c & 0xFFFF0000) | (value & 0x0000FFFF);
            break;
        case TM32CPU_RT_CH:
            processor->c =
                (processor->c & 0xFFFF00FF) | ((value & 0xFF) << 8);
            break;
        case TM32CPU_RT_CL:
            processor->c =
                (processor->c & 0xFFFFFF00) | (value & 0x000000FF);
            break;
        case TM32CPU_RT_D:
            processor->d = value;
            break;
        case TM32CPU_RT_DW:
            processor->d =
                (processor->d & 0xFFFF0000) | (value & 0x0000FFFF);
            break;
        case TM32CPU_RT_DH:
            processor->d =
                (processor->d & 0xFFFF00FF) | ((value & 0xFF) << 8);
            break;
        case TM32CPU_RT_DL:
            processor->d =
                (processor->d & 0xFFFFFF00) | (value & 0x000000FF);
            break;
        default:
            TM32CPU_LogError("Invalid register enumeration: %d.", type);
            return false;
    }

    return true;
}

bool TM32CPU_CheckFlags (
    const TM32CPU_Processor*    processor,
    uint32_t                    flagsMask
)
{
    TM32CPU_ReturnValueIf(processor == NULL, false);
    return ((processor->flags & flagsMask) == flagsMask);
}

void TM32CPU_SetFlags (
    TM32CPU_Processor*          processor,
    uint32_t                    flagsMask
)
{
    TM32CPU_ReturnIf(processor == NULL);
    processor->flags |= flagsMask;
}

void TM32CPU_ClearFlags (
    TM32CPU_Processor*          processor,
    uint32_t                    flagsMask
)
{
    TM32CPU_ReturnIf(processor == NULL);
    processor->flags &= ~flagsMask;
}

void TM32CPU_ToggleFlags (
    TM32CPU_Processor*          processor,
    uint32_t                    flagsMask
)
{
    TM32CPU_ReturnIf(processor == NULL);
    processor->flags ^= flagsMask;
}

void TM32CPU_ChangeFlags (
    TM32CPU_Processor*          processor,
    uint32_t                    flagsMask,
    bool                        set
)
{
    TM32CPU_ReturnIf(processor == NULL);
    if (set == true)
    {
        TM32CPU_SetFlags(processor, flagsMask);
    }
    else
    {
        TM32CPU_ClearFlags(processor, flagsMask);
    }
}

uint32_t TM32CPU_GetProgramCounter (
    const TM32CPU_Processor* processor
)
{
    TM32CPU_ReturnValueIf(processor == NULL, 0);
    return processor->pc;
}

bool TM32CPU_SetProgramCounter (
    TM32CPU_Processor*  processor,
    uint32_t            address
)
{
    TM32CPU_ReturnValueIf(processor == NULL, false);
    
    // Ensure that the program counter is within the valid executable memory 
    // range.
    if (
        address < TM32CPU_INTERRUPT_START &&
        address + 1 > TM32CPU_PROGRAM_END
    )
    {
        return false;
    }
    
    processor->pc = address;
    return true;
}

uint32_t TM32CPU_GetStackPointer (
    const TM32CPU_Processor* processor
)
{
    TM32CPU_ReturnValueIf(processor == NULL, 0);
    return processor->sp;
}

bool TM32CPU_SetStackPointer (
    TM32CPU_Processor*  processor,
    uint32_t            address
)
{
    TM32CPU_ReturnValueIf(processor == NULL, false);
    
    // Ensure that the stack pointer is within the valid stack range.
    if (
        address < TM32CPU_STACK_LOWER ||
        address > TM32CPU_STACK_UPPER
    )
    {
        return false;
    }
    
    processor->sp = address;
    return true;
}

uint16_t TM32CPU_GetCurrentInstruction (
    const TM32CPU_Processor* processor
)
{
    TM32CPU_ReturnValueIf(processor == NULL, 0);
    return processor->ci;
}

uint32_t TM32CPU_GetInterruptFlags (
    const TM32CPU_Processor* processor
)
{
    TM32CPU_ReturnValueIf(processor == NULL, 0);
    return processor->intflags;
}

bool TM32CPU_GetInterruptMasterEnable (
    const TM32CPU_Processor* processor
)
{
    TM32CPU_ReturnValueIf(processor == NULL, false);
    return processor->ime;
}

uint32_t TM32CPU_GetInterruptEnable (
    const TM32CPU_Processor* processor
)
{
    TM32CPU_ReturnValueIf(processor == NULL, 0);
    return processor->intenable;
}

uint8_t TM32CPU_GetErrorCode (
    const TM32CPU_Processor* processor
)
{
    TM32CPU_ReturnValueIf(processor == NULL, TM32CPU_EC_OK);
    return processor->ec;
}

void TM32CPU_SetErrorCode (
    TM32CPU_Processor*  processor,
    uint8_t             errorCode
)
{
    TM32CPU_ReturnIf(processor == NULL);
    processor->ec = errorCode;
}

bool TM32CPU_IsHalted (
    const TM32CPU_Processor* processor
)
{
    TM32CPU_ReturnValueIf(processor == NULL, false);
    return TM32CPU_CheckFlags(processor, TM32CPU_FT_L);
}

bool TM32CPU_IsStopped (
    const TM32CPU_Processor* processor
)
{
    TM32CPU_ReturnValueIf(processor == NULL, false);
    return TM32CPU_CheckFlags(processor, TM32CPU_FT_S);
}

bool TM32CPU_GetErrorString (
    const TM32CPU_Processor*    processor,
    char*                       string,
    size_t                      stringLength
)
{
    TM32CPU_ReturnValueIf(processor == NULL, false);
    TM32CPU_ReturnValueIf(string == NULL, false);
    TM32CPU_ReturnValueIf(stringLength < 256, false);

    switch (processor->ec)
    {
    case TM32CPU_EC_OK:
        snprintf(string, stringLength, "No error occurred.");
        break;
    case TM32CPU_EC_INVALID_OPCODE:
        snprintf(string, stringLength, 
            "Invalid opcode '0x%02X' encountered at address $%08X.",
            processor->itype, processor->ip);
        break;
    case TM32CPU_EC_INVALID_ARGUMENT:
        snprintf(string, stringLength, 
            "Invalid argument provided to instruction '0x%02X' at address $%08X.",
            processor->itype, processor->ip);
        break;
    case TM32CPU_EC_INVALID_READ:
        snprintf(string, stringLength, 
            "The instruction '0x%02X' at address $%08X attempted to read from invalid address $%08X.",
            processor->itype, processor->ip, processor->errorAddress);
        break;
    case TM32CPU_EC_INVALID_WRITE:
        snprintf(string, stringLength,
            "The instruction '0x%02X' at address $%08X attempted to write to invalid address $%08X.",
            processor->itype, processor->ip, processor->errorAddress);
        break;
    case TM32CPU_EC_INVALID_EXECUTE:
        snprintf(string, stringLength,
            "Attempted to execute memory at invalid address $%08X.",
            processor->errorAddress);
        break;
    case TM32CPU_EC_STACK_OVERFLOW:
        snprintf(string, stringLength,
            "Stack overflow occurred while executing instruction '0x%02X' at address $%08X.",
            processor->itype, processor->ip);
        break;
    case TM32CPU_EC_STACK_UNDERFLOW:
        snprintf(string, stringLength,
            "Stack underflow occurred while executing instruction '0x%02X' at address $%08X.",
            processor->itype, processor->ip);
        break;
    case TM32CPU_EC_DIVIDE_BY_ZERO:
        snprintf(string, stringLength,
            "A division by zero was attempted by instruction '0x%02X' at address $%08X.",
            processor->itype, processor->ip);
        break;
    case TM32CPU_EC_HARDWARE_ERROR:
        snprintf(string, stringLength,
            "A hardware error occurred while executing instruction '0x%02X' at address $%08X.",
            processor->itype, processor->ip);
        break;
    default:
        snprintf(string, stringLength,
            "Error code '0x%02X' occurred while executing instruction '0x%02X' at address $%08X.",
            processor->ec, processor->itype, processor->ip);
        break;
    }

    return true;
}

uint64_t TM32CPU_GetCycleCount (
    const TM32CPU_Processor* processor
)
{
    TM32CPU_ReturnValueIf(processor == NULL, 0);
    return processor->cycleCount;
}

void* TM32CPU_GetUserData (
    const TM32CPU_Processor* processor
)
{
    TM32CPU_ReturnValueIf(processor == NULL, NULL);
    return processor->userData;
}

bool TM32CPU_SetUserData (
    TM32CPU_Processor* processor,
    void*              userData
)
{
    TM32CPU_ReturnValueIf(processor == NULL, false);
    processor->userData = userData;
    return true;
}
