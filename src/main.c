#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "secrets.h"
#include "fullness_thread.h"
#include "mqtt_thread.h"
#include "temp_thread.h"

#define FULLNESS_STACK_SIZE (8192)
#define MQTT_STACK_SIZE (8192)
#define TEMP_STACK_SIZE (8192)

void app_main(void)
{
    #if 0
    xTaskCreate(fullness_thread, "fullness", FULLNESS_STACK_SIZE, NULL, 10, NULL);
    xTaskCreate(mqtt_thread, "mqtt", MQTT_STACK_SIZE, NULL, 10, NULL);
    #endif

    xTaskCreate(temp_thread, "temp", TEMP_STACK_SIZE, NULL, 10, NULL);
}
