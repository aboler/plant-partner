#include "uart_driver.h"
#include <string.h>

static const char *UART_TAG = "uart_driver";

void uart_init(const uart_port_t uartPort)
{
    // Install UART Driver
    ESP_ERROR_CHECK(uart_driver_install(uartPort, UART_RX_BUFFER_SIZE, UART_TX_BUFFER_SIZE, 0, NULL, 0));

    // Setup UART Port Configuration
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    ESP_ERROR_CHECK(uart_param_config(uartPort, &uart_config));

    // Set ESP Pins for respective UART Port
    switch (uartPort)
    {
        case UART_PORT0:
            ESP_ERROR_CHECK(uart_set_pin(uartPort, UART0_TX_PIN, UART0_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
            break;
        case UART_PORT1:
            ESP_ERROR_CHECK(uart_set_pin(uartPort, UART1_TX_PIN, UART1_RX_PIN, UART1_RTS_PIN, UART1_CTS_PIN));
            break;
        case UART_PORT2:
            ESP_ERROR_CHECK(uart_set_pin(uartPort, UART2_TX_PIN, UART2_RX_PIN, UART2_RTS_PIN, UART2_CTS_PIN));
            break;
        default:
            ESP_ERROR_CHECK(uart_set_pin(uartPort, UART0_TX_PIN, UART0_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
            break;
    } 
}

void uart_kill(const uart_port_t uartPort)
{
    if (uart_is_driver_installed(uartPort)) {
        ESP_ERROR_CHECK(uart_driver_delete(uartPort));
    }
}

uart_error_t uart_write_bytes_blocking(const uart_port_t uartPort, const void *data, size_t numBytes)
{
    // Check if requested driver is initialized
    if (!uart_is_driver_installed(uartPort)) {
        ESP_LOGE(UART_TAG, "write: driver not installed for port %d", uartPort);
        return UART_ERROR_DRIVER_NOT_INSTALLED;
    }

    // Send data - Check if data pointer is NULL or if numBytes is 0
    if (data == NULL) {
        ESP_LOGE(UART_TAG, "write: data pointer is NULL");
        return UART_ERROR_WRITE_FAILED;
    }
    if (numBytes == 0) {
        numBytes = strlen((const char *)data);
    }

    int written = uart_write_bytes(uartPort, (const char *)data, numBytes);
    if (written < 0) {
        return UART_ERROR_WRITE_FAILED;
    }

    // Wait a set 100 ticks for Transmission to complete
    ESP_ERROR_CHECK(uart_wait_tx_done(uartPort, UART_TICKS_TO_WAIT));

    return UART_ERROR_NONE;
}

uart_error_t uart_write_string(const uart_port_t uartPort, const char *s)
{
    // Check if string is empty then write
    if (s == NULL){
        return UART_ERROR_WRITE_FAILED;
    }
    return uart_write_bytes_blocking(uartPort, s, strlen(s));
}

uart_error_t uart_write_int(const uart_port_t uartPort, int v)
{
    // Convert integer to ASCII string and send
    char buf[32];
    int n = snprintf(buf, sizeof(buf), "%d\n", v);
    if (n < 0){
        return UART_ERROR_WRITE_FAILED;
    } 
    return uart_write_bytes_blocking(uartPort, buf, (size_t)n);
}

uart_error_t uart_read_bytes_blocking(const uart_port_t uartPort, void *buf)
{
    // Check if requested driver is initialized
    if (!uart_is_driver_installed(uartPort)) {
        ESP_LOGE(UART_TAG, "read: driver not installed for port %d", uartPort);
        return UART_ERROR_DRIVER_NOT_INSTALLED;
    }

    // Read in Receive FIFO
    int readBufferLength; 
    ESP_ERROR_CHECK(uart_get_buffered_data_len(uartPort, (size_t*)&readBufferLength));

    // Check if readBufferLength is longer than 256 bytes and clamp if needed
    if (readBufferLength > UART_RX_MAX_LENGTH){
        readBufferLength = 255;
    }
    if (readBufferLength <= 0){
        return UART_ERROR_NO_BYTES_TO_READ;
    }

    // Read in Bytes to buf
    uart_read_bytes(uartPort, buf, readBufferLength, UART_TICKS_TO_WAIT);

    return UART_ERROR_NONE;
}

// --------------------------------------------------------------------------- //
// This code is NOT finished but is meant for rs485 communication specifically //
// --------------------------------------------------------------------------- //

bool uart_rs485_init()
{
    // RS485 uses UART2 //

    // Install UART Driver: Only Receiving Buffer
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT2, UART_RX_BUFFER_SIZE, 0, 0, NULL, 0));

    // UART Port Configuration for RS485
    uart_config_t uart_config = {
        .baud_rate = UART_RS485_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_EVEN,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    ESP_ERROR_CHECK(uart_param_config(UART_PORT2, &uart_config));

    // Set ESP Pins for UART2 (RS485)
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT2, UART2_TX_PIN, UART2_RX_PIN, UART2_RTS_PIN, UART2_CTS_PIN));

    // Initialize DE/RE Pin Control to Receive Mode (Low) - Transmit Mode is High
    gpio_reset_pin(RS485_DE_RE_PIN);
    gpio_set_direction(RS485_DE_RE_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(RS485_DE_RE_PIN, 0);

    return true;
}

void uart_read_rs485(uint8_t *rx_buf){
    const char *TAG = "MODBUS";

    // Request Frame for RS485 Modbus Communication
    uint8_t modbus_request[] = {
        0x01,       // Device Address Code
        0x03,       // Function Code: Read Holding Registers
        0x00, 0x1E, // Register Start Address: 0x001E
        0x00, 0x03, // Register Length: 0x0000 - 0x0003
        0x34, 0x0D  // CRC16 (LSB/MSB)
    };

    memset(rx_buf, 0, sizeof(rx_buf));

    // Enable Transmit Mode
    uart_rs485_set_transmit_mode();
    vTaskDelay(pdMS_TO_TICKS(2));

    // Send Inquiry Frame
    uart_write_bytes(UART_PORT2, (const char *)modbus_request, sizeof(modbus_request));
    uart_wait_tx_done(UART_PORT2, pdMS_TO_TICKS(150));

    // Switch back to Receive Mode
    uart_rs485_set_receive_mode();

    // Response will be formatted as:
    // Byte 0: Device ID - 0x01
    // Byte 1: Function Code - 0x03
    // Byte 2: Byte Count - 0x06
    // Byte 3-4: Nitrogen Content (MSB/LSB)
    // Byte 5-6: Phosphorus Content (MSB/LSB)
    // Byte 7-8: Potassium Content (MSB/LSB)
    // Byte 9-10: CRC16 (LSB/MSB)

    // Read Response
    int len = uart_read_bytes(UART_PORT2, rx_buf, sizeof(rx_buf), pdMS_TO_TICKS(200));

    // If no bytes received, log warning
    if (len > 0)
    {
        ESP_LOGI(TAG, "Received %d bytes:", len);
        for (int i = 0; i < len; i++)
            ESP_LOGI(TAG, "%02X ", rx_buf[i]);
    }
    else
    {
        ESP_LOGW(TAG, "No response");
    }
}

// --------------------------------------------------------------------------- //

void uart_rs485_set_transmit_mode()
{
    gpio_set_level(RS485_DE_RE_PIN, 1);
}
void uart_rs485_set_receive_mode()
{
    gpio_set_level(RS485_DE_RE_PIN, 0);
}

