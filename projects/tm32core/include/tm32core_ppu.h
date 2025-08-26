/**
 * @file    tm32core_ppu.h
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025-08-25
 * 
 * @brief   Contains the opaque type and function prototypes for the TM32 Core
 *          pixel processing unit (PPU) component.
 */

#ifndef TM32CORE_PPU_H
#define TM32CORE_PPU_H

/* Include Files **************************************************************/

#include <tm32core_common.h>

/* Typedefs and Forward Declarations ******************************************/

/**
 * @brief   The opaque type representing the TM32 Core Bus interface.
 */
typedef struct TM32Core_Bus TM32Core_Bus;

/**
 * @brief   The opaque type representing the TM32 Core Pixel Processing Unit
 *          (PPU) component.
 */
typedef struct TM32Core_PPU TM32Core_PPU;

/* Public Enumerations ********************************************************/

/**
 * @brief   PPU drawing modes.
 */
typedef enum
{
    TM32CORE_PPU_MODE_HBLANK = 0,   /** @brief Horizontal blank period */
    TM32CORE_PPU_MODE_VBLANK = 1,   /** @brief Vertical blank period */
    TM32CORE_PPU_MODE_OAM = 2,      /** @brief OAM search period */
    TM32CORE_PPU_MODE_DRAWING = 3   /** @brief Pixel transfer period */
} TM32Core_PPUMode;

/* Public Callback Types ******************************************************/

/**
 * @brief   Defines a callback function invoked when the PPU enters VBlank mode.
 *          This callback allows frontend applications to perform rendering
 *          duties during the vertical blank period.
 * 
 * @param   ppu         A pointer to the TM32 Core PPU component that entered VBlank.
 * @param   userData    User-provided data pointer passed to the callback.
 * 
 * @return  `true` if the callback was processed successfully;
 *          `false` if an error occurred (will be logged).
 */
typedef bool (*TM32Core_VBlankCallback) (
    const TM32Core_PPU* ppu,
    void*               userData
);

/* Public Function Prototypes *************************************************/

/**
 * @brief   Allocates and creates a new instance of the TM32 Core Pixel
 *          Processing Unit (PPU) component.
 * 
 * @param   bus     A pointer to the TM32 Core Bus interface to which this
 *                  PPU component will be connected.
 * 
 * @return  A pointer to the newly created TM32 Core PPU component;
 *          `NULL` on failure.
 */
TM32CORE_API TM32Core_PPU* TM32Core_CreatePPU (
    TM32Core_Bus*   bus
);

/**
 * @brief   Initializes (or resets) the given TM32 Core PPU component,
 *          resetting its state.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component to initialize.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_InitializePPU (
    TM32Core_PPU*   ppu
);

/**
 * @brief   Clocks the given TM32 Core PPU component.
 * 
 * This function is called every time the TM32 CPU attached to this PPU's bus
 * executes an instruction.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component to clock.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_ClockPPU (
    TM32Core_PPU*   ppu
);

/**
 * @brief   Destroys and deallocates the given TM32 Core PPU component.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component to destroy.
 */
TM32CORE_API void TM32Core_DestroyPPU (
    TM32Core_PPU*   ppu
);

/* VBlank Callback Management **************************************************/

/**
 * @brief   Sets a VBlank callback function for the PPU.
 *          This callback will be invoked when the PPU enters VBlank mode,
 *          allowing frontend applications to perform rendering duties.
 * 
 * @param   ppu         A pointer to the TM32 Core PPU component.
 * @param   callback    The VBlank callback function to set, or NULL to disable.
 * @param   userData    User data pointer to pass to the callback function.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_SetVBlankCallback (
    TM32Core_PPU*           ppu,
    TM32Core_VBlankCallback callback,
    void*                   userData
);

/**
 * @brief   Clears the VBlank callback function for the PPU.
 *          This is equivalent to calling TM32Core_SetVBlankCallback with
 *          NULL callback and userData.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_ClearVBlankCallback (
    TM32Core_PPU*   ppu
);

/* PPU Register Access Functions **********************************************/

/**
 * @brief   Reads the LCDC (LCD Control) register from the PPU.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component.
 * 
 * @return  The current value of the LCDC register.
 */
TM32CORE_API uint8_t TM32Core_ReadLCDC (
    const TM32Core_PPU*   ppu
);

/**
 * @brief   Writes to the LCDC (LCD Control) register of the PPU.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component.
 * @param   value   The value to write to the LCDC register.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_WriteLCDC (
    TM32Core_PPU*   ppu,
    uint8_t         value
);

/**
 * @brief   Reads the STAT (LCD Status) register from the PPU.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component.
 * 
 * @return  The current value of the STAT register.
 */
TM32CORE_API uint8_t TM32Core_ReadSTAT (
    const TM32Core_PPU*   ppu
);

