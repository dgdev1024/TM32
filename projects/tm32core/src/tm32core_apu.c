/**
 * @file    tm32core_apu.c
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025-08-25
 * 
 * @brief   Implements the TM32 Core Audio Processing Unit (APU) component
 *          structure and functions.
 */

/* Include Files **************************************************************/

#include <tm32core_bus.h> 
#include <tm32core_apu.h>

/* Include Files **************************************************************/

#include <tm32core_bus.h> 
#include <tm32core_apu.h>

/* Private Constants **********************************************************/

#define TM32CORE_APU_SAMPLE_RATE        44100   /** @brief APU audio sample rate in Hz */
#define TM32CORE_APU_CHANNELS           4       /** @brief Number of audio channels */
#define TM32CORE_APU_CHANNEL1_PULSE     0       /** @brief Channel 1: Pulse with sweep */
#define TM32CORE_APU_CHANNEL2_PULSE     1       /** @brief Channel 2: Pulse */
#define TM32CORE_APU_CHANNEL3_WAVE      2       /** @brief Channel 3: Wavetable */
#define TM32CORE_APU_CHANNEL4_NOISE     3       /** @brief Channel 4: Noise */

/* Private Structures *********************************************************/

/**
 * @brief   APU Channel 1 state structure (Pulse with Sweep).
 */
typedef struct
{
    // NR10 - Frequency Sweep
    union
    {
        uint8_t value;
        struct
        {
            uint8_t sweepShifts     : 3;    /** @brief Sweep shift amount (0-7) */
            uint8_t sweepDirection  : 1;    /** @brief Sweep direction (0=up, 1=down) */
            uint8_t sweepPeriod     : 3;    /** @brief Sweep period (0-7) */
            uint8_t                 : 1;    /** @brief Unused bit */
        };
    } nr10;

    // NR11 - Length / Duty
    union
    {
        uint8_t value;
        struct
        {
            uint8_t length  : 6;    /** @brief Sound length (0-63) */
            uint8_t duty    : 2;    /** @brief Duty cycle (0-3) */
        };
    } nr11;

    // NR12 - Volume / Envelope
    union
    {
        uint8_t value;
        struct
        {
            uint8_t envelopePeriod      : 3;    /** @brief Envelope period (0-7) */
            uint8_t envelopeDirection   : 1;    /** @brief Envelope direction (0=down, 1=up) */
            uint8_t initialVolume       : 4;    /** @brief Initial volume (0-15) */
        };
    } nr12;

    // NR13 - Period Low Byte (write-only)
    uint8_t nr13;

    // NR14 - Period High 3 Bits / Control
    union
    {
        uint8_t value;
        struct
        {
            uint8_t periodHigh  : 3;    /** @brief Period high 3 bits */
            uint8_t             : 3;    /** @brief Unused bits */
            uint8_t lengthEnable: 1;    /** @brief Length counter enable */
            uint8_t trigger     : 1;    /** @brief Trigger (write-only) */
        };
    } nr14;

    // Internal state
    uint16_t    period;             /** @brief Current frequency period */
    uint16_t    periodTimer;        /** @brief Period timer */
    uint8_t     currentVolume;      /** @brief Current volume level */
    uint8_t     lengthCounter;      /** @brief Length counter */
    uint8_t     envelopeTimer;      /** @brief Envelope timer */
    uint8_t     sweepTimer;         /** @brief Sweep timer */
    uint8_t     dutyPosition;       /** @brief Current duty cycle position */
    bool        enabled;            /** @brief Channel enabled state */
} TM32Core_APUChannel1;

/**
 * @brief   APU Channel 2 state structure (Pulse).
 */
typedef struct
{
    // NR21 - Length / Duty
    union
    {
        uint8_t value;
        struct
        {
            uint8_t length  : 6;    /** @brief Sound length (0-63) */
            uint8_t duty    : 2;    /** @brief Duty cycle (0-3) */
        };
    } nr21;

    // NR22 - Volume / Envelope
    union
    {
        uint8_t value;
        struct
        {
            uint8_t envelopePeriod      : 3;    /** @brief Envelope period (0-7) */
            uint8_t envelopeDirection   : 1;    /** @brief Envelope direction (0=down, 1=up) */
            uint8_t initialVolume       : 4;    /** @brief Initial volume (0-15) */
        };
    } nr22;

    // NR23 - Period Low Byte (write-only)
    uint8_t nr23;

    // NR24 - Period High 3 Bits / Control
    union
    {
        uint8_t value;
        struct
        {
            uint8_t periodHigh  : 3;    /** @brief Period high 3 bits */
            uint8_t             : 3;    /** @brief Unused bits */
            uint8_t lengthEnable: 1;    /** @brief Length counter enable */
            uint8_t trigger     : 1;    /** @brief Trigger (write-only) */
        };
    } nr24;

    // Internal state
    uint16_t    period;             /** @brief Current frequency period */
    uint16_t    periodTimer;        /** @brief Period timer */
    uint8_t     currentVolume;      /** @brief Current volume level */
    uint8_t     lengthCounter;      /** @brief Length counter */
    uint8_t     envelopeTimer;      /** @brief Envelope timer */
    uint8_t     dutyPosition;       /** @brief Current duty cycle position */
    bool        enabled;            /** @brief Channel enabled state */
} TM32Core_APUChannel2;

/**
 * @brief   APU Channel 3 state structure (Wavetable).
 */
