; Real-World Application Examples
; Tests: Practical use cases, common programming patterns, realistic scenarios

.text

; Include common definitions for realistic applications
; (These would typically be in separate include files)

; Memory layout constants for a simple OS/kernel
.const KERNEL_BASE = 0x00003000
.const USER_BASE = 0x00100000
.const STACK_BASE = 0xFFFE0000
.const IO_BASE = 0xFFFF0000

; Hardware register addresses
.const TIMER_CTRL = IO_BASE + 0x0100
.const TIMER_VALUE = IO_BASE + 0x0104
.const SERIAL_DATA = IO_BASE + 0x0200
.const SERIAL_CTRL = IO_BASE + 0x0204
.const GPIO_DATA = IO_BASE + 0x0300
.const GPIO_DIR = IO_BASE + 0x0304

; System constants
.const TASK_SIZE = 1024
.const MAX_TASKS = 16
.const BUFFER_SIZE = 256

; Real-World Example 1: Memory Management
.macro ALLOC_MEMORY
    .let size = @1
    .let alignment = @2
    
    ; Validate parameters
    .assert size > 0, "Memory size must be positive"
    .assert alignment > 0, "Alignment must be positive"
    .assert (alignment & (alignment - 1)) == 0, "Alignment must be power of 2"
    
    ; Calculate aligned size
    .let aligned_size = ((size + alignment - 1) / alignment) * alignment
    
    ; Check if allocation is reasonable
    .if aligned_size > 0x100000  ; 1MB limit
        .error "Allocation too large"
    .endif
    
    ; Generate allocation code
    LD A, aligned_size
    LD B, alignment
    CALL NC, malloc_function
.endmacro

.macro FREE_MEMORY
    .let ptr_reg = @1
    
    ; Validate pointer register
    .if strcmp(ptr_reg, "A") != 0 && strcmp(ptr_reg, "B") != 0 && strcmp(ptr_reg, "C") != 0 && strcmp(ptr_reg, "D") != 0
        .error "Invalid pointer register"
    .endif
    
    ; Generate free code
    .if strcmp(ptr_reg, "A") != 0
        MV A, @1
    .endif
    CALL NC, free_function
.endmacro

; Real-World Example 2: Task Scheduler
.macro DEFINE_TASK
    .let task_id = @1
    .let priority = @2
    .let stack_size = @3
    
    ; Validate task parameters
    .assert task_id >= 0 && task_id < MAX_TASKS, "Task ID out of range"
    .assert priority >= 1 && priority <= 10, "Priority must be 1-10"
    .assert stack_size >= 256 && stack_size <= 4096, "Stack size out of range"
    
    ; Calculate task control block address
    .let tcb_addr = KERNEL_BASE + (task_id * 64)  ; 64 bytes per TCB
    
    ; Initialize task control block
    .const TASK_@1_TCB = tcb_addr
    .const TASK_@1_PRIORITY = priority
    .const TASK_@1_STACK_SIZE = stack_size
    .const TASK_@1_STACK_BASE = USER_BASE + (task_id * TASK_SIZE * 4)
    
    ; Generate task initialization code
    LD A, TASK_@1_TCB
    LD B, TASK_@1_PRIORITY
    LD C, TASK_@1_STACK_SIZE
    LD D, TASK_@1_STACK_BASE
    CALL NC, init_task
.endmacro

.macro SWITCH_TASK
    .let from_task = @1
    .let to_task = @2
    
    ; Validate task IDs
    .assert from_task >= 0 && from_task < MAX_TASKS, "From task ID invalid"
    .assert to_task >= 0 && to_task < MAX_TASKS, "To task ID invalid"
    
    ; Save current task context
    LD A, from_task
    CALL NC, save_context
    
    ; Load new task context
    LD A, to_task
    CALL NC, load_context
    
    ; Update current task pointer
    LD A, to_task
    ST [current_task], A
.endmacro

; Real-World Example 3: Device Driver Framework
.macro DEFINE_DRIVER
    .let device_type = @1  ; String identifying device type
    .let base_addr = @2
    .let irq_vector = @3
    
    ; Validate parameters
    .assert base_addr >= IO_BASE, "Device address must be in I/O space"
    .assert irq_vector >= 0 && irq_vector < 32, "IRQ vector out of range"
    
    ; Generate device-specific constants
    .if strcmp(device_type, "TIMER") == 0
        .const DEVICE_CTRL = base_addr + 0x00
        .const DEVICE_DATA = base_addr + 0x04
        .const DEVICE_STATUS = base_addr + 0x08
    .elif strcmp(device_type, "SERIAL") == 0
        .const DEVICE_DATA = base_addr + 0x00
        .const DEVICE_CTRL = base_addr + 0x04
        .const DEVICE_STATUS = base_addr + 0x08
        .const DEVICE_BAUD = base_addr + 0x0C
    .elif strcmp(device_type, "GPIO") == 0
        .const DEVICE_DATA = base_addr + 0x00
        .const DEVICE_DIR = base_addr + 0x04
        .const DEVICE_PULLUP = base_addr + 0x08
    .else
        .error "Unknown device type"
    .endif
    
    ; Register interrupt handler
    LD A, irq_vector
    LD B, device_interrupt_handler
    CALL NC, register_interrupt
