#pragma once
#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace pulsar {

class PulsarComponent : public Component, public uart::UARTDevice {
 public:
  void set_address(uint32_t address) { this->address = address; }
  void set_channels(const std::vector<int> &channels) { this->channels = channels; }

  void setup() override;
  void loop() override;

 protected:
  uint32_t address;
  std::vector<int> channels;

  uint16_t calculate_crc16(const uint8_t *data, size_t length);
  void process_response(const uint8_t *response, size_t length);
};

}  // namespace pulsar
}  // namespace esphome
