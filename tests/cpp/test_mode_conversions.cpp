#include <gtest/gtest.h>
#include "toshiba_climate_mode.h"

using namespace esphome::toshiba_suzumi;
using namespace esphome::climate;

// ── ClimateModeToInt / IntToClimateMode ─────────────────────────────────────

TEST(ClimateMode, AllModesConvertToProtocol) {
  EXPECT_EQ(ClimateModeToInt(CLIMATE_MODE_HEAT_COOL), MODE::HEAT_COOL);
  EXPECT_EQ(ClimateModeToInt(CLIMATE_MODE_COOL),      MODE::COOL);
  EXPECT_EQ(ClimateModeToInt(CLIMATE_MODE_HEAT),      MODE::HEAT);
  EXPECT_EQ(ClimateModeToInt(CLIMATE_MODE_DRY),       MODE::DRY);
  EXPECT_EQ(ClimateModeToInt(CLIMATE_MODE_FAN_ONLY),  MODE::FAN_ONLY);
}

TEST(ClimateMode, AllProtocolModesConvertToClimate) {
  EXPECT_EQ(IntToClimateMode(MODE::HEAT_COOL), CLIMATE_MODE_HEAT_COOL);
  EXPECT_EQ(IntToClimateMode(MODE::COOL),      CLIMATE_MODE_COOL);
  EXPECT_EQ(IntToClimateMode(MODE::HEAT),      CLIMATE_MODE_HEAT);
  EXPECT_EQ(IntToClimateMode(MODE::DRY),       CLIMATE_MODE_DRY);
  EXPECT_EQ(IntToClimateMode(MODE::FAN_ONLY),  CLIMATE_MODE_FAN_ONLY);
}

TEST(ClimateMode, InvalidProtocolValueFallsBackToOff) {
  EXPECT_EQ(IntToClimateMode(static_cast<MODE>(0xFF)), CLIMATE_MODE_OFF);
}

TEST(ClimateMode, RoundTrip) {
  for (auto cm : {CLIMATE_MODE_HEAT_COOL, CLIMATE_MODE_COOL, CLIMATE_MODE_HEAT,
                  CLIMATE_MODE_DRY, CLIMATE_MODE_FAN_ONLY}) {
    EXPECT_EQ(IntToClimateMode(ClimateModeToInt(cm)), cm);
  }
}

// ── ClimateSwingModeToInt / IntToClimateSwingMode ────────────────────────────

TEST(SwingMode, AllModesConvertToProtocol) {
  EXPECT_EQ(ClimateSwingModeToInt(CLIMATE_SWING_OFF),        SWING::OFF);
  EXPECT_EQ(ClimateSwingModeToInt(CLIMATE_SWING_VERTICAL),   SWING::VERTICAL);
  EXPECT_EQ(ClimateSwingModeToInt(CLIMATE_SWING_HORIZONTAL), SWING::HORIZONTAL);
  EXPECT_EQ(ClimateSwingModeToInt(CLIMATE_SWING_BOTH),       SWING::BOTH);
}

TEST(SwingMode, AllProtocolModesConvert) {
  EXPECT_EQ(IntToClimateSwingMode(SWING::OFF),        CLIMATE_SWING_OFF);
  EXPECT_EQ(IntToClimateSwingMode(SWING::VERTICAL),   CLIMATE_SWING_VERTICAL);
  EXPECT_EQ(IntToClimateSwingMode(SWING::HORIZONTAL), CLIMATE_SWING_HORIZONTAL);
  EXPECT_EQ(IntToClimateSwingMode(SWING::BOTH),       CLIMATE_SWING_BOTH);
}

TEST(SwingMode, InvalidFallsBackToOff) {
  EXPECT_EQ(IntToClimateSwingMode(static_cast<SWING>(0xFF)), CLIMATE_SWING_OFF);
}

TEST(SwingMode, RoundTrip) {
  for (auto sm : {CLIMATE_SWING_OFF, CLIMATE_SWING_VERTICAL,
                  CLIMATE_SWING_HORIZONTAL, CLIMATE_SWING_BOTH}) {
    EXPECT_EQ(IntToClimateSwingMode(ClimateSwingModeToInt(sm)), sm);
  }
}

// ── ClimateFanModeToInt ───────────────────────────────────────────────────────

