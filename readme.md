# ESPHome Samsung AC

This project provides an *ESPHome* component for *Samsung air conditioners*. It allows you to watch and control your devices via a home
automation system. And is designed to be attached to the F1,F2 communication bus between the devices.

Samsung changed there software protocol the devices usesing to communicate. The older devices use the Non NASA protocol where the newer
devices use the NASA protocol. This project supports both. For details look at the *NASA vs Non NASA* section below.

# Current Features:

- Monitor room temperature
- Change target temperature
- Change AC mode

## Hardware installation

You need:

- Any ESP devices ESPHome supports (like NodeMCU)
- An "TTL to RS485" converter (UART) - look at Amazon or Ebay
- (optional - use power directly from AC) An Mini step down power converter 12v to 9v

### Instructions

- Wire the A+ port of the "TTL to RS485" to F1 of your "Samsung AC" and B+ to F2.
- Wire the GND port of "TTL to RS485" to G of the ESP. TXD to TX, RXD to RX, VCC to 3c.
- (Optional) Wire V1 from AC to the +IN from step down converter and V2 to -IN. Then Wire +OUT to VIN of ESP and -OUT to G of ESP.

If you have problems receiving data, first check your wiring. I had a lot of problems with loose pin connectors.

## Software installation

- Create a now ESPHome Device and add the stuff from example.yaml (check our ESP chip and change if needed).
- Deploy and boot it.
- After reboot check your log. It should write out yellow received data packets.
- Wait a few seconds and watch for purple log messages like this: "known indoor devices: 20.00.00, 20.00.01, 20.00.02, 20.00.03"
- That are your indoor device addresses.
- copy the address block in your .yaml file for each unit witch is listed and change names to the names of the rooms.
- Remove properties you dont need.

## NASA vs Non NASA

It took me a while to figure out what the difference is. NASA is the new wire protocol which Samsung uses for their AC systems.
The old units are using the so-called Non NASA protocol. The protocols share some aspects like the start and end byte. But the
newer NASA protocol is more complex and allows more data to be transferred and more units to communicate.

DannyDeGaspari provides and exelant explanation of the Non NASA protocol here https://github.com/DannyDeGaspari/Samsung-HVAC-buscontrol

Hopefully I can provide a description of the NASA protocol here soon.

## Credits

Thanks goes to DannyDeGaspari https://github.com/DannyDeGaspari/Samsung-HVAC-buscontrol. He made the initial attempt to describe the
older Non NASA protocol.

Thanks goes to matthias882 https://github.com/matthias882/some_esphome_components. He made an an basic ESPHome component
for the older Non NASA protocol which was a perfect source for start playing and learning to communicate with the AC
and use ESPHome.
