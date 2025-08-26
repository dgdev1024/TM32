/**
 * @file    tm32core_serial.h
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025-08-25
 * 
 * @brief   Contains the opaque type and function prototypes for the TM32 Core's
 *          serial communication interface (SCI) component.
 */

#ifndef TM32CORE_SERIAL_H
#define TM32CORE_SERIAL_H

/* Include Files **************************************************************/

#include <tm32core_common.h>

/* Typedefs and Forward Declarations ******************************************/

/**
 * @brief   The opaque type representing the TM32 Core Bus interface.
 */
typedef struct TM32Core_Bus TM32Core_Bus;

/**
 * @brief   The opaque type representing the TM32 Core Serial Communication
 *          Interface (SCI) component.
 */
typedef struct TM32Core_Serial TM32Core_Serial;

/**
 * @brief   Callback function type for serial data transmission.
 * 
 * This callback is invoked when the serial component needs to transmit data
 * to an external device or system. The frontend can implement this to send
 * data via any communication method (IPC, network, file, etc.).
 * 
 * @param   data        The byte of data to transmit.
 * @param   userData    User-provided data pointer passed during callback setup.
 * 
 * @return  `true` if the data was successfully queued for transmission;
 *          `false` if transmission failed or is unavailable.
 */
typedef bool (*TM32Core_SerialTransmitCallback) (
    uint8_t     data,
    void*       userData
);

/**
 * @brief   Callback function type for serial data reception.
 * 
 * This callback is invoked when the serial component needs to check for
 * incoming data from an external device or system. The frontend can implement
 * this to receive data via any communication method.
 * 
 * @param   data        Pointer to store the received byte of data.
 * @param   userData    User-provided data pointer passed during callback setup.
 * 
 * @return  `true` if data was available and stored in `data`;
 *          `false` if no data is available.
 */
typedef bool (*TM32Core_SerialReceiveCallback) (
    uint8_t*    data,
    void*       userData
);

/**
 * @brief   Callback function type for external clock signal.
 * 
 * This callback is invoked when the serial component is configured for
 * external clock mode and needs to check for clock edges. The frontend
 * can implement this to provide external clock timing.
 * 
 * @param   userData    User-provided data pointer passed during callback setup.
 * 
 * @return  `true` if a clock edge was detected; `false` otherwise.
 */
typedef bool (*TM32Core_SerialClockCallback) (
    void*       userData
);

/* Public Function Prototypes *************************************************/

/**
 * @brief   Allocates and creates a new instance of the TM32 Core Serial
 *          Communication Interface (SCI) component.
 * 
 * @param   bus     A pointer to the TM32 Core Bus interface to which this
 *                  SCI component will be connected.
 * 
 * @return  A pointer to the newly created TM32 Core SCI component;
 *          `NULL` on failure.
 */
TM32CORE_API TM32Core_Serial* TM32Core_CreateSerial (
    TM32Core_Bus*   bus
);

/**
 * @brief   Initializes (or resets) the given TM32 Core SCI component,
 *          resetting its state.
 * 
 * @param   serial  A pointer to the TM32 Core SCI component to initialize.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_InitializeSerial (
    TM32Core_Serial* serial
);

/**
 * @brief   Clocks the given TM32 Core SCI component.
 * 
 * This function is called every time the TM32 CPU attached to this SCI's bus
 * clocks. It handles the timing and state changes of the SCI component.
 * 
 * @param   serial  A pointer to the TM32 Core SCI component to clock.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_ClockSerial (
    TM32Core_Serial* serial
);

/**
 * @brief   Destroys and deallocates the given TM32 Core SCI component.
 * 
 * @param   serial  A pointer to the TM32 Core SCI component to destroy.
 */
TM32CORE_API void TM32Core_DestroySerial (
    TM32Core_Serial* serial
);

/**
 * @brief   Reads the current value of the serial data register (SB).
 * 
 * This register contains the data to be transmitted or the data that has
 * been received. It can be read and written to at any time.
 * 
 * @param   serial  A pointer to the TM32 Core SCI component.
 * 
 * @return  The current value of the SB register.
 */
TM32CORE_API uint8_t TM32Core_ReadSB (
    const TM32Core_Serial*  serial
);

/**
 * @brief   Reads the current value of the serial control register (SC).
 * 
 * This register controls the serial transfer operation. Bit 7 starts the
 * transfer when set to 1, bit 1 selects the clock speed (0 = normal,
 * 1 = fast), and bit 0 selects internal/external clock (0 = external,
 * 1 = internal).
 * 
 * @param   serial  A pointer to the TM32 Core SCI component.
 * 
 * @return  The current value of the SC register.
 */
TM32CORE_API uint8_t TM32Core_ReadSC (
    const TM32Core_Serial*  serial
);

/**
 * @brief   Writes a value to the serial data register (SB).
 * 
 * This sets the data to be transmitted. The data will be sent when a
 * serial transfer is initiated via the SC register.
 * 
 * @param   serial  A pointer to the TM32 Core SCI component.
 * @param   value   The value to write to the SB register.
 * 
 * @return  `true` if the write was successful; `false` if there was an error.
 */
TM32CORE_API bool TM32Core_WriteSB (
    TM32Core_Serial*    serial,
    uint8_t             value
);

