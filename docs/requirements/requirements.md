# Project Requirements

## 1. Introduction

**Project Name:** Exit Guardian
**Version:** v2.0.0
**Author(s):** Nuno Peixoto
**Date:** 26/04/2026
**Purpose:** A device installed on the main exit door that triggers an alert with a buzzer when the main exit door is open and any window or outside door is also open. When the main door is closed, the alert is always turned off. The device is intended for use in residential and commercial buildings.

## 2. Overall System Requirements

```{req} System composed of sensors and gateway
:id: SR_001
:status: approved
:component: system

The system shall be composed of one or more wireless sensors and a gateway.
```

```{req} Sensor reports window/outside-door status
:id: SR_002
:status: approved
:component: sensor

The wireless sensors shall send the status of the monitored opening (window or outside door) to the gateway upon an open or close event.
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

The gateway shall use LEDs to indicate:
  - heartbeat — blue;
  - sensors low-battery condition — yellow (in monitoring mode);
  - waiting-for-configuration and active alert — red.
```

```{req} Gateway main-door sensor
:id: SR_009
:status: approved
:component: gateway

The gateway shall include an integrated sensor capable of detecting open and close events of the main exit door on which it is installed.
```

## 3. System Functional Requirements

```{req} Sensor sends event on open/close
:id: FR_001
:status: approved
:component: sensor

When a window or outside door is opened or closed, the sensor shall send a data packet to the gateway, through RF.
```

```{req} Sensor packet includes battery and event
:id: FR_002
:status: approved
:component: sensor

Each sensor packet shall include: (a) sensor unique identifier; (b) battery status — LOW (Vbat < threshold) or OK; (c) packet type ∈ {OPEN, CLOSE, PAIR_REQUEST, UNPAIR_REQUEST}.
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

The gateway shall indicate it is operating properly by flashing the blue LED for 100 ms every two seconds since boot, in any operation mode.
```

```{req} Gateway operating modes
:id: FR_005
:status: approved
:component: gateway

The gateway shall support 3 modes of operation: waiting-for-configuration, pairing/unpairing sensors and monitoring.
```

```{req} Boot-time mode selection
:id: FR_006
:status: approved
:component: gateway

After boot, the gateway shall restore the paired sensor list from NVM and enter monitoring mode if at least one sensor is paired, otherwise waiting-for-configuration mode.
```

```{req} Red LED ON in waiting-for-configuration
:id: FR_007
:status: approved
:component: gateway

In waiting-for-configuration mode, the red LED shall remain ON continuously.
```

```{req} Red LED OFF in monitoring mode if no alert
:id: FR_008
:status: approved
:component: gateway

In monitoring mode with no active alert, the red LED shall remain OFF.
```

```{req} Factory reset enters waiting-for-configuration mode
:id: FR_009
:status: approved
:component: gateway

After a factory RESET event, the gateway shall enter waiting-for-configuration mode of operation.
```

```{req} Monitoring mode when a sensor is added
:id: FR_010
:status: approved
:component: gateway

Being in waiting-for-configuration mode if a sensor is paired to the gateway, the gateway shall switch to monitoring mode and the red LED shall turn OFF. When the last paired sensor is unpaired, the gateway shall transition to waiting-for-configuration mode.
```

```{req} Monitor main door open/close
:id: FR_011
:status: approved
:component: gateway

In monitoring mode, the gateway shall detect open and close events of the main door.
```

```{req} Alert logic based on main door and windows
:id: FR_012
:status: approved
:component: system

In monitoring mode, the gateway shall maintain the last-known open/closed state of each paired sensor. The alert condition shall be evaluated whenever the main door state changes or any paired sensor reports a state change, using the main door state and the last-known state of each paired sensor: ALERT ⇔ (main door OPEN) ∧ (∃ paired sensor with last-known state OPEN). On entry to monitoring mode, each paired sensor's last-known state shall be initialized from NVM (NFR_008); sensors with no prior report shall default to CLOSED.
```

```{req} Alert signal pattern
:id: FR_013
:status: approved
:component: gateway

When the alert condition (FR_012) becomes true, the gateway shall, within 500 ms: (a) sound the buzzer for an intentional single 2 s ON pulse, and (b) start flashing the red LED with a 200 ms period and 50% duty cycle. The red LED shall continue flashing while the alert condition remains true. When the alert condition becomes false, the buzzer (if still active) shall be silenced and the red LED shall return to the indication corresponding to the current operating mode (FR_007 / FR_008).
```

```{req} Push button short press enters pairing/unpairing mode
:id: FR_014
:status: approved
:component: gateway

If the gateway is in monitoring or waiting-for-configuration mode and the push button is pressed and released in less or equal 2 s, the gateway shall enter pairing/unpairing mode.
```

```{req} Pairing/unpairing mode indicators and timeout
:id: FR_015
:status: approved
:component: gateway

In pairing/unpairing mode of operation, the gateway shall wait up to 20 s for a pair/unpair packet. During this period the yellow LED shall flash for 500 ms every second (50% duty cycle) and the buzzer shall beep for 500 ms every second. If no valid pair/unpair packet is received within 20 s, the gateway shall exit pairing/unpairing mode and return to its previous mode of operation, the yellow LED state following FR_023.
```

