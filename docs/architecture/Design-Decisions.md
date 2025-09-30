### Wireless Sensor

# Power budgets

MCU: ATtiny 20x/22x (we’ll call it ATtiny220 here)
TX: Silicon Labs Si4012 @ 868.3 MHz (OOK)
Supervisor: TPS3839-L30 (≈2.63 V threshold)
I’ll give you clear assumptions, equations, and a few “what-if” scenarios.
1) Assumptions (conservative but realistic)
Sleep currents (typical):
ATtiny (power-down, BOD on low setting): 0.3 µA
Si4012 (SDN = shutdown): 0.03 µA
TPS3839-L30: 0.15 µA
Subtotal, electronics sleep:
I_sleep,elec ≈ 0.48 µA
Reed bias network (ultra-low-leak topology):
Use 4.7 MΩ pull (instead of 1 MΩ) so the “worst-case DC when closed” is tiny:
I_reed,closed = Vbat / 4.7 MΩ ≈ 0.64 µA @ 3.0 V
Average depends on how often the door is closed. If closed 25% of the time:
I_reed,avg ≈ 0.16 µA
(You can cut this further with 10 MΩ if your input-noise margin is OK.)
Total sleep average:
I_sleep,total ≈ 0.48 + 0.16 = 0.64 µA
TX event (per transmission):
Si4012 TX current at modest PA: ~15 mA (set PA to keep ERP ≤ 25 mW)
On-air per event: ~6 repeats × 40 ms ≈ 0.24 s, plus tiny gaps → round to 0.30 s/event
Charge per event:
Qevent = I×t
Qevent = 15mA x 0.30/3600 h = 0.00125 mAh/event
(If your packet is 40 ms total, it’s 0.00017–0.0005 mAh/event; we’re being conservative.)
Battery:
CR2032 nominal: 220 mAh, but plan on 180–200 mAh usable for coin-cell + RF bursts.

2) Daily consumption
Sleep per day:
Csleep/day=Isleep,total×24 h=0.64 μA×24= 0.015 mAh/day

TX per day (E events/day):
Ctx/day=E×0.00125 mAh
Ctx/day=E×0.00125 mAh

Total per day:
Cday=0.015+0.00125E
Cday=0.015+0.00125E

3) Lifetime vs. event rate (using 200 mAh usable)
Events/day (E)	TX mAh/day	Total mAh/day	Est. life
10	0.0125	0.0275	~19,999 days? → Wait! (see note)
20	0.0250	0.0400	~13.7 years
40	0.0500	0.0650	~8.4 years
80	0.1000	0.1150	~4.8 years
120	0.1500	0.1650	~3.3 years

Note: The table math is straight division (200 mAh / mAh/day). In real life, calendar life and coin-cell ESR growth cap you before the math does. Treat 3–5 years as a practical upper bound for CR2032 + RF, even at low usage.

A more realistic lens:
Moderate use (40 events/day) → ~0.065 mAh/day → ≈ 8 years math, but expect ~3–5 years practical due to aging, temperature, and burst-current limits.
Busy door (80 events/day) → ≈ 4–5 years practical.
Cold environments or frequent long packets reduce this.

4) What actually dominates?
TX events dominate quickly; sleep is only ~0.015 mAh/day.
The reed bias can matter if you use a low value (e.g., 1 MΩ → 3 µA when closed). That’s why we recommend ≥ 4.7 MΩ.

5) How to extend life (knobs that matter)
Shorten packets: e.g., 25–30 ms on-air, 4 repeats instead of 6 (still OK with CRC + counter).
Halves mAh/event.
Lower PA: set Si4012 PA just high enough to meet your range + ERP margin.
Reduce repeats on chatter: send fewer repeats on “close” events if they often come in bursts.
Raise pull resistor: 10 MΩ instead of 4.7 MΩ if noise margin allows.
CR2450: bigger cell improves burst handling and real-world life (ESR and cold).

6) Quick “back-of-envelope” for your product
Let’s pick a realistic door use case:
E = 80 events/day (40 open + 40 close)
Packet on-air = 0.25 s (slightly leaner than our 0.30 s)
I_TX = 12 mA (PA optimized after tuning) → Q_event ≈ 0.00083 mAh
Daily:
C_tx/day ≈ 80 × 0.00083 = 0.066 mAh
C_sleep/day ≈ 0.015 mAh
Total ≈ 0.081 mAh/day → 200 mAh / 0.081 ≈ 2,470 days ≈ 6.8 years
Practical expectation: ~3–5 years on CR2032; 4–7 years on CR2450, depending on temperature and traffic.