/**
 * @brief   Writes a value to the serial control register (SC).
 * 
 * This controls the serial transfer operation. Setting bit 7 to 1 starts
 * a transfer with internal clock. Other bits control clock speed and
 * clock source selection.
 * 
 * @param   serial  A pointer to the TM32 Core SCI component.
 * @param   value   The value to write to the SC register.
 * 
 * @return  `true` if the write was successful; `false` if there was an error.
 */
TM32CORE_API bool TM32Core_WriteSC (
    TM32Core_Serial*    serial,
    uint8_t             value
);

/**
 * @brief   Sets the transmit callback for the serial component.
 * 
 * This callback will be invoked when the serial component needs to transmit
 * data. Frontend applications can use this to implement custom communication
 * methods such as IPC, networking, or file I/O.
 * 
 * @param   serial      A pointer to the TM32 Core SCI component.
 * @param   callback    The transmit callback function, or `NULL` to disable.
 * @param   userData    User data to pass to the callback function.
 * 
 * @return  `true` if the callback was successfully set; `false` on error.
 */
TM32CORE_API bool TM32Core_SetSerialTransmitCallback (
    TM32Core_Serial*                serial,
    TM32Core_SerialTransmitCallback callback,
    void*                           userData
);

/**
 * @brief   Sets the receive callback for the serial component.
 * 
 * This callback will be invoked when the serial component needs to check
 * for incoming data. Frontend applications can use this to implement custom
 * communication methods.
 * 
 * @param   serial      A pointer to the TM32 Core SCI component.
 * @param   callback    The receive callback function, or `NULL` to disable.
 * @param   userData    User data to pass to the callback function.
 * 
 * @return  `true` if the callback was successfully set; `false` on error.
 */
TM32CORE_API bool TM32Core_SetSerialReceiveCallback (
    TM32Core_Serial*                serial,
    TM32Core_SerialReceiveCallback  callback,
    void*                           userData
);

/**
 * @brief   Sets the external clock callback for the serial component.
 * 
 * This callback will be invoked when the serial component is in external
 * clock mode and needs to check for clock edges. Frontend applications
 * can use this to provide custom clock timing.
 * 
 * @param   serial      A pointer to the TM32 Core SCI component.
 * @param   callback    The clock callback function, or `NULL` to disable.
 * @param   userData    User data to pass to the callback function.
 * 
 * @return  `true` if the callback was successfully set; `false` on error.
 */
TM32CORE_API bool TM32Core_SetSerialClockCallback (
    TM32Core_Serial*                serial,
    TM32Core_SerialClockCallback    callback,
    void*                           userData
);

/**
 * @brief   Injects received data into the serial component.
 * 
 * This function allows frontend applications to inject received data
 * directly into the serial component when using custom communication
 * methods. This is typically used when data arrives asynchronously.
 * 
 * @param   serial  A pointer to the TM32 Core SCI component.
 * @param   data    The byte of data that was received.
 * 
 * @return  `true` if the data was successfully injected; `false` on error.
 */
TM32CORE_API bool TM32Core_InjectSerialData (
    TM32Core_Serial*    serial,
    uint8_t             data
);

/* Example Usage ************************************************************* 
 * 
 * // Example 1: Simple loopback for testing
 * static uint8_t loopbackData = 0xFF;
 * static bool hasLoopbackData = false;
 * 
 * bool transmitLoopback(uint8_t data, void* userData) {
 *     loopbackData = data;
 *     hasLoopbackData = true;
 *     return true;
 * }
 * 
 * bool receiveLoopback(uint8_t* data, void* userData) {
 *     if (hasLoopbackData) {
 *         *data = loopbackData;
 *         hasLoopbackData = false;
 *         return true;
 *     }
 *     return false;
 * }
 * 
 * // Setup: TM32Core_SetSerialTransmitCallback(serial, transmitLoopback, NULL);
 * //        TM32Core_SetSerialReceiveCallback(serial, receiveLoopback, NULL);
 * 
 * // Example 2: Network communication via TCP socket
 * typedef struct {
 *     int socket_fd;
 *     struct sockaddr_in peer_addr;
 * } NetworkContext;
 * 
 * bool transmitNetwork(uint8_t data, void* userData) {
 *     NetworkContext* ctx = (NetworkContext*)userData;
 *     return send(ctx->socket_fd, &data, 1, MSG_DONTWAIT) == 1;
 * }
 * 
 * bool receiveNetwork(uint8_t* data, void* userData) {
 *     NetworkContext* ctx = (NetworkContext*)userData;
 *     return recv(ctx->socket_fd, data, 1, MSG_DONTWAIT) == 1;
 * }
 * 
 * // Example 3: Pipe-based IPC
 * typedef struct {
 *     int read_fd, write_fd;
 * } PipeContext;
 * 
 * bool transmitPipe(uint8_t data, void* userData) {
 *     PipeContext* ctx = (PipeContext*)userData;
 *     return write(ctx->write_fd, &data, 1) == 1;
 * }
 * 
 * bool receivePipe(uint8_t* data, void* userData) {
 *     PipeContext* ctx = (PipeContext*)userData;
 *     return read(ctx->read_fd, data, 1) == 1;
 * }
 * 
 * // Example 4: Asynchronous data injection
 * // When data arrives from network/IPC in a separate thread:
 * // TM32Core_InjectSerialData(serial, received_byte);
 * 
 *****************************************************************************/

#endif // TM32CORE_SERIAL_H
