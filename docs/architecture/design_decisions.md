# Design Decisions

## System Frequency
| Feature | 433 MHz | 868 MHz | Winner |
| --- | --- | --- | --- |
| Indoor Penetration | Slightly better | Good | 433 MHz (Marginal) |
| Channel Congestion | High (High interference) | Low (Regulated duty cycles) | 868 MHz |
| Antenna Form Factor | Large (~17.3 cm) | Small (~8.6 cm) | 868 MHz |
| Battery Efficiency | Lower (More retries/collisions) | Higher (Clean, fast bursts) | 868 MHz |
| Security & Features | Generally basic | Advanced | 868 MHz |

Decision: 868 MHz

## How to Eliminate or Mitigate Collision Risks
To ensure a "window open" event is never lost, low-power wireless systems use specific firmware and protocol strategies:
1. Redundant Transmissions (The Simplest Fix)

Instead of transmitting the "Open" status just once, the sensor is programmed to transmit the same packet 3 to 5 times in rapid succession whenever an event occurs.

    To prevent subsequent packets from colliding again, a randomized delay (e.g., a random jitter between 10 to 50 milliseconds) is inserted between each retry.

    If Packet 1 collides, Packet 2 or 3 will almost certainly get through cleanly. This is the standard approach for simple, unidirectional (transmit-only) sensors.

2. Clear Channel Assessment (CCA) / Listen Before Talk (LBT)

If your transceivers support it (like the CC1101), the sensor can be configured to check the RF channel before it transmits.

    The sensor wakes up, switches to receive mode for a fraction of a millisecond, and measures the RSSI (Received Signal Strength Indicator).

    If the channel is busy (another sensor is transmitting), it backs off, waits a random number of milliseconds, and checks again. It only transmits when the air is clear.

3. Acknowledggments (ACK) and Retries

If you are building a bidirectional system (where the gateway can talk back to the sensor):

    The sensor sends the "Window Open" packet.

    It waits a brief moment for an ACK packet from the gateway.

    If it doesn't receive the ACK within a few milliseconds (indicating a collision or poor range), the sensor assumes the message was lost, waits a random back-off time, and retransmits.

    It repeats this until it receives confirmation or hits a retry limit.

    Note on Battery: While bidirectional ACKs are the most reliable, they consume more battery power because the sensor must keep its receiver on to listen for the ACK. For strict battery-optimized window sensors, the Redundant Transmissions with Random Jitter approach is usually preferred for its balance of high reliability and low power draw.

Decision: on hold. Lets see if this is really needed with the user tests.

## Sensor

### Sensor MCU

Powered by a coin cell, like CR2032 or CR2054
Minimum voltage supply: 1,8 V
Have a deep sleep mode (uA), waking up from external interrupt from input pin.
2 x digital inputs
1 x digital output
1 x SPI
1 x ADC
1 x UART (not needed for series)
Min. Flash size: 8 KB
Programming interface: 4 wires: VDD, DATA, GND, RESET
Should be as cheaper as possible among all MCUs of the same category.

Decision:
- [STM8L051F3P6] https://eu.mouser.com/ProductDetail/STMicroelectronics/STM8L051F3P6?qs=gNDSiZmRJS%252BCAYYxupi9Bg%3D%3D

#### Sensor Sleep Modes
| Mode                     | Typical Current               | What Stays Running                                   | Wake-up Sources                                 |
| ------------------------ | ----------------------------- | ---------------------------------------------------- | ----------------------------------------------- |
| **Wait**                 | Depends on active peripherals | Peripherals can continue running, CPU stopped        | Interrupts or Reset                             |
| **Low-Power Run (LPR)**  | ~5.1 µA                       | CPU running from low-speed clock, execution from RAM | Software exit or Reset                          |
| **Low-Power Wait (LPW)** | ~3 µA                         | Low-power peripherals active, CPU stopped            | Peripheral events, I/O events, DMA, Reset       |
| **Active-Halt**          | ~1.3 µA                       | RTC remains active                                   | RTC interrupt, external interrupt, Reset        |
| **Halt**                 | ~350 nA                       | RAM retained, almost everything else off             | External interrupt, selected peripherals, Reset |

GPIOs and RAM are preserved in halt mode.

#### Sensor Unused pins (current consumption)
Set them as digital outputs and set the value to low state.
PC0 and PC1 (I2C), connect them to GND because they are pure open drain.
PB3 => output low
PC0 => output low
PC1 => output low
PC4 => output low
PD0 => output low

