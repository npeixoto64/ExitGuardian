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

```{req} Gateway power supply
:id: SR_004
:status: approved
:component: gateway

The gateway shall be powered by an AC-DC power supply, 230 V AC to 5 V DC.
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

The gateway shall have a push button to allow a hard factory reset and to pair/unpair the sensors.
```

```{req} Gateway LEDs
:id: SR_007
:status: approved
:component: gateway

The gateway shall have three LEDs (blue, yellow, and red) to indicate its status.
```

```{req} LED usage (heartbeat, config, alerts)
:id: SR_008
:status: approved
:component: gateway

The gateway shall use LEDs to indicate: heartbeat (blue), sensors low-battery status (yellow), waiting-for-configuration and alert (red).
```

## 3. System Functional Requirements

```{req} Sensor sends event on open/close
:id: FR_001
:status: approved
:component: sensor

When a window is opened or closed, the sensor shall send a data packet to the gateway, through RF.
```

```{req} Sensor packet includes battery and event
:id: FR_002
:status: approved
:component: sensor

The sensor data packet shall include the identifier, battery status and the event type (open, close, pair or unpair request).
```

```{req} Unique sensor ID
:id: FR_003
:status: approved
:component: sensor

Each sensor shall have a unique identifier (ID).
```

```{req} Blue LED heartbeat
:id: FR_004
:status: approved
:component: gateway

The gateway shall indicate it is operating properly by flashing the blue LED for 100 ms every two seconds.
```

```{req} Gateway operating modes
:id: FR_005
:status: approved
:component: gateway

The gateway shall support 3 modes of operation: waiting-for-configuration, pairing/unpairing sensors and monitoring.
```

```{req} Enter waiting-for-configuration mode with no sensors
:id: FR_006
:status: approved
:component: gateway

The gateway shall enter waiting-for-configuration mode when there are no paired sensors.
```

```{req} Red LED ON in waiting-for-configuration
:id: FR_007
:status: approved
:component: gateway

In waiting-for-configuration mode, the red LED shall remain ON continuously.
```

```{req} Factory reset enters waiting-for-configuration mode
:id: FR_008
:status: approved
:component: gateway

After a factory RESET event, the gateway shall enter waiting-for-configuration mode of operation.
```

```{req} Normal mode when a sensor is added
:id: FR_009
:status: approved
:component: gateway

When at least one sensor is added to the gateway, it shall switch to normal operation mode, and the red LED shall turn OFF.
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

In monitoring mode of operation, the gateway shall evaluate window states whenever the main door changes state:
  - If the main door is **opened**:
    - If one or more windows are **OPEN** → the gateway shall trigger an alert while the door remains open.
    - If **all windows are CLOSED** → the gateway shall not trigger any kind of alert.
  - If the main door is **closed**:
    - The gateway shall immediately suspend any active alert, regardless of the window states.
```

```{req} Alert signal pattern
:id: FR_012
:status: approved
:component: gateway

The alert signal shall consist of single buzzer beep, for 1 second, and the red LED flashing every 200 ms with a 50% duty cycle.
```

```{req} Push button press enters pairing/unpairing mode
:id: FR_014
:status: approved
:component: gateway

If the gateway is in monitoring or waiting-for-configuration mode of operation and its push button is pressed once, the gateway shall enter in pairing/unpairing mode of operation.
```

```{req} Pairing/unpairing mode indicators and timeout
:id: FR_015
:status: approved
:component: gateway

In pairing/unpairing mode of operation, the gateway shall wait up to 20 seconds for a sensor add or remove packet. During this period, the yellow LED shall flash every 500 ms with a 50% duty cycle, and the buzzer shall beep every second.
```

```{req} Button press exits pairing/unpairing mode
:id: FR_016
:status: approved
:component: gateway

In case the gateway is in pairing/unpairing mode of operation and the push button is pressed once the gateway exits the pairing/unpairing mode.
```

```{req} Pair sensor confirmation
:id: FR_017
:status: approved
:component: gateway

In case a pairing request from a new sensor ID is received in pairing/unpairing mode of operation, the gateway shall add it to the monitored sensors list, turn the yellow LED OFF, sound the buzzer continuously for 2 seconds and exit pairing/unpairing mode of operation.
```

```{req} Known sensor ID pairing request handling
:id: FR_018
:status: approved
:component: gateway

In case a pairing request from an existing sensor ID is received in pairing/unpairing mode of operation, the gateway shall ignore it and exit pairing/unpairing mode of operation.
```

```{req} Unpair sensor confirmation
:id: FR_019
:status: approved
:component: gateway

In case an unpairing request from a known sensor ID is received in pairing/unpairing mode of operation, the gateway shall remove it from the monitored sensors list, turn the yellow LED OFF, sound the buzzer continuously for 2 seconds and exit pairing/unpairing mode of operation.
```

```{req} Unknown sensor ID unpairing request handling
:id: FR_020
:status: approved
:component: gateway

In case an unpairing request from a new sensor ID is received in pairing/unpairing mode of operation, the gateway shall ignore it and exit pairing/unpairing mode of operation.
```

```{req} Indicate sensor low battery via yellow + blue sync
:id: FR_021
:status: approved
:component: gateway

If any sensor reports a low battery, the gateway shall indicate this condition by flashing the yellow LED synchronized with the blue heartbeat LED.
  - This indication means that one or more sensors have low battery.
```

```{req} Long-press triggers factory reset
:id: FR_022
:status: approved
:component: gateway

If the gateway push button is pressed for more than 5 seconds, all LEDs shall start flashing every 200 ms, 50% duty-cycle; when the push button is released, a factory RESET shall occur.
```

```{req} Post-reset has no paired sensors
:id: FR_023
:status: approved
:component: gateway

After a factory RESET, the gateway shall have no sensors attached, being in waiting-for-configuration mode of operation.
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

```{req} Buzzer level 85–95 dB at 1 m
:id: NFR_004
:status: approved
:component: gateway

The buzzer acoustic level shall be ~85–95 dB at 1 m to ensure usability.
```

```{req} Watchdog for hard reset
:id: NFR_005
:status: approved
:component: gateway

The gateway shall have a watchdog to perform a hard RESET in case of issues detected in SW.
```

```{req} Pairing and unpairing supported
:id: NFR_006
:status: approved
:component: gateway

The gateway shall allow pairing and unpairing of sensors.
```

```{req} User config via button + LEDs
:id: NFR_007
:status: approved
:component: gateway

End-user configuration shall be performed via a push button and three LEDs.
```

```{req} Stable sensor list in NVM
:id: NFR_008
:status: approved
:component: gateway

The gateway shall maintain a stable list of paired sensors and their status in non-volatile memory (NVM) to ensure consistent operation after power loss.
  - The system shall not require the user to track individual sensor numbers for maintenance.
  - When a low-battery condition is reported by a sensor, the user is instructed to check each sensor through its push button.
```

```{req} Persist sensor status in NVM
:id: NFR_009
:status: approved
:component: gateway

The gateway shall save the status of each sensor in NVM (Non-volatile Memory).
```

## 5. System Assumptions & Constraints

- Security is not a critical factor.  
- No tamper detection or advanced security features.
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
