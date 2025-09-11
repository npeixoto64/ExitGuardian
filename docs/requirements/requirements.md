# Project Requirements

## 1. Introduction

- **Project Name:**  Exit Guardian
- **Version:**  v1.0.0
- **Author(s):**  Nuno Peixoto
- **Date:**  11/09/2025
- **Purpose:** A device placed on the main exit door that triggers an alert with a buzzer when the main exit door is open if any window or outside door are open. When the main door is closed the alert is shutoff always.

---

## 2. Overall System Requirements

High-level needs and constraints for the overall system.

- **SR-1:** The system shall be composed by 1 or more wireless sensors and a gateway.
- **SR-2:** The wireless sensors shall send to the gateway the status of the window, on an open or close event.
- **SR-3:** The wireless sensors shall be powered by a replaceable battery.
- **SR-4:** The gateway shall have two power supply options: replaceable battery or AC/DC connected to the main power socket, ~230V AC.
- **SR-5:** The gateway shall have a button to allow a hard factory reset add pair/unpair wireless sensors.
- **SR-6:** The gateway shall have three LEDs, green, yellow and red to indicate its own status.
- **SR-6:** The gateway red LED is always ON in case of low battery condition, and flashing in case of an alert condition.
- **SR-6:** The gateway yellow LED is related with sensors, add, remove sensors, or no sensors being monitored condition.
- **SR-6:** The gateway green LED flashing is an heart beat, indicating the gateway is working properly, it is alive.

---

## 3. System Functional Requirements

Detailed functions the system must perform.

- **FR-1:** When an open or close window event the sensor shall send a data packet to the gateway through RF.
- **FR-2:** The sensor data packet shall include the battery status and the event type: open or close.
- **FR-3:** Each sensor shall have an unique ID.
- **FR-13:** The gateway shall indicate it is operating properly having a green LED flashing for 100 ms every two seconds.
- **FR-6:** The gateway shall have a buzzer, a green LED, and yellow LED and a red LED.
- **FR-7:** The gateway shall have a push button for user interaction.
- **FR-7:** The gateway shall have have 3 different modes: waiting for configuration, listening for new sensors, monitoring.
- **FR-7:** The gateway shall be in waiting for configuration mode when there is no sensors attached to it.
- **FR-7:** When the gateway is in waiting for configuration mode the yellow LED shall be always ON.
- **FR-7:** After a factory RESET event, the gateway shall be in waiting for configuration mode.
- **FR-7:** After adding at least one sensor to the gateway, the gateway shall switch to normal operation mode, with the yellow LED OFF.
- **FR-4:** When the gateway is in monitoring mode, it shall detect open and close events of the main door.
- **FR-5:** When the gateway is in monitoring mode, on a main door open event, the gateway shall trigger an alert, while the door is open, and only if any window is open.
- **FR-8:** When the gateway is in monitoring mode, if the main door is closed the gateway shall be suspend the alert, indpendently of windows modes.
- **FR-10:** When the gateway is in monitoring mode, if the main door is open and the alert is being triggered, if all windows are closed the gateway shall suspend the alert.
- **FR-11:** The alert signal shall be a continous buzzer and the red LED flashing every 200 ms, 50%.
- **FR-12:** The gateway shall indicate a low battery condition with the red LED always ON, only when battery powered.
- **FR-13:** When the gateway push button is double pressed in 1 second time frame the gateway shall enter in listening for new sensors mode.
- **FR-13:** When the gateway is in listening for new sensors mode, it shall wait 20 seconds for a new sensor packet to be sent, the yellow LED shall be flashing every 500 ms, 50%, and the buzzer shall be biping every second.
- **FR-13:** When in listening for new sensors mode, if no new sensor signal received, in 20 seconds time frame, the listening for new sensors mode finishes and the yellow LED backs to the previous mode and the buzzer shuts OFF.
- **FR-13:** When in listening for new sensors mode, if a signal of a new sensor ID is sent the gateway shall add it to the list of monitored sensors, the yellow LED is turned OFF and the buzzer sounds continously for 3 seconds.
- **FR-13:** When in listening for new sensors mode, if an already known sensor from the list sends a signal the gateway shall update its status and continue listening for new sensors mode until the 20 seconds time expires.
- **FR-13:** When the gateway is in listening for new sensors mode, placing the battery or changing the status of a sensor (open/close event) is enough to add it to the gateway.
- **FR-13:** When a sensor has low battery the gateway shall indicate it flashing the yellow LED syncronized with the green LED.
- **FR-13:** When the gateway push button is pressed and released in 1 seconde time frame the yellow LED shall blink the number of times corresponding to the position of the sensor in the list of sensors being monitored.
- **FR-13:** The first sensor being added is the first one in te list and so on.
- **FR-13:** When the gateway push button is pressed for more than 5 seconds, all LEDs starts flashing, when the the push button is released a factory RESET occurs.
- **FR-13:** After a factory RESET a gateway shall no sensors attached to it.
- **NFR-6 (Usability):** The gateway shall save in NvM the status of each sensor.

---

## 4. System Non-Functional Requirements

Qualities of the system, not directly tied to functions.

- **NFR-1 (Performance):** The gateway shall indicate trigger an alert in, at least, 500 ms, after the main door is open.
- **NFR-2 (Reliability):** The sensor battery life shall be, at least, 3 years.
- **NFR-3 (Reliability):** The gateway battery life shall be, at least, 1 year.
- **NFR-4 (Reliability):** The sensor shall be able to send the data packet, at least, 3 times.
- **NFR-3 (Reliability):** The gateway battery shall be able to work for more 7 days after low battery alert.
- **NFR-3 (Reliability):** The gateway shall have a watchdog to perform an hard RESET in case of issues.
- **NFR-5 (Maintainability):** The sensor shall not have any MCU neither firmmware.
- **NFR-5 (Maintainability):** The gateway shall allow pairing and unpairing sensors.
- **NFR-5 (Maintainability):** The gateway shall have, no more than 2 AA batteries.
- **NFR-5 (Maintainability):** If the gateway has batteries and is being supplied by power mains, in case of power failure it shall switch to battery mode.
- **NFR-6 (Usability):** End-user configuration shall be via a push button and three LEDs.
- **NFR-6 (Usability):** End-user shall add numbers to each sensor after adding it to the gateway, to be able to know which one needs battery replacement.

---

## 5. System Assumptions & Constraints

- Security is not a critical factor.
- If the user wants to remove a sensor, he should perform a factory reset and add all sensors again.
- The sensor doesn't have any interface with the user besides the battery insertion action.
- Gateway may be powered by mains (option A) or batteries (option B).

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