#### Used pins
PA0 => SWIM
PA1 => NRST
PA2 => reed
PA3 => btn
PB0 => LED
PB1 => GDO
PB2 => ADC1_IN16
PB4 => SPI1_NSS
PB5 => SPI1_SCK
PB6 => SPI1_MOSI
PB7 => SPI1_MISO
PC5 => USART_TX
PC6 => USART_RX

### Sensor Battery

#### Comparison: CR2032 and CR2054

Voltage: 3.0 V
Capacity: 225 mAh
Constinous drain: 0.2 mA

| Feature | CR2032 | CR2054 | Winner for Your Project |
| --- | --- | --- | --- |
| Capacity | ~220 mAh | ~1000 mAh | CR2054 (Overkill) |
| Your Target Life (2–3 years) | Easily Achieved (~7+ years practical) | Easily Achieved (Limited by shelf-life) | Tie |
| Price | Extremely Cheap (cents) | Expensive | CR2032 |
| Global Availability | Supermarkets, hardware stores, bulk vendors | Specialized battery distributors only | CR2032 |
| Thickness | 3.2 mm | 5.4 mm | CR2032 |

Self discharging rate is 1% to 2% of its capacity. It has a shelf life of 8 to 10 years.

Caution with the quiescient current in a circuit. A continous parasitc drain of 30 uA will completely empty the battery in less than a year, completely overshadowing its natural shelf life.

The sensor circuit must ensure almost 0 quiescient current.

Decision:
- CR2032

### Sensor Power budgets
I_MCU_halt = 0.350 uA
I_CC1101_sleep = 0.2 uA
I_MCU_active = 3 mA
I_CC1101_active = 15 mA

I_sleep = 0.550 uA
I_active = 18 mA

T_day = 86400 s
Max_events = 32
Min_events = 4

Packet Size=8 bytes (Preamble)+2 bytes (Sync)+5 bytes (Data)+2 bytes (CRC)=17 bytes
Total Bits (B)=17 bytes×8 bits/byte=136 bits

R = 1.2 kbps (worst case scenario)
T_tx = 136 bits / 1200 bps = 113.33 ms
Let's consider 300 ms per event.

LED of 2 mA ON during 300 ms

T_min_active_per_day = 4 x 300 ms = 1200 ms = 1.2 s
T_max_active_per_day = 32 x 300 ms = 9600 ms = 9.6 s

I_min_avg = (0.55 uA x (86400-1.2) + ((18 mA + 2 mA) x 1.2) ) / 86400 s = 0.83 uA / day
I_max_avg = (0.55 uA x (86400-9.6) + ((18 mA + 2 mA) x 9.6) ) / 86400 s = 2.77 uA / day

Capacity: 200 ~ 220 mAh

Lets consider 200 mAh.
Hours = 200 mAh / 2.77 uA = 72202 hours = 3008 days = 8.2 years, much more than the battery self discharge.


### Sensor Bulk capacitor near the CC1101

Keep the CR2032, but place a low-ESR decoupling capacitor buffer in parallel with the battery holder.A $22\ \mu\text{F}$ to $47\ \mu\text{F}$ tantalum or high-quality ceramic capacitor placed physically close to the CC1101 and MCU VDD pins will act as an energy reservoir.When transmitting, the current pulse will be drawn from the capacitor, preventing the battery voltage from dipping dangerously low. Between transmissions (during the hours of deep sleep), the battery will slowly and safely recharge the capacitor.

A CR2032 coin cell has a high internal resistance (Rbat​) that increases as it discharges (starting around 10 Ω to 30 Ω when fresh, and spiking up to 100 Ω to 300 Ω near the end of its life). When the CC1101 draws 15 mA (or up to 18 mA combined with the MCU), this internal resistance causes a severe voltage drop. If the voltage drops below the MCU's Minimum Operating Voltage (Vmin​), the system will reset.

Use MLCC (Multi-Layer Ceramic Capacitors) with an ESR of less than 100 mΩ.
Low-leakage ceramic capacitors (X7R or X5R).
1 x 100 uF (6.3 V) or 2 x 47 uF capacitor.
100 uF capacitor near Vcc and GND of CC1101.
A 100 nF capacitor (Multi-Layer Ceramic Capacitor (MLCC) with an X7R dielectric material) near MCU and near CC1101.

Decision:
 - 100uF, 10 V, ceramic, multilayer:
 - https://pt.mouser.com/ProductDetail/Murata-Electronics/GRM31CR61A107MEA8L?qs=hd1VzrDQEGidurXiDTJOCg%3D%3D

### Sensor Reed Switch

Sensitivity is measured in Ampere-Turns (AT).
Lower AT more sensitive, closes at larger distance.
SPST-NO: normally open.
With a small NdFeB (N35) disc magnet of Ø8–10 mm × 2–3 mm thick (common for door sensors):

