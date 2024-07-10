#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "esp_log.h"

#define GPS_UART_NUM      UART_NUM_1
#define GPS_UART_TX_PIN   0
#define GPS_UART_RX_PIN   2
#define GPS_UART_BUF_SIZE (1024)

static const char *TAG = "GPS";

void process_gps_data(const uint8_t *data, int len) {
    // 受信したGPSデータを処理するための関数
    // ここでデータを解析したり，必要な処理を行う
    ESP_LOGI(TAG, "Processed GPS Data: %.*s", len, data);
}

void gps_task(void *arg) {
    uart_config_t uart_config = {
        .baud_rate = 38400,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    ESP_ERROR_CHECK(uart_param_config(GPS_UART_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(GPS_UART_NUM, GPS_UART_TX_PIN, GPS_UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(GPS_UART_NUM, GPS_UART_BUF_SIZE * 2, 0, 0, NULL, 0));

    uint8_t *data = (uint8_t *) malloc(GPS_UART_BUF_SIZE);
    if (data == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for data buffer");
        return;
    }

    ESP_LOGI(TAG, "GPS task started");

    while (1) {
        int len = uart_read_bytes(GPS_UART_NUM, data, GPS_UART_BUF_SIZE, 1000 / portTICK_PERIOD_MS);
        if (len > 0) {
            data[len] = '\0';
            process_gps_data(data, len);
        } else {
            ESP_LOGI(TAG, "No GPS data received");
        }
    }

    free(data);
}

void app_main(void) {
    xTaskCreate(gps_task, "gps_task", 4096, NULL, 10, NULL);
}