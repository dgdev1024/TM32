/**
 * @file    tm32core_ppu.c
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025-08-25
 * 
 * @brief   Implementation of the TM32 Core's Pixel Processing Unit (PPU)
 *          component structure and functions.
 */

/* Include Files **************************************************************/

#include <tm32core_bus.h>
#include <tm32core_ppu.h>
#include <tm32cpu_processor.h>

/* Constants ******************************************************************/

#define TM32CORE_PPU_SCREEN_WIDTH           160
#define TM32CORE_PPU_SCREEN_HEIGHT          144
#define TM32CORE_PPU_TOTAL_LINES            154
#define TM32CORE_PPU_OAM_CYCLES             80
#define TM32CORE_PPU_DRAWING_CYCLES         172
#define TM32CORE_PPU_HBLANK_CYCLES          204
#define TM32CORE_PPU_LINE_CYCLES            456
#define TM32CORE_PPU_FRAME_CYCLES           70224

/* Private Structures *********************************************************/

struct TM32Core_PPU
{
    TM32Core_Bus*       bus;                /** @brief The TM32 Bus to which this PPU is connected. */
    
    // VBlank Callback
    TM32Core_VBlankCallback vblankCallback; /** @brief VBlank callback function. */
    void*               vblankUserData;     /** @brief User data for VBlank callback. */
    
    // PPU State
    uint32_t            cycles;             /** @brief Current cycle count within the frame. */
    uint32_t            lineCycles;         /** @brief Current cycle count within the current line. */
    TM32Core_PPUMode    mode;               /** @brief Current PPU drawing mode. */
    bool                frameReady;         /** @brief Flag indicating a frame is ready for display. */
    
    // LCD Control Register (LCDC) - Port 0x40
    union
    {
        struct
        {
            uint8_t         bgWindowEnable      : 1;    /** @brief Background and window enable/priority */
            uint8_t         objEnable           : 1;    /** @brief Object enable */
            uint8_t         objSize             : 1;    /** @brief Object size (0=8x8, 1=8x16) */
            uint8_t         bgTileMapArea       : 1;    /** @brief Background tile map area */
            uint8_t         bgWindowTileDataArea: 1;    /** @brief Background and window tile data area */
            uint8_t         windowEnable        : 1;    /** @brief Window enable */
            uint8_t         windowTileMapArea   : 1;    /** @brief Window tile map area */
            uint8_t         lcdEnable           : 1;    /** @brief LCD and PPU enable */
        };
        
        uint8_t lcdc;
    };
    
    // LCD Status Register (STAT) - Port 0x41
    union
    {
        struct
        {
            uint8_t         modeBits        : 2;    /** @brief Current PPU mode (read-only) */
            uint8_t         lycEqualsLy     : 1;    /** @brief LYC=LY flag (read-only) */
            uint8_t         hBlankInterrupt : 1;    /** @brief HBlank interrupt enable */
            uint8_t         vBlankInterrupt : 1;    /** @brief VBlank interrupt enable */
            uint8_t         oamInterrupt    : 1;    /** @brief OAM interrupt enable */
            uint8_t         lycInterrupt    : 1;    /** @brief LYC=LY interrupt enable */
            uint8_t                         : 1;    /** @brief Unused bit */
        };
        
        uint8_t stat;
    };
    
    // Scroll Registers
    uint8_t             scrollY;            /** @brief SCY - Background Y scroll position */
    uint8_t             scrollX;            /** @brief SCX - Background X scroll position */
    uint8_t             currentLine;        /** @brief LY - Current scanline */
    uint8_t             lineCompare;        /** @brief LYC - Scanline compare value */
    uint8_t             windowY;            /** @brief WY - Window Y position */
    uint8_t             windowX;            /** @brief WX - Window X position */
    
    // DMG Palette Registers
    uint8_t             bgPalette;          /** @brief BGP - Background palette */
    uint8_t             objPalette0;        /** @brief OBP0 - Object palette 0 */
    uint8_t             objPalette1;        /** @brief OBP1 - Object palette 1 */
    
    // CGB VRAM Bank Select - Port 0x4F
    union
    {
        struct
        {
            uint8_t         vramBank    : 1;    /** @brief VRAM bank select */
            uint8_t                     : 7;    /** @brief Unused bits */
        };
        
        uint8_t vbk;
    };
    
    // CGB Background Palette Index - Port 0x68
    union
    {
        struct
        {
            uint8_t         bgPaletteIndex      : 6;    /** @brief Palette index (0-63) */
            uint8_t                             : 1;    /** @brief Unused bit */
            uint8_t         bgPaletteAutoIncrement : 1; /** @brief Auto-increment after write */
        };
        
        uint8_t bgpi;
    };
    
    // CGB Object Palette Index - Port 0x6A
    union
    {
        struct
        {
            uint8_t         objPaletteIndex     : 6;    /** @brief Palette index (0-63) */
            uint8_t                             : 1;    /** @brief Unused bit */
            uint8_t         objPaletteAutoIncrement : 1; /** @brief Auto-increment after write */
        };
        
        uint8_t obpi;
    };
    
    // CGB Object Priority Mode - Port 0x6C
    union
    {
        struct
        {
            uint8_t         objPriorityMode : 1;    /** @brief Object priority mode */
            uint8_t                         : 7;    /** @brief Unused bits */
        };
        
        uint8_t opri;
    };
    
    // TM32 Graphics Mode - Port 0xAD
    union
    {
        struct
        {
            uint8_t         cgbMode         : 1;    /** @brief CGB mode (0 = DMG, 1 = CGB) */
            uint8_t                         : 7;    /** @brief Unused bits */
        };
        
