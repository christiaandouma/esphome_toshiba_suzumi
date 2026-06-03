#pragma once
#include <string>
#include <vector>
#include "esphome/core/optional.h"

namespace esphome {

namespace setup_priority {
  constexpr float LATE = -100.0f;
}

template<typename T>
class Parented {
 public:
  void set_parent(T *parent) { parent_ = parent; }
  T *parent_{nullptr};
};

class Component {
 public:
  virtual void setup() {}
  virtual void loop() {}
  virtual void dump_config() {}
  virtual float get_setup_priority() const { return 0.0f; }
};

class PollingComponent : public Component {
 public:
  virtual void update() {}
};

}  // namespace esphome
