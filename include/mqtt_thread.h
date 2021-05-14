#ifndef __MQTT_THREAD__
#define __MQTT_THREAD__

#define MQTT_TAG "mqtt"

/*!
 * @brief Thread publishing to MQTT broker
 * @param[in] args No purpose, required by freeRTOS
 * 
 */
void mqtt_thread(void *args);

#endif
