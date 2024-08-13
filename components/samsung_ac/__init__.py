import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart, sensor, switch, select, number, climate
from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_TEMPERATURE,
    STATE_CLASS_MEASUREMENT,
    DEVICE_CLASS_HUMIDITY,
    CONF_UNIT_OF_MEASUREMENT,
    CONF_DEVICE_CLASS,
    CONF_FILTERS,
    UNIT_CELSIUS,
    UNIT_PERCENT,
)
from esphome.core import (
    CORE,
    Lambda
)

CODEOWNERS = ["matthias882", "lanwin"]
DEPENDENCIES = ["uart"]
AUTO_LOAD = ["sensor", "switch", "select", "number", "climate"]
MULTI_CONF = False

CONF_SAMSUNG_AC_ID = "samsung_ac_id"

samsung_ac = cg.esphome_ns.namespace("samsung_ac")
Samsung_AC = samsung_ac.class_(
    "Samsung_AC", cg.PollingComponent, uart.UARTDevice
)
Samsung_AC_Device = samsung_ac.class_("Samsung_AC_Device")
Samsung_AC_Switch = samsung_ac.class_("Samsung_AC_Switch", switch.Switch)
Samsung_AC_Mode_Select = samsung_ac.class_(
    "Samsung_AC_Mode_Select", select.Select)
Samsung_AC_Number = samsung_ac.class_("Samsung_AC_Number", number.Number)
Samsung_AC_Climate = samsung_ac.class_("Samsung_AC_Climate", climate.Climate)

# not sure why select.select_schema did not work yet
SELECT_MODE_SCHEMA = select.select_schema(Samsung_AC_Mode_Select)

NUMBER_SCHEMA = (
    number.NUMBER_SCHEMA.extend(
        {cv.GenerateID(): cv.declare_id(Samsung_AC_Number)})
)

CLIMATE_SCHEMA = (
    climate.CLIMATE_SCHEMA.extend(
        {cv.GenerateID(): cv.declare_id(Samsung_AC_Climate)})
)

CONF_DEVICE_ID = "samsung_ac_device_id"
CONF_DEVICE_ADDRESS = "address"
CONF_DEVICE_ROOM_TEMPERATURE = "room_temperature"
CONF_DEVICE_ROOM_TEMPERATURE_OFFSET = "room_temperature_offset"
CONF_DEVICE_TARGET_TEMPERATURE = "target_temperature"
CONF_DEVICE_WATER_OUTLET_TARGET = "water_outlet_target"
CONF_DEVICE_OUTDOOR_TEMPERATURE = "outdoor_temperature"
CONF_DEVICE_WATER_TEMPERATURE = "water_temperature"
CONF_DEVICE_WATER_TARGET_TEMPERATURE = "water_target_temperature"
CONF_DEVICE_POWER = "power"
CONF_DEVICE_WATER_HEATER_POWER = "water_heater_power"
CONF_DEVICE_MODE = "mode"
CONF_DEVICE_CLIMATE = "climate"
CONF_DEVICE_ROOM_HUMIDITY = "room_humidity"
CONF_DEVICE_CUSTOM = "custom_sensor"
CONF_DEVICE_CUSTOM_MESSAGE = "message"
CONF_DEVICE_CUSTOM_RAW_FILTERS = "raw_filters"

CONF_CAPABILITIES = "capabilities"
CONF_CAPABILITIES_HORIZONTAL_SWING = "horizontal_swing"
CONF_CAPABILITIES_VERTICAL_SWING = "vertical_swing"

CONF_PRESETS = "presets"
CONF_PRESET_NAME = "name"
CONF_PRESET_ENABLED = "enabled"
CONF_PRESET_VALUE = "value"


def preset_entry(
    name: str,
    value: int,
    displayName: str
): return (
    cv.Optional(name, default=False), cv.Any(cv.boolean, cv.All({
        cv.Optional(CONF_PRESET_ENABLED, default=False): cv.boolean,
        cv.Optional(CONF_PRESET_NAME, default=displayName): cv.string,
        cv.Optional(CONF_PRESET_VALUE, default=value): cv.int_
    }))
)


