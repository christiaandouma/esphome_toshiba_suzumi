#pragma once
#include <string>
#include <vector>
#include <initializer_list>
#include <set>
#include "esphome/core/optional.h"

namespace esphome {
namespace climate {

enum ClimateMode {
  CLIMATE_MODE_OFF = 0,
  CLIMATE_MODE_HEAT_COOL = 1,
  CLIMATE_MODE_COOL = 2,
  CLIMATE_MODE_HEAT = 3,
  CLIMATE_MODE_DRY = 4,
  CLIMATE_MODE_FAN_ONLY = 5,
};

enum ClimateFanMode {
  CLIMATE_FAN_ON = 0,
  CLIMATE_FAN_OFF = 1,
  CLIMATE_FAN_AUTO = 2,
  CLIMATE_FAN_LOW = 3,
  CLIMATE_FAN_MEDIUM = 4,
  CLIMATE_FAN_HIGH = 5,
  CLIMATE_FAN_MIDDLE = 6,
  CLIMATE_FAN_FOCUS = 7,
  CLIMATE_FAN_DIFFUSE = 8,
  CLIMATE_FAN_QUIET = 9,
};

enum ClimateSwingMode {
  CLIMATE_SWING_OFF = 0,
  CLIMATE_SWING_BOTH = 1,
  CLIMATE_SWING_VERTICAL = 2,
  CLIMATE_SWING_HORIZONTAL = 3,
};

enum ClimatePreset {
  CLIMATE_PRESET_NONE = 0,
  CLIMATE_PRESET_HOME = 1,
  CLIMATE_PRESET_AWAY = 2,
  CLIMATE_PRESET_BOOST = 3,
  CLIMATE_PRESET_COMFORT = 4,
  CLIMATE_PRESET_ECO = 5,
  CLIMATE_PRESET_SLEEP = 6,
  CLIMATE_PRESET_ACTIVITY = 7,
};

static const int CLIMATE_SUPPORTS_CURRENT_TEMPERATURE = 1;

inline const char *climate_mode_to_string(ClimateMode mode) {
  switch (mode) {
    case CLIMATE_MODE_OFF:       return "OFF";
    case CLIMATE_MODE_HEAT_COOL: return "HEAT_COOL";
    case CLIMATE_MODE_COOL:      return "COOL";
    case CLIMATE_MODE_HEAT:      return "HEAT";
    case CLIMATE_MODE_DRY:       return "DRY";
    case CLIMATE_MODE_FAN_ONLY:  return "FAN_ONLY";
    default:                     return "UNKNOWN";
  }
}

inline const char *climate_swing_mode_to_string(ClimateSwingMode mode) {
  switch (mode) {
    case CLIMATE_SWING_OFF:        return "OFF";
    case CLIMATE_SWING_BOTH:       return "BOTH";
    case CLIMATE_SWING_VERTICAL:   return "VERTICAL";
    case CLIMATE_SWING_HORIZONTAL: return "HORIZONTAL";
    default:                       return "UNKNOWN";
  }
}

inline const char *climate_fan_mode_to_string(ClimateFanMode mode) {
  switch (mode) {
    case CLIMATE_FAN_AUTO:   return "AUTO";
    case CLIMATE_FAN_LOW:    return "LOW";
    case CLIMATE_FAN_MEDIUM: return "MEDIUM";
    case CLIMATE_FAN_HIGH:   return "HIGH";
    case CLIMATE_FAN_QUIET:  return "QUIET";
    default:                 return "UNKNOWN";
  }
}

class ClimateTraits {
 public:
  void set_supported_modes(std::initializer_list<ClimateMode>) {}
  void set_supported_swing_modes(std::initializer_list<ClimateSwingMode>) {}
  void add_feature_flags(int) {}
  void add_supported_fan_mode(ClimateFanMode) {}
  void set_supported_custom_fan_modes(std::initializer_list<std::string>) {}
  void set_visual_temperature_step(float) {}
  void set_visual_min_temperature(float) {}
  void set_visual_max_temperature(float) {}
  void add_supported_preset(ClimatePreset) {}
  void set_supported_custom_presets(std::vector<const char *>) {}
};

class ClimateCall {
 public:
  esphome::optional<ClimateMode> get_mode() const { return {}; }
  esphome::optional<float> get_target_temperature() const { return {}; }
  esphome::optional<ClimateFanMode> get_fan_mode() const { return {}; }
  bool has_custom_fan_mode() const { return false; }
  std::string get_custom_fan_mode() const { return ""; }
  esphome::optional<ClimateSwingMode> get_swing_mode() const { return {}; }
  esphome::optional<ClimatePreset> get_preset() const { return {}; }
  bool has_custom_preset() const { return false; }
  std::string get_custom_preset() const { return ""; }
};

class Climate {
 public:
  ClimateMode mode{CLIMATE_MODE_OFF};
  float current_temperature{0.0f};
  float target_temperature{0.0f};
  ClimateSwingMode swing_mode{CLIMATE_SWING_OFF};

  virtual ClimateTraits traits() = 0;
  virtual void control(const ClimateCall &) = 0;

 protected:
  void set_fan_mode_(ClimateFanMode) {}
  void set_custom_fan_mode_(const std::string &) {}
  void set_preset_(ClimatePreset) {}
  void set_custom_preset_(const std::string &) {}
  void publish_state() {}
};

}  // namespace climate
}  // namespace esphome
