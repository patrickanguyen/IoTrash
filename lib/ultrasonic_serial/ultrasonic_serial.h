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