PRESETS = {
    "sleep": {"value": 1, "displayName": "Sleep"},
    "quiet": {"value": 2, "displayName": "Quiet"},
    "fast": {"value": 3, "displayName": "Fast"},
    "longreach": {"value": 6, "displayName": "LongReach"},
    "windfree": {"value": 9, "displayName": "WindFree"},
}

CAPABILITIES_SCHEMA = (
    cv.Schema({
        cv.Optional(CONF_CAPABILITIES_HORIZONTAL_SWING, default=False): cv.boolean,
        cv.Optional(CONF_CAPABILITIES_VERTICAL_SWING, default=False): cv.boolean,
        cv.Optional(CONF_PRESETS): cv.Schema(dict(
            [preset_entry(name, PRESETS[name]["value"],
                          PRESETS[name]["displayName"]) for name in PRESETS]
        ))
    })
)

CUSTOM_SENSOR_SCHEMA = sensor.sensor_schema().extend({
    cv.Required(CONF_DEVICE_CUSTOM_MESSAGE): cv.hex_int,
})


def custom_sensor_schema(
    message: int,
    unit_of_measurement: str = sensor._UNDEF,
    icon: str = sensor._UNDEF,
    accuracy_decimals: int = sensor._UNDEF,
    device_class: str = sensor._UNDEF,
    state_class: str = sensor._UNDEF,
    entity_category: str = sensor._UNDEF,
    raw_filters=[]
):
    return sensor.sensor_schema(
        unit_of_measurement=unit_of_measurement,
        icon=icon,
        accuracy_decimals=accuracy_decimals,
        device_class=device_class,
        state_class=state_class,
        entity_category=entity_category,
    ).extend({
        cv.Optional(CONF_DEVICE_CUSTOM_MESSAGE, default=message): cv.hex_int,
        cv.Optional(CONF_DEVICE_CUSTOM_RAW_FILTERS, default=raw_filters): sensor.validate_filters
    })


def temperature_sensor_schema(message: int):
    return custom_sensor_schema(
        message=message,
        unit_of_measurement=UNIT_CELSIUS,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_TEMPERATURE,
        state_class=STATE_CLASS_MEASUREMENT,
        raw_filters=[
            {"lambda": Lambda("return (int16_t)x;")},
            {"multiply": 0.1}
        ],
    )


def humidity_sensor_schema(message: int):
    return custom_sensor_schema(
        message=message,
        unit_of_measurement=UNIT_PERCENT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_HUMIDITY,
        state_class=STATE_CLASS_MEASUREMENT,
    )


DEVICE_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(CONF_DEVICE_ID): cv.declare_id(Samsung_AC_Device),
            cv.Optional(CONF_CAPABILITIES): CAPABILITIES_SCHEMA,
            cv.Required(CONF_DEVICE_ADDRESS): cv.string,
            cv.Optional(CONF_DEVICE_ROOM_TEMPERATURE): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_DEVICE_ROOM_TEMPERATURE_OFFSET): cv.float_,
            cv.Optional(CONF_DEVICE_OUTDOOR_TEMPERATURE): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_DEVICE_TARGET_TEMPERATURE): NUMBER_SCHEMA,
            cv.Optional(CONF_DEVICE_WATER_OUTLET_TARGET): NUMBER_SCHEMA,
            cv.Optional(CONF_DEVICE_WATER_TARGET_TEMPERATURE): NUMBER_SCHEMA,
            cv.Optional(CONF_DEVICE_POWER): switch.switch_schema(Samsung_AC_Switch),
            cv.Optional(CONF_DEVICE_WATER_HEATER_POWER): switch.switch_schema(Samsung_AC_Switch),
            cv.Optional(CONF_DEVICE_MODE): SELECT_MODE_SCHEMA,
            cv.Optional(CONF_DEVICE_CLIMATE): CLIMATE_SCHEMA,
            cv.Optional(CONF_DEVICE_CUSTOM, default=[]): cv.ensure_list(CUSTOM_SENSOR_SCHEMA),

            # keep CUSTOM_SENSOR_KEYS in sync with these
            cv.Optional(CONF_DEVICE_WATER_TEMPERATURE): temperature_sensor_schema(0x4237),
            cv.Optional(CONF_DEVICE_ROOM_HUMIDITY): humidity_sensor_schema(0x4038),
        }
    )
)

