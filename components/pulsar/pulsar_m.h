#pragma once
#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace pulsar_m {

class PulsarM : public Component, public uart::UARTDevice {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;

  void set_address(uint32_t address) { address_ = address; }
  void set_de_pin(GPIOPin *pin) { de_pin_ = pin; }

  // Сенсоры для данных и ошибок
  void set_flow_sensor(sensor::Sensor *sensor) { flow_sensor_ = sensor; }
  void set_error_sensor(sensor::Sensor *sensor) { error_sensor_ = sensor; }

 protected:
  uint32_t address_;
  GPIOPin *de_pin_{nullptr};
  sensor::Sensor *flow_sensor_{nullptr};
  sensor::Sensor *error_sensor_{nullptr};

  uint16_t crc16(const uint8_t *data, uint16_t len);
  void send_request(uint8_t function_code, const std::vector<uint8_t> &data);
  bool receive_response(std::vector<uint8_t> &response);
};

}  // namespace pulsar_m
}  // namespace esphome