        uint8_t grpm;
    };
    
    // CGB Palette Data
    uint8_t             bgPaletteData[64];  /** @brief Background palette data (32 colors * 2 bytes) */
    uint8_t             objPaletteData[64]; /** @brief Object palette data (32 colors * 2 bytes) */
    
    // Video Memory
    uint8_t             vram[2][8192];      /** @brief Video RAM - 2 banks of 8KB each */
    uint8_t             oam[160];           /** @brief Object Attribute Memory - 160 bytes */
    
    // OAM DMA State
    uint32_t            odmaSource;         /** @brief OAM DMA source address */
    bool                odmaActive;         /** @brief OAM DMA transfer active flag */
    uint8_t             odmaProgress;       /** @brief OAM DMA transfer progress (0-160) */
    
    // HDMA State
    uint32_t            hdmaSource;         /** @brief HDMA source address */
    uint32_t            hdmaDestination;    /** @brief HDMA destination address */
    union
    {
        struct
        {
            uint8_t         transferLength  : 1;    /** @brief Transfer length (0=16 bytes, 1=32 bytes) */
            uint8_t                         : 6;    /** @brief Unused bits */
            uint8_t         transferMode    : 1;    /** @brief Transfer mode (0=General Purpose, 1=HBlank) */
        };
        
        uint8_t hdmaControl;
    };
    bool                hdmaActive;         /** @brief HDMA transfer active flag */
    uint16_t            hdmaRemaining;      /** @brief HDMA remaining bytes to transfer */
    bool                hdmaHBlankDone;     /** @brief Flag to prevent multiple transfers per HBlank */
};

/* Private Function Prototypes ************************************************/

/**
 * @brief   Updates the PPU mode and triggers appropriate interrupts.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component.
 * @param   newMode The new PPU mode to set.
 * 
 * @return  `true` on success; `false` on failure.
 */
static bool TM32Core_SetPPUMode (
    TM32Core_PPU*       ppu,
    TM32Core_PPUMode    newMode
);

/**
 * @brief   Checks and handles LYC=LY comparison.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component.
 * 
 * @return  `true` on success; `false` on failure.
 */
static bool TM32Core_CheckLYCComparison (
    TM32Core_PPU*   ppu
);

/**
 * @brief   Triggers a PPU-related interrupt.
 * 
 * @param   ppu             A pointer to the TM32 Core PPU component.
 * @param   interruptVector The interrupt vector to trigger.
 * 
 * @return  `true` on success; `false` on failure.
 */
static bool TM32Core_TriggerPPUInterrupt (
    TM32Core_PPU*   ppu,
    uint32_t        interruptVector
);

/**
 * @brief   Processes DMA transfers during PPU clock cycles.
 * 
 * @param   ppu     A pointer to the TM32 Core PPU component.
 * 
 * @return  `true` on success; `false` on failure.
 */
static bool TM32Core_ProcessDMATransfers (
    TM32Core_PPU*   ppu
);

/* Private Functions - PPU Management *****************************************/

static bool TM32Core_SetPPUMode (
    TM32Core_PPU*       ppu,
    TM32Core_PPUMode    newMode
)
{
    TM32Core_ReturnValueIfNull(ppu, false);
    
    if (ppu->mode == newMode)
    {
        return true;
    }
    
    ppu->mode = newMode;
    ppu->stat &= 0xFC; // Clear mode bits
    ppu->stat |= (newMode & 0x03); // Set new mode bits
    
    // Trigger appropriate interrupts based on STAT register settings
    switch (newMode)
    {
    case TM32CORE_PPU_MODE_HBLANK:
        if (ppu->hBlankInterrupt)
        {
            return TM32Core_TriggerPPUInterrupt(ppu, TM32CORE_INT_LCD_STAT);
        }
        break;
        
    case TM32CORE_PPU_MODE_VBLANK:
        // VBlank always triggers VBlank interrupt
        if (!TM32Core_TriggerPPUInterrupt(ppu, TM32CORE_INT_VBLANK))
        {
            return false;
        }
        
        // Also trigger LCD STAT interrupt if enabled
        if (ppu->vBlankInterrupt)
        {
            return TM32Core_TriggerPPUInterrupt(ppu, TM32CORE_INT_LCD_STAT);
        }
        break;
        
    case TM32CORE_PPU_MODE_OAM:
        if (ppu->oamInterrupt)
        {
            return TM32Core_TriggerPPUInterrupt(ppu, TM32CORE_INT_LCD_STAT);
        }
        break;
        
    case TM32CORE_PPU_MODE_DRAWING:
        // Drawing mode doesn't trigger interrupts
        break;
    }
    
    return true;
}

static bool TM32Core_CheckLYCComparison (
    TM32Core_PPU*   ppu
)
{
    TM32Core_ReturnValueIfNull(ppu, false);
    
    bool lycMatch = (ppu->currentLine == ppu->lineCompare);
    
    if (lycMatch != ppu->lycEqualsLy)
    {
        ppu->lycEqualsLy = lycMatch ? 1 : 0;
        
        // Trigger LYC interrupt if enabled and LYC=LY becomes true
        if (lycMatch && ppu->lycInterrupt)
        {
            return TM32Core_TriggerPPUInterrupt(ppu, TM32CORE_INT_LCD_STAT);
        }
    }
    
    return true;
}

static bool TM32Core_TriggerPPUInterrupt (
    TM32Core_PPU*   ppu,
    uint32_t        interruptVector
)
{
    TM32Core_ReturnValueIfNull(ppu, false);
    TM32Core_ReturnValueIfNull(ppu->bus, false);
    
    return TM32CPU_RequestInterrupt(TM32Core_GetBusCPU(ppu->bus), interruptVector);
}

