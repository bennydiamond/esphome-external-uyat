import esphome.codegen as cg
from esphome.components import fan
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_NUMBER, CONF_INVERTED, CONF_MIN_VALUE, CONF_MAX_VALUE

from .. import CONF_UYAT_ID, CONF_DATAPOINT, CONF_DATAPOINT_TYPE, Uyat, uyat_ns, DPTYPE_BOOL, DPTYPE_ENUM, DPTYPE_UINT, DPTYPE_DETECT, matching_datapoint_from_config

DEPENDENCIES = ["uyat"]

CONF_SPEED = "speed"
CONF_SWITCH = "switch"
CONF_OSCILLATION = "oscillation"
CONF_SPEED = "speed"
CONF_DIRECTION = "direction"

UyatFan = uyat_ns.class_("UyatFan", cg.Component, fan.Fan)

SPEED_DP_TYPES = {
    DPTYPE_DETECT,
    DPTYPE_ENUM,
    DPTYPE_UINT,
}

OSCILLATION_DP_TYPES = {
    DPTYPE_DETECT,
    DPTYPE_BOOL,
    DPTYPE_ENUM,
}

SWITCH_DP_TYPES = {
    DPTYPE_DETECT,
    DPTYPE_BOOL,
    DPTYPE_UINT,
    DPTYPE_ENUM
}

DIRECTION_DP_TYPES = {
    DPTYPE_DETECT,
    DPTYPE_BOOL,
    DPTYPE_UINT,
    DPTYPE_ENUM
}

SPEED_CONFIG_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_DATAPOINT): cv.Any(cv.uint8_t,
            cv.Schema(
            {
                cv.Required(CONF_NUMBER): cv.uint8_t,
                cv.Optional(CONF_DATAPOINT_TYPE, default=DPTYPE_UINT): cv.one_of(
                    *SPEED_DP_TYPES, lower=True
                )
            })
        ),
        cv.Required(CONF_MIN_VALUE): cv.uint32_t,
        cv.Required(CONF_MAX_VALUE): cv.uint32_t,
    }
)

OSCILLATION_CONFIG_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_DATAPOINT): cv.Any(cv.uint8_t,
            cv.Schema(
            {
                cv.Required(CONF_NUMBER): cv.uint8_t,
                cv.Optional(CONF_DATAPOINT_TYPE, default=DPTYPE_BOOL): cv.one_of(
                    *OSCILLATION_DP_TYPES, lower=True
                )
            })
        ),
        cv.Optional(CONF_INVERTED, default=False): cv.boolean,
    }
)

SWITCH_CONFIG_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_DATAPOINT): cv.Any(cv.uint8_t,
            cv.Schema(
            {
                cv.Required(CONF_NUMBER): cv.uint8_t,
                cv.Optional(CONF_DATAPOINT_TYPE, default=DPTYPE_BOOL): cv.one_of(
                    *SWITCH_DP_TYPES, lower=True
                )
            })
        ),
        cv.Optional(CONF_INVERTED, default=False): cv.boolean,
    }
)

DIRECTION_CONFIG_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_DATAPOINT): cv.Any(cv.uint8_t,
            cv.Schema(
            {
                cv.Required(CONF_NUMBER): cv.uint8_t,
                cv.Optional(CONF_DATAPOINT_TYPE, default=DPTYPE_BOOL): cv.one_of(
                    *DIRECTION_DP_TYPES, lower=True
                )
            })
        ),
        cv.Optional(CONF_INVERTED, default=False): cv.boolean,
    }
)

CONFIG_SCHEMA = cv.All(
    fan.fan_schema(UyatFan)
    .extend(
        {
            cv.GenerateID(CONF_UYAT_ID): cv.use_id(Uyat),
            cv.Optional(CONF_OSCILLATION): OSCILLATION_CONFIG_SCHEMA,
            cv.Optional(CONF_SPEED): SPEED_CONFIG_SCHEMA,
            cv.Optional(CONF_SWITCH): SWITCH_CONFIG_SCHEMA,
            cv.Optional(CONF_DIRECTION): DIRECTION_CONFIG_SCHEMA,
        }
    )
    .extend(cv.COMPONENT_SCHEMA),
    cv.has_at_least_one_key(CONF_SPEED, CONF_SWITCH),
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_UYAT_ID])

    var = cg.new_Pvariable(config[CONF_ID], parent)
    await cg.register_component(var, config)
    await fan.register_fan(var, config)

    if CONF_SPEED in config:
        speed_config = config[CONF_SPEED]
        cg.add(var.configure_speed(await matching_datapoint_from_config(speed_config[CONF_DATAPOINT], SPEED_DP_TYPES),
                                   speed_config[CONF_MIN_VALUE],
                                   speed_config[CONF_MAX_VALUE]))
    if CONF_SWITCH in config:
        switch_config = config[CONF_SWITCH]
        cg.add(var.configure_switch(await matching_datapoint_from_config(switch_config[CONF_DATAPOINT], SWITCH_DP_TYPES), switch_config[CONF_INVERTED]))
    if CONF_OSCILLATION in config:
        oscillation_config = config[CONF_OSCILLATION]
        cg.add(var.configure_oscillation(await matching_datapoint_from_config(oscillation_config[CONF_DATAPOINT], OSCILLATION_DP_TYPES), oscillation_config[CONF_INVERTED]))
    if CONF_DIRECTION in config:
        direction_config = config[CONF_DIRECTION]
        cg.add(var.configure_direction(await matching_datapoint_from_config(direction_config[CONF_DATAPOINT], DIRECTION_DP_TYPES), direction_config[CONF_INVERTED]))
