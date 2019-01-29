#include <string.h>
#include <stdlib.h>
#include <etc.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

#include "tcpip_adapter.h"
#include "esp_event_loop.h"
#include "esp_wifi.h"
#include "esp_log.h"

#include "app_config.h"
#include "app.h"
#include "core.h"
#include "wifi_bits.h"

esp_err_t wifi_connect()
{
	ESP_LOGI(TAG, "Connecting to WIFI...");
	esp_err_t r = esp_wifi_connect();
	switch (r)
	{
		case ESP_OK:
			ESP_LOGI(TAG, "WIFI Connected.");
			break;

		case ESP_ERR_WIFI_NOT_INIT:
			ESP_LOGW(TAG, "WIFI was not initialized by esp_wifi_init");
			break;

		case ESP_ERR_WIFI_NOT_STARTED:
			ESP_LOGW(TAG, "WIFI was not started by esp_wifi_start");
			break;

		case ESP_FAIL:
			break;

		default:
			ESP_LOGW(TAG, "Couldn't connect, Err code: #%d", r);
			break;
	}

	return r;
}

static esp_err_t wifi_event_handler(void *ctx, system_event_t *event)
{
	switch (event->event_id)
	{
		case SYSTEM_EVENT_STA_START:
		{
			ESP_LOGI(TAG, "Station started");
			ESP_ERROR_CHECK(wifi_connect());
			break;
		}

		case SYSTEM_EVENT_STA_CONNECTED:
		{
			system_event_sta_connected_t *connected = &event->event_info.connected;
			ESP_LOGI(TAG, "STA \"%s\" connected on channel %d", connected->ssid, connected->channel);

			/*ESP_LOGD(TAG, "SYSTEM_EVENT_STA_CONNECTED, ssid:%s, ssid_len:%d, bssid:"
					MACSTR
					", channel:%d, authmode:%d",
					 event->event_info.connected.ssid, event->event_info.connected.ssid_len,
					 MAC2STR(event->event_info.connected.bssid), event->event_info.connected.channel,
					 event->event_info.connected.authmode);*/
			break;
		}

		case SYSTEM_EVENT_STA_GOT_IP:
		{
			xEventGroupSetBits(xWifiEventGroup, STA_CONNECTED_BIT);

			ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP");
			ESP_LOGI(TAG, "Got IP: '%s'", ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));

			break;
		}

		case SYSTEM_EVENT_STA_LOST_IP:
		{
			ESP_LOGW(TAG, "Lost IP. Reconnecting...");
			xEventGroupClearBits(xWifiEventGroup, STA_CONNECTED_BIT);

			ESP_ERROR_CHECK(esp_wifi_disconnect());

			break;
		}

		case SYSTEM_EVENT_STA_DISCONNECTED:
		{
			xEventGroupClearBits(xWifiEventGroup, STA_CONNECTED_BIT);
			ESP_ERROR_CHECK(wifi_connect());
			break;
		}

		default:
			break;
	}
	return ESP_OK;
}

/**
 * Minimal event handler for ESP-NOW support
 * @param ctx
 * @param event
 * @return
 */
static esp_err_t wifi_intranet_event_handler(void *ctx, system_event_t *event)
{
	switch (event->event_id)
	{
		case SYSTEM_EVENT_STA_START:
			ESP_LOGI(TAG, "WiFi Intranet started");
			break;

		default:
			break;
	}
	return ESP_OK;
}

void wifi_wait_for_connection()
{
	xEventGroupWaitBits(xWifiEventGroup, STA_CONNECTED_BIT, false, true, portMAX_DELAY);
}


void task_wait_for_sta_connection(void *pvParameter)
{
	wifi_wait_for_connection();

	// print the local IP address
	tcpip_adapter_ip_info_t ip_info;
	ESP_ERROR_CHECK(tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ip_info));

	ESP_LOGI(TAG, "IP Address:  %s", ip4addr_ntoa(&ip_info.ip));
	ESP_LOGI(TAG, "Subnet mask: %s", ip4addr_ntoa(&ip_info.netmask));
	ESP_LOGI(TAG, "Gateway:     %s", ip4addr_ntoa(&ip_info.gw));

	for (EVER)
	{
		vTaskDelay(1000 / portTICK_RATE_MS);
	}
}

/**
 * WIFI support for AP-STA mode
 * @param ssid
 * @param password
 */
void wifi_init_apsta(char *ssid, char *password)
{
	tcpip_adapter_init();

	ESP_ERROR_CHECK(esp_event_loop_init(wifi_event_handler, NULL));
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg))

	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

	ESP_LOGI(TAG, "Initializing WIFI in dual-mode...");

	//ESP_LOGI(TAG, "WIFI LR STA err code:%d", esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_LR));
	//ESP_LOGI(TAG, "WIFI LR AP err code:%d", esp_wifi_set_protocol(WIFI_IF_AP, WIFI_PROTOCOL_LR));

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));

	wifi_config_t wifi_ap_config = {
			.ap = {
					.ssid = "",
					.password = "",
					.max_connection = 4,
					.authmode = WIFI_AUTH_WPA_WPA2_PSK
			}
	};

	wifi_config_t wifi_sta_config = {
			.sta = {
					.ssid = "",
					.password = ""
			}
	};

	char *b[strlen(ap_ssid) + 7];
	sprintf((char *) b, "%s-%s", ap_ssid, esp_deviceMAC());

	ESP_LOGI(TAG, "Local access point SSID: %s", (char *) b);

	strcpy((char *) wifi_ap_config.ap.ssid, (char *) b);
	wifi_ap_config.ap.ssid_len = (uint8_t) strlen((char *) b);

	strcpy((char *) wifi_sta_config.sta.ssid, (char *) ssid);
	strcpy((char *) wifi_sta_config.sta.password, (char *) password);

	//wifi_config_t wifi_sta_config;

	//memset(wifi_sta_config.sta.ssid, 0, strlen((char *) ssid));
	//memset(wifi_sta_config.sta.password, 0, strlen((char *) password));

	//strcpy((char *) wifi_sta_config.sta.ssid, (char *) ssid);
	//strcpy((char *) wifi_sta_config.sta.password, (char *) password);

	if (strlen(ap_password) == 0)
	{
		wifi_ap_config.ap.authmode = WIFI_AUTH_OPEN;
	}

	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_ap_config));
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_sta_config));

	ESP_ERROR_CHECK(esp_wifi_start());
}


/**
 * WIFI support for Station mode
 * @param ssid
 * @param password
 */
void wifi_init_sta(char *ssid, char *password)
{
	tcpip_adapter_init();

	ESP_ERROR_CHECK(esp_event_loop_init(wifi_event_handler, NULL));
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg))

	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

	ESP_LOGI(TAG, "Initializing WIFI in STA-mode...");
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

	wifi_config_t wifi_sta_config = {
			.sta = {
					.ssid = "",
					.password = ""
			}
	};

	strcpy((char *) wifi_sta_config.sta.ssid, (char *) ssid);
	strcpy((char *) wifi_sta_config.sta.password, (char *) password);

	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_sta_config));

	ESP_ERROR_CHECK(esp_wifi_start());
}