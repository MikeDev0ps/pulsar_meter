import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import CONF_ID

DEPENDENCIES = ["pulsar_m"]

pulsar_m_ns = cg.esphome_ns.namespace("pulsar_m")
PulsarMSensor = pulsar_m_ns.class_("PulsarMSensor", sensor.Sensor, cg.Component)

CONFIG_SCHEMA = sensor.sensor_schema().extend(
    {
        cv.GenerateID(): cv.declare_id(PulsarMSensor),
    }
)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield sensor.register_sensor(var, config)
