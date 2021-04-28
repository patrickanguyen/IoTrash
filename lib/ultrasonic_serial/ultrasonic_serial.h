#ifndef __ULTRASONIC_SERIAL_H__
#define __ULTRASONIC_SERIAL_H__

#include <driver/uart.h>
#include <esp_err.h>

typedef struct 
{
    uart_port_t uart_num; // UART port 
} ultrasonic_serial_t;

/*!
 * @brief Initalize US-100 ultrasonic sensor
 * 
 * @param dev Pointer to US-100 descriptor
 * @return `ESP_OK` on success
 */
esp_err_t ultrasonic_serial_init(const ultrasonic_serial_t *dev);

/*!
 * @brief Measures distance 
 * 
 * @param dev Pointer to US-100 descriptor
 * @param[out] distance Distance measured by US-100
 * @return `ESP_OK` on success
 */
esp_err_t ultrasonic_measure(const ultrasonic_serial_t *dev, uint32_t *distance);

#endif