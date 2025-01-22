#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace pulsar {

class PulsarComponent : public PollingComponent, public uart::UARTDevice {
 public:
  void set_water_sensor(sensor::Sensor *water_sensor) { this->water_sensor_ = water_sensor; }
  void set_poll_interval(uint32_t interval) { this->poll_interval_ = interval; }

  void setup() override;
  void update() override;
  void dump_config() override;

 protected:
  sensor::Sensor *water_sensor_{nullptr};
  uint32_t poll_interval_{60000};  // Default: 60 seconds

  bool send_command(const uint8_t *command, size_t command_length, uint8_t *response, size_t response_length);
  bool parse_response(const uint8_t *response, size_t length, uint16_t &parsed_data);
};

}  // namespace pulsar
}  // namespace esphome
