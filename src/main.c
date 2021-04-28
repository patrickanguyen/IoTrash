#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <ultrasonic_serial.h>
#include "secrets.h"

#define ULTRASONIC_STACK_SIZE (8192)

static void ultrasonic_task(void *args)
{
    ultrasonic_serial_t dist_sensor = {
        .uart_num = UART_NUM_2,
        .tx_pin = GPIO_NUM_17,
        .rx_pin = GPIO_NUM_16
    };
    
    ESP_LOGI("Ultrasonic", "Hello There");
    ESP_ERROR_CHECK(ultrasonic_serial_init(&dist_sensor)); 
    ESP_LOGI("Ultrasonic", "Properly initialized UART");

    while (1)
    {
        uint32_t distance = 0;
        ultrasonic_measure(&dist_sensor, &distance);
        ESP_LOGI("Ultrasonic", "Distance: %d", distance);

        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}


void app_main(void)
{
    xTaskCreate(ultrasonic_task, "Ultrasonic", ULTRASONIC_STACK_SIZE, NULL, 10, NULL);
    
}