/**
 * @brief   Writes to the STAT (LCD Status) register of the PPU.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component.
 * @param   value   The value to write to the STAT register.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_WriteSTAT (
    TM32Core_PPU*   ppu,
    uint8_t         value
);

/**
 * @brief   Reads the LY (Current Scanline) register from the PPU.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component.
 * 
 * @return  The current value of the LY register.
 */
TM32CORE_API uint8_t TM32Core_ReadLY (
    const TM32Core_PPU*   ppu
);

/**
 * @brief   Reads the LYC (Scanline Compare) register from the PPU.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component.
 * 
 * @return  The current value of the LYC register.
 */
TM32CORE_API uint8_t TM32Core_ReadLYC (
    const TM32Core_PPU*   ppu
);

/**
 * @brief   Writes to the LYC (Scanline Compare) register of the PPU.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component.
 * @param   value   The value to write to the LYC register.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_WriteLYC (
    TM32Core_PPU*   ppu,
    uint8_t         value
);

/**
 * @brief   Reads a scroll or window position register from the PPU.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component.
 * @param   reg     The register to read (SCY, SCX, WY, WX).
 * 
 * @return  The current value of the specified register.
 */
TM32CORE_API uint8_t TM32Core_ReadScrollRegister (
    const TM32Core_PPU*   ppu,
    uint32_t        reg
);

/**
 * @brief   Writes to a scroll or window position register of the PPU.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component.
 * @param   reg     The register to write (SCY, SCX, WY, WX).
 * @param   value   The value to write to the register.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_WriteScrollRegister (
    TM32Core_PPU*   ppu,
    uint32_t        reg,
    uint8_t         value
);

/**
 * @brief   Reads a palette register from the PPU.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component.
 * @param   reg     The register to read (BGP, OBP0, OBP1).
 * 
 * @return  The current value of the specified palette register.
 */
TM32CORE_API uint8_t TM32Core_ReadPaletteRegister (
    const TM32Core_PPU*   ppu,
    uint32_t        reg
);

/**
 * @brief   Writes to a palette register of the PPU.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component.
 * @param   reg     The register to write (BGP, OBP0, OBP1).
 * @param   value   The value to write to the register.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_WritePaletteRegister (
    TM32Core_PPU*   ppu,
    uint32_t        reg,
    uint8_t         value
);

/**
 * @brief   Reads the VBK (VRAM Bank Select) register from the PPU.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component.
 * 
 * @return  The current value of the VBK register.
 */
TM32CORE_API uint8_t TM32Core_ReadVBK (
    const TM32Core_PPU*   ppu
);

/**
 * @brief   Writes to the VBK (VRAM Bank Select) register of the PPU.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component.
 * @param   value   The value to write to the VBK register.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_WriteVBK (
    TM32Core_PPU*   ppu,
    uint8_t         value
);

/**
 * @brief   Reads a CGB palette index/data register from the PPU.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component.
 * @param   reg     The register to read (BGPI, BGPD, OBPI, OBPD).
 * 
 * @return  The current value of the specified register.
 */
TM32CORE_API uint8_t TM32Core_ReadCGBPaletteRegister (
    const TM32Core_PPU*   ppu,
    uint32_t        reg
);

/**
 * @brief   Writes to a CGB palette index/data register of the PPU.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component.
 * @param   reg     The register to write (BGPI, BGPD, OBPI, OBPD).
 * @param   value   The value to write to the register.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_WriteCGBPaletteRegister (
    TM32Core_PPU*   ppu,
    uint32_t        reg,
    uint8_t         value
);

/**
 * @brief   Reads the OPRI (Object Priority Mode) register from the PPU.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component.
 * 
 * @return  The current value of the OPRI register.
 */
TM32CORE_API uint8_t TM32Core_ReadOPRI (
    const TM32Core_PPU*   ppu
);

/**
 * @brief   Writes to the OPRI (Object Priority Mode) register of the PPU.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component.
 * @param   value   The value to write to the OPRI register.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_WriteOPRI (
    TM32Core_PPU*   ppu,
    uint8_t         value
);

/**
 * @brief   Reads the GRPM (Graphics Mode) register from the PPU.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component.
 * 
 * @return  The current value of the GRPM register. 
 *          Bit 0: Graphics mode (0 = DMG mode, 1 = CGB mode).
 *          Bits 7-1: Always read as 1.
 */
TM32CORE_API uint8_t TM32Core_ReadGRPM (
    const TM32Core_PPU*   ppu
);

/**
 * @brief   Writes to the GRPM (Graphics Mode) register of the PPU.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component.
 * @param   value   The value to write to the GRPM register.
 *                  Only bit 0 is writable (0 = DMG mode, 1 = CGB mode).
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_WriteGRPM (
    TM32Core_PPU*   ppu,
    uint8_t         value
);

/**
 * @brief   Checks if a frame is ready for display.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component.
 * 
 * @return  `true` if a frame is ready; `false` otherwise.
 */
