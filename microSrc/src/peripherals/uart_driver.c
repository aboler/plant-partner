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
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    ESP_ERROR_CHECK(uart_param_config(uartPort, &uart_config));

    // Set ESP pins for respective UART Port
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
    // Delete UART driver if already installed
    if (uart_is_driver_installed(uartPort)) 
    {
        ESP_ERROR_CHECK(uart_driver_delete(uartPort));
    }
}

uart_error_t uart_write_bytes_blocking(const uart_port_t uartPort, const void *data, size_t numBytes)
{
    // Check if requested driver is initialized
    if (!uart_is_driver_installed(uartPort)) 
    {
        ESP_LOGE(UART_TAG, "write: driver not installed for port %d", uartPort);
        return UART_ERROR_DRIVER_NOT_INSTALLED;
    }

    // Send data - Check if data pointer is NULL, if numBytes is 0, or if nothing written
    if (data == NULL) 
    {
        ESP_LOGE(UART_TAG, "write: data pointer is NULL");
        return UART_ERROR_WRITE_FAILED;
    }

    if (numBytes == 0) 
    {
        numBytes = strlen((const char *)data);
    }

    int written = uart_write_bytes(uartPort, (const char *)data, numBytes);
    if (written < 0) 
    {
        return UART_ERROR_WRITE_FAILED;
    }

    // Wait a set 100 ticks for transmission to complete
    ESP_ERROR_CHECK(uart_wait_tx_done(uartPort, UART_TICKS_TO_WAIT));
    return UART_ERROR_NONE;
}

uart_error_t uart_write_string(const uart_port_t uartPort, const char *s)
{
    // Check if string is empty then write
    if (s == NULL)
    {
        return UART_ERROR_WRITE_FAILED;
    }

    return uart_write_bytes_blocking(uartPort, s, strlen(s));
}

uart_error_t uart_write_int(const uart_port_t uartPort, int v)
{
    // Convert integer to ASCII string and send
    char buf[32];
    int n = snprintf(buf, sizeof(buf), "%d\n", v);

    if (n < 0)
    {
        return UART_ERROR_WRITE_FAILED;
    } 

    return uart_write_bytes_blocking(uartPort, buf, (size_t)n);
}

uart_error_t uart_read_bytes_blocking(const uart_port_t uartPort, void *buf)
{
    // Check if requested driver is initialized
    if (!uart_is_driver_installed(uartPort)) 
    {
        ESP_LOGE(UART_TAG, "read: driver not installed for port %d", uartPort);
        return UART_ERROR_DRIVER_NOT_INSTALLED;
    }

    // Read in Receive FIFO
    int readBufferLength; 
    ESP_ERROR_CHECK(uart_get_buffered_data_len(uartPort, (size_t*)&readBufferLength));

    // Check if readBufferLength is longer than 256 bytes and clamp if needed
    if (readBufferLength > UART_RX_MAX_LENGTH)
    {
        readBufferLength = 255;
    }

    if (readBufferLength <= 0)
    {
        return UART_ERROR_NO_BYTES_TO_READ;
    }

    // Read in Bytes to buffer
    uart_read_bytes(uartPort, buf, readBufferLength, UART_TICKS_TO_WAIT);
    return UART_ERROR_NONE;
}

