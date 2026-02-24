import esphome.codegen as cg
from esphome.components import light
import esphome.config_validation as cv
from esphome.const import (
    CONF_COLD_WHITE_COLOR_TEMPERATURE,
    CONF_COLOR_INTERLOCK,
    CONF_DEFAULT_TRANSITION_LENGTH,
    CONF_GAMMA_CORRECT,
    CONF_MAX_VALUE,
    CONF_MIN_VALUE,
    CONF_OUTPUT_ID,
    CONF_WARM_WHITE_COLOR_TEMPERATURE,
    CONF_NUMBER,
    CONF_INVERTED,
    CONF_TYPE
)

from .. import CONF_UYAT_ID, CONF_DATAPOINT, CONF_DATAPOINT_TYPE, Uyat, uyat_ns, DPTYPE_BOOL, DPTYPE_UINT, DPTYPE_ENUM, DPTYPE_STRING, DPTYPE_DETECT, matching_datapoint_from_config, retry_config_struct_initializer

DEPENDENCIES = ["uyat"]

CONF_MIN_VALUE_DATAPOINT = "min_value_datapoint"
CONF_SWITCH = "switch"
CONF_DIMMER = "dimmer"
CONF_COLOR = "color"
CONF_WHITE_TEMPERATURE = "white_temperature"
CONF_RETRIES = "retries"
CONF_ENABLED = "enabled"
CONF_COUNT = "count"
CONF_TIMEOUT = "timeout"

UyatColorType = uyat_ns.enum("UyatColorType", is_class=True)

UyatLightConfigSwitch = uyat_ns.struct("ConfigSwitch")
UyatLightConfigDimmer = uyat_ns.struct("ConfigDimmer")
UyatLightConfigColor = uyat_ns.struct("ConfigColor")
UyatLightConfigWhiteTemperature = uyat_ns.struct("ConfigWhiteTemperature")

UyatLightBinary = uyat_ns.class_("UyatLightBinary", cg.Component)
UyatLightBinaryConfig = uyat_ns.struct("UyatLightBinary::Config")

UyatLightCT = uyat_ns.class_("UyatLightCT", cg.Component)
UyatLightCTConfig = uyat_ns.struct("UyatLightCT::Config")

UyatLightDimmer = uyat_ns.class_("UyatLightDimmer", cg.Component)
UyatLightDimmerConfig = uyat_ns.struct("UyatLightDimmer::Config")

UyatLightRGB = uyat_ns.class_("UyatLightRGB", cg.Component)
UyatLightRGBConfig = uyat_ns.struct("UyatLightRGB::Config")

UyatLightRGBCT = uyat_ns.class_("UyatLightRGBCT", cg.Component)
UyatLightRGBCTConfig = uyat_ns.struct("UyatLightRGBCT::Config")

UyatLightRGBW = uyat_ns.class_("UyatLightRGBW", cg.Component)
UyatLightRGBWConfig = uyat_ns.struct("UyatLightRGBW::Config")

UYAT_LIGHT_TYPE_BINARY = "binary"
UYAT_LIGHT_TYPE_DIMMER = "dimmer"
UYAT_LIGHT_TYPE_CT = "ct"
UYAT_LIGHT_TYPE_RGB = "rgb"
UYAT_LIGHT_TYPE_RGBW = "rgbw"
UYAT_LIGHT_TYPE_RGBCT = "rgbct"

UYAT_LIGHT_TYPES = [
    UYAT_LIGHT_TYPE_BINARY,
    UYAT_LIGHT_TYPE_DIMMER,
    UYAT_LIGHT_TYPE_CT,
    UYAT_LIGHT_TYPE_RGB,
    UYAT_LIGHT_TYPE_RGBW,
    UYAT_LIGHT_TYPE_RGBCT,
]

COLOR_TYPES = {
    "RGB": UyatColorType.RGB,
    "HSV": UyatColorType.HSV,
    "RGBHSV": UyatColorType.RGBHSV,
}

DIMMER_DP_TYPES = {
    "allowed": [
        DPTYPE_DETECT,
        DPTYPE_UINT,
        DPTYPE_ENUM,
    ],
    "default": DPTYPE_UINT
}

