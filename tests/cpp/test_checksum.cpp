#include <gtest/gtest.h>
#include <vector>
#include <cstdint>
#include <numeric>

// Mirror of checksum() in toshiba_climate.cpp — keep in sync.
// The algorithm: sum bytes[1..length-1], return (256 - sum) as uint8_t.
static uint8_t checksum(std::vector<uint8_t> data, uint8_t length) {
  uint8_t sum = 0;
  for (size_t i = 1; i < length; i++) {
    sum += data[i];
  }
  return static_cast<uint8_t>(256 - sum);
}

// sendCmd builds: {2, 0, 3, 16, 0, 0, 7, 1, 48, 1, 0, 2, cmd, value}
// Bytes[1..13] are summed for the checksum (byte 0 = 0x02 is excluded).
static std::vector<uint8_t> make_send_cmd_payload(uint8_t cmd, uint8_t value) {
  std::vector<uint8_t> p = {2, 0, 3, 16, 0, 0, 7, 1, 48, 1, 0, 2, cmd, value};
  p.push_back(checksum(p, static_cast<uint8_t>(p.size())));
  return p;
}

TEST(Checksum, KnownPayloadPowerStateOn) {
  // sendCmd(POWER_STATE=128, STATE::ON=48)
  // sum of bytes[1..13] = 0+3+16+0+0+7+1+48+1+0+2+128+48 = 254
  // checksum = uint8_t(256 - 254) = 2
  auto payload = make_send_cmd_payload(128, 48);
  EXPECT_EQ(payload.back(), 2u);
}

TEST(Checksum, KnownPayloadModeHeatCool) {
  // sendCmd(MODE=176, HEAT_COOL=65)
  // sum wraps: 0+3+16+0+0+7+1+48+1+0+2+176+65 = 319, 319 mod 256 = 63
  // checksum = uint8_t(256 - 63) = 193
  auto payload = make_send_cmd_payload(176, 65);
  EXPECT_EQ(payload.back(), 193u);
}

TEST(Checksum, SingleDataByte) {
  // data = {0x02, 0x01}, length = 2
  // sum = data[1] = 1, checksum = 255
  std::vector<uint8_t> data = {0x02, 0x01};
  EXPECT_EQ(checksum(data, 2), 255u);
}

TEST(Checksum, ZeroSumWrapsToZero) {
  // When all bytes[1..] sum to 0 (mod 256), checksum = uint8_t(256) = 0.
  // Construct bytes that sum to 256 (overflow to 0 in uint8_t arithmetic).
  std::vector<uint8_t> data = {0x02, 0x80, 0x80};  // 0x80 + 0x80 = 256 -> sum = 0
  EXPECT_EQ(checksum(data, 3), 0u);
}

TEST(Checksum, SumWrapsCorrectly) {
  // Verify uint8_t sum wrapping: 0xFF + 0x01 = 0x00 (in uint8_t)
  // sum = 255 + 1 = 256 → wraps to 0; checksum = uint8_t(256-0) = 0
  std::vector<uint8_t> data = {0x02, 0xFF, 0x01};
  EXPECT_EQ(checksum(data, 3), 0u);
}

TEST(Checksum, RoundTripValidation) {
  // A message with a correctly appended checksum has sum (bytes[1..last]) == 0 (mod 256).
  auto payload = make_send_cmd_payload(160, 65);  // FAN=160, MEDIUM=52
  // Sum all bytes[1..last] including the checksum byte — must equal 0 mod 256.
  uint8_t total = 0;
  for (size_t i = 1; i < payload.size(); i++) {
    total += payload[i];
  }
  EXPECT_EQ(total, 0u);
}

TEST(Checksum, LengthOneDoesNotReadBytes) {
  // length=1 means loop runs from i=1 to i<1, i.e., never.
  // sum stays 0, checksum = 0.
  std::vector<uint8_t> data = {0x02, 0xAB, 0xCD};
  EXPECT_EQ(checksum(data, 1), 0u);
}
