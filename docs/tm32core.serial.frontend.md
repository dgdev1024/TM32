# TM32 Core Serial Communication Frontend Guide

This document provides comprehensive guidance for frontend application 
developers on implementing serial communication with the TM32 Core library.
The TM32 Core Serial Transfer Component offers a flexible callback system
that enables various communication methods including networking, IPC, and
custom protocols.

## Table of Contents

1. [Overview](#overview)
2. [Callback System Architecture](#callback-system-architecture)
3. [Basic Setup](#basic-setup)
4. [Communication Methods](#communication-methods)
5. [Implementation Examples](#implementation-examples)
6. [Windows Network Integration](#windows-network-integration)
7. [C++ Integration with Third-Party Libraries](#c-integration-with-third-party-libraries)
8. [Best Practices](#best-practices)
9. [Error Handling](#error-handling)
10. [Performance Considerations](#performance-considerations)
11. [Troubleshooting](#troubleshooting)
12. [API Reference](#api-reference)

## Overview

The TM32 Core Serial Transfer Component emulates the Game Boy's serial 
communication interface while providing modern extensibility through a 
callback system. This allows frontend applications to implement serial 
communication using any underlying transport mechanism.

### Key Features

- **Authentic Game Boy Behavior**: Maintains compatibility with original 
  Game Boy serial protocols
- **Flexible Transport**: Support for TCP/UDP, Unix pipes, shared memory, 
  files, and custom protocols
- **Dual Clock Modes**: Internal and external clock synchronization
- **Asynchronous Support**: Non-blocking I/O and data injection
- **Thread Safety**: Safe data injection from multiple threads

### Communication Flow

```
TM32 Program → Serial Component → Callbacks → Frontend → Transport Layer
                                     ↑              ↓
External Device ← Transport Layer ← Frontend ← Callbacks
```

## Callback System Architecture

The TM32 Core Serial Component uses three types of callbacks to interface
with frontend applications:

### Transmit Callback
Called when the serial component needs to send data to an external device.

```c
typedef bool (*TM32Core_SerialTransmitCallback) (
    uint8_t     data,       // Byte to transmit
    void*       userData    // Frontend context
);
```

### Receive Callback  
Called when the serial component needs to check for incoming data.

```c
typedef bool (*TM32Core_SerialReceiveCallback) (
    uint8_t*    data,       // Buffer for received byte
    void*       userData    // Frontend context
);
```

### Clock Callback
Called in external clock mode to check for clock edges.

```c
typedef bool (*TM32Core_SerialClockCallback) (
    void*       userData    // Frontend context
);
```

## Basic Setup

### 1. Initialize Serial Component

```c
#include <tm32core_serial.h>

// Create and initialize the serial component
TM32Core_Serial* serial = TM32Core_CreateSerial(bus);
if (!serial) {
    fprintf(stderr, "Failed to create serial component\n");
    return false;
}
```

### 2. Set Up Callbacks

```c
// Define your communication context
typedef struct {
    int socket_fd;
    bool connected;
    uint8_t receive_buffer[256];
    size_t buffer_pos;
} SerialContext;

SerialContext context = {
    .socket_fd = -1,
    .connected = false,
    .buffer_pos = 0
};

// Set callbacks
TM32Core_SetSerialTransmitCallback(serial, myTransmitFunc, &context);
TM32Core_SetSerialReceiveCallback(serial, myReceiveFunc, &context);
```

### 3. Handle Serial Registers

The TM32 program will interact with the serial component through port 
registers:
- `SB` (0x01): Serial data register
- `SC` (0x02): Serial control register

No additional frontend setup is required for register handling.

## Communication Methods

### TCP/UDP Network Communication

Ideal for networked multiplayer games or remote debugging.

#### TCP Example

```c
typedef struct {
    int server_fd;
    int client_fd;
    struct sockaddr_in address;
    bool is_server;
    bool connected;
} TCPContext;

bool tcpTransmit(uint8_t data, void* userData) {
    TCPContext* ctx = (TCPContext*)userData;
    
    if (!ctx->connected || ctx->client_fd < 0) {
        return false;
    }
    
    ssize_t sent = send(ctx->client_fd, &data, 1, MSG_DONTWAIT);
    return sent == 1;
}

bool tcpReceive(uint8_t* data, void* userData) {
    TCPContext* ctx = (TCPContext*)userData;
    
    if (!ctx->connected || ctx->client_fd < 0) {
        return false;
    }
    
    ssize_t received = recv(ctx->client_fd, data, 1, MSG_DONTWAIT);
    return received == 1;
}

// Setup TCP server
bool setupTCPServer(TCPContext* ctx, int port) {
    ctx->server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (ctx->server_fd < 0) {
        return false;
    }
    
    // Set socket options
    int opt = 1;
    setsockopt(ctx->server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    ctx->address.sin_family = AF_INET;
    ctx->address.sin_addr.s_addr = INADDR_ANY;
    ctx->address.sin_port = htons(port);
    
    if (bind(ctx->server_fd, (struct sockaddr*)&ctx->address, 
             sizeof(ctx->address)) < 0) {
        close(ctx->server_fd);
        return false;
    }
    
    if (listen(ctx->server_fd, 1) < 0) {
        close(ctx->server_fd);
        return false;
    }
    
    ctx->is_server = true;
    return true;
}

// Accept connections (call in main loop)
bool acceptConnection(TCPContext* ctx) {
    if (!ctx->is_server || ctx->connected) {
        return true;
    }
    
    socklen_t addrlen = sizeof(ctx->address);
    ctx->client_fd = accept(ctx->server_fd, (struct sockaddr*)&ctx->address,
                           &addrlen);
    
    if (ctx->client_fd >= 0) {
        ctx->connected = true;
        printf("Client connected\n");
        return true;
    }
    
    return false;
}
```

#### UDP Example

```c
typedef struct {
    int socket_fd;
    struct sockaddr_in local_addr;
    struct sockaddr_in remote_addr;
    socklen_t remote_len;
    bool has_remote;
} UDPContext;

bool udpTransmit(uint8_t data, void* userData) {
    UDPContext* ctx = (UDPContext*)userData;
    
    if (!ctx->has_remote) {
        return false;
    }
    
    ssize_t sent = sendto(ctx->socket_fd, &data, 1, 0,
                         (struct sockaddr*)&ctx->remote_addr,
                         ctx->remote_len);
    return sent == 1;
}

bool udpReceive(uint8_t* data, void* userData) {
    UDPContext* ctx = (UDPContext*)userData;
    
    ssize_t received = recvfrom(ctx->socket_fd, data, 1, MSG_DONTWAIT,
                               (struct sockaddr*)&ctx->remote_addr,
                               &ctx->remote_len);
    
    if (received == 1) {
        ctx->has_remote = true;
        return true;
    }
    
    return false;
}
```

### Unix Pipe Communication

Suitable for inter-process communication and process isolation.

```c
typedef struct {
    int read_fd;
    int write_fd;
    char* pipe_name_in;
    char* pipe_name_out;
} PipeContext;

bool pipeTransmit(uint8_t data, void* userData) {
    PipeContext* ctx = (PipeContext*)userData;
    
    if (ctx->write_fd < 0) {
        // Try to open the output pipe
        ctx->write_fd = open(ctx->pipe_name_out, O_WRONLY | O_NONBLOCK);
        if (ctx->write_fd < 0) {
            return false;
        }
    }
    
    ssize_t written = write(ctx->write_fd, &data, 1);
    return written == 1;
}

bool pipeReceive(uint8_t* data, void* userData) {
    PipeContext* ctx = (PipeContext*)userData;
    
    if (ctx->read_fd < 0) {
        // Try to open the input pipe
        ctx->read_fd = open(ctx->pipe_name_in, O_RDONLY | O_NONBLOCK);
        if (ctx->read_fd < 0) {
            return false;
        }
    }
    
    ssize_t bytes_read = read(ctx->read_fd, data, 1);
    return bytes_read == 1;
}

// Setup named pipes
bool setupPipes(PipeContext* ctx, const char* base_name) {
    // Create pipe names
    asprintf(&ctx->pipe_name_in, "%s_in", base_name);
    asprintf(&ctx->pipe_name_out, "%s_out", base_name);
    
    // Create the named pipes
    if (mkfifo(ctx->pipe_name_in, 0666) != 0 && errno != EEXIST) {
        return false;
    }
    
    if (mkfifo(ctx->pipe_name_out, 0666) != 0 && errno != EEXIST) {
        return false;
    }
    
    ctx->read_fd = -1;
    ctx->write_fd = -1;
    
    return true;
}
```

### Shared Memory Communication

High-performance option for local communication.

```c
typedef struct {
    int shm_fd;
    void* shm_ptr;
    size_t shm_size;
    sem_t* tx_semaphore;
    sem_t* rx_semaphore;
    volatile uint8_t* tx_buffer;
    volatile uint8_t* rx_buffer;
    volatile bool* tx_ready;
    volatile bool* rx_ready;
} ShmContext;

bool shmTransmit(uint8_t data, void* userData) {
    ShmContext* ctx = (ShmContext*)userData;
    
    if (!ctx->shm_ptr) {
        return false;
    }
    
    // Wait for previous transmission to complete
    if (*ctx->tx_ready) {
        return false; // Previous data not yet consumed
    }
    
    *ctx->tx_buffer = data;
    *ctx->tx_ready = true;
    
    // Signal that data is available
    sem_post(ctx->tx_semaphore);
    
    return true;
}

bool shmReceive(uint8_t* data, void* userData) {
    ShmContext* ctx = (ShmContext*)userData;
    
    if (!ctx->shm_ptr || !*ctx->rx_ready) {
        return false;
    }
    
    *data = *ctx->rx_buffer;
    *ctx->rx_ready = false;
    
    return true;
}

// Setup shared memory
bool setupSharedMemory(ShmContext* ctx, const char* name) {
    char shm_name[256];
    snprintf(shm_name, sizeof(shm_name), "/tm32_serial_%s", name);
    
    ctx->shm_size = 4096; // Enough for buffers and control data
    ctx->shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    
    if (ctx->shm_fd == -1) {
        return false;
    }
    
    if (ftruncate(ctx->shm_fd, ctx->shm_size) == -1) {
        close(ctx->shm_fd);
        return false;
    }
    
    ctx->shm_ptr = mmap(NULL, ctx->shm_size, PROT_READ | PROT_WRITE,
                       MAP_SHARED, ctx->shm_fd, 0);
    
    if (ctx->shm_ptr == MAP_FAILED) {
        close(ctx->shm_fd);
        return false;
    }
    
    // Set up pointers within shared memory
    ctx->tx_buffer = (volatile uint8_t*)ctx->shm_ptr;
    ctx->rx_buffer = ctx->tx_buffer + 1;
    ctx->tx_ready = (volatile bool*)(ctx->rx_buffer + 1);
    ctx->rx_ready = ctx->tx_ready + 1;
    
    // Initialize semaphores
    char sem_name[256];
    snprintf(sem_name, sizeof(sem_name), "/tm32_tx_%s", name);
    ctx->tx_semaphore = sem_open(sem_name, O_CREAT, 0666, 0);
    
    snprintf(sem_name, sizeof(sem_name), "/tm32_rx_%s", name);
    ctx->rx_semaphore = sem_open(sem_name, O_CREAT, 0666, 0);
    
    return ctx->tx_semaphore != SEM_FAILED && ctx->rx_semaphore != SEM_FAILED;
}
```

### File-based Communication

Useful for debugging, logging, or simple communication.

```c
typedef struct {
    FILE* tx_file;
    FILE* rx_file;
    char* tx_filename;
    char* rx_filename;
    long rx_position;
} FileContext;

bool fileTransmit(uint8_t data, void* userData) {
    FileContext* ctx = (FileContext*)userData;
    
    if (!ctx->tx_file) {
        ctx->tx_file = fopen(ctx->tx_filename, "ab");
        if (!ctx->tx_file) {
            return false;
        }
    }
    
    if (fwrite(&data, 1, 1, ctx->tx_file) == 1) {
        fflush(ctx->tx_file);
        return true;
    }
    
    return false;
}

bool fileReceive(uint8_t* data, void* userData) {
    FileContext* ctx = (FileContext*)userData;
    
    if (!ctx->rx_file) {
        ctx->rx_file = fopen(ctx->rx_filename, "rb");
        if (!ctx->rx_file) {
            return false;
        }
        fseek(ctx->rx_file, ctx->rx_position, SEEK_SET);
    }
    
    if (fread(data, 1, 1, ctx->rx_file) == 1) {
        ctx->rx_position = ftell(ctx->rx_file);
        return true;
    }
    
    return false;
}
```

## Asynchronous Data Injection

For scenarios where data arrives asynchronously (e.g., from network threads),
use the data injection API:

```c
// In your network receive thread
void* networkReceiveThread(void* arg) {
    SerialContext* ctx = (SerialContext*)arg;
    TM32Core_Serial* serial = ctx->serial;
    
    while (ctx->running) {
        uint8_t data;
        if (recv(ctx->socket_fd, &data, 1, 0) == 1) {
            // Inject data directly into serial component
            TM32Core_InjectSerialData(serial, data);
        }
    }
    
    return NULL;
}

// Start the receive thread
pthread_t receive_thread;
pthread_create(&receive_thread, NULL, networkReceiveThread, &context);
```

## External Clock Implementation

For precise timing control or hardware simulation:

```c
typedef struct {
    bool clock_state;
    uint64_t clock_counter;
    uint64_t clock_frequency; // Hz
    struct timespec last_clock;
} ClockContext;

bool externalClock(void* userData) {
    ClockContext* ctx = (ClockContext*)userData;
    
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    
    // Calculate time difference
    uint64_t ns_diff = (now.tv_sec - ctx->last_clock.tv_sec) * 1000000000ULL +
                       (now.tv_nsec - ctx->last_clock.tv_nsec);
    
    // Check if enough time has passed for a clock edge
    uint64_t ns_per_half_cycle = 500000000ULL / ctx->clock_frequency;
    
    if (ns_diff >= ns_per_half_cycle) {
        ctx->clock_state = !ctx->clock_state;
        ctx->last_clock = now;
        
        // Return true on rising edge
        return ctx->clock_state;
    }
    
    return false;
}

// Setup external clock
ClockContext clock_ctx = {
    .clock_state = false,
    .clock_counter = 0,
    .clock_frequency = 8192, // 8192 Hz clock
};
clock_gettime(CLOCK_MONOTONIC, &clock_ctx.last_clock);

TM32Core_SetSerialClockCallback(serial, externalClock, &clock_ctx);
```

## Windows Network Integration

For Windows-based applications, the TM32 Core Serial component can be 
integrated with Winsock for network communication. This section provides
Windows-specific implementations using the Winsock API.

### Prerequisites

Include the necessary Windows headers and link against the Winsock library:

```c
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
```

### Winsock Initialization

Before using any Winsock functions, initialize the library:

```c
bool initializeWinsock() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    
    if (result != 0) {
        fprintf(stderr, "WSAStartup failed: %d\n", result);
        return false;
    }
    
    return true;
}

void cleanupWinsock() {
    WSACleanup();
}
```

### TCP Implementation with Winsock

```c
typedef struct {
    SOCKET server_socket;
    SOCKET client_socket;
    struct sockaddr_in address;
    bool is_server;
    bool connected;
    char error_buffer[256];
} WinsockTCPContext;

bool winsockTCPTransmit(uint8_t data, void* userData) {
    WinsockTCPContext* ctx = (WinsockTCPContext*)userData;
    
    if (!ctx->connected || ctx->client_socket == INVALID_SOCKET) {
        return false;
    }
    
    int sent = send(ctx->client_socket, (const char*)&data, 1, 0);
    if (sent == SOCKET_ERROR) {
        int error = WSAGetLastError();
        if (error == WSAECONNRESET || error == WSAECONNABORTED) {
            ctx->connected = false;
            closesocket(ctx->client_socket);
            ctx->client_socket = INVALID_SOCKET;
        }
        return false;
    }
    
    return sent == 1;
}

bool winsockTCPReceive(uint8_t* data, void* userData) {
    WinsockTCPContext* ctx = (WinsockTCPContext*)userData;
    
    if (!ctx->connected || ctx->client_socket == INVALID_SOCKET) {
        return false;
    }
    
    // Set socket to non-blocking mode for this operation
    u_long mode = 1;
    ioctlsocket(ctx->client_socket, FIONBIO, &mode);
    
    int received = recv(ctx->client_socket, (char*)data, 1, 0);
    
    if (received == SOCKET_ERROR) {
        int error = WSAGetLastError();
        if (error == WSAEWOULDBLOCK) {
            return false; // No data available
        }
        
        if (error == WSAECONNRESET || error == WSAECONNABORTED) {
            ctx->connected = false;
            closesocket(ctx->client_socket);
            ctx->client_socket = INVALID_SOCKET;
        }
        return false;
    }
    
    return received == 1;
}

bool setupWinsockTCPServer(WinsockTCPContext* ctx, int port) {
    // Create socket
    ctx->server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ctx->server_socket == INVALID_SOCKET) {
        snprintf(ctx->error_buffer, sizeof(ctx->error_buffer),
                "socket failed: %d", WSAGetLastError());
        return false;
    }
    
    // Setup address
    ctx->address.sin_family = AF_INET;
    ctx->address.sin_addr.s_addr = INADDR_ANY;
    ctx->address.sin_port = htons(port);
    
    // Bind socket
    if (bind(ctx->server_socket, (SOCKADDR*)&ctx->address, 
             sizeof(ctx->address)) == SOCKET_ERROR) {
        snprintf(ctx->error_buffer, sizeof(ctx->error_buffer),
                "bind failed: %d", WSAGetLastError());
        closesocket(ctx->server_socket);
        return false;
    }
    
    // Listen for connections
    if (listen(ctx->server_socket, 1) == SOCKET_ERROR) {
        snprintf(ctx->error_buffer, sizeof(ctx->error_buffer),
                "listen failed: %d", WSAGetLastError());
        closesocket(ctx->server_socket);
        return false;
    }
    
    ctx->is_server = true;
    ctx->client_socket = INVALID_SOCKET;
    ctx->connected = false;
    
    return true;
}

bool acceptWinsockConnection(WinsockTCPContext* ctx) {
    if (!ctx->is_server || ctx->connected) {
        return true;
    }
    
    // Set server socket to non-blocking
    u_long mode = 1;
    ioctlsocket(ctx->server_socket, FIONBIO, &mode);
    
    int addrlen = sizeof(ctx->address);
    ctx->client_socket = accept(ctx->server_socket, 
                               (SOCKADDR*)&ctx->address, &addrlen);
    
    if (ctx->client_socket == INVALID_SOCKET) {
        int error = WSAGetLastError();
        if (error != WSAEWOULDBLOCK) {
            snprintf(ctx->error_buffer, sizeof(ctx->error_buffer),
                    "accept failed: %d", error);
            return false;
        }
        return true; // No connection pending
    }
    
    ctx->connected = true;
    printf("Client connected from %s:%d\n", 
           inet_ntoa(ctx->address.sin_addr),
           ntohs(ctx->address.sin_port));
    
    return true;
}

void cleanupWinsockTCP(WinsockTCPContext* ctx) {
    if (ctx->client_socket != INVALID_SOCKET) {
        closesocket(ctx->client_socket);
        ctx->client_socket = INVALID_SOCKET;
    }
    
    if (ctx->server_socket != INVALID_SOCKET) {
        closesocket(ctx->server_socket);
        ctx->server_socket = INVALID_SOCKET;
    }
    
    ctx->connected = false;
}
```

### UDP Implementation with Winsock

```c
typedef struct {
    SOCKET socket;
    struct sockaddr_in local_addr;
    struct sockaddr_in remote_addr;
    int remote_len;
    bool has_remote;
    char error_buffer[256];
} WinsockUDPContext;

bool winsockUDPTransmit(uint8_t data, void* userData) {
    WinsockUDPContext* ctx = (WinsockUDPContext*)userData;
    
    if (!ctx->has_remote || ctx->socket == INVALID_SOCKET) {
        return false;
    }
    
    int sent = sendto(ctx->socket, (const char*)&data, 1, 0,
                     (SOCKADDR*)&ctx->remote_addr, ctx->remote_len);
    
    if (sent == SOCKET_ERROR) {
        int error = WSAGetLastError();
        snprintf(ctx->error_buffer, sizeof(ctx->error_buffer),
                "sendto failed: %d", error);
        return false;
    }
    
    return sent == 1;
}

bool winsockUDPReceive(uint8_t* data, void* userData) {
    WinsockUDPContext* ctx = (WinsockUDPContext*)userData;
    
    if (ctx->socket == INVALID_SOCKET) {
        return false;
    }
    
    // Set non-blocking mode
    u_long mode = 1;
    ioctlsocket(ctx->socket, FIONBIO, &mode);
    
    ctx->remote_len = sizeof(ctx->remote_addr);
    int received = recvfrom(ctx->socket, (char*)data, 1, 0,
                           (SOCKADDR*)&ctx->remote_addr, &ctx->remote_len);
    
    if (received == SOCKET_ERROR) {
        int error = WSAGetLastError();
        if (error == WSAEWOULDBLOCK) {
            return false; // No data available
        }
        
        snprintf(ctx->error_buffer, sizeof(ctx->error_buffer),
                "recvfrom failed: %d", error);
        return false;
    }
    
    if (received == 1) {
        ctx->has_remote = true;
        return true;
    }
    
    return false;
}

bool setupWinsockUDP(WinsockUDPContext* ctx, int port) {
    // Create UDP socket
    ctx->socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (ctx->socket == INVALID_SOCKET) {
        snprintf(ctx->error_buffer, sizeof(ctx->error_buffer),
                "socket failed: %d", WSAGetLastError());
        return false;
    }
    
    // Setup local address
    ctx->local_addr.sin_family = AF_INET;
    ctx->local_addr.sin_addr.s_addr = INADDR_ANY;
    ctx->local_addr.sin_port = htons(port);
    
    // Bind socket
    if (bind(ctx->socket, (SOCKADDR*)&ctx->local_addr,
             sizeof(ctx->local_addr)) == SOCKET_ERROR) {
        snprintf(ctx->error_buffer, sizeof(ctx->error_buffer),
                "bind failed: %d", WSAGetLastError());
        closesocket(ctx->socket);
        ctx->socket = INVALID_SOCKET;
        return false;
    }
    
    ctx->has_remote = false;
    ctx->remote_len = 0;
    
    return true;
}
```

### Windows Named Pipes

For local IPC on Windows, named pipes provide an efficient alternative:

```c
typedef struct {
    HANDLE pipe_handle;
    char pipe_name[256];
    bool is_server;
    bool connected;
    OVERLAPPED overlap;
} NamedPipeContext;

bool namedPipeTransmit(uint8_t data, void* userData) {
    NamedPipeContext* ctx = (NamedPipeContext*)userData;
    
    if (!ctx->connected || ctx->pipe_handle == INVALID_HANDLE_VALUE) {
        return false;
    }
    
    DWORD bytes_written;
    BOOL result = WriteFile(ctx->pipe_handle, &data, 1, &bytes_written, NULL);
    
    if (!result || bytes_written != 1) {
        DWORD error = GetLastError();
        if (error == ERROR_BROKEN_PIPE || error == ERROR_NO_DATA) {
            ctx->connected = false;
        }
        return false;
    }
    
    return true;
}

bool namedPipeReceive(uint8_t* data, void* userData) {
    NamedPipeContext* ctx = (NamedPipeContext*)userData;
    
    if (!ctx->connected || ctx->pipe_handle == INVALID_HANDLE_VALUE) {
        return false;
    }
    
    DWORD bytes_read;
    DWORD bytes_available;
    
    // Check if data is available without blocking
    if (!PeekNamedPipe(ctx->pipe_handle, NULL, 0, NULL, 
                      &bytes_available, NULL)) {
        DWORD error = GetLastError();
        if (error == ERROR_BROKEN_PIPE) {
            ctx->connected = false;
        }
        return false;
    }
    
    if (bytes_available == 0) {
        return false;
    }
    
    BOOL result = ReadFile(ctx->pipe_handle, data, 1, &bytes_read, NULL);
    
    if (!result || bytes_read != 1) {
        DWORD error = GetLastError();
        if (error == ERROR_BROKEN_PIPE) {
            ctx->connected = false;
        }
        return false;
    }
    
    return true;
}

bool setupNamedPipeServer(NamedPipeContext* ctx, const char* pipe_name) {
    snprintf(ctx->pipe_name, sizeof(ctx->pipe_name),
            "\\\\.\\pipe\\%s", pipe_name);
    
    ctx->pipe_handle = CreateNamedPipeA(
        ctx->pipe_name,
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
        1,              // Max instances
        1024,           // Output buffer size
        1024,           // Input buffer size
        0,              // Default timeout
        NULL            // Default security
    );
    
    if (ctx->pipe_handle == INVALID_HANDLE_VALUE) {
        return false;
    }
    
    ctx->is_server = true;
    ctx->connected = false;
    
    return true;
}

bool acceptNamedPipeConnection(NamedPipeContext* ctx) {
    if (!ctx->is_server || ctx->connected) {
        return true;
    }
    
    // Use ConnectNamedPipe in overlapped mode for non-blocking check
    memset(&ctx->overlap, 0, sizeof(ctx->overlap));
    ctx->overlap.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    
    BOOL result = ConnectNamedPipe(ctx->pipe_handle, &ctx->overlap);
    DWORD error = GetLastError();
    
    if (result) {
        ctx->connected = true;
        CloseHandle(ctx->overlap.hEvent);
        return true;
    }
    
    if (error == ERROR_PIPE_CONNECTED) {
        ctx->connected = true;
        CloseHandle(ctx->overlap.hEvent);
        return true;
    }
    
    if (error == ERROR_IO_PENDING) {
        // Check if connection completed
        DWORD wait_result = WaitForSingleObject(ctx->overlap.hEvent, 0);
        if (wait_result == WAIT_OBJECT_0) {
            ctx->connected = true;
        }
        CloseHandle(ctx->overlap.hEvent);
        return true;
    }
    
    CloseHandle(ctx->overlap.hEvent);
    return false;
}
```

## C++ Integration with Third-Party Libraries

The TM32 Core can be seamlessly integrated with modern C++ libraries for 
enhanced networking capabilities. This section demonstrates integration with
popular libraries like Boost.Asio and standalone asio.

### Boost.Asio Integration

Boost.Asio provides excellent asynchronous I/O capabilities for C++ 
applications. Here's how to integrate it with TM32 Core:

#### Prerequisites

```cpp
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <memory>
#include <queue>
#include <mutex>
#include <thread>

extern "C" {
    #include <tm32core_serial.h>
}

using boost::asio::ip::tcp;
using boost::asio::ip::udp;
```

#### TCP Server with Boost.Asio

```cpp
class AsioTCPSerial {
private:
    boost::asio::io_context& io_context_;
    tcp::acceptor acceptor_;
    std::unique_ptr<tcp::socket> socket_;
    TM32Core_Serial* serial_;
    
    std::queue<uint8_t> tx_queue_;
    std::queue<uint8_t> rx_queue_;
    std::mutex tx_mutex_;
    std::mutex rx_mutex_;
    
    bool connected_;
    std::array<uint8_t, 1024> rx_buffer_;

public:
    AsioTCPSerial(boost::asio::io_context& io_context, 
                  TM32Core_Serial* serial, int port)
        : io_context_(io_context)
        , acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
        , serial_(serial)
        , connected_(false) {
        
        // Set up TM32 Core callbacks
        TM32Core_SetSerialTransmitCallback(serial_, transmitCallback, this);
        TM32Core_SetSerialReceiveCallback(serial_, receiveCallback, this);
        
        startAccept();
    }
    
    ~AsioTCPSerial() {
        if (socket_) {
            socket_->close();
        }
        acceptor_.close();
    }

private:
    void startAccept() {
        socket_ = std::make_unique<tcp::socket>(io_context_);
        
        acceptor_.async_accept(*socket_,
            [this](const boost::system::error_code& error) {
                if (!error) {
                    connected_ = true;
                    std::cout << "Client connected: " 
                             << socket_->remote_endpoint() << std::endl;
                    startReceive();
                } else {
                    std::cerr << "Accept error: " << error.message() << std::endl;
                    startAccept(); // Continue accepting
                }
            });
    }
    
    void startReceive() {
        if (!connected_ || !socket_) {
            return;
        }
        
        socket_->async_read_some(
            boost::asio::buffer(rx_buffer_),
            [this](const boost::system::error_code& error, 
                   std::size_t bytes_transferred) {
                if (!error) {
                    // Queue received data
                    {
                        std::lock_guard<std::mutex> lock(rx_mutex_);
                        for (std::size_t i = 0; i < bytes_transferred; ++i) {
                            rx_queue_.push(rx_buffer_[i]);
                        }
                    }
                    
                    startReceive(); // Continue receiving
                } else {
                    std::cerr << "Receive error: " << error.message() << std::endl;
                    handleDisconnection();
                }
            });
    }
    
    void handleDisconnection() {
        connected_ = false;
        if (socket_) {
            socket_->close();
            socket_.reset();
        }
        
        // Clear queues
        {
            std::lock_guard<std::mutex> lock(tx_mutex_);
            std::queue<uint8_t> empty_tx;
            tx_queue_.swap(empty_tx);
        }
        {
            std::lock_guard<std::mutex> lock(rx_mutex_);
            std::queue<uint8_t> empty_rx;
            rx_queue_.swap(empty_rx);
        }
        
        // Start accepting new connections
        startAccept();
    }
    
    void asyncSend(uint8_t data) {
        if (!connected_ || !socket_) {
            return;
        }
        
        auto buffer = std::make_shared<std::array<uint8_t, 1>>();
        (*buffer)[0] = data;
        
        boost::asio::async_write(*socket_,
            boost::asio::buffer(*buffer),
            [this, buffer](const boost::system::error_code& error,
                          std::size_t bytes_transferred) {
                if (error) {
                    std::cerr << "Send error: " << error.message() << std::endl;
                    handleDisconnection();
                }
            });
    }
    
    // Static callback functions for TM32 Core
    static bool transmitCallback(uint8_t data, void* userData) {
        auto* self = static_cast<AsioTCPSerial*>(userData);
        
        if (!self->connected_) {
            return false;
        }
        
        // Queue data for asynchronous transmission
        {
            std::lock_guard<std::mutex> lock(self->tx_mutex_);
            self->tx_queue_.push(data);
        }
        
        // Process transmission asynchronously
        boost::asio::post(self->io_context_, 
            [self]() {
                std::lock_guard<std::mutex> lock(self->tx_mutex_);
                if (!self->tx_queue_.empty()) {
                    uint8_t data = self->tx_queue_.front();
                    self->tx_queue_.pop();
                    self->asyncSend(data);
                }
            });
        
        return true;
    }
    
    static bool receiveCallback(uint8_t* data, void* userData) {
        auto* self = static_cast<AsioTCPSerial*>(userData);
        
        std::lock_guard<std::mutex> lock(self->rx_mutex_);
        if (self->rx_queue_.empty()) {
            return false;
        }
        
        *data = self->rx_queue_.front();
        self->rx_queue_.pop();
        
        return true;
    }
};

// Usage example
void runAsioTCPExample() {
    // Initialize TM32 Core
    TM32Core_Bus* bus = TM32Core_CreateBus();
    TM32Core_Serial* serial = TM32Core_CreateSerial(bus);
    
    // Create Asio context and TCP server
    boost::asio::io_context io_context;
    AsioTCPSerial tcp_serial(io_context, serial, 12345);
    
    // Run in separate thread
    std::thread io_thread([&io_context]() {
        io_context.run();
    });
    
    // Your main application loop here
    // ...
    
    io_context.stop();
    io_thread.join();
    
    // Cleanup
    TM32Core_DestroySerial(serial);
    TM32Core_DestroyBus(bus);
}
```

#### UDP Communication with Boost.Asio

```cpp
class AsioUDPSerial {
private:
    boost::asio::io_context& io_context_;
    udp::socket socket_;
    udp::endpoint remote_endpoint_;
    TM32Core_Serial* serial_;
    
    std::queue<uint8_t> rx_queue_;
    std::mutex rx_mutex_;
    
    bool has_remote_;
    std::array<uint8_t, 1024> rx_buffer_;

public:
    AsioUDPSerial(boost::asio::io_context& io_context,
                  TM32Core_Serial* serial, int port)
        : io_context_(io_context)
        , socket_(io_context, udp::endpoint(udp::v4(), port))
        , serial_(serial)
        , has_remote_(false) {
        
        TM32Core_SetSerialTransmitCallback(serial_, transmitCallback, this);
        TM32Core_SetSerialReceiveCallback(serial_, receiveCallback, this);
        
        startReceive();
    }

private:
    void startReceive() {
        socket_.async_receive_from(
            boost::asio::buffer(rx_buffer_), remote_endpoint_,
            [this](const boost::system::error_code& error,
                   std::size_t bytes_transferred) {
                if (!error) {
                    has_remote_ = true;
                    
                    // Queue received data
                    {
                        std::lock_guard<std::mutex> lock(rx_mutex_);
                        for (std::size_t i = 0; i < bytes_transferred; ++i) {
                            rx_queue_.push(rx_buffer_[i]);
                        }
                    }
                    
                    startReceive(); // Continue receiving
                } else {
                    std::cerr << "UDP receive error: " << error.message() << std::endl;
                }
            });
    }
    
    void asyncSend(uint8_t data) {
        if (!has_remote_) {
            return;
        }
        
        auto buffer = std::make_shared<std::array<uint8_t, 1>>();
        (*buffer)[0] = data;
        
        socket_.async_send_to(
            boost::asio::buffer(*buffer), remote_endpoint_,
            [this, buffer](const boost::system::error_code& error,
                          std::size_t bytes_transferred) {
                if (error) {
                    std::cerr << "UDP send error: " << error.message() << std::endl;
                }
            });
    }
    
    static bool transmitCallback(uint8_t data, void* userData) {
        auto* self = static_cast<AsioUDPSerial*>(userData);
        
        if (!self->has_remote_) {
            return false;
        }
        
        boost::asio::post(self->io_context_,
            [self, data]() {
                self->asyncSend(data);
            });
        
        return true;
    }
    
    static bool receiveCallback(uint8_t* data, void* userData) {
        auto* self = static_cast<AsioUDPSerial*>(userData);
        
        std::lock_guard<std::mutex> lock(self->rx_mutex_);
        if (self->rx_queue_.empty()) {
            return false;
        }
        
        *data = self->rx_queue_.front();
        self->rx_queue_.pop();
        
        return true;
    }
};
```

### Standalone Asio Integration

For projects not using Boost, standalone asio provides similar functionality:

```cpp
#include <asio.hpp>
#include <memory>
#include <queue>
#include <mutex>

extern "C" {
    #include <tm32core_serial.h>
}

using asio::ip::tcp;

class StandaloneAsioSerial {
private:
    asio::io_context& io_context_;
    tcp::acceptor acceptor_;
    std::shared_ptr<tcp::socket> socket_;
    TM32Core_Serial* serial_;
    
    std::queue<uint8_t> tx_queue_;
    std::queue<uint8_t> rx_queue_;
    std::mutex queue_mutex_;
    
    bool connected_;
    std::array<char, 1024> rx_buffer_;

public:
    StandaloneAsioSerial(asio::io_context& io_context,
                        TM32Core_Serial* serial, int port)
        : io_context_(io_context)
        , acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
        , serial_(serial)
        , connected_(false) {
        
        TM32Core_SetSerialTransmitCallback(serial_, transmitCallback, this);
        TM32Core_SetSerialReceiveCallback(serial_, receiveCallback, this);
        
        startAccept();
    }

private:
    void startAccept() {
        socket_ = std::make_shared<tcp::socket>(io_context_);
        
        acceptor_.async_accept(*socket_,
            [this](std::error_code ec) {
                if (!ec) {
                    connected_ = true;
                    std::cout << "Client connected" << std::endl;
                    startRead();
                }
                startAccept(); // Continue accepting
            });
    }
    
    void startRead() {
        if (!connected_ || !socket_) {
            return;
        }
        
        socket_->async_read_some(asio::buffer(rx_buffer_),
            [this](std::error_code ec, std::size_t length) {
                if (!ec) {
                    {
                        std::lock_guard<std::mutex> lock(queue_mutex_);
                        for (std::size_t i = 0; i < length; ++i) {
                            rx_queue_.push(static_cast<uint8_t>(rx_buffer_[i]));
                        }
                    }
                    startRead();
                } else {
                    connected_ = false;
                    socket_.reset();
                    startAccept();
                }
            });
    }
    
    void asyncWrite(uint8_t data) {
        if (!connected_ || !socket_) {
            return;
        }
        
        auto buffer = std::make_shared<std::array<char, 1>>();
        (*buffer)[0] = static_cast<char>(data);
        
        asio::async_write(*socket_, asio::buffer(*buffer),
            [this, buffer](std::error_code ec, std::size_t /*length*/) {
                if (ec) {
                    connected_ = false;
                    socket_.reset();
                }
            });
    }
    
    static bool transmitCallback(uint8_t data, void* userData) {
        auto* self = static_cast<StandaloneAsioSerial*>(userData);
        
        if (!self->connected_) {
            return false;
        }
        
        asio::post(self->io_context_,
            [self, data]() {
                self->asyncWrite(data);
            });
        
        return true;
    }
    
    static bool receiveCallback(uint8_t* data, void* userData) {
        auto* self = static_cast<StandaloneAsioSerial*>(userData);
        
        std::lock_guard<std::mutex> lock(self->queue_mutex_);
        if (self->rx_queue_.empty()) {
            return false;
        }
        
        *data = self->rx_queue_.front();
        self->rx_queue_.pop();
        
        return true;
    }
};
```

### WebSocket Integration with Beast

For web-based frontends, WebSocket communication can be implemented using 
Boost.Beast:

```cpp
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = net::ip::tcp;

class WebSocketSerial {
private:
    net::io_context& io_context_;
    tcp::acceptor acceptor_;
    std::unique_ptr<websocket::stream<tcp::socket>> ws_;
    TM32Core_Serial* serial_;
    
    std::queue<uint8_t> rx_queue_;
    std::mutex rx_mutex_;
    bool connected_;
    
    beast::flat_buffer buffer_;

public:
    WebSocketSerial(net::io_context& io_context,
                   TM32Core_Serial* serial, int port)
        : io_context_(io_context)
        , acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
        , serial_(serial)
        , connected_(false) {
        
        TM32Core_SetSerialTransmitCallback(serial_, transmitCallback, this);
        TM32Core_SetSerialReceiveCallback(serial_, receiveCallback, this);
        
        startAccept();
    }

private:
    void startAccept() {
        auto socket = std::make_unique<tcp::socket>(io_context_);
        
        acceptor_.async_accept(*socket,
            [this, socket = std::move(socket)](beast::error_code ec) mutable {
                if (!ec) {
                    ws_ = std::make_unique<websocket::stream<tcp::socket>>(
                        std::move(*socket));
                    
                    // Accept WebSocket handshake
                    ws_->async_accept(
                        [this](beast::error_code ec) {
                            if (!ec) {
                                connected_ = true;
                                std::cout << "WebSocket client connected" << std::endl;
                                startRead();
                            }
                        });
                }
                startAccept(); // Continue accepting
            });
    }
    
    void startRead() {
        if (!connected_ || !ws_) {
            return;
        }
        
        ws_->async_read(buffer_,
            [this](beast::error_code ec, std::size_t bytes_transferred) {
                if (!ec) {
                    // Process received data
                    auto data = buffer_.data();
                    {
                        std::lock_guard<std::mutex> lock(rx_mutex_);
                        for (auto it = net::buffers_begin(data);
                             it != net::buffers_end(data); ++it) {
                            rx_queue_.push(static_cast<uint8_t>(*it));
                        }
                    }
                    
                    buffer_.clear();
                    startRead();
                } else {
                    connected_ = false;
                    ws_.reset();
                    startAccept();
                }
            });
    }
    
    void asyncSend(uint8_t data) {
        if (!connected_ || !ws_) {
            return;
        }
        
        auto buffer = std::make_shared<std::string>(1, static_cast<char>(data));
        
        ws_->async_write(net::buffer(*buffer),
            [this, buffer](beast::error_code ec, std::size_t bytes_transferred) {
                if (ec) {
                    connected_ = false;
                    ws_.reset();
                }
            });
    }
    
    static bool transmitCallback(uint8_t data, void* userData) {
        auto* self = static_cast<WebSocketSerial*>(userData);
        
        if (!self->connected_) {
            return false;
        }
        
        net::post(self->io_context_,
            [self, data]() {
                self->asyncSend(data);
            });
        
        return true;
    }
    
    static bool receiveCallback(uint8_t* data, void* userData) {
        auto* self = static_cast<WebSocketSerial*>(userData);
        
        std::lock_guard<std::mutex> lock(self->rx_mutex_);
        if (self->rx_queue_.empty()) {
            return false;
        }
        
        *data = self->rx_queue_.front();
        self->rx_queue_.pop();
        
        return true;
    }
};
```

### Modern C++ Best Practices

When integrating with C++, consider these modern practices:

#### RAII Resource Management

```cpp
class SerialConnectionManager {
private:
    TM32Core_Serial* serial_;
    std::unique_ptr<AsioTCPSerial> tcp_connection_;
    
public:
    SerialConnectionManager(TM32Core_Bus* bus) {
        serial_ = TM32Core_CreateSerial(bus);
        if (!serial_) {
            throw std::runtime_error("Failed to create serial component");
        }
    }
    
    ~SerialConnectionManager() {
        tcp_connection_.reset(); // Cleanup connection first
        
        if (serial_) {
            TM32Core_DestroySerial(serial_);
        }
    }
    
    // Non-copyable, movable
    SerialConnectionManager(const SerialConnectionManager&) = delete;
    SerialConnectionManager& operator=(const SerialConnectionManager&) = delete;
    
    SerialConnectionManager(SerialConnectionManager&& other) noexcept
        : serial_(std::exchange(other.serial_, nullptr))
        , tcp_connection_(std::move(other.tcp_connection_)) {
    }
    
    SerialConnectionManager& operator=(SerialConnectionManager&& other) noexcept {
        if (this != &other) {
            tcp_connection_.reset();
            if (serial_) {
                TM32Core_DestroySerial(serial_);
            }
            
            serial_ = std::exchange(other.serial_, nullptr);
            tcp_connection_ = std::move(other.tcp_connection_);
        }
        return *this;
    }
    
    void createTCPConnection(boost::asio::io_context& io_context, int port) {
        tcp_connection_ = std::make_unique<AsioTCPSerial>(
            io_context, serial_, port);
    }
};
```

#### Template-Based Generic Serial Interface

```cpp
template<typename TransportT>
class GenericSerialTransport {
private:
    TM32Core_Serial* serial_;
    std::unique_ptr<TransportT> transport_;
    
public:
    template<typename... Args>
    GenericSerialTransport(TM32Core_Serial* serial, Args&&... args)
        : serial_(serial) {
        transport_ = std::make_unique<TransportT>(
            serial_, std::forward<Args>(args)...);
    }
    
    TransportT& getTransport() { return *transport_; }
    const TransportT& getTransport() const { return *transport_; }
};

// Usage
using TCPSerial = GenericSerialTransport<AsioTCPSerial>;
using UDPSerial = GenericSerialTransport<AsioUDPSerial>;
using WSSerial = GenericSerialTransport<WebSocketSerial>;

// Create different transport types
auto tcp_serial = TCPSerial(serial, io_context, 12345);
auto udp_serial = UDPSerial(serial, io_context, 12346);
auto ws_serial = WSSerial(serial, io_context, 8080);
```

## Best Practices

### 1. Error Handling

Always check return values and handle errors gracefully:

```c
bool myTransmit(uint8_t data, void* userData) {
    MyContext* ctx = (MyContext*)userData;
    
    // Validate context
    if (!ctx || !ctx->initialized) {
        return false;
    }
    
    // Attempt reconnection if needed
    if (!ctx->connected) {
        if (!attemptReconnection(ctx)) {
            return false;
        }
    }
    
    // Try transmission with error handling
    int result = sendData(ctx, data);
    if (result < 0) {
        if (errno == EPIPE || errno == ECONNRESET) {
            ctx->connected = false;
        }
        return false;
    }
    
    return true;
}
```

### 2. Resource Management

Properly manage resources and clean up:

```c
void cleanupSerialContext(SerialContext* ctx) {
    if (ctx->socket_fd >= 0) {
        close(ctx->socket_fd);
        ctx->socket_fd = -1;
    }
    
    if (ctx->pipe_name_in) {
        free(ctx->pipe_name_in);
        ctx->pipe_name_in = NULL;
    }
    
    if (ctx->pipe_name_out) {
        free(ctx->pipe_name_out);
        ctx->pipe_name_out = NULL;
    }
    
    // Clear callbacks to prevent use after free
    TM32Core_SetSerialTransmitCallback(ctx->serial, NULL, NULL);
    TM32Core_SetSerialReceiveCallback(ctx->serial, NULL, NULL);
}
```

### 3. Thread Safety

When using multiple threads, ensure proper synchronization:

```c
typedef struct {
    pthread_mutex_t mutex;
    bool data_available;
    uint8_t pending_data;
} ThreadSafeContext;

bool threadSafeReceive(uint8_t* data, void* userData) {
    ThreadSafeContext* ctx = (ThreadSafeContext*)userData;
    
    pthread_mutex_lock(&ctx->mutex);
    
    bool result = false;
    if (ctx->data_available) {
        *data = ctx->pending_data;
        ctx->data_available = false;
        result = true;
    }
    
    pthread_mutex_unlock(&ctx->mutex);
    
    return result;
}

// From another thread
void injectDataFromThread(ThreadSafeContext* ctx, uint8_t data) {
    pthread_mutex_lock(&ctx->mutex);
    
    if (!ctx->data_available) {
        ctx->pending_data = data;
        ctx->data_available = true;
    }
    
    pthread_mutex_unlock(&ctx->mutex);
}
```

### 4. Performance Optimization

For high-performance scenarios:

```c
// Use buffering to reduce system call overhead
typedef struct {
    uint8_t tx_buffer[1024];
    uint8_t rx_buffer[1024];
    size_t tx_count;
    size_t rx_count;
    size_t rx_pos;
} BufferedContext;

bool bufferedTransmit(uint8_t data, void* userData) {
    BufferedContext* ctx = (BufferedContext*)userData;
    
    ctx->tx_buffer[ctx->tx_count++] = data;
    
    // Flush when buffer is full or on specific conditions
    if (ctx->tx_count >= sizeof(ctx->tx_buffer)) {
        return flushTransmitBuffer(ctx);
    }
    
    return true;
}

bool bufferedReceive(uint8_t* data, void* userData) {
    BufferedContext* ctx = (BufferedContext*)userData;
    
    // Refill buffer if empty
    if (ctx->rx_pos >= ctx->rx_count) {
        ctx->rx_count = fillReceiveBuffer(ctx);
        ctx->rx_pos = 0;
        
        if (ctx->rx_count == 0) {
            return false;
        }
    }
    
    *data = ctx->rx_buffer[ctx->rx_pos++];
    return true;
}
```

## Error Handling

### Common Error Scenarios

1. **Connection Lost**: Handle network disconnections gracefully
2. **Resource Exhaustion**: Handle file descriptor limits, memory issues
3. **Permission Denied**: Handle access permission problems
4. **Invalid Data**: Validate received data when necessary

### Error Recovery Strategies

```c
typedef struct {
    time_t last_error_time;
    int consecutive_errors;
    bool in_recovery_mode;
} ErrorRecoveryContext;

bool resilientTransmit(uint8_t data, void* userData) {
    MyContext* ctx = (MyContext*)userData;
    ErrorRecoveryContext* recovery = &ctx->recovery;
    
    bool success = attemptTransmit(ctx, data);
    
    if (success) {
        // Reset error tracking on success
        recovery->consecutive_errors = 0;
        recovery->in_recovery_mode = false;
        return true;
    }
    
    // Handle error
    time_t now = time(NULL);
    recovery->consecutive_errors++;
    recovery->last_error_time = now;
    
    // Implement exponential backoff
    if (recovery->consecutive_errors > 3) {
        recovery->in_recovery_mode = true;
        
        // Wait before retrying
        if (now - recovery->last_error_time < (1 << recovery->consecutive_errors)) {
            return false;
        }
    }
    
    // Attempt recovery
    if (recovery->in_recovery_mode) {
        if (attemptRecovery(ctx)) {
            recovery->in_recovery_mode = false;
            recovery->consecutive_errors = 0;
            return attemptTransmit(ctx, data);
        }
    }
    
    return false;
}
```

## Performance Considerations

### Callback Frequency

Serial callbacks are invoked frequently during transfers:
- Internal clock mode: Every 128-512 CPU cycles per bit
- External clock mode: As fast as clock edges are detected

Optimize callback functions for minimal latency:

```c
// Fast path - avoid system calls when possible
bool optimizedReceive(uint8_t* data, void* userData) {
    FastContext* ctx = (FastContext*)userData;
    
    // Check local buffer first (fast)
    if (ctx->buffer_pos < ctx->buffer_size) {
        *data = ctx->buffer[ctx->buffer_pos++];
        return true;
    }
    
    // Refill buffer (slower system call)
    return refillBuffer(ctx, data);
}
```

### Memory Usage

Keep context structures small and cache-friendly:

```c
// Efficient context structure
typedef struct {
    int fd;                    // 4 bytes
    uint16_t buffer_pos;       // 2 bytes
    uint16_t buffer_size;      // 2 bytes
    uint8_t buffer[64];        // 64 bytes - cache line friendly
    // Total: 72 bytes - fits in cache line
} EfficientContext;
```

### Batching Operations

For high-throughput scenarios, consider batching:

```c
void processBatchedOperations(MyContext* ctx) {
    // Process accumulated transmit data
    if (ctx->tx_batch_count > 0) {
        sendBatch(ctx->socket_fd, ctx->tx_batch, ctx->tx_batch_count);
        ctx->tx_batch_count = 0;
    }
    
    // Receive data in batches
    size_t received = receiveBatch(ctx->socket_fd, ctx->rx_batch, 
                                  sizeof(ctx->rx_batch));
    for (size_t i = 0; i < received; i++) {
        TM32Core_InjectSerialData(ctx->serial, ctx->rx_batch[i]);
    }
}
```

## Troubleshooting

### Common Issues

#### 1. No Data Transmission
**Symptoms**: Transmit callback not called
**Causes**: 
- Serial transfer not initiated by TM32 program
- SC register not configured correctly
- Transfer already in progress

**Solutions**:
```c
// Check SC register state
uint8_t sc = TM32Core_ReadSC(serial);
printf("SC register: 0x%02X\n", sc);
printf("Transfer start: %s\n", (sc & 0x80) ? "Yes" : "No");
printf("Clock source: %s\n", (sc & 0x01) ? "Internal" : "External");
```

#### 2. Data Not Received
**Symptoms**: Receive callback returns false or not called
**Causes**:
- No data available from source
- External clock not working
- Callback returning incorrect value

**Solutions**:
```c
// Debug receive callback
bool debugReceive(uint8_t* data, void* userData) {
    static int call_count = 0;
    printf("Receive callback called: %d\n", ++call_count);
    
    bool result = originalReceive(data, userData);
    if (result) {
        printf("Received data: 0x%02X\n", *data);
    } else {
        printf("No data available\n");
    }
    
    return result;
}
```

#### 3. Connection Issues
**Symptoms**: Intermittent failures, connection drops
**Causes**:
- Network instability
- Insufficient error handling
- Resource leaks

**Solutions**:
```c
// Implement connection monitoring
void monitorConnection(MyContext* ctx) {
    if (ctx->last_successful_tx + TIMEOUT_SECONDS < time(NULL)) {
        printf("Connection timeout detected\n");
        reconnect(ctx);
    }
    
    // Send periodic keepalive
    if (ctx->last_keepalive + KEEPALIVE_INTERVAL < time(NULL)) {
        sendKeepalive(ctx);
        ctx->last_keepalive = time(NULL);
    }
}
```

### Debugging Tools

#### 1. Callback Tracing

```c
// Wrapper for debugging callbacks
bool tracingTransmit(uint8_t data, void* userData) {
    TracingContext* trace_ctx = (TracingContext*)userData;
    
    printf("[TRACE] TX: 0x%02X at %lu\n", data, getCurrentTimestamp());
    
    bool result = trace_ctx->original_callback(data, trace_ctx->original_data);
    
    printf("[TRACE] TX result: %s\n", result ? "SUCCESS" : "FAILURE");
    
    return result;
}
```

#### 2. Data Logging

```c
void logSerialData(const char* direction, uint8_t data) {
    static FILE* log_file = NULL;
    
    if (!log_file) {
        log_file = fopen("serial_log.txt", "w");
    }
    
    if (log_file) {
        fprintf(log_file, "%lu %s 0x%02X\n", 
                getCurrentTimestamp(), direction, data);
        fflush(log_file);
    }
}
```

#### 3. State Monitoring

```c
void printSerialState(TM32Core_Serial* serial) {
    uint8_t sb = TM32Core_ReadSB(serial);
    uint8_t sc = TM32Core_ReadSC(serial);
    
    printf("=== Serial State ===\n");
    printf("SB (Data): 0x%02X (%d)\n", sb, sb);
    printf("SC (Control): 0x%02X\n", sc);
    printf("  Transfer Start: %s\n", (sc & 0x80) ? "Active" : "Inactive");
    printf("  Clock Speed: %s\n", (sc & 0x02) ? "Fast" : "Normal");
    printf("  Clock Source: %s\n", (sc & 0x01) ? "Internal" : "External");
    printf("===================\n");
}
```

## API Reference

### Core Functions

#### TM32Core_SetSerialTransmitCallback
```c
bool TM32Core_SetSerialTransmitCallback(
    TM32Core_Serial*                serial,
    TM32Core_SerialTransmitCallback callback,
    void*                           userData
);
```
Sets the callback function for data transmission.

**Parameters**:
- `serial`: Pointer to serial component
- `callback`: Transmit callback function or NULL to disable
- `userData`: User data passed to callback

**Returns**: `true` on success, `false` on error

#### TM32Core_SetSerialReceiveCallback
```c
bool TM32Core_SetSerialReceiveCallback(
    TM32Core_Serial*                serial,
    TM32Core_SerialReceiveCallback  callback,
    void*                           userData
);
```
Sets the callback function for data reception.

**Parameters**:
- `serial`: Pointer to serial component
- `callback`: Receive callback function or NULL to disable
- `userData`: User data passed to callback

**Returns**: `true` on success, `false` on error

#### TM32Core_SetSerialClockCallback
```c
bool TM32Core_SetSerialClockCallback(
    TM32Core_Serial*                serial,
    TM32Core_SerialClockCallback    callback,
    void*                           userData
);
```
Sets the callback function for external clock edges.

**Parameters**:
- `serial`: Pointer to serial component
- `callback`: Clock callback function or NULL to disable
- `userData`: User data passed to callback

**Returns**: `true` on success, `false` on error

#### TM32Core_InjectSerialData
```c
bool TM32Core_InjectSerialData(
    TM32Core_Serial*    serial,
    uint8_t             data
);
```
Injects received data directly into the serial component.

**Parameters**:
- `serial`: Pointer to serial component
- `data`: Byte of data to inject

**Returns**: `true` on success, `false` on error

### Callback Types

#### TM32Core_SerialTransmitCallback
```c
typedef bool (*TM32Core_SerialTransmitCallback)(
    uint8_t     data,
    void*       userData
);
```
Called when data needs to be transmitted.

**Parameters**:
- `data`: Byte to transmit
- `userData`: User context data

**Returns**: `true` if data was queued for transmission, `false` on error

#### TM32Core_SerialReceiveCallback
```c
typedef bool (*TM32Core_SerialReceiveCallback)(
    uint8_t*    data,
    void*       userData
);
```
Called when the component needs to check for received data.

**Parameters**:
- `data`: Pointer to store received byte
- `userData`: User context data

**Returns**: `true` if data was available and stored, `false` if no data

#### TM32Core_SerialClockCallback
```c
typedef bool (*TM32Core_SerialClockCallback)(
    void*       userData
);
```
Called in external clock mode to check for clock edges.

**Parameters**:
- `userData`: User context data

**Returns**: `true` if clock edge detected, `false` otherwise

### Register Access

The following functions are available for direct register access (typically
handled automatically by the bus system):

- `TM32Core_ReadSB()`: Read serial data register
- `TM32Core_WriteSB()`: Write serial data register  
- `TM32Core_ReadSC()`: Read serial control register
- `TM32Core_WriteSC()`: Write serial control register

## Conclusion

The TM32 Core Serial Transfer Component provides a flexible and powerful
foundation for implementing serial communication in frontend applications.
By leveraging the callback system, developers can implement communication
using any transport mechanism while maintaining compatibility with authentic
Game Boy serial protocols.

Key takeaways:
- Choose the appropriate transport method for your use case
- Implement robust error handling and recovery
- Optimize callback performance for high-frequency operations
- Use data injection for asynchronous communication patterns
- Monitor and debug communication issues systematically

For additional support and examples, refer to the TM32 Core documentation
and community resources.
