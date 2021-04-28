#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "secrets.h"
#include "fullness_thread.h"

#define FULLNESS_STACK_SIZE (8192)

void app_main(void)
{
    xTaskCreate(fullness_thread, "fullness", FULLNESS_STACK_SIZE, NULL, 10, NULL);
    
}