typedef struct
{
    // NR30 - DAC Enable
    union
    {
        uint8_t value;
        struct
        {
            uint8_t         : 7;    /** @brief Unused bits */
            uint8_t dacEnable: 1;   /** @brief DAC enable */
        };
    } nr30;

    // NR31 - Length (write-only)
    uint8_t nr31;

    // NR32 - Output Level
    union
    {
        uint8_t value;
        struct
        {
            uint8_t         : 5;    /** @brief Unused bits */
            uint8_t outputLevel: 2; /** @brief Output level (0-3) */
            uint8_t         : 1;    /** @brief Unused bit */
        };
    } nr32;

    // NR33 - Period Low Byte (write-only)
    uint8_t nr33;

    // NR34 - Period High 3 Bits / Control
    union
    {
        uint8_t value;
        struct
        {
            uint8_t periodHigh  : 3;    /** @brief Period high 3 bits */
            uint8_t             : 3;    /** @brief Unused bits */
            uint8_t lengthEnable: 1;    /** @brief Length counter enable */
            uint8_t trigger     : 1;    /** @brief Trigger (write-only) */
        };
    } nr34;

    // Internal state
    uint16_t    period;             /** @brief Current frequency period */
    uint16_t    periodTimer;        /** @brief Period timer */
    uint16_t    lengthCounter;      /** @brief Length counter */
    uint8_t     wavePosition;       /** @brief Current wave position */
    uint8_t     waveTable[32];      /** @brief Wave pattern table */
    bool        enabled;            /** @brief Channel enabled state */
} TM32Core_APUChannel3;

/**
 * @brief   APU Channel 4 state structure (Noise).
 */
typedef struct
{
    // NR41 - Length (write-only)
    uint8_t nr41;

    // NR42 - Volume / Envelope
    union
    {
        uint8_t value;
        struct
        {
            uint8_t envelopePeriod      : 3;    /** @brief Envelope period (0-7) */
            uint8_t envelopeDirection   : 1;    /** @brief Envelope direction (0=down, 1=up) */
            uint8_t initialVolume       : 4;    /** @brief Initial volume (0-15) */
        };
    } nr42;

    // NR43 - Frequency / Randomness
    union
    {
        uint8_t value;
        struct
        {
            uint8_t dividerRatio    : 3;    /** @brief Divider ratio (0-7) */
            uint8_t widthMode       : 1;    /** @brief Width mode (0=15-bit, 1=7-bit) */
            uint8_t shiftFrequency  : 4;    /** @brief Shift clock frequency (0-15) */
        };
    } nr43;

    // NR44 - Control
    union
    {
        uint8_t value;
        struct
        {
            uint8_t         : 6;    /** @brief Unused bits */
            uint8_t lengthEnable: 1; /** @brief Length counter enable */
            uint8_t trigger : 1;    /** @brief Trigger (write-only) */
        };
    } nr44;

    // Internal state
    uint16_t    lfsr;               /** @brief Linear feedback shift register */
    uint16_t    periodTimer;        /** @brief Period timer */
    uint8_t     currentVolume;      /** @brief Current volume level */
    uint8_t     lengthCounter;      /** @brief Length counter */
    uint8_t     envelopeTimer;      /** @brief Envelope timer */
    bool        enabled;            /** @brief Channel enabled state */
} TM32Core_APUChannel4;

/**
 * @brief   APU master control structure.
 */
typedef struct
{
    // NR50 - Master Volume Control / VIN Panning
    union
    {
        uint8_t value;
        struct
        {
            uint8_t rightVolume : 3;    /** @brief Right volume (0-7) */
            uint8_t vinRight    : 1;    /** @brief VIN to SO2 (right) */
            uint8_t leftVolume  : 3;    /** @brief Left volume (0-7) */
            uint8_t vinLeft     : 1;    /** @brief VIN to SO1 (left) */
        };
    } nr50;

    // NR51 - Channel Panning Control
    union
    {
        uint8_t value;
        struct
        {
            uint8_t channel1Right   : 1;    /** @brief Channel 1 to right */
            uint8_t channel2Right   : 1;    /** @brief Channel 2 to right */
            uint8_t channel3Right   : 1;    /** @brief Channel 3 to right */
            uint8_t channel4Right   : 1;    /** @brief Channel 4 to right */
            uint8_t channel1Left    : 1;    /** @brief Channel 1 to left */
            uint8_t channel2Left    : 1;    /** @brief Channel 2 to left */
            uint8_t channel3Left    : 1;    /** @brief Channel 3 to left */
            uint8_t channel4Left    : 1;    /** @brief Channel 4 to left */
        };
    } nr51;

    // NR52 - Master Control / Status
    union
    {
        uint8_t value;
        struct
        {
            uint8_t channel1On  : 1;    /** @brief Channel 1 status (read-only) */
            uint8_t channel2On  : 1;    /** @brief Channel 2 status (read-only) */
            uint8_t channel3On  : 1;    /** @brief Channel 3 status (read-only) */
            uint8_t channel4On  : 1;    /** @brief Channel 4 status (read-only) */
            uint8_t             : 3;    /** @brief Unused bits */
            uint8_t powerOn     : 1;    /** @brief Power control */
        };
    } nr52;

    // PCM digital outputs (read-only)
    union
    {
        uint8_t value;
        struct
        {
            uint8_t channel2Output  : 4;    /** @brief Channel 2 digital output */
            uint8_t channel1Output  : 4;    /** @brief Channel 1 digital output */
        };
    } pcm12;

    union
    {
        uint8_t value;
        struct
        {
            uint8_t channel4Output  : 4;    /** @brief Channel 4 digital output */
            uint8_t channel3Output  : 4;    /** @brief Channel 3 digital output */
        };
    } pcm34;
} TM32Core_APUMaster;

