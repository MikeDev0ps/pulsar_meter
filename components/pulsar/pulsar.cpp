#include "pulsar.h"
#include "esphome/core/log.h"

namespace esphome {
namespace pulsar {

static const char *const TAG = "pulsar";

void PulsarComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Pulsar Component...");
}

void PulsarComponent::loop() {
  uint8_t request[12];
  uint8_t response[64];

  request[0] = (this->address >> 24) & 0xFF;
  request[1] = (this->address >> 16) & 0xFF;
  request[2] = (this->address >> 8) & 0xFF;
  request[3] = this->address & 0xFF;
  request[4] = 0x01;
  request[5] = 0x0E;

  uint32_t mask = 0;
  for (int channel : this->channels) {
    mask |= (1 << (channel - 1));
  }
  memcpy(&request[6], &mask, sizeof(mask));

  uint16_t crc = calculate_crc16(request, 10);
  request[10] = crc & 0xFF;
  request[11] = (crc >> 8) & 0xFF;

  this->write_array(request, sizeof(request));
  if (this->available()) {
    size_t len = this->read_array(response, sizeof(response));
    if (len > 0) {
      this->process_response(response, len);
    }
  }
}

uint16_t PulsarComponent::calculate_crc16(const uint8_t *data, size_t length) {
  uint16_t crc = 0xFFFF;
  for (size_t i = 0; i < length; i++) {
    crc ^= data[i];
    for (uint8_t j = 0; j < 8; j++) {
      if (crc & 1)
        crc = (crc >> 1) ^ 0xA001;
      else
        crc >>= 1;
    }
  }
  return crc;
}

void PulsarComponent::process_response(const uint8_t *response, size_t length) {
  ESP_LOGD(TAG, "Processing response of length: %d", length);
}

}  // namespace pulsar
}  // namespace esphome
