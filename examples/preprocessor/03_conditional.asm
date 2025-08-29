;
; @file     examples/preprocessor/03_conditional.asm
; @brief    Contains code which demonstrates the TM32 Assembler Tool's
;           Preprocessor Component's ability to perform conditional assembly,
;           using the `.if`, `.elseif`, `.elif`, `.else` and `.endif` directives.
;

; Run this file with:
;   `tm32asm --preprocess-only examples/preprocessor/03_conditional.asm`

; ============================================================================
; CONSTANTS AND VARIABLES FOR CONDITIONAL TESTING
; ============================================================================

; Basic constants
.const DEBUG_LEVEL = 2
.const MAX_BUFFER_SIZE = 1024
.const FEATURE_ENABLED = 1
.const PLATFORM_ID = 3

; Fixed-point constants
.const PI = 3.14159
.const GRAVITY = 9.81
.const TEMPERATURE_THRESHOLD = 25.5

; Variables for testing
.let user_level = 5
.let buffer_count = 256
.let enable_logging = 1
.let current_temp = 28.7
.let precision_mode = 0

; Complex expressions as constants
.const TOTAL_MEMORY = MAX_BUFFER_SIZE * 4
.const IS_HIGH_PRECISION = PI > 3.0
.const TEMP_WARNING = TEMPERATURE_THRESHOLD + 5.0

; ============================================================================
; BASIC CONDITIONAL ASSEMBLY - SIMPLE IF/ENDIF
; ============================================================================

; Simple integer comparison
.if DEBUG_LEVEL > 0
    ; Debug code included when DEBUG_LEVEL > 0
    .let debug_enabled = 1
    .const DEBUG_MSG_COUNT = 10
.endif

; Fixed-point comparison
.if PI > 3.0
    ; High precision math routines
    .const USE_HIGH_PRECISION = 1
    .let math_precision = 32
.endif

; Variable-based condition
.if user_level >= 3
    ; Admin features available
    .const ADMIN_ACCESS = 1
    .let admin_commands = 15
.endif

; Equality check
.if PLATFORM_ID == 3
    ; Platform-specific optimizations
    .const PLATFORM_ARM = 1
    .let cache_size = 64
.endif

; ============================================================================
; IF/ELSE CONDITIONAL ASSEMBLY
; ============================================================================

; Basic if/else with integer
.if FEATURE_ENABLED
    ; Feature is enabled
    .const FEATURE_FUNCTIONS = 25
    .let feature_buffer = 512
.else
    ; Feature is disabled
    .const FEATURE_FUNCTIONS = 0
    .let feature_buffer = 0
.endif

; Temperature-based conditional
.if current_temp > TEMPERATURE_THRESHOLD
    ; Hot weather mode
    .const COOLING_ENABLED = 1
    .let fan_speed = 100
    .const TEMP_CHECK_INTERVAL = 30.0
.else
    ; Normal weather mode
    .const COOLING_ENABLED = 0
    .let fan_speed = 50
    .const TEMP_CHECK_INTERVAL = 60.0
.endif

; Buffer size optimization
.if buffer_count > 128
    ; Large buffer mode
    .const USE_LARGE_BUFFERS = 1
    .let memory_pool = buffer_count * 8
.else
    ; Small buffer mode
    .const USE_LARGE_BUFFERS = 0
    .let memory_pool = buffer_count * 4
.endif

; ============================================================================
; COMPLEX CONDITIONAL ASSEMBLY - IF/ELSEIF/ELSE/ENDIF
; ============================================================================

; Multi-level debug system
.if DEBUG_LEVEL == 0
    ; No debugging
    .const DEBUG_NONE = 1
    .let log_buffer = 0
.elseif DEBUG_LEVEL == 1
    ; Basic debugging
    .const DEBUG_BASIC = 1
    .let log_buffer = 256
    .const LOG_LEVEL = 1
.elseif DEBUG_LEVEL == 2
    ; Verbose debugging
    .const DEBUG_VERBOSE = 1
    .let log_buffer = 1024
    .const LOG_LEVEL = 2
