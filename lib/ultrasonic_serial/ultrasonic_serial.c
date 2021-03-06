/**
 * ESP-IDF driver for US-100 Ultrasonic Sensor 
 *
 * Copyright (C) 2021 Patrick Nguyen <https://github.com/patrickanguyen>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to 
 * deal in the Software without restriction, including without limitation the 
 * rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the Software is 
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <ultrasonic_serial.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string.h>
#include <esp_log.h>

#define PING_MSG "U"
#define PING_TIMEOUT 100000
#define BUFFER_SIZE 1024

#define CHECK(x) do { esp_err_t __; if ((__ = x) != ESP_OK) return __; } while (0)
#define TIMEOUT_EXPIRED(start, len) ((esp_timer_get_time() - (start)) >= (len))

static esp_err_t check_dev_arg(const ultrasonic_serial_t *dev)
{
    return (dev != NULL && dev->uart_num < UART_NUM_MAX) ? ESP_OK : ESP_ERR_INVALID_ARG;
}

static esp_err_t check_p_distance_arg(const uint32_t *p_distance)
{
    return (p_distance != NULL) ? ESP_OK : ESP_ERR_INVALID_ARG;
}

esp_err_t ultrasonic_serial_init(const ultrasonic_serial_t *dev)
{
    // Check for invalid arguments
    CHECK(check_dev_arg(dev));

    // Set communication parameters
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    
    CHECK(uart_param_config(dev->uart_num, &uart_config));

    ESP_LOGD(ULTRASONIC_TAG, "Set UART communication parameters");

    // Set communication pins
    CHECK(uart_set_pin(dev->uart_num, dev->tx_pin, dev->rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    ESP_LOGD(ULTRASONIC_TAG, "Set UART communication pins");

    // Driver Installation
    CHECK(uart_driver_install(dev->uart_num, (BUFFER_SIZE * 2), 0, 0, NULL, 0));

    ESP_LOGD(ULTRASONIC_TAG, "Installed UART driver");

    return ESP_OK;
}

esp_err_t ultrasonic_measure(const ultrasonic_serial_t *dev, uint32_t *p_distance)
{
    // Check for invalid arguments
    CHECK(check_dev_arg(dev));
    CHECK(check_p_distance_arg(p_distance));

    ESP_LOGD(ULTRASONIC_TAG, "Valid inputs for ultrasonic_measure");
    // Flush UART FIFO's
    CHECK(uart_flush(dev->uart_num));

    ESP_LOGD(ULTRASONIC_TAG, "Flushed UART FIFO");

    // Write 0x55 to US-100 Sensor
    int bytes_written = uart_write_bytes(dev->uart_num, PING_MSG, strlen(PING_MSG));
    if (bytes_written < 0)
    {
        ESP_LOGE(ULTRASONIC_TAG, "Cannot write bytes");
        return ESP_ERR_INVALID_RESPONSE;
    }

    ESP_LOGD(ULTRASONIC_TAG, "Wrote 0x55 to US-100");

    // Wait to recieve 2 bytes
    size_t length = 0;
    int64_t start = esp_timer_get_time();
    while (length < 2)
    {
        CHECK(uart_get_buffered_data_len(dev->uart_num, &length));
        // If timed out, return error
        if (TIMEOUT_EXPIRED(start, PING_TIMEOUT))
        {
            ESP_LOGE(ULTRASONIC_TAG, "ERROR, US-100 timed out");
            return ESP_ERR_TIMEOUT;
        }
    }
    
    // Read from UART RX FIFO
    uint8_t data[BUFFER_SIZE];
    int bytes_read = uart_read_bytes(dev->uart_num, data, BUFFER_SIZE, 100 / portTICK_PERIOD_MS);

    // If 2 bytes weren't read return error
    if (bytes_read != 2)
    {
        ESP_LOGE(ULTRASONIC_TAG, "Only %d bytes were read from buffer", bytes_read);
        return ESP_ERR_TIMEOUT;
    }
    
    // Otherwise change value of p_distance and return ESP_OK
    *p_distance = (data[0] << 8) | (data[1]);

    return ESP_OK;
}
