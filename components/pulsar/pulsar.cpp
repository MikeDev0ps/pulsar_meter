#include "pulsar.h"
#include "esphome/core/log.h"

namespace esphome {
namespace pulsar {

static const char *const TAG = "pulsar";

void PulsarComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Pulsar Component...");
}

void PulsarComponent::update() {
  ESP_LOGD(TAG, "Polling Pulsar device...");

  uint8_t command[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02};  // Example command
  uint8_t response[9];
  uint16_t water_data = 0;

  if (!this->send_command(command, sizeof(command), response, sizeof(response))) {
    ESP_LOGW(TAG, "Failed to communicate with Pulsar device.");
    return;
  }

  if (!this->parse_response(response, sizeof(response), water_data)) {
    ESP_LOGW(TAG, "Failed to parse response from Pulsar.");
    return;
  }

  ESP_LOGD(TAG, "Water usage: %d L", water_data);
  if (this->water_sensor_ != nullptr) {
    this->water_sensor_->publish_state(water_data);
  }
}

void PulsarComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "Pulsar:");
  LOG_SENSOR("  ", "Water Usage", this->water_sensor_);
}

void PulsarComponent::set_update_interval(uint32_t interval) {
  this->set_interval(interval);
}

bool PulsarComponent::send_command(const uint8_t *command, size_t command_length, uint8_t *response, size_t response_length) {
  this->write_array(command, command_length);
  this->flush();

  if (!this->read_array(response, response_length)) {
    ESP_LOGW(TAG, "UART read failed");
    return false;
  }

  return true;
}

bool PulsarComponent::parse_response(const uint8_t *response, size_t length, uint16_t &parsed_data) {
  if (length < 7) {  // Example minimum length
    ESP_LOGW(TAG, "Response too short");
    return false;
  }

  // Example CRC16 validation (implement your logic here)
  uint16_t crc = (response[length - 2] << 8) | response[length - 1];
  if (crc != 0xAABB) {  // Replace with real CRC calculation
    ESP_LOGW(TAG, "Invalid CRC");
    return false;
  }

  parsed_data = (response[3] << 8) | response[4];  // Example parsing
  return true;
}

}  // namespace pulsar
}  // namespace esphome