.else
    ; Maximum debugging
    .const DEBUG_MAX = 1
    .let log_buffer = 2048
    .const LOG_LEVEL = 3
.endif

; Performance optimization levels
.if user_level <= 1
    ; Beginner mode - maximum safety
    .const SAFETY_CHECKS = 1
    .let max_operations = 100
    .const TIMEOUT = 10.0
.elseif user_level <= 3
    ; Intermediate mode - balanced
    .const SAFETY_CHECKS = 1
    .let max_operations = 500
    .const TIMEOUT = 5.0
.elseif user_level <= 5
    ; Advanced mode - performance focused
    .const SAFETY_CHECKS = 0
    .let max_operations = 1000
    .const TIMEOUT = 2.0
.else
    ; Expert mode - maximum performance
    .const SAFETY_CHECKS = 0
    .let max_operations = 2000
    .const TIMEOUT = 1.0
.endif

; Platform-specific memory layout
.if PLATFORM_ID == 1
    ; Desktop platform
    .const HEAP_SIZE = 0x100000
    .const STACK_SIZE = 0x10000
    .let page_size = 4096
.elseif PLATFORM_ID == 2
    ; Mobile platform
    .const HEAP_SIZE = 0x80000
    .const STACK_SIZE = 0x8000
    .let page_size = 2048
.elseif PLATFORM_ID == 3
    ; Embedded platform
    .const HEAP_SIZE = 0x10000
    .const STACK_SIZE = 0x2000
    .let page_size = 512
.else
    ; Unknown platform - conservative settings
    .const HEAP_SIZE = 0x8000
    .const STACK_SIZE = 0x1000
    .let page_size = 256
.endif

; ============================================================================
; NESTED CONDITIONAL ASSEMBLY
; ============================================================================

; Nested conditions for graphics system
.if FEATURE_ENABLED
    .const GRAPHICS_ENABLED = 1
    
    .if user_level >= 2
        ; Advanced graphics features
        .const ADVANCED_GFX = 1
        
        .if current_temp < 30.0
            ; Full resolution mode
            .const FULL_RESOLUTION = 1
            .let screen_width = 1920
            .let screen_height = 1080
        .else
            ; Reduced resolution mode (thermal protection)
            .const FULL_RESOLUTION = 0
            .let screen_width = 1280
            .let screen_height = 720
        .endif
        
        .let graphics_memory = screen_width * screen_height * 4
    .else
        ; Basic graphics only
        .const ADVANCED_GFX = 0
        .let screen_width = 800
        .let screen_height = 600
        .let graphics_memory = screen_width * screen_height * 2
    .endif
.else
    ; No graphics support
    .const GRAPHICS_ENABLED = 0
    .let screen_width = 0
    .let screen_height = 0
    .let graphics_memory = 0
.endif

; Nested audio system configuration
.if enable_logging
    .if DEBUG_LEVEL > 1
        ; Audio debugging enabled
        .const AUDIO_DEBUG = 1
        .let audio_log_size = 512
        
        .if precision_mode
            ; High precision audio logging
            .const AUDIO_PRECISION = 1
            .let sample_rate = 96000.0
            .let bit_depth = 32
        .else
            ; Standard audio logging
            .const AUDIO_PRECISION = 0
            .let sample_rate = 44100.0
            .let bit_depth = 16
        .endif
    .else
        ; No audio debugging
        .const AUDIO_DEBUG = 0
        .let audio_log_size = 0
        .let sample_rate = 44100.0
        .let bit_depth = 16
    .endif
.endif

; ============================================================================
; CONDITIONAL ASSEMBLY WITH COMPLEX EXPRESSIONS
; ============================================================================

; Mathematical expressions in conditions
.if (PI * 2.0) > 6.0
    .const CIRCLE_MATH = 1
    .let circumference_factor = PI * 2.0
.endif

; Compound logical expressions
.if (DEBUG_LEVEL > 0) && (user_level >= 2)
    .const ADVANCED_DEBUG = 1
    .let debug_features = DEBUG_LEVEL * user_level