struct TM32Core_APU
{
    TM32Core_Bus*               bus;                /** @brief The TM32 Bus to which this APU is connected. */
    
    // Audio sample callback
    TM32Core_AudioSampleCallback audioCallback;    /** @brief Audio sample callback function. */
    void*                       audioUserData;     /** @brief User data for audio sample callback. */
    
    // APU Channels
    TM32Core_APUChannel1        channel1;          /** @brief Channel 1: Pulse with sweep */
    TM32Core_APUChannel2        channel2;          /** @brief Channel 2: Pulse */
    TM32Core_APUChannel3        channel3;          /** @brief Channel 3: Wavetable */
    TM32Core_APUChannel4        channel4;          /** @brief Channel 4: Noise */
    
    // Master control
    TM32Core_APUMaster          master;            /** @brief Master control registers */
    
    // Audio processing state
    uint32_t                    sampleAccumulator; /** @brief Cycle accumulator for sample timing */
    uint32_t                    cyclesPerSample;   /** @brief CPU cycles per audio sample */
    int16_t                     lastLeftSample;    /** @brief Last generated left sample */
    int16_t                     lastRightSample;   /** @brief Last generated right sample */
};

/* Private Function Prototypes ************************************************/

static void TM32Core_InitializeAPUChannel1 (TM32Core_APUChannel1* channel);
static void TM32Core_InitializeAPUChannel2 (TM32Core_APUChannel2* channel);
static void TM32Core_InitializeAPUChannel3 (TM32Core_APUChannel3* channel);
static void TM32Core_InitializeAPUChannel4 (TM32Core_APUChannel4* channel);
static void TM32Core_InitializeAPUMaster (TM32Core_APUMaster* master);

static void TM32Core_ClockAPUChannel1 (TM32Core_APU* apu);
static void TM32Core_ClockAPUChannel2 (TM32Core_APU* apu);
static void TM32Core_ClockAPUChannel3 (TM32Core_APU* apu);
static void TM32Core_ClockAPUChannel4 (TM32Core_APU* apu);

static int8_t TM32Core_GetChannel1Sample (const TM32Core_APUChannel1* channel);
static int8_t TM32Core_GetChannel2Sample (const TM32Core_APUChannel2* channel);
static int8_t TM32Core_GetChannel3Sample (const TM32Core_APUChannel3* channel);
static int8_t TM32Core_GetChannel4Sample (const TM32Core_APUChannel4* channel);

static void TM32Core_MixAudioSamples (TM32Core_APU* apu);
static void TM32Core_TriggerChannel1 (TM32Core_APU* apu);
static void TM32Core_TriggerChannel2 (TM32Core_APU* apu);
static void TM32Core_TriggerChannel3 (TM32Core_APU* apu);
static void TM32Core_TriggerChannel4 (TM32Core_APU* apu);

/* Public Functions ***********************************************************/

TM32Core_APU* TM32Core_CreateAPU (
    TM32Core_Bus*   bus
)
{
    TM32Core_ReturnValueIfNull(bus, NULL);

    TM32Core_APU* apu = TM32Core_CreateZero(1, TM32Core_APU);
    if (apu == NULL)
    {
        TM32Core_LogErrno("Could not allocate the TM32 Core APU component");
        return NULL;
    }

    apu->bus = bus;

    if (!TM32Core_InitializeAPU(apu))
    {
        TM32Core_LogError("Could not initialize the TM32 Core APU component");
        TM32Core_Destroy(apu);
        return NULL;
    }

    return apu;
}

bool TM32Core_InitializeAPU (
    TM32Core_APU*   apu
)
{
    TM32Core_ReturnValueIfNull(apu, false);

    // Initialize audio callback
    apu->audioCallback = NULL;
    apu->audioUserData = NULL;

    // Initialize channels
    TM32Core_InitializeAPUChannel1(&apu->channel1);
    TM32Core_InitializeAPUChannel2(&apu->channel2);
    TM32Core_InitializeAPUChannel3(&apu->channel3);
    TM32Core_InitializeAPUChannel4(&apu->channel4);
    
    // Initialize master control
    TM32Core_InitializeAPUMaster(&apu->master);
    
    // Initialize audio processing state
    // Assuming 4.194304 MHz CPU clock rate
    apu->cyclesPerSample = 4194304 / TM32CORE_APU_SAMPLE_RATE;
    apu->sampleAccumulator = 0;
    apu->lastLeftSample = 0;
    apu->lastRightSample = 0;

    return true;
}

bool TM32Core_ClockAPU (
    TM32Core_APU*   apu
)
{
    TM32Core_ReturnValueIfNull(apu, false);

    // Only process audio if APU is powered on
    if (!apu->master.nr52.powerOn)
    {
        return true;
    }

    // Clock individual channels
    TM32Core_ClockAPUChannel1(apu);
    TM32Core_ClockAPUChannel2(apu);
    TM32Core_ClockAPUChannel3(apu);
    TM32Core_ClockAPUChannel4(apu);

    // Accumulate cycles for sample generation
    apu->sampleAccumulator++;
    if (apu->sampleAccumulator >= apu->cyclesPerSample)
    {
        apu->sampleAccumulator = 0;
        TM32Core_MixAudioSamples(apu);
    }

    return true;
}

