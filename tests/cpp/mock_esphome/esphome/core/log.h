#pragma once
// Stubs for ESPHome logging — all macros are no-ops in tests.
#define ESP_LOGE(tag, fmt, ...) (void)0
#define ESP_LOGW(tag, fmt, ...) (void)0
#define ESP_LOGI(tag, fmt, ...) (void)0
#define ESP_LOGD(tag, fmt, ...) (void)0
#define ESP_LOGV(tag, fmt, ...) (void)0
#define ESP_LOGCONFIG(tag, fmt, ...) (void)0
#define LOG_STR(s) (s)
#define LOG_CLIMATE(prefix, name, obj) (void)0
#define LOG_SENSOR(prefix, name, obj) (void)0
#define LOG_SELECT(prefix, name, obj) (void)0

using LogString = const char;

// Pull in optional for code that relies on log.h dragging in core utilities
#include "esphome/core/optional.h"
#include "esphome/core/helpers.h"
