/**
 * @file    tm32core_apu.h
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025-08-25
 * 
 * @brief   Contains the opaque type and function prototypes for the TM32 Core
 *          audio processing unit (APU) component.
 */

#ifndef TM32CORE_APU_H
#define TM32CORE_APU_H

/* Include Files **************************************************************/

#include <tm32core_common.h>

/* Typedefs and Forward Declarations ******************************************/

/**
 * @brief   The opaque type representing the TM32 Core Bus interface.
 */
typedef struct TM32Core_Bus TM32Core_Bus;

/**
 * @brief   The opaque type representing the TM32 Core Audio Processing Unit
 *          (APU) component.
 */
typedef struct TM32Core_APU TM32Core_APU;

/**
 * @brief   Callback function type for audio sample output.
 *          This callback is invoked whenever the APU generates a new audio sample.
 * 
 * @param   leftSample  The left channel audio sample value (-32768 to 32767).
 * @param   rightSample The right channel audio sample value (-32768 to 32767).
 * @param   userData    User-provided data pointer passed to the callback.
 * 
 * @return  `true` if the sample was processed successfully;
 *          `false` if an error occurred (will be logged).
 */
typedef bool (*TM32Core_AudioSampleCallback) (
    int16_t leftSample,
    int16_t rightSample,
    void*   userData
);

/* Public Function Prototypes *************************************************/

/**
 * @brief   Allocates and creates a new instance of the TM32 Core Audio
 *          Processing Unit (APU) component.
 * 
 * @param   bus     A pointer to the TM32 Core Bus interface to which this
 *                  APU component will be connected.
 * 
 * @return  A pointer to the newly created TM32 Core APU component;
 *          `NULL` on failure.
 */
TM32CORE_API TM32Core_APU* TM32Core_CreateAPU (
    TM32Core_Bus*   bus
);

/**
 * @brief   Initializes (or resets) the given TM32 Core APU component,
 *          resetting its state.
 * 
 * @param   apu     A pointer to the TM32 Core APU component to initialize.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_InitializeAPU (
    TM32Core_APU*   apu
);

/**
 * @brief   Clocks the given TM32 Core APU component.
 * 
 * This function is called every time the TM32 CPU attached to this APU's bus
 * executes a single instruction cycle.
 * 
 * @param   apu     A pointer to the TM32 Core APU component to clock.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_ClockAPU (
    TM32Core_APU*   apu
);

/**
 * @brief   Destroys and deallocates the given TM32 Core APU component.
 * 
 * @param   apu     A pointer to the TM32 Core APU component to destroy.
 */
TM32CORE_API void TM32Core_DestroyAPU (
    TM32Core_APU*   apu
);

/**
 * @brief   Sets the audio sample callback function for the APU.
 *          This callback will be invoked whenever the APU generates new audio samples.
 * 
 * @param   apu         A pointer to the TM32 Core APU component.
 * @param   callback    The audio sample callback function to set, or NULL to disable.
 * @param   userData    User data pointer to pass to the callback function.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_SetAudioSampleCallback (
    TM32Core_APU*               apu,
    TM32Core_AudioSampleCallback callback,
    void*                       userData
);

/**
 * @brief   Clears the audio sample callback function for the APU.
 *          This is equivalent to calling TM32Core_SetAudioSampleCallback with
 *          NULL callback and userData.
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_ClearAudioSampleCallback (
    TM32Core_APU*   apu
);

/* APU Register Access Functions **********************************************/

/**
 * @brief   Reads the NR10 register (APU Channel 1 Frequency Sweep).
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * 
 * @return  The current value of the NR10 register.
 */
TM32CORE_API uint8_t TM32Core_ReadNR10 (
    const TM32Core_APU* apu
);

/**
 * @brief   Writes to the NR10 register (APU Channel 1 Frequency Sweep).
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * @param   value   The value to write to the NR10 register.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_WriteNR10 (
    TM32Core_APU*   apu,
    uint8_t         value
);

/**
 * @brief   Reads the NR11 register (APU Channel 1 Length / Duty).
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * 
 * @return  The current value of the NR11 register.
 */
TM32CORE_API uint8_t TM32Core_ReadNR11 (
    const TM32Core_APU* apu
);

