#ifndef WIFI_H
#define WIFI_H

#include "http.h"

void wifi_init(void);

void ChangeWiFiCredentials(const char *ssid, const char *password);
#endif