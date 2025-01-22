from esphome.components import uart
import esphome.config_validation as cv
from esphome.const import CONF_ID

DEPENDENCIES = ['uart']

pulsar_ns = cg.esphome_ns.namespace('pulsar')
PulsarComponent = pulsar_ns.class_('PulsarComponent', cg.Component, uart.UARTDevice)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(PulsarComponent),
}).extend(uart.UART_DEVICE_SCHEMA)
