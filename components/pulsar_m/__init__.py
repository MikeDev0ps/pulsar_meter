import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart, sensor
from esphome.const import CONF_ID, CONF_ADDRESS

CODEOWNERS = ["@MikeDev0ps"]
DEPENDENCIES = ["uart"]
AUTO_LOAD = ["sensor"]
MULTI_CONF = True

pulsar_m_ns = cg.esphome_ns.namespace("pulsar_m")
PulsarMComponent = pulsar_m_ns.class_("PulsarMComponent", cg.PollingComponent, uart.UARTDevice)

CONF_FLOW = "flow"
CONF_TOTAL = "total"
CONF_ERRORS = "errors"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(PulsarMComponent),
    cv.Optional(CONF_ADDRESS, default=1): cv.uint8_t,
    cv.Optional(CONF_FLOW): sensor.sensor_schema(
        unit_of_measurement="m³/h",
        accuracy_decimals=1,
    ),
    cv.Optional(CONF_TOTAL): sensor.sensor_schema(
        unit_of_measurement="m³",
        accuracy_decimals=3,
    ),
    cv.Optional(CONF_ERRORS): sensor.sensor_schema(
        icon="mdi:alert",
    ),
}).extend(cv.polling_component_schema("60s")).extend(uart.UART_DEVICE_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
    
    cg.add(var.set_address(config[CONF_ADDRESS]))
    
    if CONF_FLOW in config:
        sens = await sensor.new_sensor(config[CONF_FLOW])
        cg.add(var.set_flow_sensor(sens))
    
    if CONF_TOTAL in config:
        sens = await sensor.new_sensor(config[CONF_TOTAL])
        cg.add(var.set_total_sensor(sens))
    
    if CONF_ERRORS in config:
        sens = await sensor.new_sensor(config[CONF_ERRORS])
        cg.add(var.set_error_sensor(sens))