#### 6 AT reed

- Operate distance (close) ≈ 8–12 mm
- Release distance (open) ≈ 10–15 mm

#### 10 AT reed

- Operate distance ≈ 5–8 mm
- Release distance ≈ 7–10 mm
So: 6–10 AT → ~5–12 mm actuation range with a modest NdFeB magnet.

The reed switch shall be SMT.
The AT shall be 8-10.

### Sensor Magnet

Size: 8 - 10 mm diammeter, 2-3 mm thickness.
Type: NdFeB

### Sensor Interface Reed -> MCU and btn -> MCU

VBAT ── 1 MΩ ──●──── Reed ── GND        (pull-up + switch to ground)
               │
               └─ 100 kΩ ──► MCU_PIN    (GPIO with interrupt)
                              │
                              └─ 100 nF ── GND   (RC debounce/stretcher)

Why this topology
Ultra-low quiescent: Using an external 1 MΩ pull-up and disabling the MCU’s internal pull-up avoids the ~60 µA you’d burn with the typical 20–50 kΩ internal pull-up when the reed is closed. With 4.7 MΩ at 3 V the worst-case is just 0.64 µA during closure (and ~0 µA when open).
Debounce + pulse stretch: 100 kΩ + 100 nF gives ~10 ms RC, killing contact bounce and ensuring even very quick taps still look like a clean edge to the MCU.
EMI-friendly: The series 100 kΩ limits surge into the MCU pin; the 100 nF to GND shunts HF noise.

Decision:
 - https://pt.mouser.com/ProductDetail/MEDER-electronic/MK33-66-D?qs=olJun0bQHM8sthm3NhvY2g%3D%3D

### Sensor Matching Balun (antenna interface)

Inductors (high-Q (SRF>2GHz)) - Coilcraft 0402HP series:
- L121 [12 nH ± 5%, 0402]: Murata LQW15xx series (868/915 MHz)
- L122 [18 nH ± 5%, 0402]: Murata LQW15xx series (868/915 MHz)
- L123 [12 nH ± 5%, 0402]: Murata LQW15xx series (868/915 MHz)
- L124 [12 nH ± 5%, 0402]: Murata LQW15xx series (868/915 MHz)
- L125 [3.3 nH ± 5%, 0402]: Murata LQW15xx series (868/915 MHz) 
- L131 [12 nH ± 5%, 0402]: Murata LQW15xx series (868/915 MHz)
- L132 [18 nH ± 5%, 0402]: Murata LQW15xx series (868/915 MHz)

Capacitors (NP0/C0G) - Murata GRM1555C series:
- C121 [1.0 pF ± 0.25 pF, 0402 NP0]: Murata GRM1555C series
- C122 [1.5 pF ± 0.25 pF, 0402 NP0]: Murata GRM1555C series
- C123 [3.3 pF ± 0.25 pF, 0402 NP0]: Murata GRM1555C series
- C124 [100 pF ± 5%, 0402 NP0]: Murata GRM1555C series
- C125 [12 pF ± 5%, 0402 NP0]: Murata GRM1555C series
- C126 [47 pF ± 5%, 0402 NP0]: Murata GRM1555C series
- C131 [1.5 pF ± 0.25 pF, 0402 NP0]: Murata GRM1555C series

### Sensor PI-match:

Decision:
 - No pi-match circuit, the antenna should be connected immediately after the last capacitor.

### Sensor Antenna

1. Biggest size, more expensive and less issues probability: whip antenna.
2. Medium size, less expensive than 1 and low issues probability: helical antenna.
3. Small size, less expensive than 1 medium issues probability: chip antenna.
4. Small size, the least expensive, high probability of issues: PCB antenna.
    https://pt.mouser.com/new/johanson/johanson-0900at43a0070/?srsltid=AfmBOoobHdIGgGZuSxxAr1JtTYSkDjUd4MoxHm1Rq9-d-15OsjQQ_TCT

Decision:
    3 and 4 are too risky.
    1 or 2 to be decided, based on performance of 868 MHz modules from Amazon.

### Sensor battery holder
https://pt.mouser.com/ProductDetail/TE-Connectivity-Linx-Technologies/BAT-HLD-003-SMT?qs=TuK3vfAjtkVRZQIT6eTqjQ%3D%3D
https://pt.mouser.com/ProductDetail/TE-Connectivity-Linx-Technologies/BAT-HLD-013-SMT-TR?qs=4ASt3YYao0UKhYWrBts7tw%3D%3D

