#ifndef __TEMP_THREAD__
#define __TEMP_THREAD__

#define TEMP_TAG "temp"

/*!
 * @brief Measures current temperature
 * @param[in] args No purpose, required by freeRTOS
 * 
 */
void temp_thread(void *args);

#endif