MIN_VALUE_DP_TYPES = {
    "allowed": [
        DPTYPE_DETECT,
        DPTYPE_UINT,
        DPTYPE_ENUM,
    ],
    "default": DPTYPE_UINT
}

SWITCH_DP_TYPES = {
    "allowed": [
        DPTYPE_DETECT,
        DPTYPE_BOOL,
        DPTYPE_UINT,
        DPTYPE_ENUM
    ],
    "default": DPTYPE_BOOL
}

COLOR_DP_TYPES = {
    "allowed": [
        DPTYPE_STRING
    ],
    "default": DPTYPE_STRING
}

WHITE_TEMPERATURE_DP_TYPES = {
    "allowed": [
        DPTYPE_DETECT,
        DPTYPE_UINT,
        DPTYPE_ENUM,
    ],
    "default": DPTYPE_UINT
}

RETRIES_SCHEMA = cv.Schema(
    {
        cv.Optional(CONF_ENABLED, default=False): cv.boolean,
        cv.Optional(CONF_COUNT, default=3): cv.int_range(min=1, max=255),
        cv.Optional(CONF_TIMEOUT, default="300ms"): cv.positive_time_period_milliseconds,
    }
)


UyatLight = uyat_ns.class_("UyatLight", light.LightOutput, cg.Component)

SWITCH_CONFIG_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_DATAPOINT): cv.Any(cv.uint8_t,
            cv.Schema(
            {
                cv.Required(CONF_NUMBER): cv.uint8_t,
                cv.Optional(CONF_DATAPOINT_TYPE, default=SWITCH_DP_TYPES["default"]): cv.one_of(
                    *SWITCH_DP_TYPES["allowed"], lower=True
                )
            })
        ),
        cv.Optional(CONF_INVERTED, default=False): cv.boolean,
        cv.Optional(CONF_RETRIES): RETRIES_SCHEMA,
    }
)

DIMMER_CONFIG_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_DATAPOINT): cv.Any(cv.uint8_t,
            cv.Schema(
            {
                cv.Required(CONF_NUMBER): cv.uint8_t,
                cv.Optional(CONF_DATAPOINT_TYPE, default=DIMMER_DP_TYPES["default"]): cv.one_of(
                    *DIMMER_DP_TYPES["allowed"], lower=True
                )
            })
        ),
        cv.Optional(CONF_MIN_VALUE, default=0): cv.int_,
        cv.Optional(CONF_MAX_VALUE, default=255): cv.int_,
        cv.Optional(CONF_INVERTED, default=False): cv.boolean,
        cv.Optional(CONF_RETRIES): RETRIES_SCHEMA,
        cv.Optional(CONF_MIN_VALUE_DATAPOINT): cv.Any(cv.uint8_t,
            cv.Schema(
            {
                cv.Required(CONF_NUMBER): cv.uint8_t,
                cv.Optional(CONF_DATAPOINT_TYPE, default=MIN_VALUE_DP_TYPES["default"]): cv.one_of(
                    *MIN_VALUE_DP_TYPES["allowed"], lower=True
                )
            })
        ),
    }
)

COLOR_CONFIG_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_DATAPOINT): cv.Any(cv.uint8_t,
            cv.Schema(
            {
                cv.Required(CONF_NUMBER): cv.uint8_t,
                cv.Optional(CONF_DATAPOINT_TYPE, default=COLOR_DP_TYPES["default"]): cv.one_of(
                    *COLOR_DP_TYPES["allowed"], lower=True
                )
            })
        ),
        cv.Required(CONF_TYPE): cv.enum(COLOR_TYPES, upper=True),
        cv.Optional(CONF_RETRIES): RETRIES_SCHEMA,
    }
)

WHITE_TEMPERATURE_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_DATAPOINT): cv.Any(cv.uint8_t,
            cv.Schema(
            {
                cv.Required(CONF_NUMBER): cv.uint8_t,
                cv.Optional(CONF_DATAPOINT_TYPE, default=WHITE_TEMPERATURE_DP_TYPES["default"]): cv.one_of(
                    *WHITE_TEMPERATURE_DP_TYPES["allowed"], lower=True
                )
            })
        ),
        cv.Optional(CONF_INVERTED, default=False): cv.boolean,
        cv.Optional(CONF_MIN_VALUE, default=0): cv.int_,
        cv.Optional(CONF_MAX_VALUE, default=255): cv.int_,
        cv.Required(CONF_COLD_WHITE_COLOR_TEMPERATURE): cv.color_temperature,
        cv.Required(CONF_WARM_WHITE_COLOR_TEMPERATURE): cv.color_temperature,
        cv.Optional(CONF_RETRIES): RETRIES_SCHEMA,
    }
)

