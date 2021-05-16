#ifndef __TEMP_THREAD__
#define __TEMP_THREAD__

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#define TEMP_TAG "temp"

/*!
 * @brief Measures current temperature
 * @param[in] args No purpose, required by freeRTOS
 * 
 */
void temp_thread(void *args); 

QueueHandle_t temp_queue;

#endif
