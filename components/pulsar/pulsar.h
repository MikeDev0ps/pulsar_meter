#pragma once

#ifndef PULSAR_COMPONENT_H
#define PULSAR_COMPONENT_H

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace pulsar {

class PulsarComponent : public PollingComponent, public uart::UARTDevice {
 public:
  float get_setup_priority() const override;
  void set_water_sensor(sensor::Sensor *water_sensor);

  void update() override;
  void dump_config() override;

 protected:
  bool pulsar_write_command_(const uint8_t *command, uint8_t *response, uint8_t response_length);

  sensor::Sensor *water_sensor_{nullptr};
};

}  // namespace pulsar
}  // namespace esphome

#endif // PULSAR_COMPONENT_H
