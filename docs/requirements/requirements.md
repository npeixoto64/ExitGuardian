# Project Requirements

## 1. Introduction

**Project Name:** Exit Guardian
**Version:** v1.0.1
**Author(s):** Nuno Peixoto
**Date:** 22/09/2025
**Purpose:** A device installed on the main exit door that triggers an alert with a buzzer when the main exit door is open and any window or outside door is also open. When the main door is closed, the alert is always turned off. The device is intended for use in residential and commercial buildings.

## 2. Overall System Requirements

```{req} System composed of sensors and gateway
:id: SR_001
:status: approved
:component: system

The system shall be composed of one or more wireless sensors and a gateway.
```

```{req} Sensor reports window status
:id: SR_002
:status: approved
:component: sensor

The wireless sensors shall send the status of the window to the gateway upon an open or close event.
```

```{req} Sensor powered by replaceable battery
:id: SR_003
:status: approved
:component: sensor

The wireless sensors shall be powered by a replaceable battery.
```

```{req} Gateway dual power supply
:id: SR_004
:status: approved
:component: gateway

The gateway shall support both power supply options: replaceable battery and +5V DC. Both options must be available simultaneously, and the system shall automatically switch to battery power in case of main power failure.
```

```{req} Gateway buzzer
:id: SR_005
:status: approved
:component: gateway

The gateway shall have a buzzer.
```

```{req} Gateway button for reset and pairing
:id: SR_006
:status: approved
:component: gateway

The gateway shall have a button to allow a hard factory reset and to pair/unpair wireless sensors.
```

```{req} Gateway LEDs
:id: SR_007
:status: approved
:component: gateway

The gateway shall have three LEDs (green, yellow, and red) to indicate its status.
```

```{req} LED usage (heartbeat, config, alerts)
:id: SR_008
:status: approved
:component: gateway

The gateway shall use LEDs to indicate: heartbeat (green), configuration/low-battery status (yellow), and alerts/low-battery (red). Detailed behavior is defined in the FR section.
```

## 3. System Functional Requirements

```{req} Sensor sends event on open/close
:id: FR_001
:status: approved
:component: sensor

When a window is opened or closed, the sensor shall send a data packet to the gateway via RF.
```

```{req} Sensor packet includes battery and event
:id: FR_002
:status: approved
:component: sensor

The sensor data packet shall include the battery status and the event type (open or close).
```

```{req} Unique sensor ID
:id: FR_003
:status: approved
:component: sensor

Each sensor shall have a unique identifier (ID).
```

```{req} Green LED heartbeat
:id: FR_004
:status: approved
:component: gateway

The gateway shall indicate it is operating properly by flashing the green LED for 100 ms every two seconds.
```

```{req} Gateway operating modes
:id: FR_005
:status: approved
:component: gateway

The gateway shall support four modes of operation: waiting for configuration, listening for new sensors, remove sensors and monitoring.
```

```{req} Enter waiting-for-configuration mode with no sensors
:id: FR_006
:status: approved
:component: gateway

The gateway shall enter waiting-for-configuration mode when there are no sensors attached.
```

```{req} Yellow LED ON in waiting-for-configuration
:id: FR_007
:status: approved
:component: gateway

In waiting-for-configuration mode, the yellow LED shall remain ON continuously.
```

```{req} Reset leads to waiting-for-configuration
:id: FR_008
:status: approved
:component: gateway

After a factory RESET event, the gateway shall enter waiting-for-configuration mode.
```

```{req} Normal mode when a sensor is added
:id: FR_009
:status: approved
:component: gateway

When at least one sensor is added to the gateway, it shall switch to normal operation mode, and the yellow LED shall turn OFF.
```

```{req} Monitor main door open/close
:id: FR_010
:status: approved
:component: gateway

In monitoring mode, the gateway shall detect open and close events of the main door.
```

```{req} Alert logic based on main door and windows
:id: FR_011
:status: approved
:component: system

In monitoring mode, the gateway shall evaluate window states whenever the main door changes state:
  - If the main door is **opened**:
    - If one or more windows are **OPEN** → the gateway shall trigger an alert (buzzer + flashing red LED) and keep it active while the door remains open.
    - If **all windows are CLOSED** → the gateway shall not trigger an alert and shall blink the green LED once to indicate all clear.
  - If the main door is **closed**:
    - The gateway shall immediately suspend any active alert, regardless of the window states.
```