Decision: because it impacts the box, and the box is also impacted by antenna, 1 or 2, to be decided with the box.

### Sensor Enclosure/Box/Case: PCB size and format

#### Companies
Takachi
OKW
Bud Industries
Polycase

##### Takachi (comes with technical datasheets, 3D model)
1. CS90-W 90 (with battery holder): https://www.takachi-enclosure.com/products/CS
- External: 75 x 35 x 12 mm
- Internal: 25.4 x 26.6 x 5.3 mm (space for PCB only)

2. SIC3-5-2W (without battery holder, no screws): https://www.takachi-enclosure.com/products/SIC
- External: 54 x 25 x 18 mm
- Internal: 42.6 x 20.1 x 11.1 mm

##### Hammond Manufacturing (comes with technical datasheets, 3D model)
1. PP85B (4 screws): https://supertronic.com/en/universal-plastic-enclosure-for-electronic-devices-pp85b.html#/8-color-white_ral_9010
- External: 39 x 27 x 18 mm
- Internal: 31 x 20 x 10 mm

2. PP85C (4 screws): https://supertronic.com/en/universal-plastic-enclosure-for-electronic-devices-pp85c.html#/8-color-white_ral_9010
- External: 45 x 31 x 20 mm
- Internal: 40 x 26 x 12 mm

## Gateway

### Gateway MCU

Minimum voltage supply: 1,8 V
Have a deep sleep mode (uA), waking up from external interrupt from input pin.
1 analog input
2 digital inputs
4 digital outputs
1 differential PWM output
1 SPI
1 I2C

[STM8L151K4T6] https://pt.mouser.com/ProductDetail/STMicroelectronics/STM8L151K4T6?qs=H8t2elxe2QfV%252BTwO2SHNjg%3D%3D

### Gateway Storage Events

If you update on every open/close, use external I²C FRAM.
ATtiny406’s EEPROM (~100k cycles) will wear out fast if you flip bits for many windows over a year.
FRAM: [MB85RC64TAPNF-G-BDERE1] https://pt.mouser.com/ProductDetail/Ramxeed/MB85RC64TAPNF-G-BDERE1?qs=sGAEpiMZZMuIiYGg9i1FDKlM%252Bqda5guug2rKlLpEIh9%2FUl1rXCJRlg%3D%3D

What to keep in NVM (FRAM recommended)

Header: magic, format version, counts, pointers (for journal), CRC.
Registry (fixed table): one record per sensor you’ve paired.
id (e.g., EV1527 24-bit or your own 32-bit)
type (window, door, etc.)
flags (paired, muted, low-bat last seen…)
rssi_avg (8-bit)
last_seen (u32, seconds)
state (1 bit: open/close, plus tamper bit if you use it)
Status bitmap (fast read): 1 bit per sensor mirroring state for instant boot/UI.
Append-only journal (events): tiny records you append on every open/close (and optionally low-battery/tamper). Used to reconstruct after a crash, and for history if you want it.
Snapshot block: a periodic copy of the status bitmap + per-sensor last_seen so you don’t have to replay a huge journal on boot.
Concrete sizes (example)
Assume up to 128 sensors.
Registry record (16 bytes):
u32 id | u8 type | u8 flags | u8 rssi | u8 state_ver | u32 last_seen | u16 reserved | u16 crc
Registry: 16 B × 128 = 2 KB
Status bitmap: ceil(128/8) = 16 B
Snapshot (bitmap + compact last_seen deltas + CRC): ≈ 256–512 B typical

Journal record (6–8 bytes):
e.g., u16 seq | u7 sensor_idx | u1 new_state | u8 evt | u16 dt_seconds | u8 crc
Keep a few KB rolling window (e.g., 8–16 KB) and prune on each snapshot.
Even a tiny 32 KB FRAM (MB85RC256V) is plenty for this.
Write pattern (battery-friendly)
On each event (open/close/low-bat): update RAM mirror → append one journal record (6–8 B).
Periodically (e.g., every 15–30 min or every 64 events): write a snapshot and advance the journal head.
On boot: load latest snapshot → replay journal → done (fast).

Why this works well
FRAM → effectively unlimited endurance and instant writes (no erase), perfect for frequent state flips.
Crash-safe → append-only + CRC; you never corrupt the registry if power dies mid-write.
Fast boot → status bitmap gives instant “which windows are open,” then you can lazily read per-sensor details.
If you must start without FRAM
Internal EEPROM is OK for a small fleet (say ≤16–20 sensors) if you batch writes (e.g., every 10–30 minutes) and implement a simple ring/wear-level per record. Still, FRAM is strongly preferred for per-event updates.

