#include "pulsar.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace pulsar {

static const char *const TAG = "pulsar";

// Пример команды для запроса данных
static const uint8_t PULSAR_COMMAND_READ[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B};

namespace {
  uint16_t calculate_crc16(const uint8_t *data, uint8_t length) {
    uint16_t crc = 0xFFFF;
    for (uint8_t i = 0; i < length; i++) {
      crc ^= data[i];
      for (uint8_t j = 0; j < 8; j++) {
        if (crc & 0x0001) {
          crc >>= 1;
          crc ^= 0xA001;
        } else {
          crc >>= 1;
        }
      }
    }
    return crc;
  }
}

void PulsarComponent::update() {
  uint8_t response[9]; // Ожидаемый размер ответа
  if (!this->pulsar_write_command_(PULSAR_COMMAND_READ, response, sizeof(response))) {
    ESP_LOGW(TAG, "Failed to read data from Pulsar!");
    this->status_set_warning();
    return;
  }

  // Проверка корректности ответа
  uint16_t received_crc = (response[7] | (response[8] << 8));
  uint16_t calculated_crc = calculate_crc16(response, 7);

  if (received_crc != calculated_crc) {
    ESP_LOGW(TAG, "CRC mismatch: received=0x%04X, calculated=0x%04X", received_crc, calculated_crc);
    this->status_set_warning();
    return;
  }

  if (response[0] != 0x01 || response[1] != 0x03) {
    ESP_LOGW(TAG, "Invalid response from Pulsar: %02x %02x", response[0], response[1]);
    this->status_set_warning();
    return;
  }

  // Парсинг данных
  uint16_t water_data = (response[3] << 8) | response[4];
  ESP_LOGD(TAG, "Water data: %d", water_data);
  if (this->water_sensor_ != nullptr)
    this->water_sensor_->publish_state(water_data);

  this->status_clear_warning();
}

bool PulsarComponent::pulsar_write_command_(const uint8_t *command, uint8_t *response, uint8_t response_length) {
  this->write_array(command, 8); // Длина команды
  this->flush();

  bool ret = this->read_array(response, response_length);
  if (!ret) {
    ESP_LOGW(TAG, "Failed to read response from UART");
  }
  this->flush();
  return ret;
}

void PulsarComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "Pulsar:");
  LOG_SENSOR("  ", "Water Usage", this->water_sensor_);
  this->check_uart_settings(9600);
}

}  // namespace pulsar
}  // namespace esphome