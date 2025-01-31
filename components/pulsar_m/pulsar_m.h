#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace pulsar_m {

class PulsarMComponent : public Component {
 public:
  void set_uart_parent(uart::UARTComponent *uart) { this->uart_ = uart; }
  void set_address(uint32_t address) { this->address_ = address; }
  void setup() override;
  void loop() override;
  void dump_config() override;

  void set_total_volume_sensor(sensor::Sensor *sensor) { this->total_volume_sensor_ = sensor; }
  void set_error_sensor(sensor::Sensor *sensor) { this->error_sensor_ = sensor; }

 protected:
  uint32_t address_;
  uart::UARTComponent *uart_{nullptr};
  sensor::Sensor *total_volume_sensor_{nullptr};
  sensor::Sensor *error_sensor_{nullptr};

  uint16_t calculate_crc16(const uint8_t *data, uint16_t size);
  void send_request(uint8_t function_code, const uint8_t *data, uint8_t data_length);
  void process_response(const uint8_t *data, uint8_t length);

  void read_channel_values();
  void read_diagnostic_errors();
};

}  // namespace pulsar_m
}  // namespace esphome
