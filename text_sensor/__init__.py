import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv
from esphome.const import CONF_SENSOR_DATAPOINT, CONF_NUMBER

from .. import CONF_UYAT_ID, CONF_DATAPOINT_TYPE, Uyat, uyat_ns, DPTYPE_RAW, DPTYPE_STRING

DEPENDENCIES = ["uyat"]
CODEOWNERS = ["@dentra"]

UyatTextSensor = uyat_ns.class_("UyatTextSensor", text_sensor.TextSensor, cg.Component)

CONF_BASE64_ENCODED = "base64_encoded"
CONF_AS_HEX = "as_hex"

TEXT_SENSOR_DP_TYPES = [
    DPTYPE_RAW,
    DPTYPE_STRING,
]

CONFIG_SCHEMA = (
    text_sensor.text_sensor_schema()
    .extend(
        {
            cv.GenerateID(): cv.declare_id(UyatTextSensor),
            cv.GenerateID(CONF_UYAT_ID): cv.use_id(Uyat),
            cv.Required(CONF_SENSOR_DATAPOINT): cv.Any(cv.uint8_t,
                cv.Schema(
                {
                    cv.Required(CONF_NUMBER): cv.uint8_t,
                    cv.Optional(CONF_DATAPOINT_TYPE, default=DPTYPE_STRING): cv.one_of(
                        *TEXT_SENSOR_DP_TYPES, lower=True
                    ),
                    cv.Optional(CONF_BASE64_ENCODED, default=False): cv.boolean,
                    cv.Optional(CONF_AS_HEX, default=False): cv.boolean,
                })
            ),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)

async def to_code(config):
    var = await text_sensor.new_text_sensor(config)
    await cg.register_component(var, config)

    paren = await cg.get_variable(config[CONF_UYAT_ID])
    cg.add(var.set_uyat_parent(paren))

    dp_config = config[CONF_SENSOR_DATAPOINT]
    if not isinstance(dp_config, dict):
        cg.add(var.configure_string_dp(dp_config))
    else:
        if dp_config[CONF_DATAPOINT_TYPE]==DPTYPE_RAW:
            cg.add(var.configure_raw_dp(dp_config[CONF_NUMBER], dp_config[CONF_BASE64_ENCODED], dp_config[CONF_AS_HEX]))
        elif dp_config[CONF_DATAPOINT_TYPE]==DPTYPE_STRING:
            cg.add(var.configure_string_dp(dp_config[CONF_NUMBER], dp_config[CONF_BASE64_ENCODED], dp_config[CONF_AS_HEX]))
