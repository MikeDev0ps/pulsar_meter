#include "pulsar_m.h"
#include "esphome/core/log.h"

namespace esphome {
namespace pulsar_m {

static const char *const TAG = "pulsar_m";

uint16_t PulsarMComponent::calculate_crc16(const uint8_t *data, uint16_t size) {
  uint16_t w = 0xFFFF;
  for (uint16_t byte_cnt = 0; byte_cnt < size; byte_cnt++) {
    w ^= data[byte_cnt];
    for (uint8_t shift_cnt = 0; shift_cnt < 8; shift_cnt++) {
      uint8_t f = w & 0x0001;
      w >>= 1;
      if (f) w ^= 0xA001;
    }
  }
  return w;
}

void PulsarMComponent::send_request(uint8_t function_code, const uint8_t *data, uint8_t data_length) {
  uint8_t request[16] = {0};
  uint8_t index = 0;

  // ADDR
  request[index++] = (this->address_ >> 24) & 0xFF;
  request[index++] = (this->address_ >> 16) & 0xFF;
  request[index++] = (this->address_ >> 8) & 0xFF;
  request[index++] = this->address_ & 0xFF;

  // F
  request[index++] = function_code;

  // L
  request[index++] = data_length + 2;  // Data length + CRC16

  // DATA_IN
  for (uint8_t i = 0; i < data_length; i++) {
    request[index++] = data[i];
  }

  // CRC16
  uint16_t crc = this->calculate_crc16(request, index);
  request[index++] = crc & 0xFF;
  request[index++] = (crc >> 8) & 0xFF;

  this->write_array(request, index);
}

void PulsarMComponent::process_response(const uint8_t *data, uint8_t length) {
  if (length < 8) {
    ESP_LOGW(TAG, "Invalid response length");
    return;
  }

  uint16_t crc_received = (data[length - 2] << 8) | data[length - 1];
  uint16_t crc_calculated = this->calculate_crc16(data, length - 2);

  if (crc_received != crc_calculated) {
    ESP_LOGW(TAG, "CRC mismatch");
    return;
  }

  uint8_t function_code = data[4];
  if (function_code == 0x01) {
    // Process channel values
    float total_volume = *(float *)(data + 7);  // Assuming IEEE 754 float
    if (this->total_volume_sensor_ != nullptr) {
      this->total_volume_sensor_->publish_state(total_volume);
    }
  } else if (function_code == 0x0A) {
    // Process diagnostic errors
    uint8_t error_flags = data[7];
    if (this->error_sensor_ != nullptr) {
      this->error_sensor_->publish_state(error_flags);
    }
  }
}

void PulsarMComponent::read_channel_values() {
  uint8_t mask_ch[4] = {0x02, 0x00, 0x00, 0x00};  // Example: Read second channel
  this->send_request(0x01, mask_ch, 4);
}

void PulsarMComponent::read_diagnostic_errors() {
  uint8_t param_num[2] = {0x06, 0x00};  // PARAM_NUM=0x0006
  this->send_request(0x0A, param_num, 2);
}

void PulsarMComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Pulsar-M device with address %u", this->address_);
}

void PulsarMComponent::loop() {
  if (this->available()) {
    uint8_t buffer[32];
    int length = this->read_array(buffer, sizeof(buffer));
    this->process_response(buffer, length);
  }

  // Periodically read data
  static uint32_t last_read_time = 0;
  if (millis() - last_read_time > 5000) {  // Every 5 seconds
    this->read_channel_values();
    this->read_diagnostic_errors();
    last_read_time = millis();
  }
}

void PulsarMComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "Pulsar-M Device:");
  ESP_LOGCONFIG(TAG, "  Address: %u", this->address_);
}

}  // namespace pulsar_m
}  // namespace esphome