void TM32Core_DestroyAPU (
    TM32Core_APU*   apu
)
{
    if (apu != NULL)
    {
        TM32Core_Destroy(apu);
    }
}

bool TM32Core_SetAudioSampleCallback (
    TM32Core_APU*               apu,
    TM32Core_AudioSampleCallback callback,
    void*                       userData
)
{
    if (apu == NULL)
    {
        return false;
    }
    
    apu->audioCallback = callback;
    apu->audioUserData = userData;
    return true;
}

bool TM32Core_ClearAudioSampleCallback (
    TM32Core_APU*   apu
)
{
    if (apu == NULL)
    {
        return false;
    }
    
    apu->audioCallback = NULL;
    apu->audioUserData = NULL;
    return true;
}

/* Register Access Functions **************************************************/

uint8_t TM32Core_ReadNR10 (
    const TM32Core_APU* apu
)
{
    TM32Core_ReturnValueIfNull(apu, 0xFF);
    return apu->channel1.nr10.value | 0x80;  // Bit 7 always reads as 1
}

bool TM32Core_WriteNR10 (
    TM32Core_APU*   apu,
    uint8_t         value
)
{
    TM32Core_ReturnValueIfNull(apu, false);
    if (!apu->master.nr52.powerOn) return true;
    
    apu->channel1.nr10.value = value & 0x7F;  // Bit 7 is read-only
    return true;
}

uint8_t TM32Core_ReadNR11 (
    const TM32Core_APU* apu
)
{
    TM32Core_ReturnValueIfNull(apu, 0xFF);
    return apu->channel1.nr11.value | 0x3F;  // Lower 6 bits are write-only
}

bool TM32Core_WriteNR11 (
    TM32Core_APU*   apu,
    uint8_t         value
)
{
    TM32Core_ReturnValueIfNull(apu, false);
    if (!apu->master.nr52.powerOn) return true;
    
    apu->channel1.nr11.value = value;
    apu->channel1.lengthCounter = 64 - apu->channel1.nr11.length;
    return true;
}

uint8_t TM32Core_ReadNR12 (
    const TM32Core_APU* apu
)
{
    TM32Core_ReturnValueIfNull(apu, 0xFF);
    return apu->channel1.nr12.value;
}

bool TM32Core_WriteNR12 (
    TM32Core_APU*   apu,
    uint8_t         value
)
{
    TM32Core_ReturnValueIfNull(apu, false);
    if (!apu->master.nr52.powerOn) return true;
    
    apu->channel1.nr12.value = value;
    
    // If DAC is disabled (top 5 bits are 0), disable channel
    if ((value & 0xF8) == 0)
    {
        apu->channel1.enabled = false;
    }
    
    return true;
}

bool TM32Core_WriteNR13 (
    TM32Core_APU*   apu,
    uint8_t         value
)
{
    TM32Core_ReturnValueIfNull(apu, false);
    if (!apu->master.nr52.powerOn) return true;
    
    apu->channel1.nr13 = value;
    apu->channel1.period = (apu->channel1.period & 0x700) | value;
    return true;
}

uint8_t TM32Core_ReadNR14 (
    const TM32Core_APU* apu
)
{
    TM32Core_ReturnValueIfNull(apu, 0xFF);
    return apu->channel1.nr14.value | 0xBF;  // Only bit 6 is readable
}

bool TM32Core_WriteNR14 (
    TM32Core_APU*   apu,
    uint8_t         value
)
{
    TM32Core_ReturnValueIfNull(apu, false);
    if (!apu->master.nr52.powerOn) return true;
    
    apu->channel1.nr14.value = value;
    apu->channel1.period = (apu->channel1.period & 0xFF) | ((value & 0x07) << 8);
    
    if (apu->channel1.nr14.trigger)
    {
        TM32Core_TriggerChannel1(apu);
    }
    
    return true;
}

uint8_t TM32Core_ReadNR21 (
    const TM32Core_APU* apu
)
{
    TM32Core_ReturnValueIfNull(apu, 0xFF);
    return apu->channel2.nr21.value | 0x3F;  // Lower 6 bits are write-only
}

bool TM32Core_WriteNR21 (
    TM32Core_APU*   apu,
    uint8_t         value
)
{
    TM32Core_ReturnValueIfNull(apu, false);
    if (!apu->master.nr52.powerOn) return true;
    
    apu->channel2.nr21.value = value;
    apu->channel2.lengthCounter = 64 - apu->channel2.nr21.length;
    return true;
}

uint8_t TM32Core_ReadNR22 (
    const TM32Core_APU* apu
)
{
    TM32Core_ReturnValueIfNull(apu, 0xFF);
    return apu->channel2.nr22.value;
}

bool TM32Core_WriteNR22 (
    TM32Core_APU*   apu,
    uint8_t         value
)
{
    TM32Core_ReturnValueIfNull(apu, false);
    if (!apu->master.nr52.powerOn) return true;
    
    apu->channel2.nr22.value = value;
    
    // If DAC is disabled (top 5 bits are 0), disable channel
    if ((value & 0xF8) == 0)
    {
        apu->channel2.enabled = false;
    }
    
    return true;
}

bool TM32Core_WriteNR23 (
    TM32Core_APU*   apu,
    uint8_t         value
)
{
    TM32Core_ReturnValueIfNull(apu, false);
    if (!apu->master.nr52.powerOn) return true;
    
    apu->channel2.nr23 = value;
    apu->channel2.period = (apu->channel2.period & 0x700) | value;
    return true;
}