/**
 * @brief   Writes to the NR11 register (APU Channel 1 Length / Duty).
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * @param   value   The value to write to the NR11 register.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_WriteNR11 (
    TM32Core_APU*   apu,
    uint8_t         value
);

/**
 * @brief   Reads the NR12 register (APU Channel 1 Volume / Envelope).
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * 
 * @return  The current value of the NR12 register.
 */
TM32CORE_API uint8_t TM32Core_ReadNR12 (
    const TM32Core_APU* apu
);

/**
 * @brief   Writes to the NR12 register (APU Channel 1 Volume / Envelope).
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * @param   value   The value to write to the NR12 register.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_WriteNR12 (
    TM32Core_APU*   apu,
    uint8_t         value
);

/**
 * @brief   Writes to the NR13 register (APU Channel 1 Period Low Byte).
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * @param   value   The value to write to the NR13 register.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_WriteNR13 (
    TM32Core_APU*   apu,
    uint8_t         value
);

/**
 * @brief   Reads the NR14 register (APU Channel 1 Period High 3 Bits / Control).
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * 
 * @return  The current value of the NR14 register.
 */
TM32CORE_API uint8_t TM32Core_ReadNR14 (
    const TM32Core_APU* apu
);

/**
 * @brief   Writes to the NR14 register (APU Channel 1 Period High 3 Bits / Control).
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * @param   value   The value to write to the NR14 register.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_WriteNR14 (
    TM32Core_APU*   apu,
    uint8_t         value
);

/**
 * @brief   Reads the NR21 register (APU Channel 2 Length / Duty).
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * 
 * @return  The current value of the NR21 register.
 */
TM32CORE_API uint8_t TM32Core_ReadNR21 (
    const TM32Core_APU* apu
);

/**
 * @brief   Writes to the NR21 register (APU Channel 2 Length / Duty).
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * @param   value   The value to write to the NR21 register.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_WriteNR21 (
    TM32Core_APU*   apu,
    uint8_t         value
);

/**
 * @brief   Reads the NR22 register (APU Channel 2 Volume / Envelope).
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * 
 * @return  The current value of the NR22 register.
 */
TM32CORE_API uint8_t TM32Core_ReadNR22 (
    const TM32Core_APU* apu
);

/**
 * @brief   Writes to the NR22 register (APU Channel 2 Volume / Envelope).
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * @param   value   The value to write to the NR22 register.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_WriteNR22 (
    TM32Core_APU*   apu,
    uint8_t         value
);

/**
 * @brief   Writes to the NR23 register (APU Channel 2 Period Low Byte).
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * @param   value   The value to write to the NR23 register.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_WriteNR23 (
    TM32Core_APU*   apu,
    uint8_t         value
);

/**
 * @brief   Reads the NR24 register (APU Channel 2 Period High 3 Bits / Control).
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * 
 * @return  The current value of the NR24 register.
 */
TM32CORE_API uint8_t TM32Core_ReadNR24 (
    const TM32Core_APU* apu
);

/**
 * @brief   Writes to the NR24 register (APU Channel 2 Period High 3 Bits / Control).
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * @param   value   The value to write to the NR24 register.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_WriteNR24 (
    TM32Core_APU*   apu,
    uint8_t         value
);

/**
 * @brief   Reads the NR30 register (APU Channel 3 DAC Enable).
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * 
 * @return  The current value of the NR30 register.
 */
TM32CORE_API uint8_t TM32Core_ReadNR30 (
    const TM32Core_APU* apu
);

/**
 * @brief   Writes to the NR30 register (APU Channel 3 DAC Enable).
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * @param   value   The value to write to the NR30 register.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_WriteNR30 (
    TM32Core_APU*   apu,
    uint8_t         value
);

/**
 * @brief   Writes to the NR31 register (APU Channel 3 Length).
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * @param   value   The value to write to the NR31 register.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_WriteNR31 (
    TM32Core_APU*   apu,
    uint8_t         value
);

/**
 * @brief   Reads the NR32 register (APU Channel 3 Output Level).
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * 
 * @return  The current value of the NR32 register.
 */
TM32CORE_API uint8_t TM32Core_ReadNR32 (
    const TM32Core_APU* apu
);

/**
 * @brief   Writes to the NR32 register (APU Channel 3 Output Level).
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * @param   value   The value to write to the NR32 register.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_WriteNR32 (
    TM32Core_APU*   apu,
    uint8_t         value
);

/**
 * @brief   Writes to the NR33 register (APU Channel 3 Period Low Byte).
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * @param   value   The value to write to the NR33 register.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_WriteNR33 (
    TM32Core_APU*   apu,
    uint8_t         value
);

/**
 * @brief   Reads the NR34 register (APU Channel 3 Period High 3 Bits / Control).
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * 
 * @return  The current value of the NR34 register.
 */
