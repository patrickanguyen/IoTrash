#include "temp_thread.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <dht.h>
#include <esp_log.h>

static const dht_sensor_type_t sensor_type = DHT_TYPE_AM2301;
#define DHT_GPIO 25

#define TEMP_THREAD_PERIOD (2000 / portTICK_PERIOD_MS)

void temp_thread(void *args)
{
    int16_t temperature = 0;
    int16_t humidity = 0;

    while (1)
    {
        // Read from the DHT22 sensor every 2 seconds 

        if (dht_read_data(sensor_type, DHT_GPIO, &humidity, &temperature) == ESP_OK)
        {
            ESP_LOGI(TEMP_TAG, "Humdity: %d%% Temp: %dC", humidity/10, temperature/10);
        }
        else 
        {
            ESP_LOGE(TEMP_TAG, "Could not read from DHT22");
        }

        vTaskDelay(TEMP_THREAD_PERIOD);
    }
}