uint8_t TM32Core_ReadNR24 (
    const TM32Core_APU* apu
)
{
    TM32Core_ReturnValueIfNull(apu, 0xFF);
    return apu->channel2.nr24.value | 0xBF;  // Only bit 6 is readable
}

bool TM32Core_WriteNR24 (
    TM32Core_APU*   apu,
    uint8_t         value
)
{
    TM32Core_ReturnValueIfNull(apu, false);
    if (!apu->master.nr52.powerOn) return true;
    
    apu->channel2.nr24.value = value;
    apu->channel2.period = (apu->channel2.period & 0xFF) | ((value & 0x07) << 8);
    
    if (apu->channel2.nr24.trigger)
    {
        TM32Core_TriggerChannel2(apu);
    }
    
    return true;
}

uint8_t TM32Core_ReadNR30 (
    const TM32Core_APU* apu
)
{
    TM32Core_ReturnValueIfNull(apu, 0xFF);
    return apu->channel3.nr30.value | 0x7F;  // Only bit 7 is readable
}

bool TM32Core_WriteNR30 (
    TM32Core_APU*   apu,
    uint8_t         value
)
{
    TM32Core_ReturnValueIfNull(apu, false);
    if (!apu->master.nr52.powerOn) return true;
    
    apu->channel3.nr30.value = value;
    
    if (!apu->channel3.nr30.dacEnable)
    {
        apu->channel3.enabled = false;
    }
    
    return true;
}

bool TM32Core_WriteNR31 (
    TM32Core_APU*   apu,
    uint8_t         value
)
{
    TM32Core_ReturnValueIfNull(apu, false);
    if (!apu->master.nr52.powerOn) return true;
    
    apu->channel3.nr31 = value;
    apu->channel3.lengthCounter = 256 - value;
    return true;
}

uint8_t TM32Core_ReadNR32 (
    const TM32Core_APU* apu
)
{
    TM32Core_ReturnValueIfNull(apu, 0xFF);
    return apu->channel3.nr32.value | 0x9F;  // Only bits 6-5 are readable
}

bool TM32Core_WriteNR32 (
    TM32Core_APU*   apu,
    uint8_t         value
)
{
    TM32Core_ReturnValueIfNull(apu, false);
    if (!apu->master.nr52.powerOn) return true;
    
    apu->channel3.nr32.value = value & 0x60;  // Only bits 6-5 are writable
    return true;
}

bool TM32Core_WriteNR33 (
    TM32Core_APU*   apu,
    uint8_t         value
)
{
    TM32Core_ReturnValueIfNull(apu, false);
    if (!apu->master.nr52.powerOn) return true;
    
    apu->channel3.nr33 = value;
    apu->channel3.period = (apu->channel3.period & 0x700) | value;
    return true;
}

uint8_t TM32Core_ReadNR34 (
    const TM32Core_APU* apu
)
{
    TM32Core_ReturnValueIfNull(apu, 0xFF);
    return apu->channel3.nr34.value | 0xBF;  // Only bit 6 is readable
}

bool TM32Core_WriteNR34 (
    TM32Core_APU*   apu,
    uint8_t         value
)
{
    TM32Core_ReturnValueIfNull(apu, false);
    if (!apu->master.nr52.powerOn) return true;
    
    apu->channel3.nr34.value = value;
    apu->channel3.period = (apu->channel3.period & 0xFF) | ((value & 0x07) << 8);
    
    if (apu->channel3.nr34.trigger)
    {
        TM32Core_TriggerChannel3(apu);
    }
    
    return true;
}

bool TM32Core_WriteNR41 (
    TM32Core_APU*   apu,
    uint8_t         value
)
{
    TM32Core_ReturnValueIfNull(apu, false);
    if (!apu->master.nr52.powerOn) return true;
    
    apu->channel4.nr41 = value & 0x3F;  // Only lower 6 bits are used
    apu->channel4.lengthCounter = 64 - (value & 0x3F);
    return true;
}

uint8_t TM32Core_ReadNR42 (
    const TM32Core_APU* apu
)
{
    TM32Core_ReturnValueIfNull(apu, 0xFF);
    return apu->channel4.nr42.value;
}

bool TM32Core_WriteNR42 (
    TM32Core_APU*   apu,
    uint8_t         value
)
{
    TM32Core_ReturnValueIfNull(apu, false);
    if (!apu->master.nr52.powerOn) return true;
    
    apu->channel4.nr42.value = value;
    
    // If DAC is disabled (top 5 bits are 0), disable channel
    if ((value & 0xF8) == 0)
    {
        apu->channel4.enabled = false;
    }
    
    return true;
}

uint8_t TM32Core_ReadNR43 (
    const TM32Core_APU* apu
)
{
    TM32Core_ReturnValueIfNull(apu, 0xFF);
    return apu->channel4.nr43.value;
}

bool TM32Core_WriteNR43 (
    TM32Core_APU*   apu,
    uint8_t         value
)
{
    TM32Core_ReturnValueIfNull(apu, false);
    if (!apu->master.nr52.powerOn) return true;
    
    apu->channel4.nr43.value = value;
    return true;
}

uint8_t TM32Core_ReadNR44 (
    const TM32Core_APU* apu
)
{
    TM32Core_ReturnValueIfNull(apu, 0xFF);
    return apu->channel4.nr44.value | 0xBF;  // Only bit 6 is readable
}