## Battery

# CR2032

Voltage: 3.0 V
Capacity: 225 mAh
Constinous drain: 0.2 mA

# CR2054

Voltage: 3.0 V
Capacity: 620 mAh
Constinous drain: 0.2 mA

Short answer: yes—CR2450 makes a lot of sense if you can afford the extra size/height. Here’s the trade-off:
CR2032 vs CR2450 (for your Si4012 + ATtiny + TPS3839 sensor)
Capacity (typ.): CR2032 ≈ 220 mAh, CR2450 ≈ 500 mAh
→ 2.2× energy; practical usable often ~200 mAh vs 380–420 mAh.
Pulse headroom / ESR: CR2450 has lower ESR → less voltage sag on 10–20 mA TX bursts (better range and fewer brownouts, especially cold).
Lifetime impact: Life scales roughly with usable mAh.
Our earlier example (~0.081 mAh/day)
CR2032: ~6–7 years math (realistically 3–5 years due to aging/cold).
CR2450: ~12–13 years math (realistically 5–7+ years; self-discharge ~1–2%/yr becomes the limiter).
Temperature robustness: CR2450 holds up much better near 0 °C (lower ESR → fewer TX dropouts).
Mechanical: CR2450 is bigger & thicker (Ø24 mm × 5.0 mm) vs CR2032 (Ø20 mm × 3.2 mm). Taller holder; more enclosure volume.
RF/mechanics caution: Large metal cell near the antenna can detune it. Keep the cell away from the ANT keep-out or on the opposite edge; re-tune π-network if you change battery placement.
Cost: CR2450 cell + holder costs more, but often worth it for product reliability.
When CR2450 is the better choice
Door with high event rate (≥ ~80 events/day).
Colder installs (garages, unheated areas).
Need longer range (higher PA) or more repeats for reliability.
You want >5 years field life with margin.
When CR2032 is enough
Low/medium traffic doors (≤ ~40 events/day).
Indoor, mild temps.
Tight size/height budget and you can live with 3–5 years practical life.
Practical tips if you choose CR2450
Keep 22–47 µF bulk at Si4012 VDD (still helpful).
Maintain antenna keep-out; prefer battery away from ANT line.
Re-tune the π-match after final battery/enclosure placement.
Keep the TPS3839-L30 (2.63 V) + BOD ~2.4–2.6 V; thresholds still appropriate.
Bottom line: If the enclosure can handle it, CR2450 buys you real robustness (longer life, fewer cold-sag issues). For a commercial product aimed at “install and forget,” I’d choose CR2450 and design the mechanics around it.

# Bulk capacitor of 47 uF near the Si4012

https://pt.mouser.com/ProductDetail/KEMET/C0805C476M9PAC7800?qs=xL%2FyUNPmvLY2BGnZa3AdBg%3D%3D

## Reed Switch

Sensitivity is measured in Ampere-Turns (AT).
Lower AT more sensitive, closes at larger distance.
SPST-NO: normally open.
With a small NdFeB (N35) disc magnet of Ø8–10 mm × 2–3 mm thick (common for door sensors):

# 6 AT reed

- Operate distance (close) ≈ 8–12 mm
- Release distance (open) ≈ 10–15 mm

# 10 AT reed

- Operate distance ≈ 5–8 mm
- Release distance ≈ 7–10 mm
So: 6–10 AT → ~5–12 mm actuation range with a modest NdFeB magnet.

The reed switch shall be SMT.
The AT shall be 8-10.

# Magnet

Size: 8 - 10 mm diammeter, 2-3 mm thickness.
Type: NdFeB

# Interface Reed -> MCU

VBAT ── 1 MΩ ──●──── Reed ── GND        (pull-up + switch to ground)
               │
               └─ 100 kΩ ──► MCU_PIN    (GPIO with interrupt)
                              │
                              └─ 100 nF ── GND   (RC debounce/stretcher)

Why this topology
Ultra-low quiescent: Using an external 1 MΩ pull-up and disabling the MCU’s internal pull-up avoids the ~60 µA you’d burn with the typical 20–50 kΩ internal pull-up when the reed is closed. With 4.7 MΩ at 3 V the worst-case is just 0.64 µA during closure (and ~0 µA when open).
Debounce + pulse stretch: 100 kΩ + 100 nF gives ~10 ms RC, killing contact bounce and ensuring even very quick taps still look like a clean edge to the MCU.
EMI-friendly: The series 100 kΩ limits surge into the MCU pin; the 100 nF to GND shunts HF noise.

