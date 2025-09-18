# Exit Guardian — System Architecture Assets

This canvas includes a ready-to-use **README.md** and a set of **PlantUML** diagram files you can copy into your repo. Each diagram is standalone and renderable with PlantUML.

---

## README.md

````markdown
# Exit Guardian — System Architecture

This repository contains the architecture diagrams for the Exit Guardian system, derived directly from the requirements document.

## Contents
- `docs/architecture/diagrams/context.puml` — Context Diagram (C4 L1)
- `docs/architecture/diagrams/container_view.puml` — Container View (C4 L2)
- `docs/architecture/diagrams/system_arch.puml` — System block diagram
- `docs/architecture/diagrams/sensor_arch.puml` — Sensor block diagram
- `docs/architecture/diagrams/gateway_arch.puml` — Gateway block diagram
- `docs/architecture/diagrams/top_level_state_machine.puml` — Top Level State Machine
- `docs/architecture/diagrams/rf_sequence.puml` — RF Sequence
- `docs/architecture/diagrams/state_machine.puml` — Gateway modes & transitions
- `docs/architecture/diagrams/seq_pairing.puml` — Sequence: Pair a new sensor
- `docs/architecture/diagrams/seq_remove.puml` — Sequence: Remove a paired sensor
- `docs/architecture/diagrams/seq_alert.puml` — Sequence: Door-open alert evaluation

## How to Render
1. Install PlantUML (or use VS Code extension *PlantUML*).
2. Ensure Java is available (PlantUML requires it) or use a Docker image.
3. Render any `.puml` file:
   - VS Code: Open file → `Alt+D` (preview) → `Export`.
   - CLI (example):
     ```bash
     java -jar plantuml.jar docs/architecture/*.puml
     ```

## Assumptions Reflected
- RF at 868 MHz EU ISM, short event-driven packets.
- Gateway has wired reed for main door, RF for windows/outside doors.
- UI: Button with patterns (double ≤1s, triple ≤2s, long >5s), LEDs (G/Y/R), and buzzer.
- Power: DC/DC 5 to 3.3 V + 2×AA with seamless switchover.

## Traceability (short)
- FR-5, FR-14–FR-23 → `state_machine.puml`
- FR-10, FR-11, FR-12, FR-24 → `seq_alert.puml`
- FR-14–FR-19 → `seq_pairing.puml`
- FR-21 → `seq_remove.puml`
- SR-1..SR-8 → `system_arch.puml`

````

---

### Notes

* All diagrams are standalone; you can save each block to the indicated path.
* The sequences and state machine align with your FR/NFR numbering and timings.
* Feel free to ask for a consolidated single diagram or exports (PNG/SVG) if needed.
