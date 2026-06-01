#pragma once
#include <string>

namespace esphome {
namespace select {

class Select {
 public:
  void publish_state(const std::string &value) { last_value = value; }
  std::string last_value;
  virtual void control(const std::string &) = 0;
};

}  // namespace select
}  // namespace esphome