# MCU

Reset: unconnected.
Debug, Flash.
Use the UPDI pin (same as RESET). It should be available as well as Vdd, GND in a 3 pin header.

Header part number on Mouser:
https://pt.mouser.com/ProductDetail/Harwin/M20-9770342?qs=Jph8NoUxIfVskiTzAX%252BIFA%3D%3D

# Matching Balun (antenna interface)

Inductors (high-Q (SRF>2GHz)) - Coilcraft 0402HP series:
- [B] LM1: 120nH => https://eu.mouser.com/ProductDetail/Coilcraft/0603HP-R12XGLU?qs=zCSbvcPd3pbJT9Wmlao8cg%3D%3D
- [T] LM2: 3.6nH => https://eu.mouser.com/ProductDetail/Coilcraft/0402HP-3N6XGRW?qs=QNEnbhJQKvZDgcR8AxBsbw%3D%3D
- [T] LM3: 30nH => https://eu.mouser.com/ProductDetail/Coilcraft/0402HP-30NXJRW?qs=QNEnbhJQKvYt2krW1zXeEQ%3D%3D
- [T] LM4: 3.6nH => https://eu.mouser.com/ProductDetail/Coilcraft/0402HP-3N6XGRW?qs=QNEnbhJQKvZDgcR8AxBsbw%3D%3D
- [T] LM5: 6.8nH => https://eu.mouser.com/ProductDetail/Coilcraft/0402HP-6N8XGRW?qs=QNEnbhJQKvb1RBRny2c9yA%3D%3D

Capacitors (NP0/C0G) - Murata GRM155 series:
- [T] CM1: 2.4pF => https://eu.mouser.com/ProductDetail/Murata-Electronics/GRM1555C1H2R4WA01D?qs=ouTquLLW2S5u9sxTRF7zdQ%3D%3D
- [T] CM2: 1.2pF => https://eu.mouser.com/ProductDetail/Murata-Electronics/GRM1555C1H1R2WA01D?qs=ouTquLLW2S4ZARlph37hBQ%3D%3D
- [B] CM3: 2.4pF => https://eu.mouser.com/ProductDetail/Murata-Electronics/GRM1555C1H2R4WA01D?qs=ouTquLLW2S5u9sxTRF7zdQ%3D%3D
- [T] CM5: 5.1pF => https://eu.mouser.com/ProductDetail/Murata-Electronics/GRM1555C1H5R1WA01D?qs=ouTquLLW2S6Ac9DkjU6j%2Fw%3D%3D
- [T] CM6: 5.1pF => https://eu.mouser.com/ProductDetail/Murata-Electronics/GRM1555C1H5R1WA01D?qs=ouTquLLW2S6Ac9DkjU6j%2Fw%3D%3D
- [T] CC1: 68pF => https://eu.mouser.com/ProductDetail/Murata-Electronics/GRM1555C1H680FA01J?qs=e%252BE4OD6MgMf0uVE7NpHJPw%3D%3D

# PI-match:

[T] Lseries = 15 nH => https://eu.mouser.com/ProductDetail/Coilcraft/0402HP-15NXJRW?qs=QNEnbhJQKvYIHcbZYhY5ig%3D%3D: Currently a shunt.
[T] Csh = 0pF
[T] Csh = 0pF

# Sensor Antenna

50 ohms monopole:
ANT-868-HESM => https://eu.mouser.com/ProductDetail/TE-Connectivity-Linx-Technologies/ANT-868-HESM?qs=hWgE7mdIu5TTyqPbNERfhg%3D%3D

# Box/Case: PCB size and format

2 layers or 4 layers PCB?
2 Layers.

https://eu.mouser.com/c/enclosures
https://www.takachi-enclosure.com/search/detail

# Support

Mouser Part Number
Mouser Price/Stock
Manufacturer_Name
Manufacturer_Part_Number

# Eurocircuits DRC

## Gateway MCU

4 outputs: 3x LEDs + Buzzer
2 inputs: reed, button
SPI (TDI. TDO, CS, CLK)
Sleep
IRQ
ATtiny406: 20-Pin VQFN

# OOK RF Receiver

CC1101

# Balun

# Gateway Antenna

# Power and Power Switch

# Low battery detection??

## Buzzer

## Gateway Reed Switch
