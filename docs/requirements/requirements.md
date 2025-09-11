# Project Requirements

## 1. Introduction

- **Project Name:**  Exit Guardian
- **Version:**  v1.0.0
- **Author(s):**  Nuno Peixoto
- **Date:**  11/09/2025
- **Purpose:** A device installed on the main exit door that triggers an alert with a buzzer when the main exit door is open and any window or outside door is also open. When the main door is closed, the alert is always turned off. The device is intended for use in residential and commercial buildings.

---

## 2. Overall System Requirements

High-level needs and constraints for the overall system.

- **SR-1:** The system shall be composed of one or more wireless sensors and a gateway.
- **SR-2:** The wireless sensors shall send the status of the window to the gateway upon an open or close event.
- **SR-3:** The wireless sensors shall be powered by a replaceable battery.
- **SR-4:** The gateway shall support both power supply options: replaceable battery and AC/DC connection to the main power socket (230V AC). Both options must be available simultaneously, and the system shall automatically switch to battery power in case of main power failure.
- **SR-5:** The gateway shall have a buzzer.
- **SR-6:** The gateway shall have a button to allow a hard factory reset and to pair/unpair wireless sensors.
- **SR-7:** The gateway shall have three LEDs—green, yellow, and red—to indicate its status.
- **SR-8:** The gateway's red LED shall always be ON in case of a low battery condition.
- **SR-9:** The gateway's red LED shall flash in case of an alert condition.
- **SR-10:** The gateway's yellow LED shall indicate when sensors are being added.
- **SR-11:** The gateway's yellow LED shall indicate when no sensors are being monitored.
- **SR-12:** The gateway's yellow LED shall indicate when one or more sensors have low battery.
- **SR-13:** The gateway's green LED shall flash as a heartbeat, indicating the gateway is working properly and is operational.

---

## 3. System Functional Requirements

Detailed functions the system must perform.

- **FR-1:** When a window is opened or closed, the sensor shall send a data packet to the gateway via RF.
- **FR-2:** The sensor data packet shall include the battery status and the event type (open or close).
- **FR-3:** Each sensor shall have a unique identifier (ID).
- **FR-4:** The gateway shall indicate it is operating properly by flashing the green LED for 100 ms every two seconds.

- **FR-5:** The gateway shall support three modes of operation: waiting for configuration, listening for new sensors, and monitoring.

- **FR-6:** The gateway shall enter waiting-for-configuration mode when there are no sensors attached.

- **FR-7:** In waiting-for-configuration mode, the yellow LED shall remain ON continuously.

- **FR-8:** After a factory RESET event, the gateway shall enter waiting-for-configuration mode.

- **FR-9:** When at least one sensor is added to the gateway, it shall switch to normal operation mode, and the yellow LED shall turn OFF.

- **FR-10:** In monitoring mode, the gateway shall detect open and close events of the main door.

- **FR-11:** In monitoring mode, if the main door is opened and any window is also open, the gateway shall trigger an alert while the door remains open.

- **FR-12:** In monitoring mode, if the main door is closed, the gateway shall suspend any active alert, regardless of the window states.

- **FR-13:** In monitoring mode, if the main door is open and the alert is active, the gateway shall suspend the alert if all windows are closed.

- **FR-14:** The alert signal shall consist of a continuous buzzer and the red LED flashing every 200 ms with a 50% duty cycle.

- **FR-15:** The gateway shall indicate a low battery condition by keeping the red LED ON continuously, but only when battery powered.

- **FR-16:** If the gateway push button is double-pressed within a 1-second time frame, the gateway shall enter listening-for-new-sensors mode.

- **FR-17:** In listening-for-new-sensors mode, the gateway shall wait up to 20 seconds for a new sensor packet. During this period, the yellow LED shall flash every 500 ms with a 50% duty cycle, and the buzzer shall beep every second.