### Gateway GSFK RF Receiver

CC1101

Crystal: [NX3225GA-26.000M-STD-CRG-2, 26MHz], https://pt.mouser.com/ProductDetail/NDK/NX3225GA-26.000M-STD-CRG-2?qs=w%2Fv1CP2dgqrNyFiUoufKQw%3D%3D

### Gateway Balun

From CC1101 Datasheet:

- L131 [27 nH ± 5%, 0402]: Murata LQG15HS series (315/433 MHz)
- L121 [27 nH ± 5%, 0402]: Murata LQG15HS series (315/433 MHz)
- L122 [22 nH ± 5%, 0402]: Murata LQG15HS series (315/433 MHz)
- L123 [27 nH ± 5%, 0402]: Murata LQG15HS series (315/433 MHz)

- C131 [3.9 pF ± 0.25 pF, 0402 NP0]: Murata GRM1555C series
- C124 [220 pF ± 5%, 0402 NP0]: Murata GRM1555C series
- C121 [3.9 pF ± 0.25 pF, 0402 NP0]: Murata GRM1555C series
- C122 [8.2 pF ± 0.25 pF, 0402 NP0]: Murata GRM1555C series
- C123 [5.6 pF ± 0.25 pF, 0402 NP0]: Murata GRM1555C series
- C125 [220 pF ± 5%, 0402 NP0]: Murata GRM1555C series

### Gateway Antenna

t.b.d.

### Gateway Power Supply

From 5V DC power supply. With a 230 V AC - 5 V DC converter.

### Gateway Type of Buzzer

Piezo or magnetic buzzer?
Piezo: low current, but not so loud.
Magnetic buzzer: high current, but loud enough.
With some technics we can get a piezo lounder.
However, I found a louder one, and it's piezo, but needs driver circuit and boots from 3 V to 12 V:
https://pt.mouser.com/ProductDetail/Murata-Electronics/PKMCS1818E20A0-R1?qs=2m8Gdae5Lr0i0Y6WwcK3JA%3D%3D
We need 4 KHz PWM, which is easy with the MCU. A driver is needed.

Topology (overview)
Boost to 12 V (only during beep)
Use a tiny inductor boost with EN pin → 12 V.
Good fit: LT1615 (up to 34 V out, shutdown ≈0.5 µA). 
Analog Devices
+1
Dual CMOS driver @ 12 V
Use a dual MOSFET gate driver (built to drive capacitive loads) powered from the 12 V rail. Feed it a PWM + its inverse. Connect the two outputs straight to the piezo’s two pins → bridge drive = ~24 Vpp from a 12 V rail.
Good fit: TC4427A (12 V supply ok, TTL-compatible inputs). 
Mouser Electronics
+1
Transducer
Murata PKMCS1818E20A0-R1 (rated 100 dB typ @ 12 V_o-p, 2 kHz, 10 cm). 
Mouser Electronics
+1
Schematic sketch (blocks)
2×AA → ATtiny406 (3 V domain)
Boost (LT1615) VIN=3 V → VOUT=12 V
L = 10 µH (e.g., 2–3 A sat SMD inductor),
D = Schottky 30–40 V (SOD-123),
COUT = 2.2–4.7 µF (25 V X5R/X7R),
EN pin from Tiny406 GPIO (active only during beep).
TC4427A VDD=12 V, GND=0 V
IN-A = PWM from Tiny406 (3 V logic)
IN-B = inverted PWM (from Tiny or via XOR/not in code)
OUT-A → Piezo pin 1
OUT-B → Piezo pin 2
Optional 33–68 Ω series resistors at each OUT for EMI softening
1 MΩ across piezo pins (bleeder to discharge after beep)
Murata PKMCS1818E20A0-R1 mounted with a front sound port/vent.
This gives ~+/-12 V on the piezo (swapping ends each half-cycle), i.e., ~24 Vpp, which matches Murata’s SPL test condition and is noticeably louder than any 3 V drive. 
Mouser Electronics
Control sequence (Tiny406)
Set PWM frequency near the buzzer’s loud spot (start at ~2.0–2.5 kHz; Murata’s sheet uses 2 kHz; you can sweep 1.8–3.5 kHz to peak in your enclosure). 
Enable boost (EN=1); wait ~2–5 ms for 12 V to rise.
Enable TC4427A drive (start PWM + inverted PWM).
Beep pattern (e.g., 200 ms on / 200 ms off × N).
Stop PWM, EN=0 on boost. (The 1 MΩ bleeds the piezo.)
The LT1615’s shutdown current is sub-µA, so idle battery impact is negligible; all power is spent only during beep.

