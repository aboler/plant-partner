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
#define UART_TICKS_TO_WAIT pdMS_TO_TICKS(1000)

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

//Enum
typedef enum UART_ERROR{
    UART_ERROR_NONE = 0,
    UART_ERROR_DRIVER_NOT_INSTALLED = -1,
    UART_ERROR_WRITE_FAILED = -2,
}

// Basic init/deinit
void uart_init(const uart_port_t uartPort);
void uart_kill(const uart_port_t uartPort);

// Generic read/write helpers
int uart_write_bytes_blocking(const uart_port_t uartPort, const uint8_t *data);
int uart_read_bytes_blocking(const uart_port_t uartPort, uint8_t *buf, size_t maxlen, TickType_t ticks_to_wait);

// RS485 helpers (half duplex)
esp_err_t uart_rs485_init(const uart_port_t uartPort, int tx_io_num, int rx_io_num, int rts_io_num, int cts_io_num, uint32_t baud_rate);
esp_err_t uart_rs485_write(const uart_port_t uartPort, const uint8_t *data, size_t len, TickType_t ticks_to_wait);
int uart_rs485_read(const uart_port_t uartPort, uint8_t *buf, size_t maxlen, TickType_t ticks_to_wait);

#endif // UART_DRIVER_H