import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, uart
from esphome.const import CONF_ID, CONF_ADDRESS, CONF_SENSORS

DEPENDENCIES = ["uart"]

pulsar_m_ns = cg.esphome_ns.namespace("pulsar_m")
PulsarMComponent = pulsar_m_ns.class_("PulsarMComponent", cg.Component, uart.UARTDevice)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(PulsarMComponent),
    cv.Required(CONF_ADDRESS): cv.uint32_t,
    cv.Optional("total_volume_sensor"): sensor.sensor_schema(
        accuracy_decimals=2,
        unit_of_measurement="m³",
        icon="mdi:water",
    ),
    cv.Optional("error_sensor"): sensor.sensor_schema(
        accuracy_decimals=0,
        icon="mdi:alert-circle",
    ),
}).extend(uart.UART_DEVICE_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    cg.add(var.set_address(config[CONF_ADDRESS]))

    if "total_volume_sensor" in config:
        sens = await sensor.new_sensor(config["total_volume_sensor"])
        cg.add(var.set_total_volume_sensor(sens))

    if "error_sensor" in config:
        sens = await sensor.new_sensor(config["error_sensor"])
        cg.add(var.set_error_sensor(sens))