CUSTOM_SENSOR_KEYS = [
    CONF_DEVICE_WATER_TEMPERATURE,
    CONF_DEVICE_ROOM_HUMIDITY,
]

CONF_DEVICES = "devices"

CONF_DEBUG_MQTT_HOST = "debug_mqtt_host"
CONF_DEBUG_MQTT_PORT = "debug_mqtt_port"
CONF_DEBUG_MQTT_USERNAME = "debug_mqtt_username"
CONF_DEBUG_MQTT_PASSWORD = "debug_mqtt_password"

CONF_DEBUG_LOG_MESSAGES = "debug_log_messages"
CONF_DEBUG_LOG_MESSAGES_RAW = "debug_log_messages_raw"

CONF_NON_NASA_KEEPALIVE = "non_nasa_keepalive"

CONF_LOG_UNDEFINED_MESSAGES = "log_undefined_messages"

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(Samsung_AC),
            # cv.Optional(CONF_PAUSE, default=False): cv.boolean,
            cv.Optional(CONF_DEBUG_MQTT_HOST, default=""): cv.string,
            cv.Optional(CONF_DEBUG_MQTT_PORT, default=1883): cv.int_,
            cv.Optional(CONF_DEBUG_MQTT_USERNAME, default=""): cv.string,
            cv.Optional(CONF_DEBUG_MQTT_PASSWORD, default=""): cv.string,
            cv.Optional(CONF_DEBUG_LOG_MESSAGES, default=False): cv.boolean,
            cv.Optional(CONF_DEBUG_LOG_MESSAGES_RAW, default=False): cv.boolean,
            cv.Optional(CONF_NON_NASA_KEEPALIVE, default=False): cv.boolean,
            cv.Optional(CONF_LOG_UNDEFINED_MESSAGES, default=False): cv.boolean,
            cv.Optional(CONF_CAPABILITIES): CAPABILITIES_SCHEMA,
            cv.Required(CONF_DEVICES): cv.ensure_list(DEVICE_SCHEMA),
        }
    )
    .extend(uart.UART_DEVICE_SCHEMA)
    .extend(cv.polling_component_schema("30s"))
)


async def to_code(config):
    # For Debug_MQTT
    if CORE.is_esp8266 or CORE.is_libretiny:
        cg.add_library("heman/AsyncMqttClient-esphome", "2.0.0")

    var = cg.new_Pvariable(config[CONF_ID])
    for device_index, device in enumerate(config[CONF_DEVICES]):
        var_dev = cg.new_Pvariable(
            device[CONF_DEVICE_ID], device[CONF_DEVICE_ADDRESS], var)

        # setup capabilities
        if CONF_CAPABILITIES in device and CONF_CAPABILITIES_VERTICAL_SWING in device[CONF_CAPABILITIES]:
            cg.add(var_dev.set_supports_vertical_swing(
                device[CONF_CAPABILITIES][CONF_CAPABILITIES_VERTICAL_SWING]))
        elif CONF_CAPABILITIES in config and CONF_CAPABILITIES_VERTICAL_SWING in config[CONF_CAPABILITIES]:
            cg.add(var_dev.set_supports_vertical_swing(
                config[CONF_CAPABILITIES][CONF_CAPABILITIES_VERTICAL_SWING]))

        if CONF_CAPABILITIES in device and CONF_CAPABILITIES_HORIZONTAL_SWING in device[CONF_CAPABILITIES]:
            cg.add(var_dev.set_supports_horizontal_swing(
                device[CONF_CAPABILITIES][CONF_CAPABILITIES_HORIZONTAL_SWING]))
        elif CONF_CAPABILITIES in config and CONF_CAPABILITIES_HORIZONTAL_SWING in config[CONF_CAPABILITIES]:
            cg.add(var_dev.set_supports_horizontal_swing(
                config[CONF_CAPABILITIES][CONF_CAPABILITIES_HORIZONTAL_SWING]))

        none_added = False
        for preset in PRESETS:
            device_preset_conf = device[CONF_CAPABILITIES][CONF_PRESETS][preset] if (
                CONF_CAPABILITIES in device
                and CONF_PRESETS in device[CONF_CAPABILITIES]
                and preset in device[CONF_CAPABILITIES][CONF_PRESETS]) else None
            global_preset_conf = config[CONF_CAPABILITIES][CONF_PRESETS][preset] if (
                CONF_CAPABILITIES in config
                and CONF_PRESETS in config[CONF_CAPABILITIES]
                and preset in config[CONF_CAPABILITIES][CONF_PRESETS]) else None

            preset_conf = global_preset_conf if device_preset_conf is None else device_preset_conf
            preset_dict = isinstance(preset_conf, dict)
            if preset_conf == True or (preset_dict and preset_conf[CONF_PRESET_ENABLED] == True):
                if not none_added:
                    none_added = True
                    cg.add(var_dev.add_alt_mode("None", 0))

                cg.add(var_dev.add_alt_mode(
                    preset_conf[CONF_PRESET_NAME] if preset_dict and CONF_PRESET_NAME in preset_conf else PRESETS[preset]["displayName"],
                    preset_conf[CONF_PRESET_VALUE] if preset_dict and CONF_PRESET_VALUE in preset_conf else PRESETS[preset]["value"]
                ))

