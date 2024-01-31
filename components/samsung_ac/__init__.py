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
    UNIT_CELSIUS,
    UNIT_PERCENT,
)
from esphome.core import CORE

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
CONF_DEVICE_ROOM_HUMIDITY = "room_humidity"
CONF_DEVICE_TARGET_TEMPERATURE = "target_temperature"
CONF_DEVICE_POWER = "power"
CONF_DEVICE_MODE = "mode"
CONF_DEVICE_CLIMATE = "climate"

DEVICE_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(CONF_DEVICE_ID): cv.declare_id(Samsung_AC_Device),
            cv.Required(CONF_DEVICE_ADDRESS): cv.string,
            cv.Optional(CONF_DEVICE_ROOM_TEMPERATURE): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_DEVICE_ROOM_HUMIDITY): sensor.sensor_schema(
                unit_of_measurement=UNIT_PERCENT,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_HUMIDITY,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_DEVICE_TARGET_TEMPERATURE): NUMBER_SCHEMA,
            cv.Optional(CONF_DEVICE_POWER): switch.switch_schema(Samsung_AC_Switch),
            cv.Optional(CONF_DEVICE_MODE): SELECT_MODE_SCHEMA,
            cv.Optional(CONF_DEVICE_CLIMATE): CLIMATE_SCHEMA,
        }
    )
)

CONF_DEVICES = "devices"

CONF_DEBUG_MQTT_HOST = "debug_mqtt_host"
CONF_DEBUG_MQTT_PORT = "debug_mqtt_port"
CONF_DEBUG_MQTT_USERNAME = "debug_mqtt_username"
CONF_DEBUG_MQTT_PASSWORD = "debug_mqtt_password"

CONF_DEBUG_LOG_MESSAGES = "debug_log_messages"
CONF_DEBUG_LOG_MESSAGES_RAW = "debug_log_messages_raw"

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

        if CONF_DEVICE_POWER in device:
            conf = device[CONF_DEVICE_POWER]
            sens = await switch.new_switch(conf)
            cg.add(var_dev.set_power_switch(sens))

        if CONF_DEVICE_ROOM_TEMPERATURE in device:
            conf = device[CONF_DEVICE_ROOM_TEMPERATURE]
            sens = await sensor.new_sensor(conf)
            cg.add(var_dev.set_room_temperature_sensor(sens))

        if CONF_DEVICE_ROOM_HUMIDITY in device:
            conf = device[CONF_DEVICE_ROOM_HUMIDITY]
            sens = await sensor.new_sensor(conf)
            cg.add(var_dev.set_room_humidity_sensor(sens))

        if CONF_DEVICE_TARGET_TEMPERATURE in device:
            conf = device[CONF_DEVICE_TARGET_TEMPERATURE]
            conf[CONF_UNIT_OF_MEASUREMENT] = UNIT_CELSIUS
            conf[CONF_DEVICE_CLASS] = DEVICE_CLASS_TEMPERATURE
            num = await number.new_number(conf,
                                          min_value=16.0,
                                          max_value=30.0,
                                          step=1.0)
            cg.add(var_dev.set_target_temperature_number(num))

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

        cg.add(var.register_device(var_dev))

    cg.add(var.set_debug_mqtt(config[CONF_DEBUG_MQTT_HOST], config[CONF_DEBUG_MQTT_PORT],
           config[CONF_DEBUG_MQTT_USERNAME], config[CONF_DEBUG_MQTT_PASSWORD]))

    if (CONF_DEBUG_LOG_MESSAGES in config):
        cg.add(var.set_debug_log_messages(config[CONF_DEBUG_LOG_MESSAGES]))

    if (CONF_DEBUG_LOG_MESSAGES_RAW in config):
        cg.add(var.set_debug_log_messages_raw(
            config[CONF_DEBUG_LOG_MESSAGES_RAW]))

    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