.endif

; Mixed integer and fixed-point conditions
.if (buffer_count > 100) && (current_temp < 35.0)
    .const OPTIMAL_CONDITIONS = 1
    .let performance_boost = buffer_count / 10.0
.endif

; Nested arithmetic in conditions
.if ((user_level * 2) + DEBUG_LEVEL) >= 8
    .const HIGH_PRIVILEGE = 1
    .let privilege_score = (user_level * 2) + DEBUG_LEVEL
.endif

; Ternary operator in conditional expressions
; .if (user_level > 3) ? FEATURE_ENABLED : 0
;     .const CONDITIONAL_FEATURE = 1
;     .let feature_weight = (user_level > 3) ? 100 : 50
; .endif

; ============================================================================
; CONDITIONAL DATA DEFINITIONS
; ============================================================================

; Conditional memory allocation
.if USE_LARGE_BUFFERS
    .let primary_buffer = 2048
    .let secondary_buffer = 1024
    .let tertiary_buffer = 512
.else
    .let primary_buffer = 512
    .let secondary_buffer = 256
    .let tertiary_buffer = 128
.endif

; Conditional timing constants
.if current_temp > TEMP_WARNING
    ; Emergency mode timing
    .const WATCHDOG_TIMEOUT = 1.0
    .const POLL_INTERVAL = 0.1
    .let emergency_retries = 10
.elseif current_temp > TEMPERATURE_THRESHOLD
    ; Warning mode timing
    .const WATCHDOG_TIMEOUT = 5.0
    .const POLL_INTERVAL = 0.5
    .let emergency_retries = 5
.else
    ; Normal mode timing
    .const WATCHDOG_TIMEOUT = 30.0
    .const POLL_INTERVAL = 2.0
    .let emergency_retries = 3
.endif

; ============================================================================
; USAGE IN ASSEMBLY CODE
; ============================================================================

.text
main:
    ; Conditional feature usage
.if FEATURE_ENABLED
    ld a, FEATURE_FUNCTIONS
    ld b, feature_buffer
    add a, b
.endif

    ; Debug level dependent code
.if DEBUG_LEVEL > 0
    ld al, debug_enabled
    ld ah, DEBUG_MSG_COUNT
.endif

.if DEBUG_LEVEL >= 2
    ld bl, DEBUG_VERBOSE
    ld bh, log_buffer
.endif

    ; Platform-specific operations
.if PLATFORM_ID == 3
    ld c, HEAP_SIZE
    ld d, STACK_SIZE
    add c, d
.endif

    ; Graphics system usage
.if GRAPHICS_ENABLED
    ld a, screen_width
    ld b, screen_height
    add a, b                    ; Add dimensions together (TM32 has no MUL instruction)
    
    .if ADVANCED_GFX
        ld c, graphics_memory
        add a, c
    .endif
.endif

    ; Temperature-dependent behavior
.if COOLING_ENABLED
    ld al, fan_speed
    ld ah, TEMP_CHECK_INTERVAL
.endif

    ; User level dependent features
.if user_level >= 3
    ld a, ADMIN_ACCESS
    ld b, admin_commands
.endif

.if user_level > 5
    ld c, HIGH_PRIVILEGE
    ld d, privilege_score
.endif

    ; Memory management based on conditions
.if USE_LARGE_BUFFERS
    ld a, primary_buffer
    ld b, secondary_buffer
    ld c, tertiary_buffer
    add a, b
    add a, c                    ; Total buffer allocation
.endif

    ; Performance optimization usage
.if SAFETY_CHECKS
    ; Bounds checking code
    ld a, max_operations
    cmp a, 1000
    jpb nc, bounds_ok
    ld a, 1000                  ; Clamp to maximum
bounds_ok:
.endif

    ; Audio system usage
.if AUDIO_DEBUG
    ld a, sample_rate
    ld b, bit_depth
    ld c, audio_log_size
.endif

    ; Conditional arithmetic results
.if OPTIMAL_CONDITIONS
    ld a, performance_boost
    add a, buffer_count
