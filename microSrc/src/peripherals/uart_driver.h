#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include "driver/uart.h"
#include "freertos/FreeRTOS.h" // TickType_t
#include "freertos/queue.h"
#include "esp_err.h"
#include <string.h>
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/task.h"

#define UART_BAUD_RATE 115200
#define UART_RX_BUFFER_SIZE 2048
#define UART_TX_BUFFER_SIZE 2048
#define UART_TICKS_TO_WAIT pdMS_TO_TICKS(1000)
#define UART_RX_MAX_LENGTH 256

#define UART0_RX_PIN    GPIO_NUM_3
#define UART0_TX_PIN    GPIO_NUM_1

#define UART1_RX_PIN    GPIO_NUM_9
#define UART1_TX_PIN    GPIO_NUM_10
#define UART1_RTS_PIN   GPIO_NUM_11
#define UART1_CTS_PIN   GPIO_NUM_6

#define UART2_RX_PIN    GPIO_NUM_16
#define UART2_TX_PIN    GPIO_NUM_17
#define UART2_RTS_PIN   GPIO_NUM_7
#define UART2_CTS_PIN   GPIO_NUM_6

// Typedef for UART function ouput
typedef enum uart_error_t{
    UART_ERROR_NONE = 0,
    UART_ERROR_DRIVER_NOT_INSTALLED = -1,
    UART_ERROR_WRITE_FAILED = -2,
    UART_ERROR_NO_BYTES_TO_READ = -3
} uart_error_t;

// Basic init/deinit
void uart_init(const uart_port_t uartPort);
void uart_kill(const uart_port_t uartPort);

// Standard read/write helpers
uart_error_t uart_write_bytes_blocking(const uart_port_t uartPort, const uint8_t *data);
uart_error_t uart_read_bytes_blocking(const uart_port_t uartPort, uint8_t *buf);

// RS485 (half duplex) Specific helpers 
esp_err_t uart_rs485_init(const uart_port_t uartPort);
esp_err_t uart_rs485_write(const uart_port_t uartPort, const uint8_t *data);
int uart_rs485_read(const uart_port_t uartPort, uint8_t *buf);

#endif // UART_DRIVER_H