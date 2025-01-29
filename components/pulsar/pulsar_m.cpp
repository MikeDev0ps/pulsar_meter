#include "pulsar_m.h"
#include "esphome/core/log.h"

namespace esphome {
namespace pulsar_m {

static const char *const TAG = "pulsar_m";

void PulsarM::setup() {
  if (de_pin_) de_pin_->setup();
}

void PulsarM::dump_config() {
  ESP_LOGCONFIG(TAG, "Pulsar-M:");
  ESP_LOGCONFIG(TAG, "  Address: 0x%08X", address_);
  if (de_pin_) ESP_LOGCONFIG(TAG, "  DE/RE Pin: %d", de_pin_->get_pin());
}

uint16_t PulsarM::crc16(const uint8_t *data, uint16_t len) {
  uint16_t crc = 0xFFFF;
  while (len--) {
    crc ^= *data++;
    for (uint8_t i = 0; i < 8; i++) {
      if (crc & 0x01) {
        crc = (crc >> 1) ^ 0xA001;
      } else {
        crc >>= 1;
      }
    }
  }
  return crc;
}

void PulsarM::send_request(uint8_t function_code, const std::vector<uint8_t> &data) {
  if (de_pin_) de_pin_->digital_write(true); // Активация передачи
  delay(1);

  std::vector<uint8_t> packet;
  // Формирование пакета: ADDR (4 байта), F, L, DATA, ID, CRC
  for (int i = 3; i >= 0; i--) packet.push_back((address_ >> (8 * i)) & 0xFF);
  packet.push_back(function_code);
  packet.push_back(data.size() + 6); // L = длина DATA + 6 (ADDR, F, L, ID, CRC)
  packet.insert(packet.end(), data.begin(), data.end());
  packet.push_back(0x00); // ID (младший байт)
  packet.push_back(0x00); // ID (старший байт)

  uint16_t crc = crc16(packet.data(), packet.size());
  packet.push_back(crc & 0xFF);
  packet.push_back((crc >> 8) & 0xFF);

  write_array(packet);
  if (de_pin_) de_pin_->digital_write(false); // Возврат в режим приёма
}

bool PulsarM::receive_response(std::vector<uint8_t> &response) {
  uint8_t buffer[128];
  int len = read_array(buffer, sizeof(buffer));
  if (len < 8) return false;

  // Проверка CRC
  uint16_t received_crc = (buffer[len - 1] << 8) | buffer[len - 2];
  uint16_t calculated_crc = crc16(buffer, len - 2);
  if (received_crc != calculated_crc) {
    ESP_LOGE(TAG, "CRC error: 0x%04X != 0x%04X", received_crc, calculated_crc);
    return false;
  }

  // Извлечение данных (ADDR, F, L, DATA, ID, CRC)
  response.assign(buffer + 6, buffer + len - 2);
  return true;
}

void PulsarM::loop() {
  static uint32_t last_update = 0;
  if (millis() - last_update < 5000) return; // Обновление каждые 5 сек
  last_update = millis();

  // Запрос текущих показаний (функция 0x01)
  std::vector<uint8_t> data_in;
  data_in.push_back(0x01); // MASK_CH (младший байт, канал 1)
  send_request(0x01, data_in);

  // Обработка ответа
  std::vector<uint8_t> response;
  if (receive_response(response) && response.size() >= 8) {
    double value;
    memcpy(&value, response.data(), sizeof(double));
    if (flow_sensor_) flow_sensor_->publish_state(value);
  }

  // Запрос ошибок (параметр 0x0006)
  std::vector<uint8_t> param_request;
  param_request.push_back(0x06); // PARAM_NUM = 0x0006 (младший байт)
  param_request.push_back(0x00); // старший байт
  send_request(0x0A, param_request);

  if (receive_response(response) && response.size() >= 1) {
    uint8_t error_flags = response[0];
    if (error_sensor_) error_sensor_->publish_state(error_flags);
  }
}

}  // namespace pulsar_m
}  // namespace esphome