bool TM32Core_WriteNR44 (
    TM32Core_APU*   apu,
    uint8_t         value
)
{
    TM32Core_ReturnValueIfNull(apu, false);
    if (!apu->master.nr52.powerOn) return true;
    
    apu->channel4.nr44.value = value;
    
    if (apu->channel4.nr44.trigger)
    {
        TM32Core_TriggerChannel4(apu);
    }
    
    return true;
}

uint8_t TM32Core_ReadNR50 (
    const TM32Core_APU* apu
)
{
    TM32Core_ReturnValueIfNull(apu, 0xFF);
    return apu->master.nr50.value;
}

bool TM32Core_WriteNR50 (
    TM32Core_APU*   apu,
    uint8_t         value
)
{
    TM32Core_ReturnValueIfNull(apu, false);
    if (!apu->master.nr52.powerOn) return true;
    
    apu->master.nr50.value = value;
    return true;
}

uint8_t TM32Core_ReadNR51 (
    const TM32Core_APU* apu
)
{
    TM32Core_ReturnValueIfNull(apu, 0xFF);
    return apu->master.nr51.value;
}

bool TM32Core_WriteNR51 (
    TM32Core_APU*   apu,
    uint8_t         value
)
{
    TM32Core_ReturnValueIfNull(apu, false);
    if (!apu->master.nr52.powerOn) return true;
    
    apu->master.nr51.value = value;
    return true;
}

uint8_t TM32Core_ReadNR52 (
    const TM32Core_APU* apu
)
{
    TM32Core_ReturnValueIfNull(apu, 0xFF);
    
    // Build the status value with current channel states
    uint8_t statusValue = apu->master.nr52.value & 0x80;  // Keep power bit
    
    if (apu->channel1.enabled) statusValue |= 0x01;
    if (apu->channel2.enabled) statusValue |= 0x02;
    if (apu->channel3.enabled) statusValue |= 0x04;
    if (apu->channel4.enabled) statusValue |= 0x08;
    
    return statusValue | 0x70;  // Bits 6-4 always read as 1
}

bool TM32Core_WriteNR52 (
    TM32Core_APU*   apu,
    uint8_t         value
)
{
    TM32Core_ReturnValueIfNull(apu, false);
    
    bool wasPowered = apu->master.nr52.powerOn;
    apu->master.nr52.powerOn = (value & 0x80) != 0;
    
    // If turning off APU, clear all registers
    if (wasPowered && !apu->master.nr52.powerOn)
    {
        // Clear all channel registers
        TM32Core_InitializeAPUChannel1(&apu->channel1);
        TM32Core_InitializeAPUChannel2(&apu->channel2);
        TM32Core_InitializeAPUChannel3(&apu->channel3);
        TM32Core_InitializeAPUChannel4(&apu->channel4);
        
        // Clear master control (except power bit)
        apu->master.nr50.value = 0;
        apu->master.nr51.value = 0;
        apu->master.nr52.value &= 0x80;
    }
    
    return true;
}

uint8_t TM32Core_ReadPCM12 (
    const TM32Core_APU* apu
)
{
    TM32Core_ReturnValueIfNull(apu, 0xFF);
    return apu->master.pcm12.value;
}

uint8_t TM32Core_ReadPCM34 (
    const TM32Core_APU* apu
)
{
    TM32Core_ReturnValueIfNull(apu, 0xFF);
    return apu->master.pcm34.value;
}

/* Private Functions - Channel Initialization ********************************/

static void TM32Core_InitializeAPUChannel1 (
    TM32Core_APUChannel1* channel
)
{
    if (channel == NULL) return;
    
    // Clear all registers
    channel->nr10.value = 0;
    channel->nr11.value = 0;
    channel->nr12.value = 0;
    channel->nr13 = 0;
    channel->nr14.value = 0;
    
    // Reset internal state
    channel->period = 0;
    channel->periodTimer = 0;
    channel->currentVolume = 0;
    channel->lengthCounter = 0;
    channel->envelopeTimer = 0;
    channel->sweepTimer = 0;
    channel->dutyPosition = 0;
    channel->enabled = false;
}

static void TM32Core_InitializeAPUChannel2 (
    TM32Core_APUChannel2* channel
)
{
    if (channel == NULL) return;
    
    // Clear all registers
    channel->nr21.value = 0;
    channel->nr22.value = 0;
    channel->nr23 = 0;
    channel->nr24.value = 0;
    
    // Reset internal state
    channel->period = 0;
    channel->periodTimer = 0;
    channel->currentVolume = 0;
    channel->lengthCounter = 0;
    channel->envelopeTimer = 0;
    channel->dutyPosition = 0;
    channel->enabled = false;
}

static void TM32Core_InitializeAPUChannel3 (
    TM32Core_APUChannel3* channel
)
{
    if (channel == NULL) return;
    
    // Clear all registers
    channel->nr30.value = 0;
    channel->nr31 = 0;
    channel->nr32.value = 0;
    channel->nr33 = 0;
    channel->nr34.value = 0;
    
    // Reset internal state
    channel->period = 0;
    channel->periodTimer = 0;
    channel->lengthCounter = 0;
    channel->wavePosition = 0;
    channel->enabled = false;
    
    // Clear wave table
    for (int i = 0; i < 32; i++)
    {
        channel->waveTable[i] = 0;
    }
}