Boost: LT1615ES5-1#TRMPBF: https://pt.mouser.com/ProductDetail/Analog-Devices/LT1615ES5-1TRMPBF?qs=ytflclh7QUUp0a%252Bd7RWj%2FQ%3D%3D
Dual MOSFET: MIC4427YM: https://pt.mouser.com/ProductDetail/Microchip-Technology/MIC4427YM?qs=kh6iOki%2FeLEkjJUQ4zBy0A%3D%3D

Boost:
Cin: [0805, 10uF, 6.3V, X7R] [CGA4J1X7R0J106K125AC] https://pt.mouser.com/ProductDetail/TDK/CGA4J1X7R0J106K125AC?qs=NRhsANhppD%2FK4ZRLurir6Q%3D%3D
Cout: [1210, 4.7uF, 25V, X7S] [MCJCT32MAB7475KPPA01] https://pt.mouser.com/ProductDetail/TAIYO-YUDEN/MCJCT32MAB7475KPPA01?qs=tlsG%2FOw5FFjfKFWRiQhUjA%3D%3D
Schottky Diode: [0.5A, 20V] [MBR0520LT3G] https://pt.mouser.com/ProductDetail/onsemi/MBR0520LT3G?qs=3JMERSakebrHITezQJBmeQ%3D%3D

Feedback Resistors:
[0402]
R1 = 1.15M
R2 = 130K
Vout = 12.11 V

L = 10uH (calculated => 11.2uH) [CD43NP-100MC] https://pt.mouser.com/ProductDetail/Sumida/CD43NP-100MC?qs=ttApTud31Jaza%252BoSAZtv6w%3D%3D

### Gateway Reed Switch

Same as Wireless Sensor reed switch.

### Gateway Battery Holder

https://pt.mouser.com/ProductDetail/Keystone-Electronics/2462?qs=sGAEpiMZZMvxqoKe%252BDjhrte%252BlrLaYtz%2F4XqS9Isgiuc%3D

Electrical notes:
Reverse polarity protection: Add a P-FET ideal diode or a low-Vf Schottky between the holder and 3 V rail.
Fuse: Optional resettable PTC (~250–500 mA hold) for abuse tolerance.
VBAT sensing: If you want to measure battery level, place a divider (or use ATtiny band-gap trick) on the holder’s positive before the protection diode.

PCB & Layout tips
Footprint: Use the manufacturer drawing — Keystone publishes CAD/STEP and PCB land pattern. (KiCad libraries also have it under Battery_Holder:BatteryHolder_Keystone_2462_2xAA).
Keep-out: Leave at least 5 mm clearance between battery edge and any RF keep-out zone/antenna region. Batteries are large conductive bodies → can detune your 868 MHz antenna.
Silkscreen: Mark “+ / –” clearly at both ends; helps avoid reversed insertion during assembly/testing.
Support: If the device is handheld or shaken, add side walls or bosses in enclosure to prevent lateral stress on solder joints.
Access: Place the holder so cells can be dropped in/out without colliding with tall components or enclosure lips.

### Gateway Type of Push Button

VCC (≈3.0 V)
  |
  Rpull-up 680 kΩ … 1 MΩ   ← ultra-low standby current
  |
  +-----> MCU pin (ATtiny406 GPIO with Schmitt input)
  |         |
  |        Cdebounce 10–47 nF
  |         |
 Button     Rseries 100–220 Ω (ESD/EMI damp)
  |         |
 GND       ESD diode (if user-exposed)  → to GND

 B3F1070
 https://pt.mouser.com/ProductDetail/Omron-Electronics/B3F-1070?qs=CX134%252BdLMDEbZOltqAbCng%3D%3D
 
### Gateway LEDs
 Blue: [WP710A10LVBC_D] https://pt.mouser.com/ProductDetail/Kingbright/WP710A10LVBC-D?qs=6oMev5NRZMF7xED66hMqMg%3D%3D
 Yellow: [TLHY4400-AS12Z] https://pt.mouser.com/ProductDetail/Vishay-Semiconductors/TLHY4400-AS12Z?qs=sGAEpiMZZMvVL5Kk7ZYykSSdL1QvRuGruSF7bleI1Sw%3D
 Red: [TLHR4400-AS12Z] https://pt.mouser.com/ProductDetail/Vishay-Semiconductors/TLHR4400-AS12Z?qs=sGAEpiMZZMvVL5Kk7ZYykSSdL1QvRuGrmGBvu1dktLk%3D

### Gateway 4 Pin Programming Header:
t.b.d.

### Gateway Enclosure/Box/Case: PCB size and format

