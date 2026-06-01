#pragma once
#include <cstdint>
#include <vector>

namespace esphome {
namespace uart {

class UARTDevice {
 public:
  bool available() { return false; }
  bool read_byte(uint8_t *) { return false; }
  void write_array(const std::vector<uint8_t> &) {}
};

}  // namespace uart
}  // namespace esphome