static bool TM32Core_ProcessDMATransfers (
    TM32Core_PPU*   ppu
)
{
    TM32Core_ReturnValueIfNull(ppu, false);
    TM32Core_ReturnValueIfNull(ppu->bus, false);
    
    // Process OAM DMA if active
    if (ppu->odmaActive)
    {
        // OAM DMA transfers 1 byte per PPU cycle
        if (ppu->odmaProgress < 160)
        {
            uint32_t sourceAddr = ppu->odmaSource + ppu->odmaProgress;
            uint8_t data;
            
            // Read from source address
            if (TM32Core_ReadBusData(ppu->bus, sourceAddr, &data))
            {
                // Write directly to PPU's OAM array
                ppu->oam[ppu->odmaProgress] = data;
                ppu->odmaProgress++;
            }
            else
            {
                TM32Core_LogError("OAM DMA read failed at address 0x%08X", sourceAddr);
                ppu->odmaActive = false;
                return false;
            }
            
            // Check if transfer is complete
            if (ppu->odmaProgress >= 160)
            {
                ppu->odmaActive = false;
                TM32Core_LogInfo("OAM DMA transfer completed");
            }
        }
    }
    
    // Process HDMA transfers
    if (ppu->hdmaActive)
    {
        if (ppu->transferMode == 0)
        {
            // General Purpose DMA - immediate transfer
            // Transfer all remaining bytes at once
            if (ppu->hdmaRemaining > 0)
            {
                for (uint16_t i = 0; i < ppu->hdmaRemaining; i++)
                {
                    uint32_t sourceAddr = ppu->hdmaSource + i;
                    uint32_t destAddr = ppu->hdmaDestination + i;
                    uint8_t data;
                    
                    if (TM32Core_ReadBusData(ppu->bus, sourceAddr, &data))
                    {
                        if (!TM32Core_WriteBusData(ppu->bus, destAddr, data))
                        {
                            TM32Core_LogError("HDMA write failed at address 0x%08X", destAddr);
                            ppu->hdmaActive = false;
                            return false;
                        }
                    }
                    else
                    {
                        TM32Core_LogError("HDMA read failed at address 0x%08X", sourceAddr);
                        ppu->hdmaActive = false;
                        return false;
                    }
                }
                
                // Update source and destination addresses
                ppu->hdmaSource += ppu->hdmaRemaining;
                ppu->hdmaDestination += ppu->hdmaRemaining;
                ppu->hdmaRemaining = 0;
                ppu->hdmaActive = false;
                
                TM32Core_LogInfo("General Purpose DMA transfer completed");
            }
        }
        else
        {
            // HBlank DMA - transfer during HBlank periods
            if (ppu->mode == TM32CORE_PPU_MODE_HBLANK && !ppu->hdmaHBlankDone)
            {
                uint16_t bytesToTransfer = ppu->transferLength ? 32 : 16;
                
                for (uint16_t i = 0; i < bytesToTransfer; i++)
                {
                    uint32_t sourceAddr = ppu->hdmaSource + i;
                    uint32_t destAddr = ppu->hdmaDestination + i;
                    uint8_t data;
                    
                    if (TM32Core_ReadBusData(ppu->bus, sourceAddr, &data))
                    {
                        if (!TM32Core_WriteBusData(ppu->bus, destAddr, data))
                        {
                            TM32Core_LogError("HDMA HBlank write failed at address 0x%08X", destAddr);
                            ppu->hdmaActive = false;
                            return false;
                        }
                    }
                    else
                    {
                        TM32Core_LogError("HDMA HBlank read failed at address 0x%08X", sourceAddr);
                        ppu->hdmaActive = false;
                        return false;
                    }
                }
                
                // Update source and destination addresses for next HBlank
                ppu->hdmaSource += bytesToTransfer;
                ppu->hdmaDestination += bytesToTransfer;
                ppu->hdmaHBlankDone = true;  // Mark this HBlank as processed
                
                // Continue until manually stopped
            }
            
            // Reset HBlank done flag when leaving HBlank mode
            if (ppu->mode != TM32CORE_PPU_MODE_HBLANK)
            {
                ppu->hdmaHBlankDone = false;
            }
        }
    }
    
    return true;
}

/* Public Functions ***********************************************************/

TM32Core_PPU* TM32Core_CreatePPU (
    TM32Core_Bus*   bus
)
{
    TM32Core_ReturnValueIfNull(bus, NULL);

    TM32Core_PPU* ppu = TM32Core_CreateZero(1, TM32Core_PPU);
    if (ppu == NULL)
    {
        TM32Core_LogErrno("Could not allocate the TM32 Core PPU component");
        return NULL;
    }

    ppu->bus = bus;

    if (!TM32Core_InitializePPU(ppu))
    {
        TM32Core_LogError("Could not initialize the TM32 Core PPU component");
        TM32Core_Destroy(ppu);
        return NULL;
    }

    return ppu;
}

