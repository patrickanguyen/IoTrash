#include "temp_thread.h"

#include <dht.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_log.h>

static const dht_sensor_type_t sensor_type = DHT_TYPE_AM2301;
#define DHT_GPIO 25

#define TEMP_THREAD_PERIOD (2000 / portTICK_PERIOD_MS)

void temp_thread(void *args)
{
    int16_t temperature = 0;
    temp_queue = xQueueCreate(20, sizeof(int16_t));

    while (1)
    {
        // Read from the DHT22 sensor every 2 seconds 

        if (dht_read_data(sensor_type, DHT_GPIO, NULL, &temperature) == ESP_OK)
        {
            temperature /= 10;
            xQueueSend(temp_queue, &temperature, portMAX_DELAY);
            ESP_LOGI(TEMP_TAG, "Temp: %dC", temperature);
        }
        else 
        {
            ESP_LOGE(TEMP_TAG, "Could not read from DHT22");
        }

        vTaskDelay(TEMP_THREAD_PERIOD);
    }
}