```{req} Alert signal pattern
:id: FR_012
:status: approved
:component: gateway

The alert signal shall consist of a continuous buzzer and the red LED flashing every 200 ms with a 50% duty cycle.
```

```{req} Gateway low battery indication (red solid, battery power only)
:id: FR_013
:status: approved
:component: gateway

The gateway shall indicate a low battery condition by keeping the red LED ON continuously, but only when battery powered.
```

```{req} Double-press enters listening mode
:id: FR_014
:status: approved
:component: gateway

If the gateway push button is double-pressed within a 1-second time frame, the gateway shall enter listening-for-new-sensors mode.
```

```{req} Listening mode indicators and timeout
:id: FR_015
:status: approved
:component: gateway

In listening-for-new-sensors mode, the gateway shall wait up to 20 seconds for a new sensor packet. During this period, the yellow LED shall flash every 500 ms with a 50% duty cycle, and the buzzer shall beep every second.
```

```{req} Exit listening mode after 20s if no new sensor
:id: FR_016
:status: approved
:component: gateway

If no new sensor signal is received within 20 seconds in listening-for-new-sensors mode, the gateway shall exit this mode, return the yellow LED to its previous state, and turn the buzzer OFF.
```

```{req} Add new sensor confirmation
:id: FR_017
:status: approved
:component: gateway

If a signal from a new sensor ID is received in listening-for-new-sensors mode, the gateway shall add it to the monitored sensors list, turn the yellow LED OFF, and sound the buzzer continuously for 2 seconds.
```

```{req} Known sensor updates in listening mode
:id: FR_018
:status: approved
:component: gateway

If a known sensor from the monitored list sends a signal in listening-for-new-sensors mode, the gateway shall update its status and continue listening until the 20-second period expires.
```

```{req} Sensor battery insertion or event sufficient for add
:id: FR_019
:status: approved
:component: system

In listening-for-new-sensors mode, inserting the battery or changing the status of a sensor (open/close event) shall be sufficient to add it to the gateway.
```

```{req} Indicate sensor low battery via yellow + green sync
:id: FR_020
:status: approved
:component: gateway

If any sensor reports a low battery, the gateway shall indicate this condition by flashing the yellow LED synchronized with the green heartbeat LED.
  - This indication means that all sensor batteries should be replaced.
```

```{req} Triple-press enters remove-sensor mode
:id: FR_021
:status: approved
:component: gateway

If the gateway push button is triple-pressed within a 2-second time frame, the gateway shall enter “remove-sensor” mode.
  - In this mode, the yellow LED shall flash rapidly (e.g., every 200 ms) to indicate active removal.
  - The gateway shall wait up to 20 seconds for a signal from a known sensor.
  - If a signal is received from a known sensor ID, the gateway shall remove that sensor from the monitored list, stop flashing the yellow LED, and sound the buzzer continuously for 2 seconds to confirm removal.
  - If no known sensor signal is received within 20 seconds, the gateway shall exit remove-sensor mode and return the yellow LED to its previous state.
```

```{req} Long-press triggers factory reset
:id: FR_022
:status: approved
:component: gateway

If the gateway push button is pressed for more than 5 seconds, all LEDs shall start flashing; when the push button is released, a factory RESET shall occur.
```

```{req} Post-reset has no paired sensors
:id: FR_023
:status: approved
:component: gateway

After a factory RESET, the gateway shall have no sensors attached.
```

```{req} Concurrent low-battery indications
:id: FR_024
:status: approved
:component: system

If both gateway and sensor low-battery conditions occur simultaneously, both indications (red ON + yellow flashing) shall be active.
```

## 4. System Non-Functional Requirements

```{req} Alert latency under 500 ms
:id: NFR_001
:status: approved
:component: gateway

The gateway shall trigger an alert within 500 ms after the main door is opened.
```

```{req} Indoor RF range
:id: NFR_002
:status: approved
:component: system

The system shall support reliable operation over at least 20 m indoors through 2 concrete walls, 20 cm width.
```