2 layers PCB.
Electronic internal minimum enclosure: ?(D) x ?(W) x ?(H) mm

#### Takachi

SIC products catalog: https://www.takachi-enclosure.com/products/SIC

Option 1 (without battery holder):
SIC6-12-4W: 120 (D) x 60 (W) x 38 (H)

## Support

### KiCad BOM support

Mouser Part Number
Supplier Link
Manufacturer
Manufacturer Part Number



# Design Decision Document: Low-Power Wireless Window Sensor Node (WSN-01)

**Document ID:** DDD-2026-001  
**Status:** PROPOSED  
**Author:** Embedded Systems Engineering Team  
**Date:** May 30, 2026  
**Target Project:** Proprietary Low-Power Home Security System  

---

## 1. Executive Summary
This document establishes the hardware and firmware architectural design decisions for the low-power wireless window sensor nodes (**WSN-01**). The core objective is to design a highly cost-efficient, resilient, and ultra-low-power peripheral capable of detecting open/closed states via a magnetic reed switch and transmitting telemetry to a central alert gateway. The target operational lifespan is a minimum of **5 years** on a single CR2032 coin-cell battery. 

Based on rigorous constraint analysis, we have selected the **STM8S003F3** 8-bit microcontroller paired with the **TI CC1101** sub-1GHz RF transceiver operating in the **868 MHz ISM band**.

---

## 2. Context & Problem Statement
The home security platform requires non-obtrusive perimeter monitoring. Window nodes are highly resource-constrained devices deployed in large quantities per installation. The technical challenges include:
* **Energy Constraints:** Minimal active-state current draw; deep sleep states must dominate the power profile.
* **RF Penetration:** Residential walls attenuate 2.4 GHz (Zigbee/BLE) severely. Sub-1GHz frequencies are required for reliable building penetration.
* **BOM Cost:** To scale commercially, the total Bill of Materials (BOM) per sensor node must remain under $2.50 USD at volume.

---

## 3. Design Requirements

### 3.1 Functional Requirements
* **State Detection:** Reliable detection of window transition states (Open to Closed, Closed to Open) using a physical reed switch with debouncing hardware/software.
* **RF Transmission:** Secure packet delivery to the central gateway within a 50-meter indoor radius.
* **Keep-Alive Telemetry:** Heartbeat transmission every 60 minutes to report battery voltage and supervisory status.

### 3.2 Non-Functional Requirements
* **Power Consumption:** Average standby current $< 10\ \mu\text{A}$. Peak transmission current $< 35\ \text{mA}$.
* **Battery Life:** $\ge 5$ years operating on a standard **CR2032** battery (nominal capacity: 220 mAh).
* **Form Factor:** Compact PCB envelope fitting inside a slimline 3D-printed enclosure ($45\text{mm} \times 20\text{mm} \times 12\text{mm}$).

---

## 4. Proposed Architecture & Solution

### 4.1 Hardware Architecture
The chosen platform isolates computational tasks to an ultra-low-cost MCU and offloads RF modulation to a flexible transceiver.

* **Microcontroller:** STMicroelectronics **STM8S003F3** (8-bit, 16 MHz, 8 KB Flash, 1 KB RAM, TSSOP20). 
  * *Justification:* Offers low-power Active-Halt modes ($4.5\ \mu\text{A}$) and explicit external interrupt wakeups on GPIO pins at a fraction of the cost of 32-bit alternatives.
* **RF Transceiver:** Texas Instruments **CC1101** (Sub-1GHz flexible transceiver).
  * *Justification:* Exceptional power management (WOR - Wake-on-Radio), programmable data rates, and robust performance in the **868 MHz** European ISM band.
* **Sensor Mechanism:** Normally-Open (NO) Reed Switch pulled up via a high-value resistor ($1\ \text{M}\Omega$) to mitigate leakage during closed states, routed to a hardware debouncing low-pass filter ($R=10\ \text{k}\Omega, C=100\ \text{nF}$) feeding into an MCU External Interrupt (AWU/EXTI) pin.

### 4.2 Firmware Architecture & Power Management Strategy
The firmware operates on a strict **Event-Driven Asynchronous Topology**. The system remains in **Active-Halt mode** indefinitely until awakened by one of two specific hardware events:

1. **External GPIO Interrupt (EXTI):** Triggered by the reed switch changing state.
2. **Auto-Wakeup Unit (AWU) Timer:** Triggered every 60 minutes for the supervisory heartbeat.

