#include "fullness_thread.h"
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

#if 0 
#define FULLNESS_BUFFER_SIZE 10UL
#define FULLNESS_FIRST_QUARTILE (FULLNESS_BUFFER_SIZE / 4)
#define FULLNESS_THIRD_QUARTILE (FULLNESS_BUFFER_SIZE - FULLNESS_FIRST_QUARTILE)

static uint32_t fullness_buffer[FULLNESS_BUFFER_SIZE];

static uint32_t measure_avg_fullness(const ultrasonic_serial_t *p_sensor);
static int fill_fullness_buffer(const ultrasonic_serial_t *p_sensor);
static void sort_fullness_buffer(void);
static uint32_t calculate_interquartile_average(void);
#endif 


void fullness_thread(void* args)
{
    esp_log_level_set(ULTRASONIC_TAG, ESP_LOG_INFO);

    ultrasonic_serial_t dist_sensor = {
        .uart_num = UART_NUM_2,
        .tx_pin = GPIO_NUM_17,
        .rx_pin = GPIO_NUM_16
    };

    ESP_ERROR_CHECK(ultrasonic_serial_init(&dist_sensor)); 

    fullness_queue = xQueueCreate(20, sizeof(uint32_t));

    // If queue failed to create queue, restart system
    if (fullness_queue == NULL) 
    {
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        esp_restart();
    }

    while (1)
    {
        //uint32_t fullness = measure_avg_fullness(&dist_sensor);
        uint32_t fullness = 0;
        // Record number of failed readings
        if (ultrasonic_measure(&dist_sensor, &fullness) != ESP_OK)
        {
            ESP_LOGI(ULTRASONIC_TAG, "Sensor failed");
        }

        ESP_LOGI(ULTRASONIC_TAG, "Average fullness: %d", fullness);

        xQueueSend(fullness_queue, &fullness, portMAX_DELAY);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

#if 0
/*!
 * @brief Measures the average fullness
 * @param[in] p_sensor Pointer to US-100 descriptor
 * 
 */
static uint32_t measure_avg_fullness(const ultrasonic_serial_t *p_sensor)
{
    int failed_readings = fill_fullness_buffer(p_sensor);

    // If at least half of the readings fail, restart the system
    if (failed_readings >= FULLNESS_BUFFER_SIZE / 2)
    {
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        esp_restart();
    }

    sort_fullness_buffer();

    // Calculate of values in the interquartile range
    return calculate_interquartile_average();
}

/*!
 * @brief Fills fullness buffer with measurements
 * @param[in] p_sensor Pointer to US-100 descriptor
 */
static int fill_fullness_buffer(const ultrasonic_serial_t *p_sensor)
{
    int failed_readings = 0;
    
    // Fill buffer with fullness measurements
    for (size_t i = 0; i < FULLNESS_BUFFER_SIZE; ++i)
    {
        uint32_t fullness = 0;
        // Record number of failed readings
        if (ultrasonic_measure(p_sensor, &fullness) != ESP_OK)
        {
            ++failed_readings;
        }
        fullness_buffer[i] = fullness;
    }

    // Return number of failed readings
    return failed_readings;
}

/*!
 * @brief Sorts fullness buffer in ascending order 
 *
 */
static void sort_fullness_buffer(void)
{
    // Sort fullness buffer using insertion sort 
    
    int i, j;
    uint32_t key;

    for (i = 1; i < FULLNESS_BUFFER_SIZE; ++i)
    {
        key = fullness_buffer[i];
        j = i - 1;

        while (j >= 0 && fullness_buffer[j] > key)
        {
            fullness_buffer[j+1] = fullness_buffer[j];
            --j;
        }
        fullness_buffer[j + 1] = key;
    }
}

/*!
 * @brief Calculates average of buffer inside interquartile range
 *
 */
static uint32_t calculate_interquartile_average(void)
{
    // Calculates the average of the values inside the interquartile range

    uint32_t sum = 0;

    for (size_t i = FULLNESS_FIRST_QUARTILE; i < FULLNESS_THIRD_QUARTILE; ++i)
    {
        sum += fullness_buffer[i];
    }

    return sum / (FULLNESS_THIRD_QUARTILE - FULLNESS_FIRST_QUARTILE);
}
#endif 