#        if CONF_CAPABILITIES in device and CONF_ALT_MODES in device[CONF_CAPABILITIES]:
#            cg.add(var_dev.add_alt_mode("None", 0))
#            for alt in device[CONF_CAPABILITIES][CONF_ALT_MODES]:
#                cg.add(var_dev.add_alt_mode(alt[CONF_ALT_MODE_NAME], alt[CONF_ALT_MODE_VALUE]))
#        elif CONF_CAPABILITIES in config and CONF_ALT_MODES in config[CONF_CAPABILITIES]:
#            cg.add(var_dev.add_alt_mode("None", 0))
#            for alt in config[CONF_CAPABILITIES][CONF_ALT_MODES]:
#                cg.add(var_dev.add_alt_mode(alt[CONF_ALT_MODE_NAME], alt[CONF_ALT_MODE_VALUE]))

        if CONF_DEVICE_POWER in device:
            conf = device[CONF_DEVICE_POWER]
            sens = await switch.new_switch(conf)
            cg.add(var_dev.set_power_switch(sens))
        
        if CONF_DEVICE_WATER_HEATER_POWER in device:
            conf = device[CONF_DEVICE_WATER_HEATER_POWER]
            sens = await switch.new_switch(conf)
            cg.add(var_dev.set_water_heater_power_switch(sens))

        if CONF_DEVICE_ROOM_TEMPERATURE in device:
            conf = device[CONF_DEVICE_ROOM_TEMPERATURE]
            sens = await sensor.new_sensor(conf)
            cg.add(var_dev.set_room_temperature_sensor(sens))

        if CONF_DEVICE_ROOM_TEMPERATURE_OFFSET in device:
            cg.add(var_dev.set_room_temperature_offset(
                device[CONF_DEVICE_ROOM_TEMPERATURE_OFFSET]))

        if CONF_DEVICE_OUTDOOR_TEMPERATURE in device:
            conf = device[CONF_DEVICE_OUTDOOR_TEMPERATURE]
            sens = await sensor.new_sensor(conf)
            cg.add(var_dev.set_outdoor_temperature_sensor(sens))
            
        if CONF_DEVICE_INDOOR_EVA_IN_TEMPERATURE in device:
            conf = device[CONF_DEVICE_INDOOR_EVA_IN_TEMPERATURE]
            sens = await sensor.new_sensor(conf)
            cg.add(var_dev.set_indoor_eva_in_temperature_sensor(sens))
            
        if CONF_DEVICE_INDOOR_EVA_OUT_TEMPERATURE in device:
            conf = device[CONF_DEVICE_INDOOR_EVA_OUT_TEMPERATURE]
            sens = await sensor.new_sensor(conf)
            cg.add(var_dev.set_indoor_eva_out_temperature_sensor(sens))

        if CONF_DEVICE_WATER_TARGET_TEMPERATURE in device:
            conf = device[CONF_DEVICE_WATER_TARGET_TEMPERATURE]
            conf[CONF_UNIT_OF_MEASUREMENT] = UNIT_CELSIUS
            conf[CONF_DEVICE_CLASS] = DEVICE_CLASS_TEMPERATURE
            num = await number.new_number(conf,
                                          min_value=30.0,
                                          max_value=70.0,
                                          step=0.5)
            cg.add(var_dev.set_target_water_temperature_number(num))

        if CONF_DEVICE_TARGET_TEMPERATURE in device:
            conf = device[CONF_DEVICE_TARGET_TEMPERATURE]
            conf[CONF_UNIT_OF_MEASUREMENT] = UNIT_CELSIUS
            conf[CONF_DEVICE_CLASS] = DEVICE_CLASS_TEMPERATURE
            num = await number.new_number(conf,
                                          min_value=16.0,
                                          max_value=30.0,
                                          step=1.0)
            cg.add(var_dev.set_target_temperature_number(num))
            
        if CONF_DEVICE_WATER_OUTLET_TARGET in device:
            conf = device[CONF_DEVICE_WATER_OUTLET_TARGET]
            conf[CONF_UNIT_OF_MEASUREMENT] = UNIT_CELSIUS
            conf[CONF_DEVICE_CLASS] = DEVICE_CLASS_TEMPERATURE
            num = await number.new_number(conf,
                                          min_value=15.0,
                                          max_value=55.0,
                                          step=0.1)
            cg.add(var_dev.set_water_outlet_target_number(num))

        if CONF_DEVICE_MODE in device:
            conf = device[CONF_DEVICE_MODE]
            values = ["Auto", "Cool", "Dry", "Fan", "Heat"]
            sel = await select.new_select(conf, options=values)
            cg.add(var_dev.set_mode_select(sel))

        if CONF_DEVICE_CLIMATE in device:
            conf = device[CONF_DEVICE_CLIMATE]
            var_cli = cg.new_Pvariable(conf[CONF_ID])
            await climate.register_climate(var_cli, conf)
            cg.add(var_dev.set_climate(var_cli))

        if CONF_DEVICE_CUSTOM in device:
            for cust_sens in device[CONF_DEVICE_CUSTOM]:
                sens = await sensor.new_sensor(cust_sens)
                cg.add(var_dev.add_custom_sensor(
                    cust_sens[CONF_DEVICE_CUSTOM_MESSAGE], sens))

        for key in CUSTOM_SENSOR_KEYS:
            if key in device:
                conf = device[key]
                # combine raw filters with any user-defined filters
                conf_copy = conf.copy()
                conf_copy[CONF_FILTERS] = (conf[CONF_DEVICE_CUSTOM_RAW_FILTERS] if CONF_DEVICE_CUSTOM_RAW_FILTERS in conf else [
                ]) + (conf[CONF_FILTERS] if CONF_FILTERS in conf else [])
                sens = await sensor.new_sensor(conf_copy)
                cg.add(var_dev.add_custom_sensor(
                    conf[CONF_DEVICE_CUSTOM_MESSAGE], sens))

        cg.add(var.register_device(var_dev))

    cg.add(var.set_debug_mqtt(config[CONF_DEBUG_MQTT_HOST], config[CONF_DEBUG_MQTT_PORT],
           config[CONF_DEBUG_MQTT_USERNAME], config[CONF_DEBUG_MQTT_PASSWORD]))

    if (CONF_DEBUG_LOG_MESSAGES in config):
        cg.add(var.set_debug_log_messages(config[CONF_DEBUG_LOG_MESSAGES]))

    if (CONF_DEBUG_LOG_MESSAGES_RAW in config):
        cg.add(var.set_debug_log_messages_raw(
            config[CONF_DEBUG_LOG_MESSAGES_RAW]))
            
    if (CONF_NON_NASA_KEEPALIVE in config):
        cg.add(var.set_non_nasa_keepalive(config[CONF_NON_NASA_KEEPALIVE]))
        
    if (CONF_LOG_UNDEFINED_MESSAGES in config):
        cg.add(var.set_log_undefined_messages(config[CONF_LOG_UNDEFINED_MESSAGES]))

    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
