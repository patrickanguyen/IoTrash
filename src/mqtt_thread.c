#include "mqtt_thread.h"
#include "fullness_thread.h"
#include "secrets.h"

#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "esp_log.h"
#include "mqtt_client.h"

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1
#define WIFI_MAX_ENTRY 10

static EventGroupHandle_t wifi_event_group;
static int s_retry_num = 0;

static esp_mqtt_client_handle_t mqtt_client;
static int mqtt_connected = 0;

#define MQTT_THREAD_PERIOD (5000 / portTICK_PERIOD_MS)

static void wifi_init(void);
static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
static void mqtt_app_start(void);
static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event);


void mqtt_thread(void *args)
{
    // Initialize Wi-Fi & MQTT client
    nvs_flash_init();
    wifi_init();
    mqtt_app_start();
    
    char fullness_str[32];

    while (1)
    {
        // Wait for fullness from fullness thread
        uint32_t fullness;
        xQueueReceive(fullness_queue, &fullness, portMAX_DELAY);

        // If connected to MQTT broker, publish fullness to MQTT broker
        if (mqtt_connected) 
        {
            sprintf(fullness_str, "%d", fullness);
            int msg_id = esp_mqtt_client_publish(mqtt_client, MQTT_TOPIC, fullness_str, 0, 1, 0);
            ESP_LOGI(MQTT_TAG, "MQTT published successful, msg_id=%d", msg_id);
        }

        vTaskDelay(MQTT_THREAD_PERIOD);
    }
}


/*!
 * @brief Initialize Wi-Fi in station mode 
 *
 */
static void wifi_init(void)
{
    // Initialize Wi-Fi configuration

    wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASSWORD,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(MQTT_TAG, "wifi_init_sta finished.");

    // Wait for Wi-Fi to be connected or failed 
    EventBits_t bits = xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) 
    {
        ESP_LOGI(MQTT_TAG, "connected to ap");
    } 
    else if (bits & WIFI_FAIL_BIT) 
    {
        ESP_LOGI(MQTT_TAG, "Failed to connect");
    } 
    else 
    {
        ESP_LOGE(MQTT_TAG, "UNEXPECTED EVENT");
    }

}


/*!
 * @brief Wi-Fi Driver event handler
 * @param[in] arg Argument of event handler
 * @param[in] event_base Base ID of event to register handler for
 * @param[in] event_id ID of event to register the handler for
 * @param[in] event_data Data that is passed to handler
 * 
 */
static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) 
    {
        esp_wifi_connect();
    } 
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) 
    {
        if (s_retry_num < WIFI_MAX_ENTRY) 
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(MQTT_TAG, "retry to connect to the AP");
        } 
        else 
        {
            xEventGroupSetBits(wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(MQTT_TAG,"connect to the AP fail");
    } 
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) 
    {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(MQTT_TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }

}


/*!
 * @brief Initalize MQTT protocol client 
 * 
 */
static void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = MQTT_URI,
        .event_handle = mqtt_event_handler
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(client);
}


/*!
 * @brief MQTT client event handler 
 * @param[in] event MQTT related events
 * 
 */
static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event) 
{
    mqtt_client = event->client;

    switch(event->event_id)
    {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(MQTT_TAG, "MQTT_EVENT_CONNECTED");
            mqtt_connected = 1;
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(MQTT_TAG, "MQTT_EVENT_DISCONNECTED");
            mqtt_connected = 0;
            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(MQTT_TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(MQTT_TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(MQTT_TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(MQTT_TAG, "MQTT_EVENT_DATA");
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(MQTT_TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(MQTT_TAG, "Other event id:%d", event->event_id);
            break;
    }
    return ESP_OK;
}


