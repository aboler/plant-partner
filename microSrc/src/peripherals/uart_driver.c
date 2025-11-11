#include "uart_driver.h"

static const char *UART_TAG = "uart_driver";

void uart_init(const uart_port_t uartPort)
{
    // Install UART Driver
    ESP_ERROR_CHECK(uart_driver_install(uartPort, UART_RX_BUFFER_SIZE, 0, 0, NULL, 0));

    // Setup UART Port Configuration
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    ESP_ERROR_CHECK(uart_param_config(uartPort, &uart_config));

    // Set ESP Pins for respective UART Port
    switch (uartPort)
    {
    case UART_NUM_0:
        ESP_ERROR_CHECK(uart_set_pin(uartPort, UART0_RX_PIN, UART0_TX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
        break;
    case UART_NUM_1:
        ESP_ERROR_CHECK(uart_set_pin(uartPort, UART1_RX_PIN, UART1_TX_PIN, UART1_RTS_PIN, UART1_CTS_PIN));
        break;
    case UART_NUM_2:
        ESP_ERROR_CHECK(uart_set_pin(uartPort, UART2_RX_PIN, UART2_TX_PIN, UART2_RTS_PIN, UART2_CTS_PIN));
        break;
    default:
        ESP_ERROR_CHECK(uart_set_pin(uartPort, UART0_RX_PIN, UART0_TX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
        break;
    }
    
}

void uart_kill(const uart_port_t uartPort)
{
    if (uart_is_driver_installed(uartPort)) {
        ESP_ERROR_CHECK(uart_driver_delete(uartPort));
    }
}

int uart_write_bytes_blocking(const uart_port_t uartPort, const uint8_t *data)
{
    // Check if requested driver is initialized
    if (!uart_is_driver_installed(uartPort)) {
        ESP_LOGE(UART_TAG, "write: driver not installed for port %d", uartPort);
        return UART_ERROR_DRIVER_NOT_INSTALLED;
    }

    // Send data
    int written = uart_write_bytes(uartPort, (const char *)data, sizeof(data));
    if (written < 0) {
        return UART_ERROR_WRITE_FAILED;
    }

    // Wait until transmission is complete
    ESP_ERROR_CHECK(uart_wait_tx_done(uartPort, UART_TICKS_TO_WAIT));

    return written;
}

int uart_read_bytes_blocking(const uart_port_t uartPort, uint8_t *buf, size_t maxlen, TickType_t ticks_to_wait)
{
    if (!uart_is_driver_installed(uartPort)) {
        ESP_LOGE(UART_TAG, "read: driver not installed for port %d", uartPort);
        return -1;
    }
    size_t avail = 0;
    esp_err_t err = uart_get_buffered_data_len(uartPort, &avail);
    if (err != ESP_OK) {
        ESP_LOGE(UART_TAG, "uart_get_buffered_data_len failed: %d", err);
        return -1;
    }
    size_t to_read = (avail > maxlen) ? maxlen : avail;
    if (to_read == 0) {
        // no data right now: wait for up to ticks_to_wait by attempting a read
        int r = uart_read_bytes(uartPort, buf, maxlen, ticks_to_wait);
        return r;
    }
    int r = uart_read_bytes(uartPort, buf, to_read, 0);
    return r;
}

esp_err_t uart_rs485_init(const uart_port_t uartPort, int tx_io_num, int rx_io_num, int rts_io_num, int cts_io_num, uint32_t baud_rate)
{
    // Install driver with ring buffers for RS485
    uart_config_t uart_config = {
        .baud_rate = baud_rate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0,
    };

    ESP_ERROR_CHECK(uart_param_config(uartPort, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(uartPort, tx_io_num, rx_io_num, rts_io_num, cts_io_num));

    // Install driver: give both rx and tx buffers to allow background TX
    ESP_ERROR_CHECK(uart_driver_install(uartPort, UART_RX_BUFFER_SIZE, UART_RX_BUFFER_SIZE, 0, NULL, 0));

    // Enable RS485 half duplex mode so driver controls RTS for DE/RE pin
    ESP_ERROR_CHECK(uart_set_mode(uartPort, UART_MODE_RS485_HALF_DUPLEX));

    return ESP_OK;
}

esp_err_t uart_rs485_write(const uart_port_t uartPort, const uint8_t *data, size_t len, TickType_t ticks_to_wait)
{
    int written = uart_write_bytes_blocking(uartPort, data, len, ticks_to_wait);
    return (written >= 0) ? ESP_OK : ESP_FAIL;
}

int uart_rs485_read(const uart_port_t uartPort, uint8_t *buf, size_t maxlen, TickType_t ticks_to_wait)
{
    return uart_read_bytes_blocking(uartPort, buf, maxlen, ticks_to_wait);
}