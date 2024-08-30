#pragma once
#include <cstdio>
#include <string>

namespace esphome {

#define ESP_LOGD(tag, format, ...)            \
    do {                                      \
        printf("[%s] " format "\n", tag, ##__VA_ARGS__); \
    } while (0);

#define ESP_LOGE(tag, format, ...)            \
    do {                                      \
        printf("[%s] " format "\n", tag, ##__VA_ARGS__); \
    } while (0);

#define ESP_LOGW(tag, format, ...)            \
    do {                                      \
        printf("[%s] " format "\n", tag, ##__VA_ARGS__); \
    } while (0);

#define ESP_LOGV(tag, format, ...)            \
    do {                                      \
        printf("[%s] " format "\n", tag, ##__VA_ARGS__); \
    } while (0);

#define ESP_LOGI(tag, format, ...)            \
    do {                                      \
        printf("[%s] " format "\n", tag, ##__VA_ARGS__); \
    } while (0);

} // namespace esphome