.endmacro

.macro DEVICE_READ
    .let device_reg = @1
    .let dest_reg = @2
    
    ; Generate appropriate read operation
    LD @2, [device_reg]
.endmacro

.macro DEVICE_WRITE
    .let device_reg = @1
    .let src_reg = @2
    
    ; Generate appropriate write operation
    ST [device_reg], @2
.endmacro

; Real-World Example 4: Protocol Handler
.macro HANDLE_PACKET
    .let protocol_type = @1
    .let buffer_ptr = @2
    .let length = @3
    
    ; Validate parameters
    .if strcmp(buffer_ptr, "A") != 0 && strcmp(buffer_ptr, "B") != 0 && strcmp(buffer_ptr, "C") != 0 && strcmp(buffer_ptr, "D") != 0
        .error "Buffer pointer must be a 32-bit register"
    .endif
    
    ; Protocol dispatch
    .if strcmp(protocol_type, "ETHERNET") == 0
        ; Validate Ethernet frame
        .assert length >= 64 && length <= 1518, "Invalid Ethernet frame size"
        LD A, @2  ; Buffer pointer
        LD B, @3  ; Length
        CALL NC, process_ethernet
        
    .elif strcmp(protocol_type, "IP") == 0
        ; Validate IP packet
        .assert length >= 20 && length <= 65535, "Invalid IP packet size"
        LD A, @2  ; Buffer pointer
        LD B, @3  ; Length
        CALL NC, process_ip
        
    .elif strcmp(protocol_type, "TCP") == 0
        ; Validate TCP segment
        .assert length >= 20, "TCP segment too small"
        LD A, @2  ; Buffer pointer
        LD B, @3  ; Length
        CALL NC, process_tcp
        
    .elif strcmp(protocol_type, "UDP") == 0
        ; Validate UDP datagram
        .assert length >= 8, "UDP datagram too small"
        LD A, @2  ; Buffer pointer
        LD B, @3  ; Length
        CALL NC, process_udp
        
    .else
        .error "Unknown protocol type"
    .endif
.endmacro

; Real-World Example 5: Mathematical Library Functions
.macro MATRIX_MULTIPLY
    .let matrix_a = @1  ; Base address of matrix A
    .let matrix_b = @2  ; Base address of matrix B
    .let result = @3    ; Base address of result matrix
    .let size = @4      ; Matrix size (assuming square matrices)
    
    ; Validate parameters
    .assert size > 0 && size <= 16, "Matrix size must be 1-16"
    
    ; Generate matrix multiplication code
    .let i = 0
    .while i < size
        .let j = 0
        .while j < size
            ; Calculate result[i][j] = sum(A[i][k] * B[k][j])
            .let sum_addr = result + ((i * size + j) * 4)
            
            ; Initialize sum to zero
            LD A, 0
            ST [sum_addr], A
            
            .let k = 0
            .while k < size
                ; Load A[i][k]
                .let a_addr = matrix_a + ((i * size + k) * 4)
                LD B, [a_addr]
                
                ; Load B[k][j]
                .let b_addr = matrix_b + ((k * size + j) * 4)
                LD C, [b_addr]
                
                ; Multiply and add to sum
                ; This would typically call a multiply function
                LD A, B
                CALL NC, multiply_add_to_sum
                
                .let k = k + 1
            .endwhile
            
            .let j = j + 1
        .endwhile
        .let i = i + 1
    .endwhile
.endmacro

.macro CALCULATE_CRC32
    .let data_ptr = @1
    .let length = @2
    .let result_ptr = @3
    
    ; Validate parameters
    .assert length > 0, "Data length must be positive"
    
    ; Initialize CRC
    LD A, 0xFFFFFFFF
    
    ; Process each byte
    .let i = 0
    .while i < length && i < 1024  ; Safety limit
        ; Load byte
        LD B, [data_ptr + i]
        
        ; CRC calculation (simplified)
        XOR A, B
        
        ; Process 8 bits
        .let bit = 0
        .while bit < 8
            ; Check if bit 0 is set
            LD C, A
            AND C, 1
            .if C != 0
                SRL A
                XOR A, 0xEDB88320  ; CRC32 polynomial
            .else
                SRL A
            .endif
            .let bit = bit + 1
        .endwhile
        
        .let i = i + 1
    .endwhile
    
    ; Finalize and store result
    NOT A
    ST [result_ptr], A
