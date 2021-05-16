#ifndef __FULLNESS_THREAD__
#define __FULLNESS_THREAD__

#include <ultrasonic_serial.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

/*!
 * @brief Thread measuring fullness of trash can
 * @param[in] args No purpose, required by freeRTOS
 * 
 */
void fullness_thread(void* args);

QueueHandle_t fullness_queue;

#endif 