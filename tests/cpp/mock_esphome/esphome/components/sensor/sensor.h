#pragma once

namespace esphome {
namespace sensor {

class Sensor {
 public:
  void publish_state(float value) { last_value = value; }
  float last_value{0.0f};
};

}  // namespace sensor
}  // namespace esphome
