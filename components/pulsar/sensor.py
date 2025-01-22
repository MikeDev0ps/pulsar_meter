import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, uart
from esphome.const import (
    UNIT_LITERS,
    ICON_WATER,
    DEVICE_CLASS_WATER,
    STATE_CLASS_TOTAL_INCREASING,
    CONF_ID,
    CONF_NAME,
    CONF_UPDATE_INTERVAL,
)

DEPENDENCIES = ["uart"]
AUTO_LOAD = ["sensor"]

pulsar_ns = cg.esphome_ns.namespace("pulsar")
PulsarComponent = pulsar_ns.class_("PulsarComponent", cg.PollingComponent, uart.UARTDevice)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(PulsarComponent),
        cv.Required(CONF_NAME): cv.string,
        cv.Optional(CONF_UPDATE_INTERVAL, default="60s"): cv.update_interval,
    }
).extend(uart.UART_DEVICE_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    sensor_name = config[CONF_NAME]
    sens = await sensor.new_sensor(sensor_name)
    cg.add(var.set_water_sensor(sens))

    if CONF_UPDATE_INTERVAL in config:
        cg.add(var.set_update_interval(config[CONF_UPDATE_INTERVAL]))
