#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace pulsar_m {

class PulsarM : public PollingComponent, public uart::UARTDevice {
 public:
  void set_address(uint8_t address) { address_ = address; }
  void set_total_sensor(sensor::Sensor *sensor) { total_sensor_ = sensor; }
  void set_flow_rate_sensor(sensor::Sensor *sensor) { flow_rate_sensor_ = sensor; }
  void set_errors_sensor(sensor::Sensor *sensor) { errors_sensor_ = sensor; }

  void setup() override;
  void update() override;
  void dump_config() override;

 protected:
  uint8_t address_;
  bool parse_response_(const std::vector<uint8_t> &data);
  
  sensor::Sensor *total_sensor_{nullptr};
  sensor::Sensor *flow_rate_sensor_{nullptr};
  sensor::Sensor *errors_sensor_{nullptr};
  
  void send_command_(uint8_t command);
  void query_data_();
};

}  // namespace pulsar_m
}  // namespace esphome