TEST(FanMode, StandardModesReturnValues) {
  EXPECT_EQ(ClimateFanModeToInt(CLIMATE_FAN_AUTO),   FAN::FAN_AUTO);
  EXPECT_EQ(ClimateFanModeToInt(CLIMATE_FAN_QUIET),  FAN::FAN_QUIET);
  EXPECT_EQ(ClimateFanModeToInt(CLIMATE_FAN_LOW),    FAN::FAN_LOW);
  EXPECT_EQ(ClimateFanModeToInt(CLIMATE_FAN_MEDIUM), FAN::FAN_MEDIUM);
  EXPECT_EQ(ClimateFanModeToInt(CLIMATE_FAN_HIGH),   FAN::FAN_HIGH);
}

TEST(FanMode, StandardModesReturnOptional) {
  EXPECT_TRUE(ClimateFanModeToInt(CLIMATE_FAN_AUTO).has_value());
  EXPECT_TRUE(ClimateFanModeToInt(CLIMATE_FAN_HIGH).has_value());
}

TEST(FanMode, UnknownModeReturnsNullopt) {
  EXPECT_FALSE(ClimateFanModeToInt(static_cast<ClimateFanMode>(99)).has_value());
}

// ── StringToFanLevel — this was the pointer-comparison bug ───────────────────

TEST(CustomFanLevel, LowMediumConverts) {
  // Previously broken: used pointer comparison instead of string comparison.
  auto result = StringToFanLevel("Low-Medium");
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), FAN::FANMODE_2);
}

TEST(CustomFanLevel, MediumHighConverts) {
  auto result = StringToFanLevel("Medium-High");
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), FAN::FANMODE_4);
}

TEST(CustomFanLevel, CaseInsensitive) {
  EXPECT_EQ(StringToFanLevel("low-medium").value(),  FAN::FANMODE_2);
  EXPECT_EQ(StringToFanLevel("LOW-MEDIUM").value(),  FAN::FANMODE_2);
  EXPECT_EQ(StringToFanLevel("medium-high").value(), FAN::FANMODE_4);
}

TEST(CustomFanLevel, UnknownStringReturnsNullopt) {
  EXPECT_FALSE(StringToFanLevel("unknown").has_value());
  EXPECT_FALSE(StringToFanLevel("").has_value());
  EXPECT_FALSE(StringToFanLevel("High").has_value());
}

// ── IntToCustomFanMode ────────────────────────────────────────────────────────

TEST(CustomFanLevel, IntToCustomFanModeConverts) {
  EXPECT_STREQ(IntToCustomFanMode(FAN::FANMODE_2), CUSTOM_FAN_LEVEL_2);
  EXPECT_STREQ(IntToCustomFanMode(FAN::FANMODE_4), CUSTOM_FAN_LEVEL_4);
}

// ── StringToPwrLevel / IntToPowerLevel ───────────────────────────────────────

TEST(PowerLevel, StringToPwrLevelConverts) {
  EXPECT_EQ(StringToPwrLevel("50 %").value(),  PWR_LEVEL::PCT_50);
  EXPECT_EQ(StringToPwrLevel("75 %").value(),  PWR_LEVEL::PCT_75);
  EXPECT_EQ(StringToPwrLevel("100 %").value(), PWR_LEVEL::PCT_100);
}

TEST(PowerLevel, StringToPwrLevelCaseInsensitive) {
  EXPECT_EQ(StringToPwrLevel("50 %").value(), PWR_LEVEL::PCT_50);
}

TEST(PowerLevel, StringToPwrLevelUnknownReturnsNullopt) {
  EXPECT_FALSE(StringToPwrLevel("unknown").has_value());
  EXPECT_FALSE(StringToPwrLevel("").has_value());
}

TEST(PowerLevel, IntToPowerLevelConverts) {
  EXPECT_EQ(IntToPowerLevel(PWR_LEVEL::PCT_50),  CUSTOM_PWR_LEVEL_50);
  EXPECT_EQ(IntToPowerLevel(PWR_LEVEL::PCT_75),  CUSTOM_PWR_LEVEL_75);
  EXPECT_EQ(IntToPowerLevel(PWR_LEVEL::PCT_100), CUSTOM_PWR_LEVEL_100);
}

// ── PresetToSpecialMode / SpecialModeToPreset ────────────────────────────────

