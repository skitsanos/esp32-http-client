#include <stdio.h>
#include <esp_err.h>
#include <esp_event_legacy.h>
#include <esp_log.h>
#include <freertos/event_groups.h>
#include <wifi_bits.h>
#include <core.h>
#include <nvs_flash.h>
#include <wifi_services.h>
#include <etc.h>
#include <esp_http_client.h>
#include <config.h>
#include <string.h>
#include <console_colors.h>

#include "app_config.h"
#include "app.h"

static uint32_t stats_heap_on_boot = 0;

static esp_err_t app_create_event_groups()
{
    xCoreEventGroup = xEventGroupCreate();
    xWifiEventGroup = xEventGroupCreate();

    return ESP_OK;
}

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id)
    {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client))
            {
                // Write out data
                // printf("%.*s", evt->data_len, (char*)evt->data);
            }

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;
}


static void http_rest()
{
    char *buf;
    //asprintf(&buf, "http://192.168.1.27/?heap=%d&core_temp=%.2f&core_hall=%d",
    asprintf(&buf, "http://api.skitsanos.com/?heap=%d&core_temp=%.2f&core_hall=%d",
             esp_deviceFreeMem(),
             esp_deviceTemperature(),
             esp_deviceHall());
    //ESP_LOGW(">>>", "%s", buf);

    esp_http_client_config_t config = {
            .url = buf,
            .event_handler = _http_event_handler,
    };

    ESP_LOGW(">>>", "Connecting to %s", config.url);

    esp_http_client_handle_t client = esp_http_client_init(&config);

    // GET
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %d",
                 esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));
    }
    else
    {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }

    /* // POST
     const char *post_data = "field1=value1&field2=value2";
     esp_http_client_set_url(client, "/post");
     esp_http_client_set_method(client, HTTP_METHOD_POST);
     esp_http_client_set_post_field(client, post_data, strlen(post_data));
     err = esp_http_client_perform(client);
     if (err == ESP_OK)
     {
         ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %d",
                  esp_http_client_get_status_code(client),
                  esp_http_client_get_content_length(client));
     }
     else
     {
         ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
     }

     //PUT
     esp_http_client_set_url(client, "/put");
     esp_http_client_set_method(client, HTTP_METHOD_PUT);
     err = esp_http_client_perform(client);
     if (err == ESP_OK)
     {
         ESP_LOGI(TAG, "HTTP PUT Status = %d, content_length = %d",
                  esp_http_client_get_status_code(client),
                  esp_http_client_get_content_length(client));
     }
     else
     {
         ESP_LOGE(TAG, "HTTP PUT request failed: %s", esp_err_to_name(err));
     }

     //PATCH
     esp_http_client_set_url(client, "/patch");
     esp_http_client_set_method(client, HTTP_METHOD_PATCH);
     esp_http_client_set_post_field(client, NULL, 0);
     err = esp_http_client_perform(client);
     if (err == ESP_OK)
     {
         ESP_LOGI(TAG, "HTTP PATCH Status = %d, content_length = %d",
                  esp_http_client_get_status_code(client),
                  esp_http_client_get_content_length(client));
     }
     else
     {
         ESP_LOGE(TAG, "HTTP PATCH request failed: %s", esp_err_to_name(err));
     }

     //DELETE
     esp_http_client_set_url(client, "/delete");
     esp_http_client_set_method(client, HTTP_METHOD_DELETE);
     err = esp_http_client_perform(client);
     if (err == ESP_OK)
     {
         ESP_LOGI(TAG, "HTTP DELETE Status = %d, content_length = %d",
                  esp_http_client_get_status_code(client),
                  esp_http_client_get_content_length(client));
     }
     else
     {
         ESP_LOGE(TAG, "HTTP DELETE request failed: %s", esp_err_to_name(err));
     }

     //HEAD
     esp_http_client_set_url(client, "/get");
     esp_http_client_set_method(client, HTTP_METHOD_HEAD);
     err = esp_http_client_perform(client);
     if (err == ESP_OK)
     {
         ESP_LOGI(TAG, "HTTP HEAD Status = %d, content_length = %d",
                  esp_http_client_get_status_code(client),
                  esp_http_client_get_content_length(client));
     }
     else
     {
         ESP_LOGE(TAG, "HTTP HEAD request failed: %s", esp_err_to_name(err));
     }*/

    free(buf);

    esp_http_client_cleanup(client);
}


static void task_request() //https://github.com/tuanpmt/esp-request
{
    wifi_wait_for_connection();

    http_rest();
}

void task_repeated(void *ignore)
{
    for (EVER)
    {
        task_request();
        ESP_LOGI(TAG, "Memory stats: \033[35m%d\033[0m/\033[36;1m%d\033[0m", stats_heap_on_boot, esp_deviceFreeMem());
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void app_main()
{
    console_set_color(CONSOLE_COLORS_RESET);

    printf("\033[46;1m ╔═══════════════════════════════════╗\033[0m\n");
    printf("\033[46;1m ║     http-client v.1.0.20190110    ║\033[0m\n");
    printf("\033[46;1m ╚═══════════════════════════════════╝\033[0m\n");

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);

    stats_heap_on_boot = esp_deviceFreeMem();

    ESP_ERROR_CHECK(app_create_event_groups());

    wifi_init_sta("Skitsanos", "zzz");

    xTaskCreate(task_repeated, "request", 2048, NULL, 10, NULL);
}