#ifndef __FULLNESS_THREAD__
#define __FULLNESS_THREAD__

#include <ultrasonic_serial.h>

/*!
 * @brief Thread measuring fullness of trash can
 * @param[in] args No purpose, required by freeRTOS
 * 
 */
void fullness_thread(void* args);

#endif 