TEST(SpecialMode, PresetToSpecialModeAllPresets) {
  EXPECT_EQ(PresetToSpecialMode("Standard").value(),    SPECIAL_MODE::STANDARD);
  EXPECT_EQ(PresetToSpecialMode("Hi POWER").value(),    SPECIAL_MODE::HI_POWER);
  EXPECT_EQ(PresetToSpecialMode("ECO").value(),         SPECIAL_MODE::ECO);
  EXPECT_EQ(PresetToSpecialMode("Fireplace 1").value(), SPECIAL_MODE::FIREPLACE_1);
  EXPECT_EQ(PresetToSpecialMode("Fireplace 2").value(), SPECIAL_MODE::FIREPLACE_2);
  EXPECT_EQ(PresetToSpecialMode("8 degrees").value(),   SPECIAL_MODE::EIGHT_DEG);
  EXPECT_EQ(PresetToSpecialMode("Silent#1").value(),    SPECIAL_MODE::SILENT_1);
  EXPECT_EQ(PresetToSpecialMode("Silent#2").value(),    SPECIAL_MODE::SILENT_2);
  EXPECT_EQ(PresetToSpecialMode("Sleep").value(),       SPECIAL_MODE::SLEEP);
  EXPECT_EQ(PresetToSpecialMode("Floor").value(),       SPECIAL_MODE::FLOOR);
  EXPECT_EQ(PresetToSpecialMode("Comfort").value(),     SPECIAL_MODE::COMFORT);
}

TEST(SpecialMode, PresetToSpecialModeCaseInsensitive) {
  EXPECT_EQ(PresetToSpecialMode("standard").value(), SPECIAL_MODE::STANDARD);
  EXPECT_EQ(PresetToSpecialMode("STANDARD").value(), SPECIAL_MODE::STANDARD);
  EXPECT_EQ(PresetToSpecialMode("eco").value(),      SPECIAL_MODE::ECO);
}

TEST(SpecialMode, PresetToSpecialModeUnknownReturnsNullopt) {
  EXPECT_FALSE(PresetToSpecialMode("unknown").has_value());
  EXPECT_FALSE(PresetToSpecialMode("").has_value());
}

TEST(SpecialMode, SpecialModeToPresetAllModes) {
  EXPECT_STREQ(SpecialModeToPreset(SPECIAL_MODE::STANDARD),    SPECIAL_MODE_STANDARD);
  EXPECT_STREQ(SpecialModeToPreset(SPECIAL_MODE::HI_POWER),    SPECIAL_MODE_HI_POWER);
  EXPECT_STREQ(SpecialModeToPreset(SPECIAL_MODE::ECO),         SPECIAL_MODE_ECO);
  EXPECT_STREQ(SpecialModeToPreset(SPECIAL_MODE::FIREPLACE_1), SPECIAL_MODE_FIREPLACE_1);
  EXPECT_STREQ(SpecialModeToPreset(SPECIAL_MODE::FIREPLACE_2), SPECIAL_MODE_FIREPLACE_2);
  EXPECT_STREQ(SpecialModeToPreset(SPECIAL_MODE::EIGHT_DEG),   SPECIAL_MODE_EIGHT_DEG);
  EXPECT_STREQ(SpecialModeToPreset(SPECIAL_MODE::SILENT_1),    SPECIAL_MODE_SILENT_1);
  EXPECT_STREQ(SpecialModeToPreset(SPECIAL_MODE::SILENT_2),    SPECIAL_MODE_SILENT_2);
  EXPECT_STREQ(SpecialModeToPreset(SPECIAL_MODE::SLEEP),       SPECIAL_MODE_SLEEP);
  EXPECT_STREQ(SpecialModeToPreset(SPECIAL_MODE::FLOOR),       SPECIAL_MODE_FLOOR);
  EXPECT_STREQ(SpecialModeToPreset(SPECIAL_MODE::COMFORT),     SPECIAL_MODE_COMFORT);
}

TEST(SpecialMode, SpecialModePresetRoundTrip) {
  for (auto mode : {SPECIAL_MODE::STANDARD, SPECIAL_MODE::HI_POWER, SPECIAL_MODE::ECO,
                    SPECIAL_MODE::FIREPLACE_1, SPECIAL_MODE::FIREPLACE_2,
                    SPECIAL_MODE::EIGHT_DEG, SPECIAL_MODE::SILENT_1, SPECIAL_MODE::SILENT_2,
                    SPECIAL_MODE::SLEEP, SPECIAL_MODE::FLOOR, SPECIAL_MODE::COMFORT}) {
    auto preset_str = SpecialModeToPreset(mode);
    auto back = PresetToSpecialMode(preset_str);
    ASSERT_TRUE(back.has_value()) << "Round-trip failed for mode " << (int)mode;
    EXPECT_EQ(back.value(), mode);
  }
}

