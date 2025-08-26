/**
 * @file    tm32core_rtc.c
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025-08-25
 * 
 * @brief   Implements the TM32 Core Real-Time Clock (RTC) component structure
 *          and functions.
 */

/* Include Files **************************************************************/

#include <time.h>

#include <tm32core_bus.h>
#include <tm32core_rtc.h>

/* Private Structures *********************************************************/

/**
 * @brief   The TM32 Core Real-Time Clock (RTC) component structure.
 */
struct TM32Core_RTC
{
    TM32Core_Bus*   bus;            /** @brief The TM32 Bus to which this RTC is connected. */
    
    // Real-time clock state
    uint64_t        cycleCounter;   /** @brief Cycle counter for time tracking. */
    uint64_t        baseTimestamp;  /** @brief Base timestamp when RTC was initialized. */
    uint32_t        cyclesToSecond; /** @brief Cycles needed to advance one second. */
    
    // Current time values (updated from real time)
    uint8_t         seconds;        /** @brief Current seconds (0-59). */
    uint8_t         minutes;        /** @brief Current minutes (0-59). */
    uint8_t         hours;          /** @brief Current hours (0-23). */
    uint16_t        days;           /** @brief Current day counter (0-65535). */
    
    // Latched time values (for atomic reading)
    uint8_t         latchedSeconds; /** @brief Latched seconds value. */
    uint8_t         latchedMinutes; /** @brief Latched minutes value. */
    uint8_t         latchedHours;   /** @brief Latched hours value. */
    uint16_t        latchedDays;    /** @brief Latched day counter. */
    bool            hasLatchedTime; /** @brief Whether time has been latched. */
    
    // RTC Control Register (RTCC) - Port 0x85
    union {
        uint8_t value;              /** @brief Raw register value. */
        struct {
            uint8_t enable      : 1;    /** @brief Bit 0: RTC Enable (EN). */
            uint8_t reserved1   : 1;    /** @brief Bit 1: Reserved. */
            uint8_t interrupt   : 1;    /** @brief Bit 2: Interrupt Enable (IE). */
            uint8_t reserved2   : 5;    /** @brief Bits 7-3: Reserved. */
        };
    } rtcc;
};

/* Private Function Prototypes ************************************************/

/**
 * @brief   Updates the RTC time from the current system time.
 * 
 * @param   rtc     A pointer to the TM32 Core RTC component.
 */
static void TM32Core_UpdateRTCTime (
    TM32Core_RTC*   rtc
);

/**
 * @brief   Latches the current RTC time for atomic reading.
 * 
 * @param   rtc     A pointer to the TM32 Core RTC component.
 */
static void TM32Core_LatchRTCTime (
    TM32Core_RTC*   rtc
);

/* Private Functions **********************************************************/

static void TM32Core_UpdateRTCTime (
    TM32Core_RTC*   rtc
)
{
    TM32Core_ReturnIfNull(rtc);
    
    if (!rtc->rtcc.enable)
    {
        return; // RTC is disabled
    }
    
    // Get current system time
    time_t currentTime = time(NULL);
    uint64_t elapsedSeconds = (uint64_t)(currentTime - (time_t)rtc->baseTimestamp);
    
    // Calculate time components
    rtc->seconds = elapsedSeconds % 60;
    rtc->minutes = (elapsedSeconds / 60) % 60;
    rtc->hours = (elapsedSeconds / 3600) % 24;
    rtc->days = (uint16_t)((elapsedSeconds / 86400) % 65536);
}

static void TM32Core_LatchRTCTime (
    TM32Core_RTC*   rtc
)
{
    TM32Core_ReturnIfNull(rtc);
    
    // Update current time first
    TM32Core_UpdateRTCTime(rtc);
    
    // Latch the current values
    rtc->latchedSeconds = rtc->seconds;
    rtc->latchedMinutes = rtc->minutes;
    rtc->latchedHours = rtc->hours;
    rtc->latchedDays = rtc->days;
    rtc->hasLatchedTime = true;
}

/* Public Functions ***********************************************************/

TM32Core_RTC* TM32Core_CreateRTC (
    TM32Core_Bus*   bus
)
{
    TM32Core_ReturnValueIfNull(bus, NULL);

    TM32Core_RTC* rtc = TM32Core_CreateZero(1, TM32Core_RTC);
    if (rtc == NULL)
    {
        TM32Core_LogErrno("Could not allocate the TM32 Core RTC component");
        return NULL;
    }

    rtc->bus = bus;

    if (!TM32Core_InitializeRTC(rtc))
    {
        TM32Core_LogError("Could not initialize the TM32 Core RTC component");
        TM32Core_Destroy(rtc);
        return NULL;
    }

    return rtc;
}