static void TM32Core_InitializeAPUChannel4 (
    TM32Core_APUChannel4* channel
)
{
    if (channel == NULL) return;
    
    // Clear all registers
    channel->nr41 = 0;
    channel->nr42.value = 0;
    channel->nr43.value = 0;
    channel->nr44.value = 0;
    
    // Reset internal state
    channel->lfsr = 0x7FFF;
    channel->periodTimer = 0;
    channel->currentVolume = 0;
    channel->lengthCounter = 0;
    channel->envelopeTimer = 0;
    channel->enabled = false;
}

static void TM32Core_InitializeAPUMaster (
    TM32Core_APUMaster* master
)
{
    if (master == NULL) return;
    
    // Clear all master registers
    master->nr50.value = 0;
    master->nr51.value = 0;
    master->nr52.value = 0;
    master->pcm12.value = 0;
    master->pcm34.value = 0;
}

/* Private Functions - Channel Clocking **************************************/

static void TM32Core_ClockAPUChannel1 (
    TM32Core_APU* apu
)
{
    if (!apu->channel1.enabled) return;
    
    // Clock period timer
    if (apu->channel1.periodTimer > 0)
    {
        apu->channel1.periodTimer--;
    }
    else
    {
        apu->channel1.periodTimer = (2048 - apu->channel1.period) * 4;
        apu->channel1.dutyPosition = (apu->channel1.dutyPosition + 1) & 7;
    }
    
    // Update PCM output
    int8_t sample = TM32Core_GetChannel1Sample(&apu->channel1);
    apu->master.pcm12.channel1Output = (sample + 128) >> 4;  // Convert to 4-bit unsigned
}

static void TM32Core_ClockAPUChannel2 (
    TM32Core_APU* apu
)
{
    if (!apu->channel2.enabled) return;
    
    // Clock period timer
    if (apu->channel2.periodTimer > 0)
    {
        apu->channel2.periodTimer--;
    }
    else
    {
        apu->channel2.periodTimer = (2048 - apu->channel2.period) * 4;
        apu->channel2.dutyPosition = (apu->channel2.dutyPosition + 1) & 7;
    }
    
    // Update PCM output
    int8_t sample = TM32Core_GetChannel2Sample(&apu->channel2);
    apu->master.pcm12.channel2Output = (sample + 128) >> 4;  // Convert to 4-bit unsigned
}

static void TM32Core_ClockAPUChannel3 (
    TM32Core_APU* apu
)
{
    if (!apu->channel3.enabled) return;
    
    // Clock period timer
    if (apu->channel3.periodTimer > 0)
    {
        apu->channel3.periodTimer--;
    }
    else
    {
        apu->channel3.periodTimer = (2048 - apu->channel3.period) * 2;
        apu->channel3.wavePosition = (apu->channel3.wavePosition + 1) & 31;
    }
    
    // Update PCM output
    int8_t sample = TM32Core_GetChannel3Sample(&apu->channel3);
    apu->master.pcm34.channel3Output = (sample + 128) >> 4;  // Convert to 4-bit unsigned
}

static void TM32Core_ClockAPUChannel4 (
    TM32Core_APU* apu
)
{
    if (!apu->channel4.enabled) return;
    
    // Clock period timer
    if (apu->channel4.periodTimer > 0)
    {
        apu->channel4.periodTimer--;
    }
    else
    {
        // Calculate period based on NR43 settings
        uint32_t divisor = apu->channel4.nr43.dividerRatio == 0 ? 8 : apu->channel4.nr43.dividerRatio << 4;
        apu->channel4.periodTimer = divisor << apu->channel4.nr43.shiftFrequency;
        
        // Clock LFSR
        uint16_t bit = (apu->channel4.lfsr & 1) ^ ((apu->channel4.lfsr >> 1) & 1);
        apu->channel4.lfsr >>= 1;
        apu->channel4.lfsr |= bit << 14;
        
        if (apu->channel4.nr43.widthMode)
        {
            apu->channel4.lfsr &= ~(1 << 6);
            apu->channel4.lfsr |= bit << 6;
        }
    }
    
    // Update PCM output
    int8_t sample = TM32Core_GetChannel4Sample(&apu->channel4);
    apu->master.pcm34.channel4Output = (sample + 128) >> 4;  // Convert to 4-bit unsigned
}

/* Private Functions - Sample Generation *************************************/

static int8_t TM32Core_GetChannel1Sample (
    const TM32Core_APUChannel1* channel
)
{
    if (!channel->enabled) return 0;
    
    // Duty cycle patterns (Game Boy style)
    static const uint8_t dutyPatterns[4] = { 0x01, 0x81, 0x87, 0x7E };
    
    uint8_t pattern = dutyPatterns[channel->nr11.duty];
    bool output = (pattern >> channel->dutyPosition) & 1;
    
    return output ? (int8_t)channel->currentVolume : -(int8_t)channel->currentVolume;
}

static int8_t TM32Core_GetChannel2Sample (
    const TM32Core_APUChannel2* channel
)
{
    if (!channel->enabled) return 0;
    
    // Duty cycle patterns (Game Boy style)
    static const uint8_t dutyPatterns[4] = { 0x01, 0x81, 0x87, 0x7E };
    
    uint8_t pattern = dutyPatterns[channel->nr21.duty];
    bool output = (pattern >> channel->dutyPosition) & 1;
    
    return output ? (int8_t)channel->currentVolume : -(int8_t)channel->currentVolume;
}

