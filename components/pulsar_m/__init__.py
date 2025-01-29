import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart, sensor
from esphome.const import CONF_ID, CONF_ADDRESS, CONF_FLOW_SENSOR, CONF_ERROR_SENSOR

CODEOWNERS = ["@MikeDev0ps"]
DEPENDENCIES = ["uart"]

pulsar_m_ns = cg.esphome_ns.namespace("pulsar_m")
PulsarM = pulsar_m_ns.class_("PulsarM", cg.Component, uart.UARTDevice)

CONF_DE_PIN = "de_pin"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(PulsarM),
        cv.Required(CONF_ADDRESS): cv.hex_uint32_t,
        cv.Optional(CONF_DE_PIN): cv.use_id(cg.GPIOPin),
        cv.Optional(CONF_FLOW_SENSOR): sensor.sensor_schema(
            unit_of_measurement="m³",
            accuracy_decimals=3,
            icon="mdi:water",
        ),
        cv.Optional(CONF_ERROR_SENSOR): sensor.sensor_schema(
            icon="mdi:alert",
        ),
    }
).extend(uart.UART_DEVICE_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield uart.register_uart_device(var, config)

    cg.add(var.set_address(config[CONF_ADDRESS]))
    if CONF_DE_PIN in config:
        de_pin = yield cg.get_variable(config[CONF_DE_PIN])
        cg.add(var.set_de_pin(de_pin))

    if CONF_FLOW_SENSOR in config:
        flow_sensor = yield sensor.new_sensor(config[CONF_FLOW_SENSOR])
        cg.add(var.set_flow_sensor(flow_sensor))

    if CONF_ERROR_SENSOR in config:
        error_sensor = yield sensor.new_sensor(config[CONF_ERROR_SENSOR])
        cg.add(var.set_error_sensor(error_sensor))

