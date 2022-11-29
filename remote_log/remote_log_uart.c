//#include "remote_log_uart.h"

static const char *TAG = "Remote Log, uart";

uint8_t remote_log_uart_num = 0;

esp_err_t configure_uart(uint8_t uart_num, uint32_t baud_rate, uint8_t tx_pin, uint8_t rx_pin)
{
    remote_log_uart_num = uart_num;

    /* Configure parameters of an UART driver,
    * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = baud_rate,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    int intr_alloc_flags = 0;

    #if CONFIG_UART_ISR_IN_IRAM
        intr_alloc_flags = ESP_INTR_FLAG_IRAM;
    #endif

    ESP_ERROR_CHECK(uart_driver_install(uart_num, 2048 * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(uart_num, tx_pin, rx_pin, REMOTE_LOG_RTS, REMOTE_LOG_CTS));
    
    ESP_LOGI(TAG, "UART%d configured @%d baud", uart_num, baud_rate);
    return ESP_OK;
}

esp_err_t remote_uart_send(const void *payload, size_t len)
{
    uart_write_bytes(remote_log_uart_num, payload, len);
    return ESP_OK;
}