TM32CORE_API bool TM32Core_IsFrameReady (
    const TM32Core_PPU*   ppu
);

/**
 * @brief   Acknowledges that the current frame has been processed.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_AcknowledgeFrame (
    TM32Core_PPU*   ppu
);

/**
 * @brief   Checks if the PPU is in CGB (Color Game Boy) mode.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component.
 * 
 * @return  `true` if in CGB mode; `false` if in DMG mode or on error.
 */
TM32CORE_API bool TM32Core_IsCGBMode (
    const TM32Core_PPU*   ppu
);

/* VRAM and OAM Access Functions *********************************************/

/**
 * @brief   Reads a byte from Video RAM (VRAM) at the specified offset.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component.
 * @param   offset  The offset within VRAM (0x0000-0x1FFF for 8KB).
 * @param   data    A pointer to store the read data.
 * 
 * @return  `true` on success; `false` on error. When VRAM is inaccessible
 *          (during pixel transfer), returns `true` but sets data to 0xFF.
 */
TM32CORE_API bool TM32Core_ReadVRAM (
    const TM32Core_PPU*   ppu,
    uint16_t        offset,
    uint8_t*        data
);

/**
 * @brief   Writes a byte to Video RAM (VRAM) at the specified offset.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component.
 * @param   offset  The offset within VRAM (0x0000-0x1FFF for 8KB).
 * @param   data    The data byte to write.
 * 
 * @return  `true` on success; `false` on error. When VRAM is inaccessible
 *          (during pixel transfer), returns `true` but write has no effect.
 */
TM32CORE_API bool TM32Core_WriteVRAM (
    TM32Core_PPU*   ppu,
    uint16_t        offset,
    uint8_t         data
);

/**
 * @brief   Reads a byte from Object Attribute Memory (OAM) at the specified offset.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component.
 * @param   offset  The offset within OAM (0x00-0x9F for 160 bytes).
 * @param   data    A pointer to store the read data.
 * 
 * @return  `true` on success; `false` if OAM is inaccessible or on error.
 */
TM32CORE_API bool TM32Core_ReadOAM (
    const TM32Core_PPU*   ppu,
    uint8_t         offset,
    uint8_t*        data
);

/**
 * @brief   Writes a byte to Object Attribute Memory (OAM) at the specified offset.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component.
 * @param   offset  The offset within OAM (0x00-0x9F for 160 bytes).
 * @param   data    The data byte to write.
 * 
 * @return  `true` on success; `false` on error. When OAM is inaccessible
 *          (during pixel transfer or OAM search), returns `true` but write
 *          has no effect.
 */
TM32CORE_API bool TM32Core_WriteOAM (
    TM32Core_PPU*   ppu,
    uint8_t         offset,
    uint8_t         data
);

/* DMA Functions **************************************************************/

/**
 * @brief   Writes to an OAM DMA source address register.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component.
 * @param   byteIndex The byte index (0-3, where 0 is LSB and 3 is MSB).
 * @param   value   The value to write to the specified byte.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_WriteODMASource (
    TM32Core_PPU*   ppu,
    uint8_t         byteIndex,
    uint8_t         value
);

/**
 * @brief   Reads from the ODMA (OAM DMA Start) register.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component.
 * 
 * @return  The current value of the ODMA register.
 */
TM32CORE_API uint8_t TM32Core_ReadODMA (
    const TM32Core_PPU*   ppu
);

/**
 * @brief   Writes to the ODMA (OAM DMA Start) register to initiate OAM DMA transfer.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component.
 * @param   value   The value to write (ignored, any write initiates transfer).
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_WriteODMA (
    TM32Core_PPU*   ppu,
    uint8_t         value
);

/**
 * @brief   Writes to an HDMA source address register.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component.
 * @param   byteIndex The byte index (0-3, where 0 is LSB and 3 is MSB).
 * @param   value   The value to write to the specified byte.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_WriteHDMASource (
    TM32Core_PPU*   ppu,
    uint8_t         byteIndex,
    uint8_t         value
);

/**
 * @brief   Writes to an HDMA destination address register.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component.
 * @param   byteIndex The byte index (0-3, where 0 is LSB and 3 is MSB).
 * @param   value   The value to write to the specified byte.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_WriteHDMADestination (
    TM32Core_PPU*   ppu,
    uint8_t         byteIndex,
    uint8_t         value
);

/**
 * @brief   Reads from the HDMA (HDMA Start/Control) register.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component.
 * 
 * @return  The current value of the HDMA register.
 */
TM32CORE_API uint8_t TM32Core_ReadHDMA (
    const TM32Core_PPU*   ppu
);

/**
 * @brief   Writes to the HDMA (HDMA Start/Control) register.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component.
 * @param   value   The control value to write.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_WriteHDMA (
    TM32Core_PPU*   ppu,
    uint8_t         value
);

#endif // TM32CORE_PPU_H
