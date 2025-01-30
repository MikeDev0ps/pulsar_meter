#include "pulsar_m.h"
#include "esphome/core/log.h"

namespace esphome {
namespace pulsar_m {

static const char *const TAG = "pulsar_m";

void PulsarMComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Pulsar M...");
}

void PulsarMComponent::update() {
  send_command_(0x03); // Пример команды чтения данных
}

void PulsarMComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "Pulsar M:");
  ESP_LOGCONFIG(TAG, "  Address: 0x%02X", address_);
  LOG_SENSOR("  ", "Flow Rate", flow_sensor_);
  LOG_SENSOR("  ", "Total Volume", total_sensor_);
  LOG_SENSOR("  ", "Errors", error_sensor_);
}

void PulsarMComponent::send_command_(uint8_t function_code) {
  std::vector<uint8_t> cmd = {address_, function_code, 0x00, 0x00, 0x00, 0x02};
  write_array(cmd);
}

bool PulsarMComponent::parse_response_(const std::vector<uint8_t> &data) {
  if (data.size() < 5) return false;
  
  // Пример парсинга данных (зависит от протокола)
  if (data[1] == 0x03) {
    float flow = (data[3] << 8 | data[4]) / 10.0f;
    float total = (data[5] << 24 | data[6] << 16 | data[7] << 8 | data[8]) / 1000.0f;
    
    if (flow_sensor_) flow_sensor_->publish_state(flow);
    if (total_sensor_) total_sensor_->publish_state(total);
    return true;
  }
  return false;
}

}  // namespace pulsar_m
}  // namespace esphome