bool TM32Core_InitializePPU (
    TM32Core_PPU*   ppu
)
{
    TM32Core_ReturnValueIfNull(ppu, false);
    
    // Initialize VBlank callback
    ppu->vblankCallback = NULL;
    ppu->vblankUserData = NULL;
    
    // Reset all PPU state
    ppu->cycles = 0;
    ppu->lineCycles = 0;
    ppu->mode = TM32CORE_PPU_MODE_OAM;
    ppu->frameReady = false;
    
    // Initialize registers to Game Boy power-on state
    ppu->lcdc = 0x91;           // LCD enabled, BG enabled, objects enabled
    ppu->stat = 0x85;           // Mode 1 (VBlank), LYC interrupt enabled
    ppu->scrollY = 0x00;
    ppu->scrollX = 0x00;
    ppu->currentLine = 0x00;
    ppu->lineCompare = 0x00;
    ppu->windowY = 0x00;
    ppu->windowX = 0x00;
    ppu->bgPalette = 0xFC;      // White, light gray, dark gray, black
    ppu->objPalette0 = 0xFF;
    ppu->objPalette1 = 0xFF;
    ppu->vbk = 0x00;            // VRAM bank 0
    ppu->bgpi = 0x00;
    ppu->obpi = 0x00;
    ppu->opri = 0x00;
    ppu->grpm = 0x00;           // Default graphics mode
    
    // Clear palette data
    memset(ppu->bgPaletteData, 0, sizeof(ppu->bgPaletteData));
    memset(ppu->objPaletteData, 0, sizeof(ppu->objPaletteData));
    
    // Clear VRAM and OAM
    memset(ppu->vram, 0, sizeof(ppu->vram));
    memset(ppu->oam, 0, sizeof(ppu->oam));
    
    // Initialize DMA state
    ppu->odmaSource = 0x00000000;
    ppu->odmaActive = false;
    ppu->odmaProgress = 0;
    ppu->hdmaSource = 0x00000000;
    ppu->hdmaDestination = 0x00000000;
    ppu->hdmaControl = 0x00;
    ppu->hdmaActive = false;
    ppu->hdmaRemaining = 0;
    ppu->hdmaHBlankDone = false;
    
    // Set initial mode and check LYC comparison
    TM32Core_SetPPUMode(ppu, TM32CORE_PPU_MODE_OAM);
    TM32Core_CheckLYCComparison(ppu);
    
    return true;
}

bool TM32Core_ClockPPU (
    TM32Core_PPU*   ppu
)
{
    TM32Core_ReturnValueIfNull(ppu, false);
    
    // If LCD is disabled, do nothing
    if (!ppu->lcdEnable)
    {
        return true;
    }
    
    ppu->cycles++;
    ppu->lineCycles++;
    
    // Process DMA transfers
    if (!TM32Core_ProcessDMATransfers(ppu))
    {
        return false;
    }
    
    // Handle mode transitions based on current scanline and cycle count
    if (ppu->currentLine < TM32CORE_PPU_SCREEN_HEIGHT)
    {
        // Visible scanlines (0-143)
        if (ppu->lineCycles <= TM32CORE_PPU_OAM_CYCLES)
        {
            // OAM search period
            if (ppu->mode != TM32CORE_PPU_MODE_OAM)
            {
                if (!TM32Core_SetPPUMode(ppu, TM32CORE_PPU_MODE_OAM))
                {
                    return false;
                }
            }
        }
        else if (ppu->lineCycles <= TM32CORE_PPU_OAM_CYCLES + TM32CORE_PPU_DRAWING_CYCLES)
        {
            // Pixel transfer period
            if (ppu->mode != TM32CORE_PPU_MODE_DRAWING)
            {
                if (!TM32Core_SetPPUMode(ppu, TM32CORE_PPU_MODE_DRAWING))
                {
                    return false;
                }
            }
        }
        else
        {
            // HBlank period
            if (ppu->mode != TM32CORE_PPU_MODE_HBLANK)
            {
                if (!TM32Core_SetPPUMode(ppu, TM32CORE_PPU_MODE_HBLANK))
                {
                    return false;
                }
            }
        }
    }
    else
    {
        // VBlank period (lines 144-153)
        if (ppu->mode != TM32CORE_PPU_MODE_VBLANK)
        {
            if (!TM32Core_SetPPUMode(ppu, TM32CORE_PPU_MODE_VBLANK))
            {
                return false;
            }
            
            // Set frame ready flag when entering VBlank
            if (ppu->currentLine == TM32CORE_PPU_SCREEN_HEIGHT)
            {
                ppu->frameReady = true;
            }
            
            // Invoke VBlank callback if set
            if (ppu->vblankCallback != NULL)
            {
                if (!ppu->vblankCallback(ppu, ppu->vblankUserData))
                {
                    // Log callback error if needed - callback returned false
                    // For now, we'll continue execution
                }
            }
        }
    }
    
    // Check for end of scanline
    if (ppu->lineCycles >= TM32CORE_PPU_LINE_CYCLES)
    {
        ppu->lineCycles = 0;
        ppu->currentLine++;
        
        // Check for end of frame
        if (ppu->currentLine >= TM32CORE_PPU_TOTAL_LINES)
        {
            ppu->currentLine = 0;
            ppu->cycles = 0;
        }
        
        // Check LYC comparison after updating LY
        if (!TM32Core_CheckLYCComparison(ppu))
        {
            return false;
        }
    }
    
    return true;
}

void TM32Core_DestroyPPU (
    TM32Core_PPU*   ppu
)
{
    if (ppu != NULL)
    {
        TM32Core_Destroy(ppu);
    }
}

bool TM32Core_SetVBlankCallback (
    TM32Core_PPU*           ppu,
    TM32Core_VBlankCallback callback,
    void*                   userData
)
{
    if (ppu == NULL)
    {
        return false;
    }
    
    ppu->vblankCallback = callback;
    ppu->vblankUserData = userData;
    return true;
}

