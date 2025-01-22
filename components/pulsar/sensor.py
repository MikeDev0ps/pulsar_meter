import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, uart

DEPENDENCIES = ["uart"]
AUTO_LOAD = ["sensor"]

pulsar_ns = cg.esphome_ns.namespace("pulsar")
PulsarComponent = pulsar_ns.class_("PulsarComponent", cg.PollingComponent, uart.UARTDevice)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(PulsarComponent),
            cv.Required("name"): sensor.sensor_schema(
                unit_of_measurement="L",  # Используем строку вместо UNIT_LITERS
                icon="mdi:water",
                accuracy_decimals=1,
                device_class="water",
                state_class="total_increasing",
            ),
        }
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(uart.UART_DEVICE_SCHEMA)
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    sens = await sensor.new_sensor(config["name"])
    cg.add(var.set_water_sensor(sens))