TM32CORE_API uint8_t TM32Core_ReadNR34 (
    const TM32Core_APU* apu
);

/**
 * @brief   Writes to the NR34 register (APU Channel 3 Period High 3 Bits / Control).
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * @param   value   The value to write to the NR34 register.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_WriteNR34 (
    TM32Core_APU*   apu,
    uint8_t         value
);

/**
 * @brief   Writes to the NR41 register (APU Channel 4 Length).
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * @param   value   The value to write to the NR41 register.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_WriteNR41 (
    TM32Core_APU*   apu,
    uint8_t         value
);

/**
 * @brief   Reads the NR42 register (APU Channel 4 Volume / Envelope).
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * 
 * @return  The current value of the NR42 register.
 */
TM32CORE_API uint8_t TM32Core_ReadNR42 (
    const TM32Core_APU* apu
);

/**
 * @brief   Writes to the NR42 register (APU Channel 4 Volume / Envelope).
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * @param   value   The value to write to the NR42 register.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_WriteNR42 (
    TM32Core_APU*   apu,
    uint8_t         value
);

/**
 * @brief   Reads the NR43 register (APU Channel 4 Frequency / Randomness).
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * 
 * @return  The current value of the NR43 register.
 */
TM32CORE_API uint8_t TM32Core_ReadNR43 (
    const TM32Core_APU* apu
);

/**
 * @brief   Writes to the NR43 register (APU Channel 4 Frequency / Randomness).
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * @param   value   The value to write to the NR43 register.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_WriteNR43 (
    TM32Core_APU*   apu,
    uint8_t         value
);

/**
 * @brief   Reads the NR44 register (APU Channel 4 Control).
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * 
 * @return  The current value of the NR44 register.
 */
TM32CORE_API uint8_t TM32Core_ReadNR44 (
    const TM32Core_APU* apu
);

/**
 * @brief   Writes to the NR44 register (APU Channel 4 Control).
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * @param   value   The value to write to the NR44 register.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_WriteNR44 (
    TM32Core_APU*   apu,
    uint8_t         value
);

/**
 * @brief   Reads the NR50 register (APU Master Volume Control / VIN Panning).
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * 
 * @return  The current value of the NR50 register.
 */
TM32CORE_API uint8_t TM32Core_ReadNR50 (
    const TM32Core_APU* apu
);

/**
 * @brief   Writes to the NR50 register (APU Master Volume Control / VIN Panning).
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * @param   value   The value to write to the NR50 register.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_WriteNR50 (
    TM32Core_APU*   apu,
    uint8_t         value
);

/**
 * @brief   Reads the NR51 register (APU Channel Panning Control).
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * 
 * @return  The current value of the NR51 register.
 */
TM32CORE_API uint8_t TM32Core_ReadNR51 (
    const TM32Core_APU* apu
);

/**
 * @brief   Writes to the NR51 register (APU Channel Panning Control).
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * @param   value   The value to write to the NR51 register.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_WriteNR51 (
    TM32Core_APU*   apu,
    uint8_t         value
);

/**
 * @brief   Reads the NR52 register (APU Master Control / Status).
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * 
 * @return  The current value of the NR52 register.
 */
TM32CORE_API uint8_t TM32Core_ReadNR52 (
    const TM32Core_APU* apu
);

/**
 * @brief   Writes to the NR52 register (APU Master Control / Status).
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * @param   value   The value to write to the NR52 register.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_WriteNR52 (
    TM32Core_APU*   apu,
    uint8_t         value
);

/**
 * @brief   Reads the PCM12 register (APU Digital Output 1 & 2).
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * 
 * @return  The current value of the PCM12 register.
 */
TM32CORE_API uint8_t TM32Core_ReadPCM12 (
    const TM32Core_APU* apu
);

/**
 * @brief   Reads the PCM34 register (APU Digital Output 3 & 4).
 * 
 * @param   apu     A pointer to the TM32 Core APU component.
 * 
 * @return  The current value of the PCM34 register.
 */
TM32CORE_API uint8_t TM32Core_ReadPCM34 (
    const TM32Core_APU* apu
);

#endif // TM32CORE_APU_H