```{req} Sensor battery life ≥ 3 years
:id: NFR_003
:status: approved
:component: sensor

The sensor battery life shall be at least 3 years.
```

```{req} Gateway battery life ≥ 1 year
:id: NFR_004
:status: approved
:component: gateway

The gateway battery life shall be at least 1 year.
```

```{req} Buzzer level 85–95 dB at 1 m
:id: NFR_005
:status: approved
:component: gateway

The buzzer acoustic level shall be ~85–95 dB at 1 m to ensure usability.
```

```{req} Sensor retransmits ≥ 3 times per event
:id: NFR_006
:status: approved
:component: sensor

The sensor shall be able to send the data packet at least 3 times per open/close event.
```

```{req} Gateway reserve ≥ 7 days after low battery
:id: NFR_007
:status: approved
:component: gateway

The gateway battery shall be able to operate for at least 7 more days after a low battery alert.
```

```{req} Watchdog for hard reset
:id: NFR_008
:status: approved
:component: gateway

The gateway shall have a watchdog to perform a hard RESET in case of issues.
```

```{req} Pairing and unpairing supported
:id: NFR_009
:status: approved
:component: gateway

The gateway shall allow pairing and unpairing of sensors.
```

```{req} Gateway battery ≤ 2x AA
:id: NFR_010
:status: approved
:component: gateway

The gateway shall use no more than two AA batteries, alkaline or NiMH.
```

```{req} Seamless mains-to-battery switchover
:id: NFR_011
:status: approved
:component: gateway

If the gateway has batteries and is being supplied by mains power, it shall switch to battery mode in case of a power failure. The switch shall be seamless, with no interruption to system operation.
```

```{req} User config via button + LEDs
:id: NFR_012
:status: approved
:component: gateway

End-user configuration shall be performed via a push button and three LEDs.
```

```{req} Stable sensor list in NVM
:id: NFR_013
:status: approved
:component: gateway

The gateway shall maintain a stable list of paired sensors in non-volatile memory (NVM) to ensure consistent operation after power loss.
  - The system shall not require the user to track individual sensor numbers for maintenance.
  - When a low-battery condition is reported, the user is instructed to replace the batteries of all sensors.
```

```{req} Persist sensor status in NVM
:id: NFR_014
:status: approved
:component: gateway

The gateway shall save the status of each sensor in NvM (Non-volatile Memory).
```

## 5. System Assumptions & Constraints

- Security is not a critical factor.  
- No tamper detection or advanced security features.  
- Cannot directly identify which sensor has low battery.  
- Sensor has no UI beyond battery insertion.  
- Gateway may be powered by 5V DC jack or batteries.  
- System operates in 0–40 °C, 10–90% RH, non-condensing.  

## 6. Legal & Regulatory Requirements

```{req} CE compliance
:id: LR_001
:status: approved
:component: system

Device shall comply with CE marking requirements (Low Voltage Directive, EMC Directive).
```

```{req} EU RF compliance (868 MHz ISM)
:id: LR_002
:status: approved
:component: system

RF module shall operate only on frequencies allowed in the EU (e.g., 868 MHz ISM band, duty cycle limits).
```

```{req} RoHS / REACH compliance
:id: LR_003
:status: approved
:component: system

Materials must comply with RoHS / REACH.
```

```{req} EU Battery Directive compliance
:id: LR_004
:status: approved
:component: system

Product must comply with EU Battery Directive for labeling and recycling.
```

## 7. Open Issues

- None.

## 8. Traceability

- None.

## 9. Glossary

**Buzzer:** Device that produces a sound for alerts.  
**CE Marking:** EU conformity certification.  
**Duty Cycle:** % of a period when a signal is active.  
**Factory RESET:** Restores device to original settings.  
**Gateway:** Central device that receives data and manages alerts.  
**LED:** Light Emitting Diode. Indicator for system status.  
**Low Voltage Directive:** EU directive for electrical safety.  
**MCU:** Microcontroller Unit.  
**NVM:** Non-Volatile Memory.  
**NiMH:** Nickel-Metal Hydride battery type.  
**RF:** Radio Frequency communication.  
**RoHS / REACH:** EU environmental compliance directives.  
**Sensor:** Wireless device detecting window/door events.  
**Watchdog:** Timer that resets system if program fails.
