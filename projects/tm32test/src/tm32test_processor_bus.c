/**
 * @file    tm32test_processor_bus.h
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025-08-21
 * 
 * @brief   Implementation of the TM32Test_CPUBus functions.
 */

#include <tm32test_processor_bus.h>

/* Private Functions **********************************************************/

static bool TM32Test_BusReadCallback (
    uint32_t    address,
    uint8_t*    data,
    void*       userData
)
{
    if (data == NULL || userData == NULL)
    {
        return false; // Invalid parameters
    }

    TM32Test_ProcessorBus* bus = (TM32Test_ProcessorBus*) userData;

    if (
        address >= TM32CPU_PROGRAM_START && 
        address < (TM32CPU_PROGRAM_START + TM32TEST_PROCESSOR_BUS_ROM_SIZE)
    )
    {
        // Read from ROM
        *data = bus->rom[address - TM32CPU_PROGRAM_START];
        return true;
    }
    else if (
        address >= TM32CPU_RAM_START 
        && address < (TM32CPU_RAM_START + TM32TEST_PROCESSOR_BUS_RAM_SIZE)
    )
    {
        // Read from RAM
        *data = bus->ram[address - TM32CPU_RAM_START];
        return true;
    }
    else if (
        address >= TM32CPU_STACK_START 
        && address < (TM32CPU_STACK_START + TM32TEST_PROCESSOR_BUS_STACK_SIZE)
    )
    {
        // Read from Stack
        *data = bus->stack[address - TM32CPU_STACK_START];
        return true;
    }

    return false;
}

static bool TM32Test_BusWriteCallback (
    uint32_t    address,
    uint8_t     data,
    void*       userData
)
{
    TM32Test_ProcessorBus* bus = (TM32Test_ProcessorBus*) userData;

    if (
        address >= TM32CPU_RAM_START 
        && address < (TM32CPU_RAM_START + TM32TEST_PROCESSOR_BUS_RAM_SIZE)
    )
    {
        // Write to RAM
        bus->ram[address - TM32CPU_RAM_START] = data;
        return true;
    }
    else if (
        address >= TM32CPU_STACK_START 
        && address < (TM32CPU_STACK_START + TM32TEST_PROCESSOR_BUS_STACK_SIZE)
    )
    {
        // Write to Stack
        bus->stack[address - TM32CPU_STACK_START] = data;
        return true;
    }

    return false;
}

static bool TM32Test_ClockCallback (
    uint32_t    cycles,
    void*       userData
)
{
    return true;
}

/* Public Functions ***********************************************************/

TM32Test_ProcessorBus* TM32Test_CreateProcessorBus ()
{
    // Create the bus.
    TM32Test_ProcessorBus* bus = TM32CPU_Create(1, TM32Test_ProcessorBus);
    if (bus == NULL)
    {
        TM32CPU_LogErrno("Could not create the test processor bus");
        return NULL;
    }

    // Allocate memory for ROM, RAM, and stack.
    bus->rom    = TM32CPU_Create(TM32TEST_PROCESSOR_BUS_ROM_SIZE, uint8_t);
    bus->ram    = TM32CPU_Create(TM32TEST_PROCESSOR_BUS_RAM_SIZE, uint8_t);
    bus->stack  = TM32CPU_Create(TM32TEST_PROCESSOR_BUS_STACK_SIZE, uint8_t);
    if (bus->rom == NULL || bus->ram == NULL || bus->stack == NULL)
    {
        TM32CPU_LogError("Could not allocate memory for the test processor bus.");
        TM32CPU_Destroy(bus->rom);
        TM32CPU_Destroy(bus->ram);
        TM32CPU_Destroy(bus->stack);
        TM32CPU_Destroy(bus);
        return NULL;
    }

    // Create and initialize the processor.
    bus->processor = TM32CPU_CreateProcessor(
        TM32Test_BusReadCallback,
        TM32Test_BusWriteCallback,
        TM32Test_ClockCallback,
        bus
    );
    if (bus->processor == NULL)
    {
        TM32CPU_LogError("Could not create the test processor.");
        TM32CPU_Destroy(bus->rom);
        TM32CPU_Destroy(bus->ram);
        TM32CPU_Destroy(bus->stack);
        TM32CPU_Destroy(bus);
        return NULL;
    }

    // Set the user data for the processor.
    TM32CPU_SetUserData(bus->processor, bus);

    return bus;
}

void TM32Test_DestroyProcessorBus (TM32Test_ProcessorBus* bus)
{
    if (bus != NULL)
    {
        // Destroy the processor.
        TM32CPU_SetUserData(bus->processor, NULL);
        TM32CPU_DestroyProcessor(bus->processor);

        // Free allocated memory.
        TM32CPU_Destroy(bus->rom);
        TM32CPU_Destroy(bus->ram);
        TM32CPU_Destroy(bus->stack);

        // Free the bus structure itself.
        TM32CPU_Destroy(bus);
    }
}

void TM32Test_SetProcessorBusClockCallback (
    TM32Test_ProcessorBus* bus,
    TM32CPU_ClockCallback  callback
)
{
    if (bus != NULL)
    {
        bus->clockCallback = callback;
    }
}