```{req} In pairing/unpairing mode the alerts are suspended
:id: FR_016
:status: approved
:component: gateway

While in pairing/unpairing mode, the gateway shall not evaluate or trigger the alert condition (FR_012).
```

```{req} Button press exits pairing/unpairing mode
:id: FR_017
:status: approved
:component: gateway

If the gateway is in pairing/unpairing mode of operation and the push button is short-pressed once, the gateway shall exit pairing/unpairing mode and return to its previous mode of operation (waiting-for-configuration if no sensors are paired, otherwise monitoring).
```

```{req} Pair sensor confirmation
:id: FR_018
:status: approved
:component: gateway

In case a pairing request from a new sensor ID is received in pairing/unpairing mode of operation, the gateway shall add it to the monitored sensors list, stop the pairing-mode yellow flashing, sound the buzzer continuously for 2 seconds and exit pairing/unpairing mode of operation and return to waiting-for-configuration mode if no sensors remain paired, otherwise to monitoring mode.
```

```{req} Known sensor ID pairing request handling
:id: FR_019
:status: approved
:component: gateway

In case a pairing request from an existing sensor ID is received in pairing/unpairing mode of operation, the gateway shall ignore it and exit pairing/unpairing mode of operation.
```

```{req} Unpair sensor confirmation
:id: FR_020
:status: approved
:component: gateway

In case an unpairing request from a known sensor ID is received in pairing/unpairing mode of operation, the gateway shall remove it from the monitored sensors list, stop the pairing-mode yellow flashing, sound the buzzer continuously for 2 seconds and exit pairing/unpairing mode of operation and return to waiting-for-configuration mode if no sensors remain paired, otherwise to monitoring mode.
```

```{req} Unknown sensor ID unpairing request handling
:id: FR_021
:status: approved
:component: gateway

In case an unpairing request from an unknown (non-paired) sensor ID is received in pairing/unpairing mode of operation, the gateway shall ignore it and exit pairing/unpairing mode of operation.
```

```{req} Indicate sensor low battery via yellow + blue sync
:id: FR_022
:status: approved
:component: gateway

While at least one paired sensor's last-reported battery status is LOW (and the gateway is not in pairing/unpairing mode), the yellow LED shall flash for 100 ms every 2 s, in phase with the blue heartbeat (FR_004). Otherwise the yellow LED shall be OFF.
```

```{req} Yellow LED functionality
:id: FR_023
:status: approved
:component: gateway

While in pairing/unpairing mode, the yellow LED shall follow the pairing-mode pattern (FR_015) and the low-battery indication (FR_022) shall be suspended. On exit from pairing/unpairing mode, the yellow LED state shall follow FR_022.
```

```{req} Long-press triggers factory reset
:id: FR_024
:status: approved
:component: gateway

If the push button is held for ≥ 5 s, the gateway shall flash all LEDs at a 200 ms period and 50% duty cycle for the remainder of the press; on release after the 5 s threshold, a factory RESET shall occur.
```

```{req} Push button press
:id: FR_025
:status: approved
:component: gateway

A press released in (2 s, 5 s) shall be ignored (no mode change, no factory RESET).
```

```{req} Post-reset has no paired sensors
:id: FR_026
:status: approved
:component: gateway

After a factory RESET, the gateway shall have no sensors attached, being in waiting-for-configuration mode of operation.
```

## 4. System Non-Functional Requirements

```{req} Indoor RF range
:id: NFR_001
:status: approved
:component: system

≥ 99% packet delivery success at 20 m indoors through two 20 cm concrete walls, measured over 1000 packets at the operating frequency.
```

```{req} Sensor battery life ≥ 3 years
:id: NFR_002
:status: approved
:component: sensor

Sensor battery life ≥ 3 years assuming ≤ 4 events/day at 25 °C with the specified battery (e.g., CR2032).
```

```{req} Buzzer beep level
:id: NFR_003
:status: approved
:component: gateway

The buzzer SPL shall be ≥ 85 dB(A) and ≤ 95 dB(A) at 1 m, measured in a free field per [standard, e.g., IEC 60268-22].
```

```{req} Watchdog for hard reset
:id: NFR_004
:status: approved
:component: gateway

The gateway shall have a watchdog to perform a hard RESET in case of issues detected in SW.
```

```{req} Pairing and unpairing supported
:id: NFR_005
:status: approved
:component: gateway

The gateway shall allow pairing and unpairing of sensors.
```

```{req} User config via button + LEDs
:id: NFR_006
:status: approved
:component: gateway

End-user configuration shall be performed via a push button and three LEDs.
```

```{req} Stable sensor list in NVM
:id: NFR_007
:status: approved
:component: gateway

The gateway shall maintain the list of paired sensors in non-volatile memory (NVM) so that pairings are preserved across power loss and resets (other than factory RESET, see FR_026).
```

```{req} Persist sensor status in NVM
:id: NFR_008
:status: approved
:component: gateway

The gateway shall persist, in NVM, for each paired sensor, at least: unique identifier, last reported open/close state, and last reported battery status, so that operation resumes consistently after power loss.
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

- Behaviour upon loss of communication with a paired sensor (e.g., RF link timeout, sensor failure) is not yet specified. To be defined in a future revision.

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
**RF:** Radio Frequency communication.  
**RoHS / REACH:** EU environmental compliance directives.  
**Sensor:** Wireless device detecting window/door events.  
**Watchdog:** Timer that resets system if program fails.