bool TM32Core_ClearVBlankCallback (
    TM32Core_PPU*   ppu
)
{
    if (ppu == NULL)
    {
        return false;
    }
    
    ppu->vblankCallback = NULL;
    ppu->vblankUserData = NULL;
    return true;
}

/* Register Access Functions **************************************************/

uint8_t TM32Core_ReadLCDC (
    const TM32Core_PPU*   ppu
)
{
    TM32Core_ReturnValueIfNull(ppu, 0x00);
    
    return ppu->lcdc;
}

bool TM32Core_WriteLCDC (
    TM32Core_PPU*   ppu,
    uint8_t         value
)
{
    TM32Core_ReturnValueIfNull(ppu, false);
    
    // If LCD is being disabled, reset PPU state
    if (ppu->lcdEnable && !(value & 0x80))
    {
        ppu->currentLine = 0;
        ppu->cycles = 0;
        ppu->lineCycles = 0;
        ppu->mode = TM32CORE_PPU_MODE_HBLANK;
        ppu->stat &= 0xFC; // Clear mode bits
    }
    
    ppu->lcdc = value;
    
    return true;
}

uint8_t TM32Core_ReadSTAT (
    const TM32Core_PPU*   ppu
)
{
    TM32Core_ReturnValueIfNull(ppu, 0x00);
    
    return ppu->stat | 0x80; // Bit 7 is always set
}

bool TM32Core_WriteSTAT (
    TM32Core_PPU*   ppu,
    uint8_t         value
)
{
    TM32Core_ReturnValueIfNull(ppu, false);
    
    // Only bits 6-3 are writable
    ppu->stat = (ppu->stat & 0x07) | (value & 0x78);
    
    return true;
}

uint8_t TM32Core_ReadLY (
    const TM32Core_PPU*   ppu
)
{
    TM32Core_ReturnValueIfNull(ppu, 0x00);
    
    return ppu->currentLine;
}

uint8_t TM32Core_ReadLYC (
    const TM32Core_PPU*   ppu
)
{
    TM32Core_ReturnValueIfNull(ppu, 0x00);
    
    return ppu->lineCompare;
}

bool TM32Core_WriteLYC (
    TM32Core_PPU*   ppu,
    uint8_t         value
)
{
    TM32Core_ReturnValueIfNull(ppu, false);
    
    ppu->lineCompare = value;
    
    // Check LYC comparison immediately after writing
    return TM32Core_CheckLYCComparison(ppu);
}

uint8_t TM32Core_ReadScrollRegister (
    const TM32Core_PPU*   ppu,
    uint32_t        reg
)
{
    TM32Core_ReturnValueIfNull(ppu, 0x00);
    
    switch (reg)
    {
    case TM32CORE_PORT_SCY:
        return ppu->scrollY;
    case TM32CORE_PORT_SCX:
        return ppu->scrollX;
    case TM32CORE_PORT_WY:
        return ppu->windowY;
    case TM32CORE_PORT_WX:
        return ppu->windowX;
    default:
        TM32Core_LogError("Invalid scroll register: 0x%08X", reg);
        return 0x00;
    }
}

bool TM32Core_WriteScrollRegister (
    TM32Core_PPU*   ppu,
    uint32_t        reg,
    uint8_t         value
)
{
    TM32Core_ReturnValueIfNull(ppu, false);
    
    switch (reg)
    {
    case TM32CORE_PORT_SCY:
        ppu->scrollY = value;
        return true;
    case TM32CORE_PORT_SCX:
        ppu->scrollX = value;
        return true;
    case TM32CORE_PORT_WY:
        ppu->windowY = value;
        return true;
    case TM32CORE_PORT_WX:
        ppu->windowX = value;
        return true;
    default:
        TM32Core_LogError("Invalid scroll register: 0x%08X", reg);
        return false;
    }
}

uint8_t TM32Core_ReadPaletteRegister (
    const TM32Core_PPU*   ppu,
    uint32_t        reg
)
{
    TM32Core_ReturnValueIfNull(ppu, 0x00);
    
    switch (reg)
    {
    case TM32CORE_PORT_BGP:
        return ppu->bgPalette;
    case TM32CORE_PORT_OBP0:
        return ppu->objPalette0;
    case TM32CORE_PORT_OBP1:
        return ppu->objPalette1;
    default:
        TM32Core_LogError("Invalid palette register: 0x%08X", reg);
        return 0x00;
    }
}

bool TM32Core_WritePaletteRegister (
    TM32Core_PPU*   ppu,
    uint32_t        reg,
    uint8_t         value
)
{
    TM32Core_ReturnValueIfNull(ppu, false);
    
    switch (reg)
    {
    case TM32CORE_PORT_BGP:
        ppu->bgPalette = value;
        return true;
    case TM32CORE_PORT_OBP0:
        ppu->objPalette0 = value;
        return true;
    case TM32CORE_PORT_OBP1:
        ppu->objPalette1 = value;
        return true;
    default:
        TM32Core_LogError("Invalid palette register: 0x%08X", reg);
        return false;
    }
}

uint8_t TM32Core_ReadVBK (
    const TM32Core_PPU*   ppu
)
{
    TM32Core_ReturnValueIfNull(ppu, 0x00);
    
    return ppu->vbk | 0xFE; // Upper 7 bits are always set
}

bool TM32Core_WriteVBK (
    TM32Core_PPU*   ppu,
    uint8_t         value
)
{
    TM32Core_ReturnValueIfNull(ppu, false);
    
    ppu->vbk = value & 0x01; // Only bit 0 is writable
    
    return true;
}

