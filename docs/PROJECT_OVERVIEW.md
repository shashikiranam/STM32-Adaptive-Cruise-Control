# Project Overview

## 1. Purpose

The STM32 Adaptive Cruise Control (ACC) and Collision Avoidance System is a benchtop, portfolio
and learning-oriented reference implementation of the sensing, control, and actuation patterns
found in real Advanced Driver Assistance Systems (ADAS). It demonstrates how a resource
constrained microcontroller (the STM32G071RB, a Cortex-M0+ part with 128 KB flash / 36 KB RAM)
can implement a real-time, safety-conscious control loop using only the vendor HAL — no RTOS,
no dynamic memory allocation, and a fully deterministic execution model.

## 2. Problem Statement

A simple DC-motor-driven "vehicle analog" needs to:

1. Maintain an operator-selected cruise speed (via potentiometer) when the path ahead is clear.
2. Detect an obstacle ahead using ultrasonic ranging.
3. Proportionally reduce speed as the obstacle gets closer (adaptive following).
4. Apply hard braking and, if necessary, an emergency stop, if the obstacle continues to close.
5. Recover back to cruise once the obstacle clears, without oscillating at the zone boundaries.
6. Fail safe: if the sensor becomes unreliable (timeout, implausible reading), the system must
   default to motor-off rather than continuing to trust bad data.
7. Report its internal state continuously over UART for debugging, evaluation, and demonstration.

## 3. Design Goals

| Goal | Rationale |
|---|---|
| **Determinism** | Fixed 50 ms control loop period, no blocking waits in the hot path, all timing driven by hardware timers — behavior is reproducible and analyzable. |
| **Modularity** | Each hardware concern (ultrasonic, PWM, motor, GPIO, timer) is a self-contained driver module with a narrow header-defined API. The ACC/FSM logic depends only on those APIs, never on raw registers. |
| **Portability** | All peripheral access goes through STM32 HAL calls. Board/pin specifics are isolated in `system_config.h` so the project can be retargeted to another STM32G0/F0/F4 board with a CubeMX pin re-map. |
| **Safety-first bias** | Every failure mode (sensor timeout, implausible echo, ADC fault) defaults to "motor off," never to "assume last known good value and keep driving." |
| **Observability** | A structured UART telemetry frame is emitted every control cycle so behavior can be logged, plotted, and reviewed without a debugger attached. |
| **Educational clarity** | Code favors explicit, well-commented logic over cleverness; the FSM states map 1:1 to the states discussed in `ALGORITHM.md`. |

## 4. Non-Goals

This project explicitly does **not** attempt to:

- Achieve automotive functional-safety certification (ISO 26262) — see the Disclaimer in the
  root README.
- Model realistic vehicle dynamics (this is a single DC motor, not a vehicle with mass/inertia
  simulation).
- Provide sensor fusion, computer vision, or radar-based sensing (ultrasonic-only, single sensor).
- Provide a production-grade UART protocol (the telemetry format is human-readable text, not a
  binary/versioned protocol).

## 5. Requirements

### 5.1 Functional Requirements

| ID | Requirement |
|---|---|
| FR-1 | System shall measure obstacle distance using an HC-SR04 ultrasonic sensor at ≥ 15 Hz. |
| FR-2 | System shall read a potentiometer-based cruise speed setpoint via ADC. |
| FR-3 | System shall drive a 12V DC motor via PWM through an L298N H-bridge. |
| FR-4 | System shall implement the five-zone FSM described in `ALGORITHM.md` (Cruise, Slow-Down, Brake, Emergency-Stop, Fault). |
| FR-5 | System shall apply hysteresis at every zone boundary to prevent state chatter. |
| FR-6 | System shall indicate current zone/state via three status LEDs. |
| FR-7 | System shall emit a UART telemetry frame once per control cycle at 115200 baud. |
| FR-8 | System shall force motor output to 0% duty within one control cycle of entering Emergency-Stop or Fault. |

### 5.2 Non-Functional Requirements

| ID | Requirement |
|---|---|
| NFR-1 | Control loop period shall be fixed at 50 ms ± 2 ms. |
| NFR-2 | No dynamic memory allocation (`malloc`/`free`) anywhere in firmware. |
| NFR-3 | No busy-wait polling longer than the HC-SR04 echo timeout (30 ms) shall block the main loop. |
| NFR-4 | All magic numbers (pins, thresholds, timing) shall be centralized in `system_config.h`. |
| NFR-5 | Firmware shall build cleanly under STM32CubeIDE with `-Wall -Wextra` and produce zero warnings. |

## 6. Target Audience

- Embedded systems students learning HAL-based STM32 development
- Portfolio reviewers (recruiters/engineering managers) evaluating applied embedded C skills
- Hobbyists building a benchtop ADAS demo for a robotics or automotive club

## 7. Assumptions and Constraints

- Single-obstacle scenario: the ultrasonic sensor's ~15° cone is assumed to see one obstacle at a
  time; multi-object discrimination is out of scope.
- Ambient temperature affects the speed of sound and thus HC-SR04 accuracy; no temperature
  compensation is implemented (documented as a limitation in `ALGORITHM.md`).
- The 12V DC motor is treated as an actuator analog for "vehicle speed," not a real drivetrain.