- **FR-18:** If no new sensor signal is received within 20 seconds in listening-for-new-sensors mode, the gateway shall exit this mode, return the yellow LED to its previous state, and turn the buzzer OFF.

- **FR-19:** If a signal from a new sensor ID is received in listening-for-new-sensors mode, the gateway shall add it to the monitored sensors list, turn the yellow LED OFF, and sound the buzzer continuously for 3 seconds.

- **FR-20:** If a known sensor from the monitored list sends a signal in listening-for-new-sensors mode, the gateway shall update its status and continue listening until the 20-second period expires.

- **FR-21:** In listening-for-new-sensors mode, inserting the battery or changing the status of a sensor (open/close event) shall be sufficient to add it to the gateway.

- **FR-22:** If a sensor has a low battery, the gateway shall indicate this by flashing the yellow LED synchronized with the green LED.

- **FR-23:** If the gateway push button is pressed and released within a 1-second time frame, the yellow LED shall blink the number of times corresponding to the position of the sensor with low battery in the monitored sensors list. If there are no sensors with low battery, the yellow LED shall not blink.

- **FR-24:** The first sensor added to the gateway shall be assigned the first position in the monitored sensors list, the second sensor the second position, and so on.

- **FR-25:** If the gateway push button is pressed for more than 5 seconds, all LEDs shall start flashing; when the push button is released, a factory RESET shall occur.

- **FR-26:** After a factory RESET, the gateway shall have no sensors attached.

---

## 4. System Non-Functional Requirements

Qualities of the system, not directly tied to functions.

- **NFR-1 (Performance):** The gateway shall trigger an alert within 500 ms after the main door is opened.
- **NFR-2 (Reliability):** The sensor battery life shall be at least 3 years.
- **NFR-3 (Reliability):** The gateway battery life shall be at least 1 year.
- **NFR-4 (Reliability):** The sensor shall be able to send the data packet at least 3 times per open/close event.
- **NFR-5 (Reliability):** The gateway battery shall be able to operate for at least 7 more days after a low battery alert.
- **NFR-6 (Reliability):** The gateway shall have a watchdog to perform a hard RESET in case of issues.
- **NFR-7 (Maintainability):** The sensor shall not have any MCU or firmware. (Note: This is a strong constraint; ensure feasibility for your use case.)
- **NFR-8 (Maintainability):** The gateway shall allow pairing and unpairing of sensors.
- **NFR-9 (Maintainability):** The gateway shall use no more than two AA batteries.
- **NFR-10 (Maintainability):** If the gateway has batteries and is being supplied by mains power, it shall switch to battery mode in case of a power failure. The switch shall be seamless, with no interruption to system operation.
- **NFR-11 (Usability):** End-user configuration shall be performed via a push button and three LEDs.
- **NFR-12 (Usability):** The end-user shall assign numbers to each sensor after adding it to the gateway, to identify which one needs battery replacement.
- **NFR-13 (Usability):** The gateway shall save the status of each sensor in non-volatile memory (NvM).

---

## 5. System Assumptions & Constraints

- Security is not a critical factor. No tamper detection or advanced security features are included.
- If the user wants to remove a sensor, the user should perform a factory reset and add all sensors again.
- The sensor does not have any user interface other than battery insertion.
- The gateway may be powered by mains (option A) or batteries (option B).
- The system shall operate within standard indoor environmental conditions (0–40°C, 10–90% RH, non-condensing).

---

## 6. Legal & Regulatory Requirements

- **LR-1 (Safety):** Device shall comply with CE marking requirements (Low Voltage Directive, EMC Directive).
- **LR-2 (Radio):** RF module shall operate only on frequencies allowed in the EU (e.g., 868 MHz ISM band, duty cycle limits).
- **LR-3 (Environment):** Materials must comply with RoHS / REACH.
- **LR-4 (Battery):** Product must comply with EU Battery Directive for labeling and recycling.

---

## 7. Open Issues

- None.

---

## 8. Traceability

(If needed later: map requirements → design → implementation → tests.)

---