.endif

.if CIRCLE_MATH
    ld a, circumference_factor
    add a, 100                  ; Scale for integer math
.endif

done:
    jpb nc, done

;
; The preprocessor should resolve conditional blocks based on the values of
; constants and variables, including or excluding code sections accordingly.
;
; Expected behavior examples:
; ==========================
;
; Since DEBUG_LEVEL = 2:
; - Debug code will be included (DEBUG_LEVEL > 0)
; - Verbose debugging will be selected (DEBUG_LEVEL == 2)
; - ADVANCED_DEBUG will be enabled (DEBUG_LEVEL > 0 && user_level >= 2)
;
; Since user_level = 5:
; - Admin access will be granted (user_level >= 3)
; - Advanced performance mode will be selected (user_level <= 5 but > 3)
; - HIGH_PRIVILEGE will be enabled ((user_level * 2) + DEBUG_LEVEL >= 8)
;
; Since current_temp = 28.7 and TEMPERATURE_THRESHOLD = 25.5:
; - Cooling will be enabled (current_temp > TEMPERATURE_THRESHOLD)
; - Warning mode timing will be used (TEMP_WARNING = 30.5, so 28.7 > 25.5 but < 30.5)
;
; Since FEATURE_ENABLED = 1:
; - All feature-dependent code will be included
; - Graphics system will be enabled
; - Advanced graphics will be enabled (user_level >= 2)
;
; Since PLATFORM_ID = 3:
; - Embedded platform settings will be used
; - HEAP_SIZE = 0x10000, STACK_SIZE = 0x2000, page_size = 512
;
; Since PI = 3.14159:
; - High precision math will be enabled (PI > 3.0)
; - Circle math will be enabled ((PI * 2.0) > 6.0)
;
; Variable Value Summary After Conditional Processing:
; ==================================================
; DEBUG_LEVEL = 2                 (constant)
; debug_enabled = 1               (included due to DEBUG_LEVEL > 0)
; DEBUG_VERBOSE = 1               (included due to DEBUG_LEVEL == 2)
; log_buffer = 1024               (from verbose debugging)
; LOG_LEVEL = 2                   (from verbose debugging)
; user_level = 5                  (variable)
; ADMIN_ACCESS = 1                (included due to user_level >= 3)
; admin_commands = 15             (included due to user_level >= 3)
; max_operations = 1000           (from advanced mode: user_level <= 5 but > 3)
; TIMEOUT = 2.0                   (from advanced mode)
; SAFETY_CHECKS = 0               (from advanced mode)
; current_temp = 28.7             (variable)
; COOLING_ENABLED = 1             (current_temp > TEMPERATURE_THRESHOLD)
; fan_speed = 100                 (from hot weather mode)
; TEMP_CHECK_INTERVAL = 30.0      (from hot weather mode)
; FEATURE_ENABLED = 1             (constant)
; FEATURE_FUNCTIONS = 25          (feature enabled)
; feature_buffer = 512            (feature enabled)
; GRAPHICS_ENABLED = 1            (feature enabled)
; ADVANCED_GFX = 1                (user_level >= 2)
; FULL_RESOLUTION = 1             (current_temp < 30.0)
; screen_width = 1920             (full resolution)
; screen_height = 1080            (full resolution)
; graphics_memory = 8294400       (1920 * 1080 * 4)
; PLATFORM_ID = 3                 (constant)
; HEAP_SIZE = 0x10000            (embedded platform)
; STACK_SIZE = 0x2000            (embedded platform)
; page_size = 512                 (embedded platform)
; PI = 3.14159                    (constant)
; USE_HIGH_PRECISION = 1          (PI > 3.0)
; math_precision = 32             (high precision enabled)
; CIRCLE_MATH = 1                 (PI * 2.0 > 6.0)
; circumference_factor = 6.28318  (PI * 2.0)
; ADVANCED_DEBUG = 1              (DEBUG_LEVEL > 0 && user_level >= 2)
; debug_features = 10             (DEBUG_LEVEL * user_level = 2 * 5)
; buffer_count = 256              (variable)
; USE_LARGE_BUFFERS = 1           (buffer_count > 128)
; memory_pool = 2048              (buffer_count * 8)
; primary_buffer = 2048           (large buffers)
; secondary_buffer = 1024         (large buffers)
; tertiary_buffer = 512           (large buffers)
; OPTIMAL_CONDITIONS = 1          (buffer_count > 100 && current_temp < 35.0)
; performance_boost = 25.6        (buffer_count / 10.0)
; HIGH_PRIVILEGE = 1              ((user_level * 2) + DEBUG_LEVEL = 12 >= 8)
; privilege_score = 12            ((5 * 2) + 2)
; enable_logging = 1              (variable)
; precision_mode = 0              (variable)
; AUDIO_DEBUG = 1                 (enable_logging && DEBUG_LEVEL > 1)
; audio_log_size = 512            (audio debugging enabled)
; AUDIO_PRECISION = 0             (precision_mode = 0)
; sample_rate = 44100.0           (standard precision)
; bit_depth = 16                  (standard precision)
; WATCHDOG_TIMEOUT = 5.0          (warning mode: current_temp > TEMPERATURE_THRESHOLD but < TEMP_WARNING)
; POLL_INTERVAL = 0.5             (warning mode)
; emergency_retries = 5           (warning mode)
;
; Code sections that will be included:
; ===================================
; - All DEBUG_LEVEL > 0 sections
; - All user_level >= 3 sections  
; - All FEATURE_ENABLED sections
; - All GRAPHICS_ENABLED and ADVANCED_GFX sections
; - All current_temp > TEMPERATURE_THRESHOLD sections
; - All PLATFORM_ID == 3 sections
; - All mathematical precision sections
; - Audio debugging sections
; - Large buffer optimization sections
; - All conditional arithmetic and feature code
;
; This demonstrates the preprocessor's ability to evaluate complex conditional
; expressions involving integers, fixed-point numbers, variables, constants,
; and compound logical expressions to selectively include or exclude code
; sections during the preprocessing stage.
;
; Expected Assembly Preprocessor Output:
; ======================================
;
; ```
; .text
; main:
;     ; Conditional feature usage - FEATURE_ENABLED resolved to true
;     ld a, 25
;     ld b, 512
;     add a, b
; 
;     ; Debug level dependent code - DEBUG_LEVEL > 0 resolved to true
;     ld al, 1
;     ld ah, 10
; 
;     ; DEBUG_LEVEL >= 2 resolved to true
;     ld bl, 1
;     ld bh, 1024
; 
;     ; Platform-specific operations - PLATFORM_ID == 3 resolved to true
;     ld c, 0x10000
;     ld d, 0x2000
;     add c, d
; 
;     ; Graphics system usage - GRAPHICS_ENABLED resolved to true
;     ld a, 1920
;     ld b, 1080
;     mul a, b                    ; Calculate total pixels
;     
;     ; ADVANCED_GFX resolved to true
;     ld c, 8294400
;     add a, c
; 
;     ; Temperature-dependent behavior - COOLING_ENABLED resolved to true
;     ld al, 100
;     ld ah, 30.0
; 
;     ; User level dependent features - user_level >= 3 resolved to true
;     ld a, 1
;     ld b, 15
; 
;     ; Memory management based on conditions - USE_LARGE_BUFFERS resolved to true
;     ld a, 2048
;     ld b, 1024
;     ld c, 512
;     add a, b
;     add a, c                    ; Total buffer allocation
; 
;     ; Audio system usage - AUDIO_DEBUG resolved to true
;     ld a, 44100.0
;     ld b, 16
;     ld c, 512
; 
;     ; Conditional arithmetic results - OPTIMAL_CONDITIONS resolved to true
;     ld a, 25.6
;     add a, 256
; 
;     ; CIRCLE_MATH resolved to true
;     ld a, 6.28318
;     add a, 100                  ; Scale for integer math
; 
; done:
;     jpb nc, done
; ```
;
