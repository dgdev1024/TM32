/**
 * @file    tm32test_processor_bus.h
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * 
 * @brief   Provides a system bus to be used by unit tests for the TM32 CPU
 *          library.
 */

#ifndef TM32TEST_PROCESSOR_BUS_H
#define TM32TEST_PROCESSOR_BUS_H

/* Include Files **************************************************************/

#include <tm32cpu_processor.h>

/* Public Constants ***********************************************************/

#define TM32TEST_PROCESSOR_BUS_ROM_SIZE     0x10000 // 64K ROM
#define TM32TEST_PROCESSOR_BUS_RAM_SIZE     0x10000 // 64K RAM
#define TM32TEST_PROCESSOR_BUS_STACK_SIZE   0x10000 // 64K Stack

/* Public Structures **********************************************************/

typedef struct
{
    TM32CPU_Processor*      processor;
    uint8_t*                rom;
    uint8_t*                ram;
    uint8_t*                stack;
    TM32CPU_ClockCallback   clockCallback;
} TM32Test_ProcessorBus;

/* Public Function Prototypes *************************************************/

TM32Test_ProcessorBus* TM32Test_CreateProcessorBus ();
void TM32Test_DestroyProcessorBus (TM32Test_ProcessorBus* bus);
void TM32Test_SetProcessorBusClockCallback (
    TM32Test_ProcessorBus* bus,
    TM32CPU_ClockCallback  callback
);

#endif // TM32TEST_PROCESSOR_BUS_H