```
+--------------------------------------------------------+
|                                                        |
|                      ACTIVE-HALT                       |
|          (MCU Deep Sleep, CC1101 SLEEP Mode)           |
|                Standby Current: ~6.5 uA                |
|                                                        |
+---------------------------+----------------------------+
                            |
            +---------------+---------------+
            |                               |
    [ Reed Switch EXTI ]             [ AWU Timer ISR ]
            |                               |
            +---------------+---------------+
                            |
                            v
+--------------------------------------------------------+
|                      WAKEUP & MCU                      |
|                     INITIALIZATION                     |
+---------------------------+----------------------------+
                            |
                            v
+--------------------------------------------------------+
|                  SPI COMM & TX POWER                   |
|           Configure CC1101 -> Burst Transmit           |
|                 Peak Current: ~30 mA                   |
+---------------------------+----------------------------+
                            |
                            v
+--------------------------------------------------------+
|                   CC1101 STROBE SLEEP                  |
|          Return MCU to Active-Halt Immediately         |
+--------------------------------------------------------+
```

#### Power Optimization Code Strategy (STM8S Pseudocode)
```c
void main(void) {
    // Disable peripheral clocks not in use to shave off microamps
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1, DISABLE);
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC, DISABLE);
    
    // Initialize GPIO for Reed Switch with External Interrupt
    GPIO_Init(GPIOC, GPIO_PIN_3, GPIO_MODE_IN_FL_IT); 
    EXTI_SetRegisterPinSensitivity(EXTI_PORT_GPIOC, EXTI_SENSITIVITY_RISE_FALL);
    
    // Initialize CC1101 into Power Down / SLEEP state
    CC1101_PowerDown();
    
    enableInterrupts();
    
    while(1) {
        // Drop into deep low-power mode
        halt(); 
    }
}

// Reed Switch Interrupt Service Routine
INTERRUPT_HANDLER(EXTI_PORTC_IRQHandler, 5) {
    // 1. Quick software debounce verification
    // 2. Wake up CC1101 via SPI
    // 3. Transmit state packet (Open/Closed + Battery Voltage)
    // 4. Force CC1101 back to SPWD (Sleep)
}
```

---

## 5. Alternative Options Evaluated

| Attribute | Chosen Vector: STM8S + CC1101 (868MHz) | Option B: ESP32-C3 (2.4GHz Wi-Fi/BLE) | Option C: Attiny85 + RFM69HCW (433MHz) |
| :--- | :--- | :--- | :--- |
| **BOM Cost** | **Ultra-Low (~$1.10 combined)** | Moderate (~$1.80) | High (~$3.10 due to legacy IC pricing) |
| **Deep Sleep Current**| **~6.5 $\mu$A** | ~130 $\mu$A (Too high for CR2032) | ~7.0 $\mu$A |
| **RF Penetration** | **Excellent (868 MHz sub-1GHz)** | Poor (2.4 GHz attenuation) | Excellent (433 MHz) |
| **Assembly Footprint**| **Compact (TSSOP20 + QFN20)** | Medium (Module format) | Large (DIP/SOIC legacy sizes) |
| **Verdict** | **SELECTED** | **REJECTED:** Power profile completely unviable for coin-cell longevity. | **REJECTED:** Total component cost unviable for commercial scale. |

---

## 6. Risks, Trade-offs, & Mitigations

### 6.1 Packet Collision & Data Loss (Unidirectional Link)
* **Risk:** The node utilizes a simplex/unidirectional burst transmission to save power, meaning it does not listen for a gateway acknowledgment (ACK). If two sensors fire simultaneously, data packets collide and are lost.
* **Mitigation:** Implement a redundant firing algorithm. For every state change event, the node transmits the packet **3 times** sequentially, separated by a pseudo-random delay interval calculated via an internal LFSR (Linear Feedback Shift Register) seed value ($T_{\text{delay}} = 20\text{ms} + \text{rand}(0, 15)\text{ms}$).

### 6.2 Contact Bounce on Reed Switch
* **Risk:** Mechanical reed switches vibrate upon closing, generating false multi-trigger interrupts that cycle the transmitter rapidly, killing the battery.
* **Mitigation:** A dual-layer mitigation framework is deployed: a physical RC hardware low-pass filter stage on the PCB, combined with an algorithmic 15ms lockout timer within the MCU interrupt service routine.

### 6.3 Battery Voltage Sag under TX Load
* **Risk:** Internal resistance of a aging CR2032 cell increases dramatically. The $\sim30\text{mA}$ current burst during CC1101 transmission can cause a transient voltage dip below the MCU brown-out reset (BOR) threshold.
* **Mitigation:** Place a low-ESR **$100\ \mu\text{F}$ Tantalum decoupling capacitor** in close parallel alignment with the battery terminal traces to supply transient instantaneous peak currents.
