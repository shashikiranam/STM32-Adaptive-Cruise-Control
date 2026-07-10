# Bill of Materials (BOM)

| # | Component | Specification | Qty | Notes |
|---|---|---|---|---|
| 1 | STM32 NUCLEO-G071RB | STM32G071RBT6, Cortex-M0+, 64 MHz, 128KB Flash, 36KB RAM | 1 | Onboard ST-LINK/V2-1 |
| 2 | HC-SR04 Ultrasonic Sensor | Range 2–400 cm, 5V logic, Trig/Echo interface | 1 | Confirm 5V-tolerant GPIO before direct wiring |
| 3 | L298N Motor Driver Module | Dual H-bridge, up to 2A/channel, 5–35V motor supply | 1 | Onboard 5V regulator (unused in this design) |
| 4 | 12V DC Brushed Motor | Small hobby-scale gear motor, ≤ 2A stall current | 1 | Match to L298N channel current rating |
| 5 | 10 kΩ Linear Potentiometer | Single-turn, 3-pin | 1 | Cruise speed setpoint |
| 6 | LED — Green (5mm) | Standard forward voltage ~2.0V | 1 | Cruise indicator |
| 7 | LED — Yellow (5mm) | Standard forward voltage ~2.1V | 1 | Slow-down indicator |
| 8 | LED — Red (5mm) | Standard forward voltage ~2.0V | 1 | Brake / emergency indicator |
| 9 | Resistor 330Ω, 1/4W | ±5% tolerance | 3 | LED current-limiting |
| 10 | External 12V DC Power Supply | ≥ 2A rated, regulated bench supply or battery pack | 1 | Dedicated motor rail — do not share with MCU 5V/3.3V |
| 11 | Breadboard (full-size) | 830 tie-point | 1 | Prototyping |
| 12 | Jumper Wire Set | M-M, M-F, F-F assortment | 1 set | |
| 13 | USB Micro-B / USB-A Cable | Data-capable (not charge-only) | 1 | ST-LINK programming/debug/UART |
| 14 | (Optional) Logic-level shifter | 5V↔3.3V bidirectional | 1 | Extra safety margin on HC-SR04 Echo line |

## Sourcing Notes

- The NUCLEO-G071RB is available directly from STMicroelectronics distributors (Mouser, DigiKey,
  Farnell) and is one of ST's low-cost ($10–15) Nucleo boards.
- HC-SR04 and L298N modules are widely available from generic electronics suppliers
  (Amazon, AliExpress, SparkFun, Adafruit) — part-to-part tolerance varies, so re-calibrate
  distance thresholds after swapping sensor units.
- Any small 12V brushed DC gear motor rated within the L298N's per-channel current limit (2A)
  works; match motor stall current to your power supply's current capability.

## Estimated Cost (approximate, for planning only)

| Category | Approx. Cost (USD) |
|---|---|
| NUCLEO-G071RB | $13 |
| HC-SR04 | $3 |
| L298N module | $4 |
| 12V DC motor | $6 |
| Potentiometer, LEDs, resistors, breadboard, wires | $8 |
| 12V power supply | $10 |
| **Total** | **≈ $44** |

Prices vary by region/supplier and are provided only as a rough project-planning estimate, not a
quote.