CONFIG_UYAT_LIGHT_COMMON_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_UYAT_ID): cv.use_id(Uyat),
    }
).extend(cv.COMPONENT_SCHEMA)


CONFIG_SCHEMA = cv.typed_schema(
    {
        UYAT_LIGHT_TYPE_BINARY:
            light.BINARY_LIGHT_SCHEMA
            .extend(CONFIG_UYAT_LIGHT_COMMON_SCHEMA)
            .extend(
            {
                cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(UyatLightBinary),
                cv.Required(CONF_SWITCH): SWITCH_CONFIG_SCHEMA,
            }
        ),
        UYAT_LIGHT_TYPE_DIMMER:
            light.BRIGHTNESS_ONLY_LIGHT_SCHEMA
            .extend(CONFIG_UYAT_LIGHT_COMMON_SCHEMA)
            .extend(
            {
                cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(UyatLightDimmer),
                cv.Required(CONF_SWITCH): SWITCH_CONFIG_SCHEMA,
                cv.Required(CONF_DIMMER): DIMMER_CONFIG_SCHEMA,
                # Change the default gamma_correct and default transition length settings.
                # The MCU handles transitions and gamma correction on its own.
                cv.Optional(CONF_GAMMA_CORRECT, default=1.0): cv.positive_float,
                cv.Optional(
                    CONF_DEFAULT_TRANSITION_LENGTH, default="0s"
                ): cv.positive_time_period_milliseconds,
            }
        ),
        UYAT_LIGHT_TYPE_CT:
            light.BRIGHTNESS_ONLY_LIGHT_SCHEMA
            .extend(CONFIG_UYAT_LIGHT_COMMON_SCHEMA)
            .extend(
            {
                cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(UyatLightCT),
                cv.Required(CONF_SWITCH): SWITCH_CONFIG_SCHEMA,
                cv.Required(CONF_DIMMER): DIMMER_CONFIG_SCHEMA,
                cv.Required(CONF_WHITE_TEMPERATURE): WHITE_TEMPERATURE_SCHEMA,
                # Change the default gamma_correct and default transition length settings.
                # The MCU handles transitions and gamma correction on its own.
                cv.Optional(CONF_GAMMA_CORRECT, default=1.0): cv.positive_float,
                cv.Optional(
                    CONF_DEFAULT_TRANSITION_LENGTH, default="0s"
                ): cv.positive_time_period_milliseconds,
            }
        ),
        UYAT_LIGHT_TYPE_RGB:
            light.RGB_LIGHT_SCHEMA
            .extend(CONFIG_UYAT_LIGHT_COMMON_SCHEMA)
            .extend(
            {
                cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(UyatLightRGB),
                cv.Required(CONF_SWITCH): SWITCH_CONFIG_SCHEMA,
                cv.Required(CONF_COLOR): COLOR_CONFIG_SCHEMA,
            }
        ),
        UYAT_LIGHT_TYPE_RGBW:
            light.BRIGHTNESS_ONLY_LIGHT_SCHEMA
            .extend(CONFIG_UYAT_LIGHT_COMMON_SCHEMA)
            .extend(
            {
                cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(UyatLightRGBW),
                cv.Required(CONF_SWITCH): SWITCH_CONFIG_SCHEMA,
                cv.Required(CONF_DIMMER): DIMMER_CONFIG_SCHEMA,
                cv.Required(CONF_COLOR): COLOR_CONFIG_SCHEMA,
                cv.Optional(CONF_COLOR_INTERLOCK, default=False): cv.boolean,
                # Change the default gamma_correct and default transition length settings.
                # The MCU handles transitions and gamma correction on its own.
                cv.Optional(CONF_GAMMA_CORRECT, default=1.0): cv.positive_float,
                cv.Optional(
                    CONF_DEFAULT_TRANSITION_LENGTH, default="0s"
                ): cv.positive_time_period_milliseconds,
            }
        ),
        UYAT_LIGHT_TYPE_RGBCT:
            light.BRIGHTNESS_ONLY_LIGHT_SCHEMA
            .extend(CONFIG_UYAT_LIGHT_COMMON_SCHEMA)
            .extend(
            {
                cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(UyatLightRGBCT),
                cv.Required(CONF_SWITCH): SWITCH_CONFIG_SCHEMA,
                cv.Required(CONF_DIMMER): DIMMER_CONFIG_SCHEMA,
                cv.Required(CONF_COLOR): COLOR_CONFIG_SCHEMA,
                cv.Required(CONF_WHITE_TEMPERATURE): WHITE_TEMPERATURE_SCHEMA,
                cv.Optional(CONF_COLOR_INTERLOCK, default=False): cv.boolean,
                # Change the default gamma_correct and default transition length settings.
                # The MCU handles transitions and gamma correction on its own.
                cv.Optional(CONF_GAMMA_CORRECT, default=1.0): cv.positive_float,
                cv.Optional(
                    CONF_DEFAULT_TRANSITION_LENGTH, default="0s"
                ): cv.positive_time_period_milliseconds,
            }
        ),
    },
    lower=True,
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_UYAT_ID])

    if config[CONF_TYPE] == UYAT_LIGHT_TYPE_BINARY:
        switch_config = config[CONF_SWITCH]
        switch_conf_struct = cg.StructInitializer(UyatLightConfigSwitch,
                                                  ("switch_dp", await matching_datapoint_from_config(switch_config[CONF_DATAPOINT], SWITCH_DP_TYPES)),
                                                  ("inverted", switch_config[CONF_INVERTED]),
                                                  ("retry_config", retry_config_struct_initializer(switch_config)))
        full_config_struct = cg.StructInitializer(UyatLightBinaryConfig, ("switch_config", switch_conf_struct))

    elif config[CONF_TYPE] == UYAT_LIGHT_TYPE_DIMMER:
        switch_config = config[CONF_SWITCH]
        switch_conf_struct = cg.StructInitializer(UyatLightConfigSwitch,
                                                  ("switch_dp", await matching_datapoint_from_config(switch_config[CONF_DATAPOINT], SWITCH_DP_TYPES)),
                                                  ("inverted", switch_config[CONF_INVERTED]),
                                                  ("retry_config", retry_config_struct_initializer(switch_config)))

        dimmer_config = config[CONF_DIMMER]
        if CONF_MIN_VALUE_DATAPOINT in config:
            min_value_dp = await matching_datapoint_from_config(dimmer_config[CONF_MIN_VALUE_DATAPOINT], MIN_VALUE_DP_TYPES)
        else:
            min_value_dp = cg.RawExpression("{}")

        dimmer_conf_struct = cg.StructInitializer(UyatLightConfigDimmer,
                                                  ("dimmer_dp", await matching_datapoint_from_config(dimmer_config[CONF_DATAPOINT], DIMMER_DP_TYPES)),
                                                  ("min_value", dimmer_config[CONF_MIN_VALUE]),
                                                  ("max_value", dimmer_config[CONF_MAX_VALUE]),
                                                  ("inverted", dimmer_config[CONF_INVERTED]),
                                                  ("min_value_dp", min_value_dp),
                                                  ("retry_config", retry_config_struct_initializer(dimmer_config)))

        full_config_struct = cg.StructInitializer(UyatLightDimmerConfig,
                                                  ("switch_config", switch_conf_struct),
                                                  ("dimmer_config", dimmer_conf_struct))

    elif config[CONF_TYPE] == UYAT_LIGHT_TYPE_CT:
        switch_config = config[CONF_SWITCH]
        switch_conf_struct = cg.StructInitializer(UyatLightConfigSwitch,
                                                  ("switch_dp", await matching_datapoint_from_config(switch_config[CONF_DATAPOINT], SWITCH_DP_TYPES)),
                                                  ("inverted", switch_config[CONF_INVERTED]),
                                                  ("retry_config", retry_config_struct_initializer(switch_config)))

        dimmer_config = config[CONF_DIMMER]
        if CONF_MIN_VALUE_DATAPOINT in config:
            min_value_dp = await matching_datapoint_from_config(dimmer_config[CONF_MIN_VALUE_DATAPOINT], MIN_VALUE_DP_TYPES)
        else:
            min_value_dp = cg.RawExpression("{}")

        dimmer_conf_struct = cg.StructInitializer(UyatLightConfigDimmer,
                                                  ("dimmer_dp", await matching_datapoint_from_config(dimmer_config[CONF_DATAPOINT], DIMMER_DP_TYPES)),
                                                  ("min_value", dimmer_config[CONF_MIN_VALUE]),
                                                  ("max_value", dimmer_config[CONF_MAX_VALUE]),
                                                  ("inverted", dimmer_config[CONF_INVERTED]),
                                                  ("min_value_dp", min_value_dp),
                                                  ("retry_config", retry_config_struct_initializer(dimmer_config)))

        white_temperature_config = config[CONF_WHITE_TEMPERATURE]
        white_temperature_conf_struct = cg.StructInitializer(UyatLightConfigWhiteTemperature,
                                                             ("white_temperature_dp", await matching_datapoint_from_config(white_temperature_config[CONF_DATAPOINT], WHITE_TEMPERATURE_DP_TYPES)),
                                                             ("min_value", white_temperature_config[CONF_MIN_VALUE]),
                                                             ("max_value", white_temperature_config[CONF_MAX_VALUE]),
                                                             ("inverted", white_temperature_config[CONF_INVERTED]),
                                                             ("cold_white_temperature", white_temperature_config[CONF_COLD_WHITE_COLOR_TEMPERATURE]),
                                                             ("warm_white_temperature", white_temperature_config[CONF_WARM_WHITE_COLOR_TEMPERATURE]),
                                                             ("retry_config", retry_config_struct_initializer(white_temperature_config)))

        full_config_struct = cg.StructInitializer(UyatLightCTConfig,
                                                  ("switch_config", switch_conf_struct),
                                                  ("dimmer_config", dimmer_conf_struct),
                                                  ("wt_config", white_temperature_conf_struct))

    elif config[CONF_TYPE] == UYAT_LIGHT_TYPE_RGB:
        switch_config = config[CONF_SWITCH]
        switch_conf_struct = cg.StructInitializer(UyatLightConfigSwitch,
                                                  ("switch_dp", await matching_datapoint_from_config(switch_config[CONF_DATAPOINT], SWITCH_DP_TYPES)),
                                                  ("inverted", switch_config[CONF_INVERTED]),
                                                  ("retry_config", retry_config_struct_initializer(switch_config)))

        color_config = config[CONF_COLOR]
        color_conf_struct = cg.StructInitializer(UyatLightConfigColor,
                                                 ("color_dp", await matching_datapoint_from_config(color_config[CONF_DATAPOINT], COLOR_DP_TYPES)),
                                                 ("color_type", color_config[CONF_TYPE]),
                                                 ("retry_config", retry_config_struct_initializer(color_config)))

        full_config_struct = cg.StructInitializer(UyatLightRGBConfig,
                                                  ("switch_config", switch_conf_struct),
                                                  ("color_config", color_conf_struct))

    elif config[CONF_TYPE] == UYAT_LIGHT_TYPE_RGBW:
        switch_config = config[CONF_SWITCH]
        switch_conf_struct = cg.StructInitializer(UyatLightConfigSwitch,
                                                  ("switch_dp", await matching_datapoint_from_config(switch_config[CONF_DATAPOINT], SWITCH_DP_TYPES)),
                                                  ("inverted", switch_config[CONF_INVERTED]),
                                                  ("retry_config", retry_config_struct_initializer(switch_config)))

        dimmer_config = config[CONF_DIMMER]
        if CONF_MIN_VALUE_DATAPOINT in config:
            min_value_dp = await matching_datapoint_from_config(dimmer_config[CONF_MIN_VALUE_DATAPOINT], MIN_VALUE_DP_TYPES)
        else:
            min_value_dp = cg.RawExpression("{}")

        dimmer_conf_struct = cg.StructInitializer(UyatLightConfigDimmer,
                                                  ("dimmer_dp", await matching_datapoint_from_config(dimmer_config[CONF_DATAPOINT], DIMMER_DP_TYPES)),
                                                  ("min_value", dimmer_config[CONF_MIN_VALUE]),
                                                  ("max_value", dimmer_config[CONF_MAX_VALUE]),
                                                  ("inverted", dimmer_config[CONF_INVERTED]),
                                                  ("min_value_dp", min_value_dp),
                                                  ("retry_config", retry_config_struct_initializer(dimmer_config)))

        color_config = config[CONF_COLOR]
        color_conf_struct = cg.StructInitializer(UyatLightConfigColor,
                                                 ("color_dp", await matching_datapoint_from_config(color_config[CONF_DATAPOINT], COLOR_DP_TYPES)),
                                                 ("color_type", color_config[CONF_TYPE]),
                                                 ("retry_config", retry_config_struct_initializer(color_config)))

        full_config_struct = cg.StructInitializer(UyatLightRGBWConfig,
                                                  ("switch_config", switch_conf_struct),
                                                  ("dimmer_config", dimmer_conf_struct),
                                                  ("color_config", color_conf_struct),
                                                  ("color_interlock", config[CONF_COLOR_INTERLOCK]))

    elif config[CONF_TYPE] == UYAT_LIGHT_TYPE_RGBCT:
        switch_config = config[CONF_SWITCH]
        switch_conf_struct = cg.StructInitializer(UyatLightConfigSwitch,
                                                  ("switch_dp", await matching_datapoint_from_config(switch_config[CONF_DATAPOINT], SWITCH_DP_TYPES)),
                                                  ("inverted", switch_config[CONF_INVERTED]),
                                                  ("retry_config", retry_config_struct_initializer(switch_config)))

        dimmer_config = config[CONF_DIMMER]
        if CONF_MIN_VALUE_DATAPOINT in config:
            min_value_dp = await matching_datapoint_from_config(dimmer_config[CONF_MIN_VALUE_DATAPOINT], MIN_VALUE_DP_TYPES)
        else:
            min_value_dp = cg.RawExpression("{}")

        dimmer_conf_struct = cg.StructInitializer(UyatLightConfigDimmer,
                                                  ("dimmer_dp", await matching_datapoint_from_config(dimmer_config[CONF_DATAPOINT], DIMMER_DP_TYPES)),
                                                  ("min_value", dimmer_config[CONF_MIN_VALUE]),
                                                  ("max_value", dimmer_config[CONF_MAX_VALUE]),
                                                  ("inverted", dimmer_config[CONF_INVERTED]),
                                                  ("min_value_dp", min_value_dp),
                                                  ("retry_config", retry_config_struct_initializer(dimmer_config)))

        color_config = config[CONF_COLOR]
        color_conf_struct = cg.StructInitializer(UyatLightConfigColor,
                                                 ("color_dp", await matching_datapoint_from_config(color_config[CONF_DATAPOINT], COLOR_DP_TYPES)),
                                                 ("color_type", color_config[CONF_TYPE]),
                                                 ("retry_config", retry_config_struct_initializer(color_config)))

        white_temperature_config = config[CONF_WHITE_TEMPERATURE]

        white_temperature_conf_struct = cg.StructInitializer(UyatLightConfigWhiteTemperature,
                                                             ("white_temperature_dp", await matching_datapoint_from_config(white_temperature_config[CONF_DATAPOINT], WHITE_TEMPERATURE_DP_TYPES)),
                                                             ("min_value", white_temperature_config[CONF_MIN_VALUE]),
                                                             ("max_value", white_temperature_config[CONF_MAX_VALUE]),
                                                             ("inverted", white_temperature_config[CONF_INVERTED]),
                                                             ("cold_white_temperature", white_temperature_config[CONF_COLD_WHITE_COLOR_TEMPERATURE]),
                                                             ("warm_white_temperature", white_temperature_config[CONF_WARM_WHITE_COLOR_TEMPERATURE]),
                                                             ("retry_config", retry_config_struct_initializer(white_temperature_config)))

        full_config_struct = cg.StructInitializer(UyatLightRGBCTConfig,
                                                  ("switch_config", switch_conf_struct),
                                                  ("dimmer_config", dimmer_conf_struct),
                                                  ("color_config", color_conf_struct),
                                                  ("wt_config", white_temperature_conf_struct),
                                                  ("color_interlock", config[CONF_COLOR_INTERLOCK]))

    var = cg.new_Pvariable(config[CONF_OUTPUT_ID], parent, full_config_struct)
    await cg.register_component(var, config)
    await light.register_light(var, config)
