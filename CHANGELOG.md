# Changelog

All notable changes to this project will be documented in this file.

## [20240213.1] - Initial release
This is the first version of the project.

---

### PR #193 - Introduce Swing Control and Preset Flexibility
This PR introduces three main updates:
1. Ensures both `true` and `false` values for horizontal and vertical swing control are properly handled. 
2. **Preset Configuration Flexibility**: Now users can configure presets in both short and long formats for more customization.
3. Updated `example.yaml` to reflect these changes.

- Resolves #187 
- Resolves #192 
- Tested on an actual device to ensure functionality.


------------------------

### PR #190 - Grammar/Documentation fixes
- Fixed grammar, spelling, and literary elements in README and example files.
- Added eco mode preset from #172 to `example.yaml`.
- Improved documentation for outdoor unit setup and sensor placement.


------------------------

### PR #175 - Update readme.md
- Added more models to "Known to work" list.


------------------------

### PR #174 - Update readme.md
- Further additions to the "Known to work" list.


------------------------

### PR #172 - Add "Eco Mode" to preset
- Added support for mode 7 on AR12TXFYAWKNEU indoor unit.


------------------------

### PR #169 - Handle UI Update Issues - Improved Value Change Handling in Samsung AC Integration
- Enhanced value change detection for device modes and improved UI responsiveness.
- Added timeout handling (750ms) to manage stale values and force updates when necessary.
- Improved debug logging for device state tracking.
- Data processing is initialized only after the first update, improving stability.


------------------------

### PR #167 - Error code monitoring
- Added an error code sensor to capture and monitor error codes from the HVAC system.
- Implemented a blueprint for sending error code notifications to mobile devices.


------------------------

### PR #166 - Fix build on ESP-IDF: Change logging message type interpolation
- Resolved issue #165 related to build errors with the ESP-IDF framework.


------------------------

### PR #160 - Add Automatic Cleaning Mode and Logging Features to Samsung AC Integration
- Introduced automatic cleaning mode for the Samsung AC integration.
- Enhanced logging features for message tracking and debugging.


------------------------

### PR #159 - [NASA] Add Automatic Cleaning mode for Samsung AC - Deprecated due to issues
- Deprecated due to unresolved issues.


------------------------

### PR #158 - Water Heater Mode
- Added support for water heater mode control.


------------------------

### PR #147 - Add Water outlet setpoint
- Added control for water outlet temperature and compensation curve support.


------------------------

### PR #146 - Add Water Heater Power
- Added power switch control for domestic hot water.


------------------------

### PR #145 - Dhw target temp
- Updated to include support for domestic hot water target temperature.


------------------------

### PR #143 - Clarify uart debugging info
- Improved UART debugging information.


------------------------

### PR #137 - Improve README by adding clarifying remarks, links, and correcting typos
- Enhanced README with more clarifications and links to supported devices.


------------------------

### PR #136 - Update readme.md - NonNASA add AJ050NCJ2EG
- Added model AJ050NCJ2EG to the NonNASA support list.


------------------------

### PR #123 - Merge from source

------------------------

### PR #122 - Correct the README.md file
- Revised grammatical errors in the FAQ section.


------------------------

### PR #114 - Stabilize UI when changing state for Non-NASA devices
- Prevented UI state from bouncing between states after a control command was issued for Non-NASA devices.


------------------------

### PR #112 - Controller Registration, Command Resending, and Keepalive Support for Non-NASA Devices
- Added support for registering Non-NASA devices, command resending, and keepalive functionality.


------------------------

### PR #107 - Fix typo: commad -> command
- Corrected a small typo in the code.


------------------------

### PR #105 - Fix minor typos in readme

------------------------

### PR #99 - Allow configuring alt and swing modes
- Added configuration for alternative and swing modes across devices.


------------------------

### PR #98 - [NASA] Move nasa-specific sensors into python config
- Moved NASA-specific sensors into the Python configuration.


------------------------

### PR #97 - [NASA] Add outdoor temperature sensor
- Added outdoor temperature sensor for NASA devices.


------------------------

### PR #96 - [NASA] Add accumulated power sensor

------------------------

### PR #95 - [NASA] Add target water temperature
- Added sensor for target water temperature.


------------------------

### PR #94 - [NASA] Custom sensors
- Introduced custom sensor support for arbitrary message numbers.


------------------------

### PR #92 - Add water temperature sensor
- Added a sensor for reporting water temperature.


------------------------

### PR #89 - Rename converions.cpp to conversions.cpp
- Fixed a typo in the file name.


------------------------

### PR #86 - Decoded commands c0, c1, f0, and f1 for more values from outdoor unit and inverter
- Decoded additional values from outdoor units and inverters.


------------------------

### PR #84 - Refactor to use one request object
- Refactored the code to use a single request object for batching changes.


------------------------

### PR #83 - Turbo fan mode
- Added support for turbo fan mode.


------------------------

### PR #81 - Swing mode
- Added support for swing mode control.


------------------------

### PR #80 - WindFree and other presets
- Added support for WindFree and other presets via `NASA_ALTERNATIVE_MODE`.


------------------------

### PR #75 - Inverter power and thermal power requirement decoded
- Decoded additional values related to inverter power and thermal power.


------------------------

### PR #74 - Tested delay in non-nasa-communication
- Tested and optimized delays in the Non-NASA communication protocol.


------------------------

### PR #73 - Non-Nasa send only one command when changing modes
- Fixed mode change issue by sending only one command to Non-NASA devices.


------------------------

### PR #71 - Send data after command f8 instead of command c6
- Improved data handling in the Non-NASA protocol.


------------------------

### PR #70 - Update protocol_nasa.cpp to make it compile with ESP-IDF
- Fixed compilation issues in the ESP-IDF framework.


------------------------

### PR #62 - Update samsung_ac.cpp to compile with the ESP-IDF framework
- Fixed Samsung AC compilation errors with ESP-IDF.


------------------------

### PR #48 - Make nasa.cpp compile with the esp-idf framework
- Resolved issues with compiling NASA.cpp on the ESP-IDF framework.


------------------------

### PR #38 - Improve configuration files and README.md
- Made the configuration files easier to use, especially for new users.


------------------------

### PR #36 - Update samsung_ac.h to fix debug log
- Fixed the debug log issue in samsung_ac.h.


------------------------

### PR #21 - Update Documentation
- Fixed typos and added links for M5STACK documentation and official shop.


------------------------

### PR #16 - Improve README.md formatting and content
- Enhanced the formatting, clarity, and content of the README.md.


------------------------

