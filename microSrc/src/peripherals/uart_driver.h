#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include <string.h>
#include <stdio.h>
#include "driver/uart.h"
#include "freertos/FreeRTOS.h" // TickType_t
#include "freertos/queue.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/task.h"

#define UART_BAUD_RATE       115200
#define UART_RS485_BAUD_RATE 9600
#define UART_RX_BUFFER_SIZE  2048
#define UART_TX_BUFFER_SIZE  2048
#define UART_TICKS_TO_WAIT   pdMS_TO_TICKS(1000)
#define UART_RX_MAX_LENGTH   256

// NOTE UART_PORT0: This is for USB-UART connections - Do not touch unless you DO NOT want ESP_LOG functionality //
#define UART_PORT0      UART_NUM_0
#define UART0_RX_PIN    GPIO_NUM_3
#define UART0_TX_PIN    GPIO_NUM_1

#define UART_PORT1      UART_NUM_1
#define UART1_RX_PIN    GPIO_NUM_22
#define UART1_TX_PIN    GPIO_NUM_23
#define UART1_RTS_PIN   UART_PIN_NO_CHANGE
#define UART1_CTS_PIN   UART_PIN_NO_CHANGE

#define UART_PORT2      UART_NUM_2
#define UART2_RX_PIN    GPIO_NUM_16
#define UART2_TX_PIN    GPIO_NUM_17
#define UART2_RTS_PIN   GPIO_NUM_7
#define UART2_CTS_PIN   GPIO_NUM_6

#define RS485_DE_RE_PIN GPIO_NUM_21  // Pin to control RS485 Driver Enable/~Receiver Enable

// Typedef for UART function ouput
typedef enum uart_error_t{
    UART_ERROR_NONE = 0,
    UART_ERROR_DRIVER_NOT_INSTALLED = -1,
    UART_ERROR_WRITE_FAILED = -2,
    UART_ERROR_NO_BYTES_TO_READ = -3
} uart_error_t;

// UART init & kill functions
void uart_init(const uart_port_t uartPort);
void uart_kill(const uart_port_t uartPort);

// Standard UART Read & Write Functions
uart_error_t uart_write_bytes_blocking(const uart_port_t uartPort, const void *data, size_t numBytes);
uart_error_t uart_read_bytes_blocking(const uart_port_t uartPort, void *buf);

// Helper Functions
uart_error_t uart_write_string(const uart_port_t uartPort, const char *s);
uart_error_t uart_write_int(const uart_port_t uartPort, int v);

// // RS485 Specific helpers 
void uart_rs485_init();
void uart_rs485_set_transmit_mode();
void uart_rs485_set_receive_mode();
void uart_read_rs485();

#endif // UART_DRIVER_H