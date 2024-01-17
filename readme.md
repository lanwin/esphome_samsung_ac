# ESPHome Samsung AC

This project offers a [ESPHome](https://esphome.io/index.html) component for integrating Samsung air conditioners into your home automation system. It provides the ability to monitor and control your AC units effortlessly. 

This component is designed to connect to the F1 and F2 communication bus between the indoor and the outdoor devices.

Samsung has employed different software protocols for their AC devices over the years. The older devices utilize the Non NASA protocol, while the newer ones utilize the NASA protocol. This ESPHome component is designed to support both protocols, ensuring compatibility with a wide range of Samsung AC units.

## Current Features

The current implementation offers the following features:

- **Mutlisplit Support:** One ESP can control all indoor units connected to the outdoor unit.

- **Temperature Monitoring:** You can monitor the room temperature.

- **Humidity Monitoring (NASA Protocol):** For systems using the NASA protocol, the controller allows you to monitor humidity levels, providing additional environmental insights.

- **Target Temperature Adjustment:** You have the ability to change the target temperature, allowing you to set your desired climate.

- **AC Mode Control:** The controller enables you to change the AC mode, giving you control over cooling, heating, or other operational modes.


## Hardware installation

An ESPHome compatible device and an RS-485 to serial adapter is required to run this project. While its possible to run it on an ESP8266 its better to chose an ESP32 since it handles the incoming message stream better (cause it has more CPU power and ram).

### We recommand to use the M5STACK ATOM Lite + M5STACK RS-485 kit
Its cheap, comes with a tiny case (witch can fit inside an indoor unit) and allow directly to use the 12V comming from the V1/V2 lines witch some AC units provide.

1. Purchase the following components and stack them:
   - **M5STACK ATOM Lite** - [Aliexpress](https://a.aliexpress.com/_mO88aeK), [M5STACK store](https://shop.m5stack.com/products/atom-lite-esp32-development-kit), [documentation](https://docs.m5stack.com/en/core/ATOM%20Lite)
   - **M5STACK ATOM RS-485 Kit** - [Aliexpress](https://a.aliexpress.com/_mLhOZQA), [M5STACK store](https://shop.m5stack.com/products/atom-rs485-kit?variant=34787900194980), [documentation](https://docs.m5stack.com/en/atom/atomic485)

1. Connect the components as follows:
   - Connect F1 on the AC unit to A on the M5STACK controller.
   - Connect F2 on the AC unit to B on the M5STACK controller.
   - Connect V1 on the AC unit to DC on the M5STACK controller.
   - Connect V2 on the AC unit to G on the M5STACK controller.

<img alt='M5STACK Wiring Diagram' src='https://github.com/lanwin/esphome_samsung_ac/assets/32042186/42a6757d-bfcf-4a29-be87-cf1b204e248a' width='400'>

## Software Installation

Follow these steps to install and configure the software for your AC unit controller:

1. **Create a New ESPHome Device:**
   - Begin by creating a new ESPHome device in your Home Assistant instance or ESPHome comand line tool.
   - Use the configuration from the provided `example.yaml` file as a template. Make sure to verify that the ESP chip model matches your hardware. For M5STACK devices, set the `board` to 'm5stack-atom' and RX, TX pins for UART to GPIO19 and GPIO22 in the configuration.

   ```yaml
   esp32:
     board: m5stack-atom
   # ... (other configurations)
   uart:
     tx_pin: GPIO19
     rx_pin: GPIO22
     baud_rate: 9600
     parity: EVEN
   ```

1. **Deploy and Boot:**
   - Deploy the configured firmware to your ESP device.
   - After deploying, power on the ESP.

1. **Check the Log:**
   - Monitor the log output of your ESPHome device. You should see yellow log messages indicating the reception of data packets.
  
1. **Identify Indoor Device Addresses:**
   - Wait for a few seconds and watch for purple log messages in the format: "known indoor devices: 20.00.00, 20.00.01, 20.00.02, 20.00.03" for NASA and "known indoor devices: c8" for NonNASA.
   - These are the addresses of your indoor devices.
  
1. **Update Your YAML File:**
   - Copy the address block containing the indoor device addresses from the log.
   - In your ESPHome configuration YAML file, create a section for each indoor unit using the copied addresses.
   - Assign meaningful names to each unit based on the rooms they control.
   - Customize the configuration properties as needed for each unit.

1. **Remove Unneeded Properties:**
   - Review and clean up the configuration by removing any properties that are not relevant or needed for your setup.

## Troubleshooting

* Check your wiring (I had a lot problems cause the wire connection was loose)
* Check that you really connected to the same pins/cables as our outdoor device (usally F1/F2). Not to the pins/calbes of a remote control unit.
* Test if swapping F1/F2 helps
* Change **baud_rate** from 9600 to 2400 (some older hardware uses a lower baud rate)
* Add the following to your yaml witch dumps all data witch is received via RS484 to logs. This helps to check if you get any data. This also helps when reporting problems.
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

* **Did I need to power cycle my Samsung devices to make it work?** No, but the should be turned on.
* **Did this works also with Samsung heat pumps?** Yes, while it was not desinged in the first place for them, we have reports that it also works there.
* **Did I need a ESP for each indoor device?** When all your indoor devices are connected to the same outdoor device, then you need just one. Otherwise you need one for each outdoor device.

## NASA vs Non NASA

It took me a while to figure out what the difference is. NASA is the new wire protocol which Samsung uses for their AC systems.
The old units are using the so-called Non NASA protocol. The protocols share some aspects like the start and end byte. But the
newer NASA protocol is more complex and allows more data to be transferred and more units to communicate.

If you're working with older Samsung AC units and need detailed information about the Non NASA protocol, you can refer to [DannyDeGaspari's excellent explanation](https://github.com/DannyDeGaspari/Samsung-HVAC-buscontrol) of the Non NASA protocol.

Hopefully I can provide a description of the NASA protocol here soon.

## Credits

Thanks goes to DannyDeGaspari https://github.com/DannyDeGaspari/Samsung-HVAC-buscontrol. He made the initial attempt to describe the
older Non NASA protocol.

Thanks goes to matthias882 https://github.com/matthias882/some_esphome_components. He made an an basic ESPHome component
for the older Non NASA protocol which was a perfect source for start playing and learning to communicate with the AC
and use ESPHome.
