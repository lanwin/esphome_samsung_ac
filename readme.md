# ESPHome Samsung AC

The ESPHome Samsung AC project offers a versatile component for integrating Samsung air conditioners into your home automation system. It provides the ability to monitor and control your AC units effortlessly. This component is designed to connect to the F1 and F2 communication bus between the devices.

Samsung has employed different software protocols for their AC devices over the years. The older devices utilize the Non NASA protocol, while the newer ones utilize the NASA protocol. This ESPHome component is designed to support both protocols, ensuring compatibility with a wide range of Samsung AC units.

For detailed information on the differences between the NASA and Non NASA protocols, please refer to the "NASA vs Non NASA" section below.


## Current Features

The current implementation offers the following features:

- **Mutlisplit Support:** One ESP can control all indoor units connected to the outdoor unit.

- **Temperature Monitoring:** You can monitor the room temperature.

- **Humidity Monitoring (NASA Protocol):** For systems using the NASA protocol, the controller allows you to monitor humidity levels, providing additional environmental insights.

- **Target Temperature Adjustment:** You have the ability to change the target temperature, allowing you to set your desired climate.

- **AC Mode Control:** The controller enables you to change the AC mode, giving you control over cooling, heating, or other operational modes.


## Hardware installation

This section provides instructions for setting up AC unit controller for your Home Assistant using ESPHome and compatible hardware. You can choose between a "Plug and Play" approach or a "DIY" approach based on your preferences and skill level.

### Plug and Play

The easiest way to set up the AC unit controller is through the "Plug and Play" method. Follow these steps:

1. Purchase the following components:
   - **M5STACK ATOM Lite** - [Aliexpress](https://a.aliexpress.com/_mO88aeK), [M5STACK store](https://shop.m5stack.com/products/atom-lite-esp32-development-kit), [documentation](https://docs.m5stack.com/en/core/ATOM%20Lite)
   - **M5STACK ATOM RS-485 Kit** - [Aliexpress](https://a.aliexpress.com/_mLhOZQA), [M5STACK store](https://shop.m5stack.com/products/atom-rs485-kit?variant=34787900194980), [documentation](https://docs.m5stack.com/en/atom/atomic485)

1. Connect the components as follows:
   - Connect F1 on the AC unit to A on the M5STACK controller.
   - Connect F2 on the AC unit to B on the M5STACK controller.
   - Connect V1 on the AC unit to DC on the M5STACK controller.
   - Connect V2 on the AC unit to G on the M5STACK controller.

![Plug and Play Wiring Diagram](https://github.com/lanwin/esphome_samsung_ac/assets/32042186/42a6757d-bfcf-4a29-be87-cf1b204e248a)

### DIY

If you prefer a more customized approach, you can build the AC unit controller yourself. Here's what you'll need:

- Any ESP devices that ESPHome supports (e.g., NodeMCU)
- A "TTL to RS485" converter (UART) - available on Amazon or Ebay
- (Optional - to power directly from AC) A Mini step-down power converter 12V to 9V

Follow these steps to set up the DIY AC unit controller:

1. Wire the components as follows:
   - Connect the A+ port of the "TTL to RS485" converter to F1 on your "Samsung AC."
   - Connect the B+ port of the "TTL to RS485" converter to F2 on your "Samsung AC."
   - Connect the GND port of the "TTL to RS485" converter to the GND of the ESP device.
   - Wire TXD to TX, RXD to RX, VCC to 3.3V on the ESP device.
   
   (Optional) If you want to power the ESP directly from the AC unit:
   - Wire V1 from the AC unit to the +IN of the step-down converter.
   - Wire V2 from the AC unit to the -IN of the step-down converter.
   - Connect +OUT of the step-down converter to VIN of the ESP.
   - Connect -OUT of the step-down converter to the GND of the ESP.

1. Ensure your wiring is secure to prevent loose connections.

If you encounter any issues with data reception, double-check your wiring connections.

Feel free to modify the hardware setup based on your specific requirements and hardware availability.

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
   - After deploying, power on the device.

1. **Check the Log:**
   - Monitor the log output of your ESPHome device. You should see yellow log messages indicating the reception of data packets.
  
1. **Identify Indoor Device Addresses:**
   - Wait for a few seconds and watch for purple log messages in the format: "known indoor devices: 20.00.00, 20.00.01, 20.00.02, 20.00.03".
   - These are the addresses of your indoor devices.
  
1. **Update Your YAML File:**
   - Copy the address block containing the indoor device addresses from the log.
   - In your ESPHome configuration YAML file, create a section for each indoor unit using the copied addresses.
   - Assign meaningful names to each unit based on the rooms they control.
   - Customize the configuration properties as needed for each unit.

1. **Remove Unneeded Properties:**
   - Review and clean up the configuration by removing any properties that are not relevant or needed for your setup.


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
