# ESPHome Samsung HVAC Bus

This project offers an [ESPHome](https://esphome.io/index.html) component for integrating Samsung HVAC units (Air conditioners or heatpumps) into Home Assistant. 

This component focuses on Samsung HVAC units that communicate between the indoor and outdoor units via a hardware bus (additional cables connecting each unit, usually F1/F2, sometimes called R1/R2).

Samsung has employed different software protocols for their AC devices over the years. The older devices utilize the NonNASA protocol, while the newer ones utilize the NASA protocol. This ESPHome component is designed to support both protocols, ensuring compatibility with a wide range of Samsung AC units.

## Current Features

The current implementation offers the following features:

- **Mutlisplit Support:** One ESP can control all indoor units connected to the outdoor unit.

- **Temperature Monitoring:** You can monitor the room temperature.

- **Humidity Monitoring (NASA Protocol):** For systems using the NASA protocol, the controller allows you to monitor humidity levels, providing additional environmental insights.

- **Target Temperature Adjustment:** You have the ability to change the target temperature, allowing you to set your desired climate.

- **AC Mode Control:** The controller enables you to change the AC mode, giving you control over cooling, heating, or other operational modes.

## New Feature: ESPHome Samsung AC Blueprint

We are excited to introduce a new Blueprint that integrates with the esphome_samsung_ac component. This Blueprint enhances your Home Assistant setup by enabling you to monitor error codes from your Samsung AC units and receive real-time notifications directly to your selected devices.

### Key Features:
- **Error Code Monitoring:** Continuously monitors error codes from your Samsung AC unit, ensuring that you are always informed about any issues.
- **Automated Notifications:** Sends detailed notifications with corresponding error messages to selected devices whenever an error code is detected.
- **User-Friendly Setup:** Easily import and configure this Blueprint through Home Assistant’s Blueprint feature.

For more detailed instructions on setup and usage, please refer to the [Blueprint documentation](https://github.com/omerfaruk-aran/esphome_samsung_ac_blueprint).

This Blueprint adds a new layer of functionality to your Home Assistant setup, allowing for more proactive and informed management of your Samsung AC units.

[![Add Blueprint to Home Assistant](https://community-assets.home-assistant.io/original/4X/d/7/6/d7625545838a4970873f3a996172212440b7e0ae.svg)](https://my.home-assistant.io/redirect/blueprint_import/?blueprint_url=https://raw.githubusercontent.com/omerfaruk-aran/esphome_samsung_ac_blueprint/main/blueprints/automation/esphome_samsung_ac/notification_blueprint.yaml)

## Compatibility

In general, all devices with dedicated communication wires (not only power) should work. If you want to be safe when buying a new AC, then just ask for NASA support.

[@hnykda](https://github.com/hnykda) found a way to decode Samsung's product numbers. For more info, see [this issue](https://github.com/lanwin/esphome_samsung_ac/issues/101#issuecomment-2098206070).

There are also two Discussion threads about confirmed [NASA]([url](https://github.com/lanwin/esphome_samsung_ac/discussions/82)) and [NonNASA]([url](https://github.com/lanwin/esphome_samsung_ac/discussions/78)) uses. If you made this working on a model that has not been confirmed yet, please do so in there!

### Known to work (not exhaustive)

#### NASA

- AJ080TXJ4KG, AJ026TN1DKG, AR24TXHZAWKNEU, AR24TXFCAWKN, AR09TXFCAWKNEU, AC030KNZDCH/AA (CNH30ZDK), AE090RNYDEG, AE090RXEDEG, AE160JXYDEH/EU, AR09TXFCAWKNEU, AR07TXFCAWKNEU, AR12TSFABWKNCV, AR12TSFACWKX

#### NonNASA

- RJ100F5HXBA, AJ050NCJ2EG

### Dont work

- AR12BSEAMWKX (no communication wires)

## Hardware installation

An ESPHome compatible device and an RS-485 to (TTL) serial adapter is required to run this project. While it's possible to run it on an ESP8266, it's better to chose an ESP32 since it handles the incoming message stream better (because it has more CPU power and RAM).

### We recommand to use the M5STACK ATOM Lite + M5STACK RS-485 kit

It's cheap, comes with a tiny case (which can fit inside an indoor unit) and allows direct use of the 12V comming from the V1/V2 lines which some AC units provide.

> If your AC did not has V1/V2 please check out this [post](https://github.com/lanwin/esphome_samsung_ac/discussions/39#discussioncomment-8383733).

1. Purchase the following components and stack them:

   - **M5STACK ATOM Lite** - [Aliexpress](https://a.aliexpress.com/_mO88aeK), [M5STACK store](https://shop.m5stack.com/products/atom-lite-esp32-development-kit), [documentation](https://docs.m5stack.com/en/core/ATOM%20Lite)
   - **M5STACK ATOM RS-485 Base** - [Aliexpress](https://a.aliexpress.com/_mLhOZQA), [M5STACK store](https://shop.m5stack.com/products/atomic-rs485-base), [documentation](https://docs.m5stack.com/en/atom/atomic485)

1. Connect the components as follows:
   - Connect F1 on the AC unit to A on the M5STACK controller.
   - Connect F2 on the AC unit to B on the M5STACK controller.
   - Connect V1 on the AC unit to DC on the M5STACK controller.
   - Connect V2 on the AC unit to G on the M5STACK controller.

<img alt='M5STACK Wiring Diagram' src='https://github.com/lanwin/esphome_samsung_ac/assets/32042186/42a6757d-bfcf-4a29-be87-cf1b204e248a' width='400'>

## Software Installation

Follow these steps to install and configure the software for your AC unit controller:

1. **Create a New ESPHome Device:**

   - Begin by creating a new ESPHome device in your Home Assistant instance or ESPHome command line tool.
   - Use the configuration from [example.yaml](https://github.com/lanwin/esphome_samsung_ac/blob/main/example.yaml) file as a template and copy over the `api` and `ota` sections from the newly created YAML.

1. **Deploy and Boot:**

   - Deploy the configured firmware to your ESP device.
   - After deploying, power on the ESP.

1. **Check the Log:**

   - Monitor the log output of your ESPHome device. You should see yellow log messages indicating the reception of data packets. If you only see something like `[samsung_ac] ... update` every 30 seconds or so, you are not receiving messages, and something is wrong.

1. **Identify Indoor Device Addresses:**

   - Wait for a minute and watch for purple log messages and the "Discovered devices" section in the format " indoor: 20.00.00, 20.00.01" or " indoor: 00, 01."
   - These are the addresses of your indoor devices.
   - If you don't see this - look into the Troubleshooting section below.

1. **Update Your YAML File:**

   - Copy the address block containing the indoor device addresses from the log.
   - In your ESPHome configuration YAML file, create a section for each indoor unit using the copied addresses.
   - Assign meaningful names to each unit based on the rooms they control.
   - Customize the configuration properties as needed for each unit.

1. **Remove Unneeded Properties:**
   - Review and clean up the configuration by removing any properties that are not relevant or needed for your setup.

## Troubleshooting

- Check your wiring (I had a lot of problems because the wire connection was loose)
- Check that you really connected to the same pins/cables as your outdoor device (usually F1/F2), not to the pins/cables of a remote control unit.
- Test if swapping F1/F2 helps
- Change **baud_rate** from 9600 to 2400 (some older hardware uses a lower baud rate)
- For some boards (like NodeMCU) you need to disable serial logging, since it blocks the pins required for the RS485 serial communication. Just add `baud_rate: 0` to the logger section.
- Add the following to your [YAML uart configuration section](https://esphome.io/components/uart.html#debugging) which dumps all data which is received via RS485 to logs. This helps to check if you get any data. This also helps when reporting problems.

```yaml
  debug:
    direction: BOTH
    dummy_receiver: false
      after:
        delimiter: "\n"
        sequence:
        - lambda: UARTDebug::log_hex(direction, bytes, ' ');
```

## FAQ

- **Do I need to power cycle my Samsung devices to make it work?** No, but they should be turned on.
- **Does this also work with Samsung heat pumps?** Yes, while it was not designed in the first place for them, we have reports that they also work.
- **Do I need a ESP for each indoor device?** When all your indoor devices are connected to the same outdoor device, then you need just one. Otherwise you need one for each outdoor device.
- **Do I need to turn off my climate devices when I connect the ESP?** No, but it's advised to do so, as beside the F1/F2 connectors there is 240V AC, which can be deadly. It is safer to disconnect the unit from power while installing the ESP, then reconnect it.

- **My device has no additional F1/F2 connectors, how do I connect it?** Somethimes they are called R1/R2. On some devices it seems that this connectors use only one cable (and ground) but we are not sure yet. Please follow the discussions.

## Development

The following YAML configuration is not included in the example since it is for development purposes.

```yaml
# All this values are optional. Only use the ones you need.
samsung_ac:
  # Sends all NASA package values to MQTT so the can be analysed or monitored.
  debug_mqtt_host: 10.10.10.10
  debug_mqtt_port: 1883
  debug_mqtt_username: user
  debug_mqtt_password: password

  # Prints the parsed message data to the log
  debug_log_messages: true
  # Prints the binary message data (HEX encoded) to the log
  debug_log_messages_raw: true
```

## NASA vs NonNASA

NASA is the new wire protocol which Samsung uses for their AC systems.
The old units are using the so-called NonNASA protocol. The protocols share some aspects like the start and end byte. But the newer NASA protocol is more complex, allows more data to be transferred and more units to communicate.

### NASA

The NASA protocol is pretty generic. It's basically designed to transport variables as a key (number) and a value (with a datatype like Enum, Int, Long, Bytes). All meaning is defined to the keys. For instance, if you want to know the room temperature you need to know the number and wait for it.

[Foxhill67](https://github.com/Foxhill67) started to document the NASA protocol [here](https://wiki.myehs.eu/wiki/NASA_Protocol).

### NonNASA

The NonNASA protocol is specifically desined to transport AC data.

[DannyDeGaspari](https://github.com/DannyDeGaspari) started to document the NonNASA protocol (but from wall controller side) [here](https://github.com/DannyDeGaspari/Samsung-HVAC-buscontrol).

## Credits

Thanks goes to DannyDeGaspari https://github.com/DannyDeGaspari/Samsung-HVAC-buscontrol. He made the initial attempt to describe the older NonNASA protocol.

Thanks goes to matthias882 https://github.com/matthias882/some_esphome_components. He made a basic ESPHome component for the older NonNASA protocol which was a perfect source to start playing and learning to communicate with the AC and use ESPHome.
