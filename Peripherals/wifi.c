#include <stdio.h>

#include "wifi.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "nvs_flash.h"

#define WIFI_SSID       "Galaxy"
#define WIFI_PASSWORD   "98765432"

static const char *TAG = "WIFI";


static void wifi_event_handler(void *arg,
                               esp_event_base_t event_base,
                               int32_t event_id,
                               void *event_data)
{
    if (event_base == WIFI_EVENT &&
        event_id == WIFI_EVENT_STA_START)
    {
        ESP_LOGI(TAG, "Wi-Fi started");
        esp_wifi_connect();
    }

    else if (event_base == WIFI_EVENT &&
             event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        ESP_LOGI(TAG, "Wi-Fi disconnected");
        ESP_LOGI(TAG, "Trying to reconnect...");

        esp_wifi_connect();
    }

    else if (event_base == IP_EVENT &&
             event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event =
            (ip_event_got_ip_t *)event_data;

        ESP_LOGI(TAG, "Wi-Fi connected!");

        ESP_LOGI(TAG,
                 "IP Address: " IPSTR,
                 IP2STR(&event->ip_info.ip));
    }
}


void wifi_init(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        nvs_flash_erase();
        nvs_flash_init();
    }

    // Initialize network interface
    esp_netif_init();

    // Create default event loop
    esp_event_loop_create_default();

    // Create Wi-Fi station interface
    esp_netif_create_default_wifi_sta();

    // Initialize Wi-Fi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    esp_wifi_init(&cfg);

    // Register Wi-Fi event handler
    esp_event_handler_register(
        WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        &wifi_event_handler,
        NULL
    );

    // Register IP event handler
    esp_event_handler_register(
        IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        &wifi_event_handler,
        NULL
    );

    // Set Wi-Fi mode to Station
    esp_wifi_set_mode(WIFI_MODE_STA);

    // Wi-Fi configuration
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASSWORD
        }
    };

    // Apply configuration
    esp_wifi_set_config(
        WIFI_IF_STA,
        &wifi_config
    );

    // Start Wi-Fi
    esp_wifi_start();

    ESP_LOGI(TAG, "Wi-Fi initialization completed");
}