static int8_t TM32Core_GetChannel3Sample (
    const TM32Core_APUChannel3* channel
)
{
    if (!channel->enabled || !channel->nr30.dacEnable) return 0;
    
    uint8_t sample = channel->waveTable[channel->wavePosition];
    
    // Apply output level scaling
    switch (channel->nr32.outputLevel)
    {
    case 0: return 0;                           // Mute
    case 1: return (int8_t)(sample - 128);      // 100% volume
    case 2: return (int8_t)((sample - 128) >> 1);  // 50% volume
    case 3: return (int8_t)((sample - 128) >> 2);  // 25% volume
    default: return 0;
    }
}

static int8_t TM32Core_GetChannel4Sample (
    const TM32Core_APUChannel4* channel
)
{
    if (!channel->enabled) return 0;
    
    bool output = !(channel->lfsr & 1);
    return output ? (int8_t)channel->currentVolume : -(int8_t)channel->currentVolume;
}

static void TM32Core_MixAudioSamples (
    TM32Core_APU* apu
)
{
    // Get individual channel samples
    int8_t ch1Sample = TM32Core_GetChannel1Sample(&apu->channel1);
    int8_t ch2Sample = TM32Core_GetChannel2Sample(&apu->channel2);
    int8_t ch3Sample = TM32Core_GetChannel3Sample(&apu->channel3);
    int8_t ch4Sample = TM32Core_GetChannel4Sample(&apu->channel4);
    
    // Mix left channel
    int16_t leftMix = 0;
    if (apu->master.nr51.channel1Left) leftMix += ch1Sample;
    if (apu->master.nr51.channel2Left) leftMix += ch2Sample;
    if (apu->master.nr51.channel3Left) leftMix += ch3Sample;
    if (apu->master.nr51.channel4Left) leftMix += ch4Sample;
    leftMix = (leftMix * (apu->master.nr50.leftVolume + 1)) << 6;
    
    // Mix right channel
    int16_t rightMix = 0;
    if (apu->master.nr51.channel1Right) rightMix += ch1Sample;
    if (apu->master.nr51.channel2Right) rightMix += ch2Sample;
    if (apu->master.nr51.channel3Right) rightMix += ch3Sample;
    if (apu->master.nr51.channel4Right) rightMix += ch4Sample;
    rightMix = (rightMix * (apu->master.nr50.rightVolume + 1)) << 6;
    
    // Store samples
    apu->lastLeftSample = leftMix;
    apu->lastRightSample = rightMix;
    
    // Invoke callback if set
    if (apu->audioCallback != NULL)
    {
        if (!apu->audioCallback(leftMix, rightMix, apu->audioUserData))
        {
            // Log callback error if needed - callback returned false
            // For now, we'll continue execution
        }
    }
}

/* Private Functions - Channel Triggering ************************************/

static void TM32Core_TriggerChannel1 (
    TM32Core_APU* apu
)
{
    apu->channel1.enabled = true;
    apu->channel1.periodTimer = (2048 - apu->channel1.period) * 4;
    apu->channel1.currentVolume = apu->channel1.nr12.initialVolume;
    apu->channel1.envelopeTimer = apu->channel1.nr12.envelopePeriod;
    apu->channel1.sweepTimer = apu->channel1.nr10.sweepPeriod;
    
    if (apu->channel1.lengthCounter == 0)
    {
        apu->channel1.lengthCounter = 64;
    }
    
    // Check if DAC is enabled
    if ((apu->channel1.nr12.value & 0xF8) == 0)
    {
        apu->channel1.enabled = false;
    }
}

static void TM32Core_TriggerChannel2 (
    TM32Core_APU* apu
)
{
    apu->channel2.enabled = true;
    apu->channel2.periodTimer = (2048 - apu->channel2.period) * 4;
    apu->channel2.currentVolume = apu->channel2.nr22.initialVolume;
    apu->channel2.envelopeTimer = apu->channel2.nr22.envelopePeriod;
    
    if (apu->channel2.lengthCounter == 0)
    {
        apu->channel2.lengthCounter = 64;
    }
    
    // Check if DAC is enabled
    if ((apu->channel2.nr22.value & 0xF8) == 0)
    {
        apu->channel2.enabled = false;
    }
}

static void TM32Core_TriggerChannel3 (
    TM32Core_APU* apu
)
{
    apu->channel3.enabled = true;
    apu->channel3.periodTimer = (2048 - apu->channel3.period) * 2;
    
    if (apu->channel3.lengthCounter == 0)
    {
        apu->channel3.lengthCounter = 256;
    }
    
    // Check if DAC is enabled
    if (!apu->channel3.nr30.dacEnable)
    {
        apu->channel3.enabled = false;
    }
}

static void TM32Core_TriggerChannel4 (
    TM32Core_APU* apu
)
{
    apu->channel4.enabled = true;
    apu->channel4.lfsr = 0x7FFF;
    apu->channel4.currentVolume = apu->channel4.nr42.initialVolume;
    apu->channel4.envelopeTimer = apu->channel4.nr42.envelopePeriod;
    
    // Calculate initial period
    uint32_t divisor = apu->channel4.nr43.dividerRatio == 0 ? 8 : apu->channel4.nr43.dividerRatio << 4;
    apu->channel4.periodTimer = divisor << apu->channel4.nr43.shiftFrequency;
    
    if (apu->channel4.lengthCounter == 0)
    {
        apu->channel4.lengthCounter = 64;
    }
    
    // Check if DAC is enabled
    if ((apu->channel4.nr42.value & 0xF8) == 0)
    {
        apu->channel4.enabled = false;
    }
}
