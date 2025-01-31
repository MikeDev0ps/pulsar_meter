#include "pulsar_m.h"
#include "esphome/core/log.h"

namespace esphome {
namespace pulsar_m {

static const char *const TAG = "pulsar_m";

void PulsarM::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Pulsar M...");
}

void PulsarM::update() {
  ESP_LOGD(TAG, "Polling data from Pulsar M");
  this->query_data_();
}

void PulsarM::dump_config() {
  ESP_LOGCONFIG(TAG, "Pulsar M:");
  LOG_SENSOR("  ", "Total", this->total_sensor_);
  LOG_SENSOR("  ", "Flow Rate", this->flow_rate_sensor_);
  LOG_SENSOR("  ", "Errors", this->errors_sensor_);
  LOG_UPDATE_INTERVAL(this);
}

void PulsarM::query_data_() {
  // Request total consumption (example command - adjust according to protocol)
  uint8_t cmd[] = {0x01, address_, 0x03, 0x00, 0x00, 0x02};
  this->send_command_(0x03);
}

void PulsarM::send_command_(uint8_t command) {
  // Example command frame (adjust according to protocol documentation)
  std::vector<uint8_t> frame = {0x01, address_, command, 0x00, 0x00};
  
  // Calculate CRC (implement proper CRC calculation from protocol)
  frame.push_back(0x00); // CRC1 placeholder
  frame.push_back(0x00); // CRC2 placeholder
  
  this->write_array(frame);
  this->flush();
}

bool PulsarM::parse_response_(const std::vector<uint8_t> &data) {
  if(data.size() < 6) {
    ESP_LOGE(TAG, "Invalid response length");
    return false;
  }
  
  // Validate address and CRC
  if(data[0] != address_) {
    ESP_LOGE(TAG, "Address mismatch");
    return false;
  }
  
  // Parse data according to protocol structure
  // Example for total consumption (4 bytes, big-endian)
  if(data[1] == 0x03 && data.size() >= 8) {
    uint32_t total = (data[3] << 24) | (data[4] << 16) | (data[5] << 8) | data[6];
    float total_m3 = total / 1000.0f;
    
    if(total_sensor_ != nullptr)
      total_sensor_->publish_state(total_m3);
      
    return true;
  }
  
  // Add parsing for other commands (flow rate, errors)
  return false;
}

}  // namespace pulsar_m
}  // namespace esphome