bool TM32Core_InitializeRTC (
    TM32Core_RTC*   rtc
)
{
    TM32Core_ReturnValueIfNull(rtc, false);

    // Initialize RTC state
    rtc->cycleCounter = 0;
    rtc->baseTimestamp = (uint64_t)time(NULL);
    rtc->cyclesToSecond = 4194304; // Default: ~4.2MHz clock for 1 second
    
    // Initialize time values
    rtc->seconds = 0;
    rtc->minutes = 0;
    rtc->hours = 0;
    rtc->days = 0;
    
    // Initialize latched values
    rtc->latchedSeconds = 0;
    rtc->latchedMinutes = 0;
    rtc->latchedHours = 0;
    rtc->latchedDays = 0;
    rtc->hasLatchedTime = false;
    
    // Initialize control register (RTC enabled by default)
    rtc->rtcc.value = 0;
    rtc->rtcc.enable = 1;
    rtc->rtcc.interrupt = 0;

    return true;
}

void TM32Core_ClockRTC (
    TM32Core_RTC*   rtc
)
{
    TM32Core_ReturnIfNull(rtc);
    
    if (!rtc->rtcc.enable)
    {
        return; // RTC is disabled
    }
    
    // Update time based on system clock
    TM32Core_UpdateRTCTime(rtc);
}

void TM32Core_DestroyRTC (
    TM32Core_RTC*   rtc
)
{
    if (rtc != NULL)
    {
        TM32Core_Destroy(rtc);
    }
}

/* RTC Register Access Functions *********************************************/

uint8_t TM32Core_ReadRTCS (
    TM32Core_RTC*   rtc
)
{
    TM32Core_ReturnValueIfNull(rtc, 0);
    
    if (rtc->hasLatchedTime)
    {
        return rtc->latchedSeconds & 0x3F; // Mask to 6 bits (0-59)
    }
    else
    {
        TM32Core_UpdateRTCTime(rtc);
        return rtc->seconds & 0x3F;
    }
}

uint8_t TM32Core_ReadRTCM (
    TM32Core_RTC*   rtc
)
{
    TM32Core_ReturnValueIfNull(rtc, 0);
    
    if (rtc->hasLatchedTime)
    {
        return rtc->latchedMinutes & 0x3F; // Mask to 6 bits (0-59)
    }
    else
    {
        TM32Core_UpdateRTCTime(rtc);
        return rtc->minutes & 0x3F;
    }
}

uint8_t TM32Core_ReadRTCH (
    TM32Core_RTC*   rtc
)
{
    TM32Core_ReturnValueIfNull(rtc, 0);
    
    if (rtc->hasLatchedTime)
    {
        return rtc->latchedHours & 0x1F; // Mask to 5 bits (0-23)
    }
    else
    {
        TM32Core_UpdateRTCTime(rtc);
        return rtc->hours & 0x1F;
    }
}

uint8_t TM32Core_ReadRTCD1 (
    TM32Core_RTC*   rtc
)
{
    TM32Core_ReturnValueIfNull(rtc, 0);
    
    if (rtc->hasLatchedTime)
    {
        return (uint8_t)((rtc->latchedDays >> 8) & 0xFF); // High byte
    }
    else
    {
        TM32Core_UpdateRTCTime(rtc);
        return (uint8_t)((rtc->days >> 8) & 0xFF);
    }
}

uint8_t TM32Core_ReadRTCD0 (
    TM32Core_RTC*   rtc
)
{
    TM32Core_ReturnValueIfNull(rtc, 0);
    
    if (rtc->hasLatchedTime)
    {
        return (uint8_t)(rtc->latchedDays & 0xFF); // Low byte
    }
    else
    {
        TM32Core_UpdateRTCTime(rtc);
        return (uint8_t)(rtc->days & 0xFF);
    }
}

uint8_t TM32Core_ReadRTCC (
    TM32Core_RTC*   rtc
)
{
    TM32Core_ReturnValueIfNull(rtc, 0);
    
    return rtc->rtcc.value;
}

void TM32Core_WriteRTCC (
    TM32Core_RTC*   rtc,
    uint8_t         value
)
{
    TM32Core_ReturnIfNull(rtc);
    
    // Update control register, masking to valid bits
    rtc->rtcc.value = value & 0x05; // Only bits 0 and 2 are writable
    
    // If RTC was just enabled, update the base timestamp
    if (rtc->rtcc.enable)
    {
        rtc->baseTimestamp = (uint64_t)time(NULL);
    }
}

void TM32Core_WriteRTCL (
    TM32Core_RTC*   rtc,
    uint8_t         value
)
{
    TM32Core_ReturnIfNull(rtc);
    
    // Latch current time (value is ignored)
    (void)value;
    TM32Core_LatchRTCTime(rtc);
}