// ── StringToClimatePreset / ClimatePresetToString ────────────────────────────

TEST(ClimatePreset, StringToClimatePresetMappedModes) {
  EXPECT_EQ(StringToClimatePreset("Standard"), CLIMATE_PRESET_NONE);
  EXPECT_EQ(StringToClimatePreset("ECO"),      CLIMATE_PRESET_ECO);
  EXPECT_EQ(StringToClimatePreset("Hi POWER"), CLIMATE_PRESET_BOOST);
  EXPECT_EQ(StringToClimatePreset("Sleep"),    CLIMATE_PRESET_SLEEP);
  EXPECT_EQ(StringToClimatePreset("Comfort"),  CLIMATE_PRESET_COMFORT);
}

TEST(ClimatePreset, StringToClimatePresetCustomModesReturnNone) {
  // Modes without a standard ESPHome preset mapping fall back to NONE
  EXPECT_EQ(StringToClimatePreset("Fireplace 1"), CLIMATE_PRESET_NONE);
  EXPECT_EQ(StringToClimatePreset("8 degrees"),   CLIMATE_PRESET_NONE);
  EXPECT_EQ(StringToClimatePreset("Silent#1"),    CLIMATE_PRESET_NONE);
  EXPECT_EQ(StringToClimatePreset("Floor"),       CLIMATE_PRESET_NONE);
}

TEST(ClimatePreset, ClimatePresetToStringConverts) {
  EXPECT_STREQ(ClimatePresetToString(CLIMATE_PRESET_NONE),    SPECIAL_MODE_STANDARD);
  EXPECT_STREQ(ClimatePresetToString(CLIMATE_PRESET_ECO),     SPECIAL_MODE_ECO);
  EXPECT_STREQ(ClimatePresetToString(CLIMATE_PRESET_BOOST),   SPECIAL_MODE_HI_POWER);
  EXPECT_STREQ(ClimatePresetToString(CLIMATE_PRESET_SLEEP),   SPECIAL_MODE_SLEEP);
  EXPECT_STREQ(ClimatePresetToString(CLIMATE_PRESET_COMFORT), SPECIAL_MODE_COMFORT);
}

TEST(ClimatePreset, ClimatePresetToStringUnknownFallsBack) {
  EXPECT_STREQ(ClimatePresetToString(static_cast<ClimatePreset>(99)), SPECIAL_MODE_STANDARD);
}

// ── SpecialModeToClimatePreset ────────────────────────────────────────────────

TEST(ClimatePreset, SpecialModeToClimatePresetMappedModes) {
  EXPECT_EQ(SpecialModeToClimatePreset(SPECIAL_MODE::STANDARD).value(), CLIMATE_PRESET_NONE);
  EXPECT_EQ(SpecialModeToClimatePreset(SPECIAL_MODE::ECO).value(),      CLIMATE_PRESET_ECO);
  EXPECT_EQ(SpecialModeToClimatePreset(SPECIAL_MODE::HI_POWER).value(), CLIMATE_PRESET_BOOST);
  EXPECT_EQ(SpecialModeToClimatePreset(SPECIAL_MODE::SLEEP).value(),    CLIMATE_PRESET_SLEEP);
  EXPECT_EQ(SpecialModeToClimatePreset(SPECIAL_MODE::COMFORT).value(),  CLIMATE_PRESET_COMFORT);
}

TEST(ClimatePreset, SpecialModeToClimatePresetCustomModesReturnNullopt) {
  EXPECT_FALSE(SpecialModeToClimatePreset(SPECIAL_MODE::FIREPLACE_1).has_value());
  EXPECT_FALSE(SpecialModeToClimatePreset(SPECIAL_MODE::FIREPLACE_2).has_value());
  EXPECT_FALSE(SpecialModeToClimatePreset(SPECIAL_MODE::EIGHT_DEG).has_value());
  EXPECT_FALSE(SpecialModeToClimatePreset(SPECIAL_MODE::SILENT_1).has_value());
  EXPECT_FALSE(SpecialModeToClimatePreset(SPECIAL_MODE::SILENT_2).has_value());
  EXPECT_FALSE(SpecialModeToClimatePreset(SPECIAL_MODE::FLOOR).has_value());
}