uint8_t TM32Core_ReadCGBPaletteRegister (
    const TM32Core_PPU*   ppu,
    uint32_t        reg
)
{
    TM32Core_ReturnValueIfNull(ppu, 0x00);
    
    switch (reg)
    {
    case TM32CORE_PORT_BGPI:
        return ppu->bgpi;
    case TM32CORE_PORT_BGPD:
        if (ppu->bgPaletteIndex < 64)
        {
            return ppu->bgPaletteData[ppu->bgPaletteIndex];
        }
        return 0x00;
    case TM32CORE_PORT_OBPI:
        return ppu->obpi;
    case TM32CORE_PORT_OBPD:
        if (ppu->objPaletteIndex < 64)
        {
            return ppu->objPaletteData[ppu->objPaletteIndex];
        }
        return 0x00;
    default:
        TM32Core_LogError("Invalid CGB palette register: 0x%08X", reg);
        return 0x00;
    }
}

bool TM32Core_WriteCGBPaletteRegister (
    TM32Core_PPU*   ppu,
    uint32_t        reg,
    uint8_t         value
)
{
    TM32Core_ReturnValueIfNull(ppu, false);
    
    switch (reg)
    {
    case TM32CORE_PORT_BGPI:
        ppu->bgpi = value;
        return true;
        
    case TM32CORE_PORT_BGPD:
        if (ppu->bgPaletteIndex < 64)
        {
            ppu->bgPaletteData[ppu->bgPaletteIndex] = value;
            
            // Auto-increment if enabled
            if (ppu->bgPaletteAutoIncrement)
            {
                ppu->bgPaletteIndex = (ppu->bgPaletteIndex + 1) & 0x3F;
            }
        }
        return true;
        
    case TM32CORE_PORT_OBPI:
        ppu->obpi = value;
        return true;
        
    case TM32CORE_PORT_OBPD:
        if (ppu->objPaletteIndex < 64)
        {
            ppu->objPaletteData[ppu->objPaletteIndex] = value;
            
            // Auto-increment if enabled
            if (ppu->objPaletteAutoIncrement)
            {
                ppu->objPaletteIndex = (ppu->objPaletteIndex + 1) & 0x3F;
            }
        }
        return true;
        
    default:
        TM32Core_LogError("Invalid CGB palette register: 0x%08X", reg);
        return false;
    }
}

uint8_t TM32Core_ReadOPRI (
    const TM32Core_PPU*   ppu
)
{
    TM32Core_ReturnValueIfNull(ppu, 0x00);
    
    return ppu->opri | 0xFE; // Upper 7 bits are always set
}

bool TM32Core_WriteOPRI (
    TM32Core_PPU*   ppu,
    uint8_t         value
)
{
    TM32Core_ReturnValueIfNull(ppu, false);
    
    ppu->opri = value & 0x01; // Only bit 0 is writable
    
    return true;
}

uint8_t TM32Core_ReadGRPM (
    const TM32Core_PPU*   ppu
)
{
    TM32Core_ReturnValueIfNull(ppu, 0x00);
    
    return ppu->grpm | 0xFE; // Upper 7 bits are always set
}

bool TM32Core_WriteGRPM (
    TM32Core_PPU*   ppu,
    uint8_t         value
)
{
    TM32Core_ReturnValueIfNull(ppu, false);
    
    ppu->grpm = value & 0x01; // Only bit 0 is writable (0 = DMG, 1 = CGB)
    
    return true;
}

/* Frame Management Functions *************************************************/

bool TM32Core_IsFrameReady (
    const TM32Core_PPU*   ppu
)
{
    TM32Core_ReturnValueIfNull(ppu, false);
    
    return ppu->frameReady;
}

bool TM32Core_AcknowledgeFrame (
    TM32Core_PPU*   ppu
)
{
    TM32Core_ReturnValueIfNull(ppu, false);
    
    ppu->frameReady = false;
    
    return true;
}

bool TM32Core_IsCGBMode (
    const TM32Core_PPU*   ppu
)
{
    TM32Core_ReturnValueIfNull(ppu, false);
    
    return ppu->cgbMode == 1;
}

/* DMA Functions **************************************************************/

bool TM32Core_WriteODMASource (
    TM32Core_PPU*   ppu,
    uint8_t         byteIndex,
    uint8_t         value
)
{
    TM32Core_ReturnValueIfNull(ppu, false);
    
    if (byteIndex > 3)
    {
        TM32Core_LogError("Invalid OAM DMA source byte index: %u", byteIndex);
        return false;
    }
    
    // Store the byte in the appropriate position of the source address
    uint32_t mask = 0xFF << (byteIndex * 8);
    uint32_t shiftedValue = (uint32_t)value << (byteIndex * 8);
    
    ppu->odmaSource = (ppu->odmaSource & ~mask) | shiftedValue;
    
    return true;
}

uint8_t TM32Core_ReadODMA (
    const TM32Core_PPU*   ppu
)
{
    TM32Core_ReturnValueIfNull(ppu, 0xFF);
    
    // ODMA register returns 0xFF when no transfer is active, 
    // or the transfer progress when active
    if (ppu->odmaActive)
    {
        return ppu->odmaProgress;
    }
    
    return 0xFF;
}

bool TM32Core_WriteODMA (
    TM32Core_PPU*   ppu,
    uint8_t         value
)
{
    TM32Core_ReturnValueIfNull(ppu, false);
    
    // Any write to ODMA starts the OAM DMA transfer
    // The value written is ignored
    (void)value;
    
    // Don't start a new transfer if one is already active
    if (ppu->odmaActive)
    {
        return true;
    }
    
    // Validate source address is within accessible memory ranges
    // According to specification, OAM DMA can read from any memory location
    // but during transfer, only high RAM is accessible to CPU
    
    // Initialize OAM DMA transfer
    ppu->odmaActive = true;
    ppu->odmaProgress = 0;
    
    TM32Core_LogInfo("OAM DMA transfer started from address 0x%08X", ppu->odmaSource);
    
    return true;
}

