#include <stdio.h>
#include <string.h>

#include "wifi.h"
#include "http.h"
#include "cJSON.h"
#include "esp_log.h"
#include "esp_http_server.h"


httpd_handle_t server = NULL;

static const char *TAG = "HTTP";

static void MessageReceivedCallback(char *buffer);

static esp_err_t receive_handler(httpd_req_t *req)
{
    char buffer[256];

    int received = httpd_req_recv(
        req,
        buffer,
        sizeof(buffer) - 1
    );

    if (received <= 0)
    {
        ESP_LOGE(TAG, "Failed to receive data");
        return ESP_FAIL;
    }

    buffer[received] = '\0';

    if (received > 0)
    {
        MessageReceivedCallback(buffer);
    }

    httpd_resp_send(
        req,
        "Message received",
        HTTPD_RESP_USE_STRLEN
    );

    return ESP_OK;
}

void stop_server()
{
	if (server != NULL) {
        httpd_stop(server);
        server = NULL;
    }
}

void http_server_start(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    //httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK)
    {
        httpd_uri_t uri = {
            .uri = "/message",
            .method = HTTP_PUT,
            .handler = receive_handler,
            .user_ctx = NULL
        };

        httpd_register_uri_handler(server, &uri);

        ESP_LOGI(TAG, "HTTP server started");
    }
}

static  void MessageReceivedCallback(char *buffer)
{
    if (buffer == NULL)
    {
        return;
    }

    cJSON *root = cJSON_Parse(buffer);
    
    if (root == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        ESP_LOGE(TAG, "Invalid JSON received: %s", buffer);
        if (error_ptr != NULL)
        {
            ESP_LOGE(TAG, "JSON parse error near: %s", error_ptr);
        }
        return;
    }

    cJSON *WIFImessageSSID = cJSON_GetObjectItemCaseSensitive(root, "SSID");
    cJSON *WIFImessagePassword = cJSON_GetObjectItemCaseSensitive(root, "Password");

    if (cJSON_IsString(WIFImessageSSID) && (WIFImessageSSID->valuestring != NULL))
    {
        //ESP_LOGI(TAG, "Parsed JSON message: %s", WIFImessageSSID->valuestring);

        if (cJSON_IsString(WIFImessagePassword) && (WIFImessagePassword->valuestring != NULL))
        {
            ESP_LOGI(TAG, "Wi-Fi credentials updated: SSID=%s, Password=%s", WIFImessageSSID->valuestring, WIFImessagePassword->valuestring);
            ChangeWiFiCredentials(WIFImessageSSID->valuestring, WIFImessagePassword->valuestring);
        }
        else
        {
            ESP_LOGI(TAG, "JSON received but no 'Password' field: %s", cJSON_PrintUnformatted(root));
        }
    }
    else
    {
        ESP_LOGI(TAG, "JSON received but no 'SSID' field: %s", cJSON_PrintUnformatted(root));
    }

    cJSON_Delete(root);
}