.endmacro

; Real-World Example 6: Configuration System
.macro CONFIG_SET
    .let param_name = @1
    .let value = @2
    
    ; System configuration parameters
    .if strcmp(param_name, "CLOCK_SPEED") == 0
        .assert value >= 1000000 && value <= 100000000, "Clock speed out of range"
        .const SYS_CLOCK_SPEED = value
        
    .elif strcmp(param_name, "MEMORY_SIZE") == 0
        .assert value >= 0x100000 && value <= 0x10000000, "Memory size out of range"
        .const SYS_MEMORY_SIZE = value
        
    .elif strcmp(param_name, "TASK_QUANTUM") == 0
        .assert value >= 1 && value <= 1000, "Task quantum out of range"
        .const SYS_TASK_QUANTUM = value
        
    .elif strcmp(param_name, "DEBUG_LEVEL") == 0
        .assert value >= 0 && value <= 3, "Debug level must be 0-3"
        .const SYS_DEBUG_LEVEL = value
        
    .else
        .error "Unknown configuration parameter"
    .endif
.endmacro

.macro FEATURE_ENABLE
    .let feature = @1
    
    .if strcmp(feature, "NETWORKING") == 0
        .const FEATURE_NETWORKING = 1
        ; Enable networking-related code generation
        
    .elif strcmp(feature, "GRAPHICS") == 0
        .const FEATURE_GRAPHICS = 1
        ; Enable graphics-related code generation
        
    .elif strcmp(feature, "USB") == 0
        .const FEATURE_USB = 1
        ; Enable USB-related code generation
        
    .elif strcmp(feature, "CRYPTO") == 0
        .const FEATURE_CRYPTO = 1
        ; Enable cryptography-related code generation
        
    .else
        .error "Unknown feature"
    .endif
.endmacro

; Application main function demonstrating real-world usage
main:
    ; System configuration
    CONFIG_SET "CLOCK_SPEED", 50000000    ; 50 MHz
    CONFIG_SET "MEMORY_SIZE", 0x1000000   ; 16 MB
    CONFIG_SET "TASK_QUANTUM", 10         ; 10ms
    CONFIG_SET "DEBUG_LEVEL", 2           ; Medium debug level
    
    ; Enable features
    FEATURE_ENABLE "NETWORKING"
    FEATURE_ENABLE "GRAPHICS"
    
    ; Define system tasks
    DEFINE_TASK 0, 10, 1024   ; Kernel task (highest priority)
    DEFINE_TASK 1, 5, 2048    ; Network task
    DEFINE_TASK 2, 3, 1024    ; User interface task
    DEFINE_TASK 3, 1, 512     ; Background task (lowest priority)
    
    ; Define device drivers
    DEFINE_DRIVER "TIMER", TIMER_CTRL, 5
    DEFINE_DRIVER "SERIAL", SERIAL_DATA, 6
    DEFINE_DRIVER "GPIO", GPIO_DATA, 7
    
    ; Memory management example
    ALLOC_MEMORY 1024, 32     ; Allocate 1KB aligned to 32 bytes
    FREE_MEMORY A             ; Free the allocated memory
    
    ; Protocol handling example
    HANDLE_PACKET "ETHERNET", B, 1518
    HANDLE_PACKET "IP", C, 576
    HANDLE_PACKET "TCP", D, 1460
    
    ; Mathematical operations
    CALCULATE_CRC32 0x80001000, 256, 0x80002000
    
    ; Task switching
    SWITCH_TASK 0, 1          ; Switch from kernel to network task
    
    ; Device I/O
    DEVICE_READ TIMER_VALUE, A
    DEVICE_WRITE GPIO_DATA, B
    
    HALT

; Supporting function placeholders (would be implemented elsewhere)
malloc_function:
    RET NC

free_function:
    RET NC

init_task:
    RET NC

save_context:
    RET NC

load_context:
    RET NC

register_interrupt:
    RET NC

process_ethernet:
    RET NC

process_ip:
    RET NC

process_tcp:
    RET NC

process_udp:
    RET NC

multiply_add_to_sum:
    RET NC

device_interrupt_handler:
    RETI

current_task:
    .const current_task = 0x80003000