bool TM32Core_WriteHDMASource (
    TM32Core_PPU*   ppu,
    uint8_t         byteIndex,
    uint8_t         value
)
{
    TM32Core_ReturnValueIfNull(ppu, false);
    
    if (byteIndex > 3)
    {
        TM32Core_LogError("Invalid HDMA source byte index: %u", byteIndex);
        return false;
    }
    
    // Store the byte in the appropriate position of the source address
    uint32_t mask = 0xFF << (byteIndex * 8);
    uint32_t shiftedValue = (uint32_t)value << (byteIndex * 8);
    
    ppu->hdmaSource = (ppu->hdmaSource & ~mask) | shiftedValue;
    
    return true;
}

bool TM32Core_WriteHDMADestination (
    TM32Core_PPU*   ppu,
    uint8_t         byteIndex,
    uint8_t         value
)
{
    TM32Core_ReturnValueIfNull(ppu, false);
    
    if (byteIndex > 3)
    {
        TM32Core_LogError("Invalid HDMA destination byte index: %u", byteIndex);
        return false;
    }
    
    // Store the byte in the appropriate position of the destination address
    uint32_t mask = 0xFF << (byteIndex * 8);
    uint32_t shiftedValue = (uint32_t)value << (byteIndex * 8);
    
    ppu->hdmaDestination = (ppu->hdmaDestination & ~mask) | shiftedValue;
    
    return true;
}

uint8_t TM32Core_ReadHDMA (
    const TM32Core_PPU*   ppu
)
{
    TM32Core_ReturnValueIfNull(ppu, 0xFF);
    
    // Return the current HDMA control register value
    if (ppu->hdmaActive && ppu->transferMode == 1)
    {
        // HBlank DMA active - return 0x80 | transferLength
        return 0x80 | ppu->transferLength;
    }
    else if (ppu->hdmaActive && ppu->transferMode == 0)
    {
        // General Purpose DMA active - return transferLength
        return ppu->transferLength;
    }
    
    // No transfer active
    return 0xFF;
}

bool TM32Core_WriteHDMA (
    TM32Core_PPU*   ppu,
    uint8_t         value
)
{
    TM32Core_ReturnValueIfNull(ppu, false);
    
    // Writing 0x00 during HBlank DMA stops the transfer
    if (value == 0x00 && ppu->hdmaActive && ppu->transferMode == 1)
    {
        ppu->hdmaActive = false;
        ppu->hdmaRemaining = 0;
        ppu->hdmaHBlankDone = false;
        TM32Core_LogInfo("HBlank DMA transfer stopped");
        return true;
    }
    
    // Don't start a new transfer if one is already active
    if (ppu->hdmaActive)
    {
        return true;
    }
    
    // Parse the control value
    ppu->hdmaControl = value;
    ppu->transferLength = value & 0x01;
    ppu->transferMode = (value >> 7) & 0x01;
    
    // Validate destination address is within VRAM range for typical use
    // HDMA is commonly used for VRAM transfers, but specification allows any destination
    if (ppu->hdmaDestination >= TM32CORE_VIDEO_RAM_START && 
        ppu->hdmaDestination <= TM32CORE_VIDEO_RAM_END)
    {
        // VRAM destination is typical and valid
    }
    else
    {
        // Log warning for non-VRAM destinations but allow the transfer
        TM32Core_LogWarn("HDMA destination 0x%08X is outside VRAM range", 
            ppu->hdmaDestination);
    }
    
    // Start the HDMA transfer
    ppu->hdmaActive = true;
    ppu->hdmaHBlankDone = false;
    
    if (ppu->transferMode == 0)
    {
        // General Purpose DMA - immediate transfer
        uint16_t transferSize = ppu->transferLength ? 32 : 16;
        ppu->hdmaRemaining = transferSize;
        
        TM32Core_LogInfo("General Purpose DMA transfer started: 0x%08X -> 0x%08X (%u bytes)",
            ppu->hdmaSource, ppu->hdmaDestination, transferSize);
    }
    else
    {
        // HBlank DMA - transfer during HBlank periods
        ppu->hdmaRemaining = 0; // Will be set when HBlank periods occur
        
        TM32Core_LogInfo("HBlank DMA transfer started: 0x%08X -> 0x%08X (%u bytes per HBlank)",
            ppu->hdmaSource, ppu->hdmaDestination, ppu->transferLength ? 32 : 16);
    }
    
    return true;
}

/* VRAM and OAM Access Functions *********************************************/

bool TM32Core_ReadVRAM (
    const TM32Core_PPU*   ppu,
    uint16_t        offset,
    uint8_t*        data
)
{
    TM32Core_ReturnValueIfNull(ppu, false);
    TM32Core_ReturnValueIfNull(data, false);
    
    // Check if offset is within VRAM bounds (0x0000-0x1FFF for 8KB per bank)
    if (offset >= 8192)
    {
        TM32Core_LogError("VRAM offset 0x%04X is out of bounds", offset);
        return false;
    }
    
    // Check if LCD is enabled
    if (!ppu->lcdEnable)
    {
        // VRAM is always accessible when LCD is disabled
        *data = ppu->vram[ppu->vramBank][offset];
        return true;
    }
    
    // VRAM access restrictions based on PPU mode
    switch (ppu->mode)
    {
    case TM32CORE_PPU_MODE_HBLANK:
    case TM32CORE_PPU_MODE_VBLANK:
    case TM32CORE_PPU_MODE_OAM:
        // VRAM is accessible during these modes
        *data = ppu->vram[ppu->vramBank][offset];
        return true;
        
    case TM32CORE_PPU_MODE_DRAWING:
        // VRAM is inaccessible during pixel transfer
        TM32Core_LogWarn("VRAM read blocked during pixel transfer mode");
        *data = 0xFF; // Return 0xFF when blocked (Game Boy behavior)
        return true; // Not an error, just blocked access
        
    default:
        TM32Core_LogError("Unknown PPU mode: %d", ppu->mode);
        return false;
    }
}

