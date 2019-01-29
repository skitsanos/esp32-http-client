void task_wait_for_sta_connection(void *pvParameter);

void wifi_init_apsta(char *ssid, char *password);

void wifi_init_sta(char *ssid, char *password);

void wifi_init_intranet();

void wifi_connect();

void wifi_wait_for_connection();