### Wireless Sensor

Minimum voltage: 1,8 V
Have a deep sleep mode (uA), waking up from ext. interrupt from input pin.
2 digital inputs
1 digital output
1 I2C

[STM8L001J3M3] https://pt.mouser.com/ProductDetail/STMicroelectronics/STM8L001J3M3?qs=W0yvOO0ixfEEZjTt2jCv0Q%3D%3D


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

2 layers PCB.
Box not selected yet.

https://eu.mouser.com/c/enclosures
https://www.takachi-enclosure.com/search/detail

# Support

Mouser Part Number
Mouser Price/Stock
Manufacturer_Name
Manufacturer_Part_Number

## Gateway MCU

4 outputs: 3x LEDs + Buzzer
2 inputs: reed, button
SPI (TDI. TDO, CS, CLK)
Sleep
IRQ
ATtiny406: 20-Pin VQFN

# Storage events
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

# OOK RF Receiver

CC1101

Crystal: [NX3225GA-26.000M-STD-CRG-2, 26MHz], https://pt.mouser.com/ProductDetail/NDK/NX3225GA-26.000M-STD-CRG-2?qs=w%2Fv1CP2dgqrNyFiUoufKQw%3D%3D

# Balun
From CC1101 Datasheet:
- L131 [12 nH ± 5%, 0402]: Murata LQW15xx series (868/915 MHz)
- L132 [18 nH, ± 5%, 0402]: Murata LQW15xx series (868/915 MHz)
- L121 [12 nH ± 5%, 0402]: Murata LQW15xx series (868/915 MHz)
- L122 [18 nH ± 5%, 0402]: Murata LQW15xx series (868/915 MHz)
- L123 [12 nH ± 5%, 0402]: Murata LQW15xx series (868/915 MHz)
- L124 [12 nH ± 5%, 0402]: Murata LQW15xx series (868/915 MHz)
- L125 [3.3 nH ± 5%, 0402]: Murata LQW15xx series (868/915 MHz)

- C131 [1.5 pF ± 0.25 pF, 0402 NP0]: Murata GRM1555C series
- C121 [1.0 pF ± 0.25 pF, 0402 NP0]: Murata GRM1555C series
- C122 [1.5 pF ± 0.25 pF, 0402 NP0]: Murata GRM1555C series
- C124 [100 pF ± 5%, 0402 NP0]: Murata GRM1555C series
- C123 [3.3 pF ± 0.25 pF, 0402 NP0]: Murata GRM1555C series
- C125 [12 pF ± 5%, 0402 NP0]: Murata GRM1555C series
- C126 [47 pF ± 5%, 0402 NP0]: Murata GRM1555C series

# Gateway Antenna
50 ohms monopole:
ANT-868-HESM => https://eu.mouser.com/ProductDetail/TE-Connectivity-Linx-Technologies/ANT-868-HESM?qs=hWgE7mdIu5TTyqPbNERfhg%3D%3D

# Power Supply
Let's see if it will be directly. I need to discuss that.
Directly from battery: MCU and CC1101.

Recommended architecture (simple, longest life)
Run everything straight from the battery rail (VBAT) and avoid a always-on regulator.
VBAT range (2 cells):
• Alkaline: ~3.2 V fresh → ~1.8 V at end
• Ni-MH (LSD types like Eneloop): ~2.8–3.0 V fresh → ~2.0 V at end
Device limits:
• ATtiny406: 1.8–5.5 V → OK across VBAT range
• CC1101: 1.8–3.6 V → OK across VBAT range
Implication: you can skip the LDO/buck-boost (saves quiescent current) and just size Brown-Out Detect (BOD) around ~2.0–2.2 V so the MCU never runs too low.

Parts notes
Reverse polarity: P-MOSFET SOT-23 (e.g., –20…–30 V, RDS(on) ≤ 50 mΩ). Source→VBAT, Drain→SYS_3V, Gate→VBAT via 100 k, plus 100 Ω series from gate to VBAT node (optional) to tame hot-plug.
ESD/TVS: if user-replaceable cells in harsh env, add a small 5–6 V TVS at the battery input.
Sense battery: use the bandgap-vs-VCC ADC trick (zero extra parts) or a gated high-value divider to an ADC pin for better accuracy.

