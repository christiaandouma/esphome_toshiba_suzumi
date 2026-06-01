#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <cctype>
#include <cstdio>

namespace esphome {

inline bool str_equals_case_insensitive(const std::string &a, const std::string &b) {
  if (a.size() != b.size()) return false;
  for (size_t i = 0; i < a.size(); i++) {
    if (std::tolower(static_cast<unsigned char>(a[i])) !=
        std::tolower(static_cast<unsigned char>(b[i])))
      return false;
  }
  return true;
}

inline std::string format_hex_pretty(const uint8_t *data, size_t len) {
  std::string result;
  char buf[5];
  for (size_t i = 0; i < len; i++) {
    snprintf(buf, sizeof(buf), "%02X ", data[i]);
    result += buf;
  }
  return result;
}

inline std::string format_hex_pretty(const std::vector<uint8_t> &data) {
  return format_hex_pretty(data.data(), data.size());
}

}  // namespace esphome
