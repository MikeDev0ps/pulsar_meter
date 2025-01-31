import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import uart
from esphome.const import (
    CONF_ADDRESS,
    CONF_ID,
    CONF_UART_ID,
    DEVICE_CLASS_WATER,
    STATE_CLASS_TOTAL_INCREASING,
    UNIT_CUBIC_METER,
    ICON_COUNTER,
    ICON_ALERT,
)

CODEOWNERS = ["@MikeDev0ps"]
DEPENDENCIES = ["uart"]
MULTI_CONF = True
AUTO_LOAD = ["sensor"]

pulsar_m_ns = cg.esphome_ns.namespace("pulsar_m")
PulsarM = pulsar_m_ns.class_("PulsarM", cg.PollingComponent, uart.UARTDevice)

CONF_FLOW_RATE = "flow_rate"
CONF_TOTAL = "total"
CONF_ERRORS = "errors"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(PulsarM),
    cv.Required(CONF_ADDRESS): cv.hex_uint8_t,
    cv.Optional(CONF_TOTAL): sensor.sensor_schema(
        unit_of_measurement=UNIT_CUBIC_METER,
        accuracy_decimals=3,
        device_class=DEVICE_CLASS_WATER,
        state_class=STATE_CLASS_TOTAL_INCREASING,
        icon=ICON_COUNTER,
    ),
    cv.Optional(CONF_FLOW_RATE): sensor.sensor_schema(
        unit_of_measurement=f"{UNIT_CUBIC_METER}/h",
        accuracy_decimals=2,
        icon="mdi:water-outline",
    ),
    cv.Optional(CONF_ERRORS): sensor.sensor_schema(
        icon=ICON_ALERT,
        accuracy_decimals=0,
    ),
}).extend(cv.polling_component_schema("60s")).extend(uart.UART_DEVICE_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
    
    cg.add(var.set_address(config[CONF_ADDRESS]))
    
    if CONF_TOTAL in config:
        sens = await sensor.new_sensor(config[CONF_TOTAL])
        cg.add(var.set_total_sensor(sens))
    
    if CONF_FLOW_RATE in config:
        sens = await sensor.new_sensor(config[CONF_FLOW_RATE])
        cg.add(var.set_flow_rate_sensor(sens))
    
    if CONF_ERRORS in config:
        sens = await sensor.new_sensor(config[CONF_ERRORS])
        cg.add(var.set_errors_sensor(sens))