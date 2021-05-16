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
#ifndef __ULTRASONIC_SERIAL_H__
#define __ULTRASONIC_SERIAL_H__

#include <driver/uart.h>
#include <driver/gpio.h>
#include <esp_err.h>

typedef struct 
{
    uart_port_t uart_num; // UART port (UART_NUM0 - UART_NUM1)
    gpio_num_t tx_pin; // TX pin of UART
    gpio_num_t rx_pin; // RX pin of UART
} ultrasonic_serial_t;

#define ULTRASONIC_TAG "ultrasonic"

/*!
 * @brief Initalize US-100 ultrasonic sensor
 * 
 * @param dev Pointer to US-100 descriptor
 * @return `ESP_OK` on success
 */
esp_err_t ultrasonic_serial_init(const ultrasonic_serial_t *dev);

/*!
 * @brief Measures distance in millimeters
 * 
 * @param dev Pointer to US-100 descriptor
 * @param[out] p_distance Pointer to distance measured by US-100 in millimeters
 * @return `ESP_OK` on success
 */
esp_err_t ultrasonic_measure(const ultrasonic_serial_t *dev, uint32_t *p_distance);

#endif