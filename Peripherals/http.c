#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "esp_http_server.h"

static const char *TAG = "HTTP";

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

    ESP_LOGI(TAG, "Received: %s", buffer);

    httpd_resp_send(
        req,
        "Message received",
        HTTPD_RESP_USE_STRLEN
    );

    return ESP_OK;
}

void http_server_start(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    httpd_handle_t server = NULL;

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