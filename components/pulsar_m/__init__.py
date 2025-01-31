# __init__.py

import esphome.config_validation as cv
from esphome.components import sensor
from esphome.core import CORE

DEPENDENCIES = ["uart"]  # Указываем зависимости компонента

def validate_config(config):
    # Здесь можно добавить валидацию конфигурации
    return config

def to_code(config):
    # Здесь можно добавить генерацию кода для ESPHome
    pass
