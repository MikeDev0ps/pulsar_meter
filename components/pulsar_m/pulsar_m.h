#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace pulsar_m {

class PulsarMComponent : public PollingComponent, public uart::UARTDevice {
 public:
  void setup() override;
  void update() override;
  void dump_config() override;

  void set_address(uint8_t address) { address_ = address; }
  void set_flow_sensor(sensor::Sensor *sensor) { flow_sensor_ = sensor; }
  void set_total_sensor(sensor::Sensor *sensor) { total_sensor_ = sensor; }
  void set_error_sensor(sensor::Sensor *sensor) { error_sensor_ = sensor; }

 protected:
  uint8_t address_{1};
  sensor::Sensor *flow_sensor_{nullptr};
  sensor::Sensor *total_sensor_{nullptr};
  sensor::Sensor *error_sensor_{nullptr};
  
  bool parse_response_(const std::vector<uint8_t> &data);
  void send_command_(uint8_t function_code);
};

}  // namespace pulsar_m
}  // namespace esphome