bool TM32Core_WriteVRAM (
    TM32Core_PPU*   ppu,
    uint16_t        offset,
    uint8_t         data
)
{
    TM32Core_ReturnValueIfNull(ppu, false);
    
    // Check if offset is within VRAM bounds (0x0000-0x1FFF for 8KB per bank)
    if (offset >= 8192)
    {
        TM32Core_LogError("VRAM offset 0x%04X is out of bounds", offset);
        return false;
    }
    
    // Check if LCD is enabled
    if (!ppu->lcdEnable)
    {
        // VRAM is always accessible when LCD is disabled
        ppu->vram[ppu->vramBank][offset] = data;
        return true;
    }
    
    // VRAM access restrictions based on PPU mode
    switch (ppu->mode)
    {
    case TM32CORE_PPU_MODE_HBLANK:
    case TM32CORE_PPU_MODE_VBLANK:
    case TM32CORE_PPU_MODE_OAM:
        // VRAM is accessible during these modes
        ppu->vram[ppu->vramBank][offset] = data;
        return true;
        
    case TM32CORE_PPU_MODE_DRAWING:
        // VRAM is inaccessible during pixel transfer
        TM32Core_LogWarn("VRAM write blocked during pixel transfer mode");
        return true; // Not an error, just blocked access - write has no effect
        
    default:
        TM32Core_LogError("Unknown PPU mode: %d", ppu->mode);
        return false;
    }
}

bool TM32Core_ReadOAM (
    const TM32Core_PPU*   ppu,
    uint8_t         offset,
    uint8_t*        data
)
{
    TM32Core_ReturnValueIfNull(ppu, false);
    TM32Core_ReturnValueIfNull(data, false);
    
    // Check if offset is within OAM bounds (0x00-0x9F for 160 bytes)
    if (offset >= 160)
    {
        TM32Core_LogError("OAM offset 0x%02X is out of bounds", offset);
        return false;
    }
    
    // Check if LCD is enabled
    if (!ppu->lcdEnable)
    {
        // OAM is always accessible when LCD is disabled
        *data = ppu->oam[offset];
        return true;
    }
    
    // Check if OAM DMA is active
    if (ppu->odmaActive)
    {
        // OAM is inaccessible during OAM DMA transfer
        TM32Core_LogWarn("OAM read blocked during OAM DMA transfer");
        *data = 0xFF; // Return 0xFF when blocked (Game Boy behavior)
        return true; // Not an error, just blocked access
    }
    
    // OAM access restrictions based on PPU mode
    switch (ppu->mode)
    {
    case TM32CORE_PPU_MODE_HBLANK:
    case TM32CORE_PPU_MODE_VBLANK:
        // OAM is accessible during these modes
        *data = ppu->oam[offset];
        return true;
        
    case TM32CORE_PPU_MODE_OAM:
    case TM32CORE_PPU_MODE_DRAWING:
        // OAM is inaccessible during OAM scan and pixel transfer
        TM32Core_LogWarn("OAM read blocked during mode %d", ppu->mode);
        *data = 0xFF; // Return 0xFF when blocked (Game Boy behavior)
        return true; // Not an error, just blocked access
        
    default:
        TM32Core_LogError("Unknown PPU mode: %d", ppu->mode);
        return false;
    }
}

bool TM32Core_WriteOAM (
    TM32Core_PPU*   ppu,
    uint8_t         offset,
    uint8_t         data
)
{
    TM32Core_ReturnValueIfNull(ppu, false);
    
    // Check if offset is within OAM bounds (0x00-0x9F for 160 bytes)
    if (offset >= 160)
    {
        TM32Core_LogError("OAM offset 0x%02X is out of bounds", offset);
        return false;
    }
    
    // Check if LCD is enabled
    if (!ppu->lcdEnable)
    {
        // OAM is always accessible when LCD is disabled
        ppu->oam[offset] = data;
        return true;
    }
    
    // Check if OAM DMA is active
    if (ppu->odmaActive)
    {
        // OAM is inaccessible during OAM DMA transfer
        TM32Core_LogWarn("OAM write blocked during OAM DMA transfer");
        return true; // Not an error, just blocked access - write has no effect
    }
    
    // OAM access restrictions based on PPU mode
    switch (ppu->mode)
    {
    case TM32CORE_PPU_MODE_HBLANK:
    case TM32CORE_PPU_MODE_VBLANK:
        // OAM is accessible during these modes
        ppu->oam[offset] = data;
        return true;
        
    case TM32CORE_PPU_MODE_OAM:
    case TM32CORE_PPU_MODE_DRAWING:
        // OAM is inaccessible during OAM scan and pixel transfer
        TM32Core_LogWarn("OAM write blocked during mode %d", ppu->mode);
        return true; // Not an error, just blocked access - write has no effect
        
    default:
        TM32Core_LogError("Unknown PPU mode: %d", ppu->mode);
        return false;
    }
}
