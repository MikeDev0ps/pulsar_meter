import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID

DEPENDENCIES = ['uart']

pulsar_ns = cg.esphome_ns.namespace('pulsar')
PulsarComponent = pulsar_ns.class_('PulsarComponent', cg.Component, uart::UARTDevice)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(PulsarComponent),
    cv.Required('address'): cv.hex_uint32_t,
    cv.Required('channels'): cv.ensure_list(cv.int_range(min=1, max=16)),
}).extend(uart.UART_DEVICE_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    cg.add(var.set_address(config['address']))
    cg.add(var.set_channels(config['channels']))
    yield cg.register_component(var, config)
    yield uart.register_uart_device(var, config)