Why not regulate?
LDO: even the best low-Iq LDOs burn some headroom and drop out below ~VOUT+dropout → you throw away tail capacity under 2.2–2.4 V.
Buck-boost: gives constant 3.0–3.3 V, but typical Iq = 10–30 µA (or more), which eats into a ~270 µA average budget for 1-year life. If you ever need it, choose one with ≤1–2 µA Iq and gate it off during deep sleep.
Alkaline vs Ni-MH: what changes?
Alkaline: higher initial voltage and energy at low current; more internal resistance at high pulse loads and cold. Great for your duty-cycled design.
Ni-MH (LSD): more flat discharge and better pulse current; slightly lower starting voltage. They self-discharge (LSD fixes most), so shelf life is shorter than alkaline if device sits unused for many months.
Firmware: keep two threshold tables (WARN/CRITICAL) since the curves differ. Example starting points (tune after measuring):
Alkaline: WARN ~2.5 V, CRIT ~2.2 V
Ni-MH: WARN ~2.3–2.4 V, CRIT ~2.0–2.1 V
Interactions with your buzzer boost
Your 12 V boost for the piezo is on its own EN pin → off = ~0 µA.
During beeps, VBAT droops a bit; keep beep bursts short (e.g., 200 ms) and add 22–47 µF on the boost output to keep tone amplitude stable.

# Polarity Inversion protection

https://www.onsemi.com/download/application-notes/pdf/and90146-d.pdf
https://www.powerelectronicsnews.com/comparing-n-channel-and-p-channel-mosfets-which-is-best-for-your-application/

Best fit:
https://pt.mouser.com/ProductDetail/Infineon-Technologies/IRLML6402TRPBF?qs=9%252BKlkBgLFf0HuZuONx2Ewg%3D%3D

# Low battery detection
Use the MCU
1) Divider-less (zero extra parts) — measure VCC via the internal bandgap
Configure the ADC to use VCC as the ADC reference and select the internal bandgap channel as the input.

Read the ADC value adc_bg; since ADC = 1024 * VBG / VCC, compute:
VCC(mV) ≈ VBG(mV)×1024 / adc_bg

Use the nominal bandgap (e.g., ~1100 mV) or store a calibrated value during production for better accuracy.
Pros: no resistors, zero bleed current.
Cons: accuracy limited by bandgap/ADC tolerance (often ±5–10% uncalibrated). Calibrate once and you’re golden.

2) Higher accuracy — tiny resistor divider to an ADC pin
Use the ADC with internal fixed reference (e.g., ~1.1 V) and feed VBAT through a high-value divider (e.g., 1.0 MΩ : 330 kΩ → ~0.33×).
Add a 100 nF cap from ADC pin to GND for a steady reading.
To eliminate divider bleed in sleep, switch the high leg with a GPIO (set high only while measuring) or a small P-MOSFET.
Pros: better absolute accuracy and temperature stability.
Cons: two resistors (+ optional FET).
Practical thresholds for 2×AA (alkaline or NiMH)
Pick two levels with hysteresis so you don’t flap around the boundary:
WARN: ~2.4–2.5 V (alkaline getting low / NiMH near nominal under load)
CRITICAL: ~2.1–2.2 V (below this many radios/MCUs get flaky)
Also set BOD (Brown-Out Detect) near your minimum safe VCC (e.g., ~2.0–2.2 V) so you never run code at an unsafe voltage. Use BOD-in-sleep off if you need ultra-low standby current and your risk analysis allows it.
Make the reading robust
Measure under (light) load occasionally (e.g., keep CC1101 on for a few ms or blink a small LED) so you see real droop, not just open-circuit voltage.
Average 4–8 samples, discard obvious spikes, and apply hysteresis between WARN↔OK.
Do it rarely to save power (e.g., once every 10–30 minutes, or after wake events).
Temperature matters—if you don’t calibrate, keep a little margin on thresholds.

# Type of Buzzer
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

# Gateway Reed Switch
Same as Wireless Sensor reed switch.

# Battery holder
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

# Type of Push Button
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
 
 # LEDs
 Blue: [WP710A10LVBC_D] https://pt.mouser.com/ProductDetail/Kingbright/WP710A10LVBC-D?qs=6oMev5NRZMF7xED66hMqMg%3D%3D
 Yellow: [TLHY4400-AS12Z] https://pt.mouser.com/ProductDetail/Vishay-Semiconductors/TLHY4400-AS12Z?qs=sGAEpiMZZMvVL5Kk7ZYykSSdL1QvRuGruSF7bleI1Sw%3D
 Red: [TLHR4400-AS12Z] https://pt.mouser.com/ProductDetail/Vishay-Semiconductors/TLHR4400-AS12Z?qs=sGAEpiMZZMvVL5Kk7ZYykSSdL1QvRuGrmGBvu1dktLk%3D


# Requirements
Minimum supply voltage: 1,8 V
Have a deep sleep mode (uA), waking up from ext. Interrupt from input pin.
1 analog input
2 digital inputs
4 digital outputs
1 differential PWM output
1 SPI
1 I2C

[STM8L151K4T6] https://pt.mouser.com/ProductDetail/STMicroelectronics/STM8L151K4T6?qs=H8t2elxe2QfV%252BTwO2SHNjg%3D%3D

# Eurocircuits DRC

