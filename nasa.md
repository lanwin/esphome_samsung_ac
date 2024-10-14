# Samsung Air Conditioner NASA Protocol

The document describes the data format which is used by newer Samsung Air condiationers in communication from indoor to outdoor units.
Older units use the Non-NASA protocol which DannyDeGaspari described here https://github.com/DannyDeGaspari/Samsung-HVAC-buscontrol.

> Most of the stuff here is from reverse engeneering Samsungs "SNET Pro Service Software V1.5.3 (NASA)" Software.
> This software is build in Microsofts dotnet and can simply be decompiled an read in example via ILSpy.

### Preamble

Before the message starts there can be a preamble 0x55 which can be 100 bytes long. It's not clear whats the purpose of this.

### Basics

Max packet size 1500 byte.
Min packet size 16 byte.
Timeout send/recv 1000ms