// --------------------------------------------------------------------------- //
// Code meant for our specific rs485 communication for the NPK sensor          //
// --------------------------------------------------------------------------- //
// Compute Modbus CRC16 (polynomial 0xA001), returns 16-bit CRC (LSB first when sent)
static uint16_t modbus_crc16(const uint8_t *buf, int len)
{
    uint16_t crc = 0xFFFF;
    for (int pos = 0; pos < len; pos++) 
    {
        crc ^= (uint16_t)buf[pos];
        for (int i = 0; i < 8; i++) 
        {
            crc = (crc & 0x0001) ? ((crc >> 1) ^ 0xA001) : (crc >> 1);
        }
    }
    return crc;
}
void uart_rs485_init()
{
    // RS485 uses UART2 //
    // Install UART Driver: Only Receiving Buffer
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT2, UART_RX_BUFFER_SIZE, UART_TX_BUFFER_SIZE, 0, NULL, 0));
    ESP_LOGI(UART_TAG, "UART2 driver installed with RX buffer size %d and TX buffer size %d", UART_RX_BUFFER_SIZE, UART_TX_BUFFER_SIZE);

    // UART Port Configuration for RS485
    uart_config_t uart_config = {
        .baud_rate = UART_RS485_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    ESP_ERROR_CHECK(uart_param_config(UART_PORT2, &uart_config));
    ESP_LOGI(UART_TAG, "UART2 configured for RS485 communication at %d baud, %d data bits, %d stop bits, and %d parity", UART_RS485_BAUD_RATE, uart_config.data_bits, uart_config.stop_bits, uart_config.parity);

    // Set ESP Pins for UART2 (RS485)
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT2, UART2_TX_PIN, UART2_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_LOGI(UART_TAG, "UART2 pins set: TX=%d, RX=%d, RTS=%d, CTS=%d, DE/RE=%d", UART2_TX_PIN, UART2_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, RS485_DE_RE_PIN);

    // Initialize DE/RE Pin Control to Receive Mode (Low) - Transmit Mode is High
    gpio_reset_pin(RS485_DE_RE_PIN);
    gpio_set_direction(RS485_DE_RE_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(RS485_DE_RE_PIN, 0);
}

void uart_rs485_read()
{
    const char *TAG = "MODBUS";

    // Build base request (without CRC). CRC will be computed and appended.
    uint8_t modbus_request_base[] = 
    {
        0x01,       // Device ID (slave address)
        0x03,       // Function code: Read Holding Registers
        0x00, 0x1E, // Start address: 0x001E
        0x00, 0x03  // Number of registers: 3
    };

    uint8_t tx_buf[16];
    int base_len = sizeof(modbus_request_base);
    memcpy(tx_buf, modbus_request_base, base_len);
    uint16_t crc = modbus_crc16(tx_buf, base_len);

    // Append CRC (LSB first)
    tx_buf[base_len]     = (uint8_t)(crc & 0xFF);
    tx_buf[base_len + 1] = (uint8_t)((crc >> 8) & 0xFF);
    int tx_len = base_len + 2;

    uint8_t rx_buf[256];
    memset(rx_buf, 0, sizeof(rx_buf));

    // Enable Transmit Mode and give DE time to assert
    uart_rs485_set_transmit_mode();
    vTaskDelay(pdMS_TO_TICKS(5));

    // Send Inquiry Frame (binary safe)
    int written = uart_write_bytes(UART_PORT2, (const char *)tx_buf, tx_len);
    if (written != tx_len) 
    {
        ESP_LOGW(TAG, "Write mismatch: expected %d wrote %d", tx_len, written);
    }

    // Wait for TX to complete. Allow generous time for line turnaround.
    ESP_ERROR_CHECK(uart_wait_tx_done(UART_PORT2, pdMS_TO_TICKS(500)));
    // Small delay to ensure last stop bits are on the wire
    vTaskDelay(pdMS_TO_TICKS(2));

    // Switch back to Receive Mode (DE low)
    uart_rs485_set_receive_mode();

    // Poll for incoming bytes with a timeout
    const TickType_t total_wait = pdMS_TO_TICKS(500); // total wait time for response
    const TickType_t poll_delay = pdMS_TO_TICKS(20);
    TickType_t waited = 0;
    int read_len = 0;
    int buf_len = 0;

    while (waited < total_wait) 
    {
        size_t available = 0;
        esp_err_t res = uart_get_buffered_data_len(UART_PORT2, &available);

        if (res == ESP_OK && available > 0) {
            buf_len = (int)available;

            if (buf_len > sizeof(rx_buf))
            { 
                buf_len = sizeof(rx_buf);
            }

            read_len = uart_read_bytes(UART_PORT2, rx_buf, buf_len, pdMS_TO_TICKS(100));
            break;
        }

        vTaskDelay(poll_delay);
        waited += poll_delay;
    }

    uint16_t computed = modbus_crc16(rx_buf, read_len - 2);
    uint16_t received = rx_buf[read_len - 2] | (rx_buf[read_len - 1] << 8);

    if (computed != received) 
    {
        ESP_LOGW(TAG, "CRC Mismatch: Computed 0x%04X, Received 0x%04X", computed, received);
    } 

    ESP_LOGI(TAG, "Read %d bytes from RS485", read_len);
    if (read_len > 0)
    {
        // Parse Data - REG Data is MSB then LSB
        if (read_len >= 9 && rx_buf[0] == 0x01 && rx_buf[1] == 0x03) 
        {
            uint16_t regN = (rx_buf[3] << 8) | rx_buf[4];
            uint16_t regP = (rx_buf[5] << 8) | rx_buf[6];
            uint16_t regK = (rx_buf[7] << 8) | rx_buf[8];
            ESP_LOGI(TAG, "Nitrogen = 0x%04X\nPhosphorus = 0x%04X\nPotassium = 0x%04X", regN, regP, regK);
            
            // Convert into Decimal Values -> mg/kg
            int16_t nConverted = (int16_t)regN; // Cast to signed
            int16_t pConverted = (int16_t)regP;
            int16_t kConverted = (int16_t)regK;
            ESP_LOGI(TAG, "Nitrogen = %d mg/kg\nPhosphorus = %d mg/kg\nPotassium = %d mg/kg", nConverted, pConverted, kConverted);
        } 
        else 
        {
            ESP_LOGW(TAG, "Unexpected response format");
        }
    }
    else
    {
        ESP_LOGW(TAG, "No response (timeout %d ms)", (int)pdTICKS_TO_MS(total_wait));
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