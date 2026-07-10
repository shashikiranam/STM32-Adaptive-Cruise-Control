# STM32-Based Adaptive Cruise Control and Collision Avoidance System

<p align="center">
  <img src="images/banner.png" alt="STM32 Adaptive Cruise Control Banner" width="100%">
</p>

<p align="center">

![Platform](https://img.shields.io/badge/Platform-STM32G071RB-03234B?style=for-the-badge&logo=stmicroelectronics&logoColor=white)
![Language](https://img.shields.io/badge/Language-Embedded_C-blue?style=for-the-badge)
![IDE](https://img.shields.io/badge/IDE-STM32CubeIDE-success?style=for-the-badge)
![Framework](https://img.shields.io/badge/Framework-STM32_HAL-orange?style=for-the-badge)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)
![Version](https://img.shields.io/badge/Version-v1.0.0-blueviolet?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Completed-brightgreen?style=for-the-badge)

</p>

---

# Overview

The STM32-Based Adaptive Cruise Control and Collision Avoidance System is an Embedded Systems project that demonstrates real-time obstacle detection and automatic vehicle speed control using the STM32 NUCLEO-G071RB development board.

The system continuously measures the distance between the vehicle and an obstacle using an HC-SR04 ultrasonic sensor. Based on the measured distance, the STM32 executes an Adaptive Cruise Control (ACC) algorithm that adjusts the PWM duty cycle to control a DC motor through an L298N motor driver.

As the obstacle approaches, the controller gradually reduces the motor speed. If the obstacle enters the emergency zone, the motor is stopped immediately to prevent a collision.

This project demonstrates practical applications of Embedded C programming, STM32 HAL, timer input capture, PWM motor control, ADC interfacing, finite state machines, and automotive embedded system design.

---

# Table of Contents

- Overview
- Features
- Hardware Components
- Software Stack
- System Architecture
- Block Diagram
- Project Workflow
- Hardware Setup
- Components Layout
- Wiring Diagram
- Firmware Flowchart
- ACC Algorithm
- State Machine
- Runtime Results
- Repository Structure
- Getting Started
- Future Improvements
- Skills Demonstrated
- Author
- License

---

# Features

- Adaptive Cruise Control (ACC)
- Collision Avoidance
- Real-Time Obstacle Detection
- Automatic Speed Regulation
- PWM-Based DC Motor Control
- HC-SR04 Ultrasonic Sensor Interface
- STM32 HAL Driver Architecture
- ADC Cruise Speed Adjustment
- Finite State Machine
- UART Debug Output
- Status LED Indication
- Modular Embedded C Firmware

---

# Hardware Components

| Component | Quantity | Purpose |
|-----------|---------:|---------|
| STM32 NUCLEO-G071RB | 1 | Main Controller |
| HC-SR04 Ultrasonic Sensor | 1 | Distance Measurement |
| L298N Motor Driver | 1 | Motor Control |
| 12V DC Motor | 1 | Vehicle Motion Simulation |
| Potentiometer | 1 | Cruise Speed Input |
| LEDs | 3 | Status Indication |
| Breadboard | 1 | Circuit Assembly |
| 12V Power Supply | 1 | Motor Supply |

---

# Software Stack

| Software | Purpose |
|-----------|---------|
| STM32CubeIDE | Firmware Development |
| STM32CubeMX | Peripheral Configuration |
| STM32 HAL | Hardware Abstraction Layer |
| Embedded C | Firmware Development |
| Git | Version Control |
| GitHub | Repository Hosting |
---

# System Architecture

<p align="center">
  <img src="images/architecture/system_architecture.png" alt="System Architecture" width="95%">
</p>

The Adaptive Cruise Control system is organized into four major functional blocks:

- Sensing Layer
- Processing Layer
- Control Layer
- Feedback Layer

The HC-SR04 ultrasonic sensor continuously measures the distance to an obstacle. The STM32 processes this information, executes the Adaptive Cruise Control algorithm, generates the required PWM signal, and controls the DC motor through the L298N motor driver. Status LEDs and UART telemetry provide continuous system feedback.

---

# Block Diagram

<p align="center">
  <img src="images/architecture/block_diagram.png" alt="Block Diagram" width="95%">
</p>

The block diagram illustrates the interaction between all hardware modules used in the project.

### Data Flow

```
Obstacle
    │
    ▼
HC-SR04 Ultrasonic Sensor
    │
    ▼
STM32 NUCLEO-G071RB
    │
    ▼
Adaptive Cruise Control Algorithm
    │
    ▼
PWM Generation
    │
    ▼
L298N Motor Driver
    │
    ▼
12V DC Motor
    │
    ▼
Vehicle Motion
```

---

# Project Workflow

<p align="center">
  <img src="images/architecture/workflow.png" alt="Project Workflow" width="95%">
</p>

The firmware continuously repeats the following operations:

1. Initialize peripherals
2. Measure obstacle distance
3. Read cruise-speed setpoint
4. Execute the Adaptive Cruise Control algorithm
5. Generate PWM duty cycle
6. Drive the DC motor
7. Update LED indicators
8. Send UART debug information
9. Repeat the control loop

---

# Hardware Setup

<p align="center">
  <img src="images/hardware/hardware_setup.png" alt="Hardware Setup" width="95%">
</p>

The prototype hardware consists of the STM32 development board, ultrasonic sensor, motor driver, DC motor, potentiometer, LEDs, and an external power supply.

The STM32 acts as the central controller and manages all sensing, processing, and actuation tasks.

---

# Components Layout

<p align="center">
  <img src="images/hardware/components_layout.png" alt="Components Layout" width="95%">
</p>

The components layout provides a top-level view of the physical arrangement of the hardware used in the prototype.

Major components include:

- STM32 NUCLEO-G071RB
- HC-SR04 Ultrasonic Sensor
- L298N Motor Driver
- 12V DC Motor
- Potentiometer
- LEDs
- Breadboard
- External Power Supply

---

# Wiring Diagram

<p align="center">
  <img src="images/hardware/wiring_diagram.png" alt="Wiring Diagram" width="95%">
</p>

The wiring diagram illustrates all electrical connections between the STM32 development board and the external hardware modules.

### Peripheral Interfaces

| Peripheral | Interface |
|------------|-----------|
| HC-SR04 | GPIO + Timer Input Capture |
| Potentiometer | ADC |
| L298N | PWM + GPIO |
| LEDs | GPIO |
| UART | USART2 |

The wiring follows STM32 HAL peripheral assignments configured using STM32CubeMX.
---

# Firmware Flowchart

<p align="center">
  <img src="images/flowcharts/firmware_flowchart.png" alt="Firmware Flowchart" width="95%">
</p>

The firmware follows a cyclic execution model commonly used in real-time embedded systems.

## Execution Sequence

1. Initialize STM32 peripherals
2. Configure GPIO, Timers, ADC and UART
3. Trigger the HC-SR04 sensor
4. Measure echo pulse width
5. Calculate obstacle distance
6. Read cruise speed from ADC
7. Execute Adaptive Cruise Control logic
8. Generate PWM output
9. Update motor speed
10. Update LEDs
11. Send UART debug information
12. Repeat continuously

---

# Adaptive Cruise Control Algorithm

<p align="center">
  <img src="images/flowcharts/acc_algorithm.png" alt="ACC Algorithm" width="95%">
</p>

The Adaptive Cruise Control (ACC) algorithm continuously monitors the distance to the obstacle and automatically adjusts the motor speed.

## Operating Logic

| Distance | Vehicle Action |
|-----------|----------------|
| > 40 cm | Cruise at selected speed |
| 25–40 cm | Gradually reduce speed |
| 12–25 cm | Heavy braking |
| < 12 cm | Emergency Stop |

The PWM duty cycle decreases proportionally as the obstacle gets closer, ensuring smooth and safe deceleration.

---

# State Machine

<p align="center">
  <img src="images/flowcharts/state_machine.png" alt="State Machine" width="95%">
</p>

The controller operates using a deterministic Finite State Machine (FSM).

## States

| State | Description |
|--------|-------------|
| INIT | Initialize hardware |
| CRUISE | Normal driving |
| SLOW_DOWN | Reduce vehicle speed |
| BRAKE | Aggressive braking |
| EMERGENCY_STOP | Immediate stop |
| RECOVER | Resume normal operation |

The state machine guarantees predictable transitions and improves system reliability.

---

# Runtime Results

The following figures demonstrate the successful implementation of the Adaptive Cruise Control system.

---

## Serial Monitor Output

<p align="center">
  <img src="images/results/serial_monitor.png" alt="Serial Monitor" width="95%">
</p>

The UART terminal displays real-time system information including:

- Obstacle Distance
- PWM Duty Cycle
- Motor Speed
- Controller State
- Warning Messages

---

## Distance vs Speed

<p align="center">
  <img src="images/results/distance_vs_speed.png" alt="Distance vs Speed" width="95%">
</p>

The graph illustrates how the Adaptive Cruise Control algorithm automatically reduces the vehicle speed as the measured obstacle distance decreases.

Key observations:

- Smooth speed reduction
- Stable operation
- Immediate response near the emergency threshold

---

## PWM Output

<p align="center">
  <img src="images/results/pwm_output.png" alt="PWM Output" width="95%">
</p>

The PWM waveform generated by the STM32 timer controls the motor speed through the L298N motor driver.

The duty cycle varies dynamically according to the calculated vehicle speed.

---

## Hardware Testing

<p align="center">
  <img src="images/results/testing_setup.png" alt="Testing Setup" width="95%">
</p>

The complete hardware prototype was tested using:

- STM32 NUCLEO-G071RB
- HC-SR04 Ultrasonic Sensor
- L298N Motor Driver
- 12V DC Motor
- Potentiometer
- External Power Supply

Testing confirmed correct obstacle detection, adaptive speed regulation, PWM generation, and collision avoidance functionality.

---
# Repository Structure

```text
STM32-Adaptive-Cruise-Control/
│
├── .github/
│
├── docs/
│   ├── PROJECT_OVERVIEW.md
│   ├── HARDWARE.md
│   ├── SOFTWARE.md
│   ├── ALGORITHM.md
│   └── TESTING.md
│
├── firmware/
│   ├── Core/
│   ├── Drivers/
│   ├── Inc/
│   ├── Src/
│   └── stm32cubemx/
│
├── hardware/
│
├── images/
│   ├── architecture/
│   ├── flowcharts/
│   ├── hardware/
│   ├── results/
│   ├── banner.png
│   └── social_preview.jpg
│
├── simulations/
│
├── .gitignore
├── CHANGELOG.md
├── CODE_OF_CONDUCT.md
├── CONTRIBUTING.md
├── CITATION.cff
├── LICENSE
└── README.md
```

---

# Getting Started

## Requirements

Install the following software before starting the project:

- STM32CubeIDE
- STM32CubeMX
- STM32 HAL Drivers
- ST-LINK USB Driver
- Git

---

## Clone Repository

```bash
git clone https://github.com/shashikiranam/STM32-Adaptive-Cruise-Control.git
```

Move into the project directory.

```bash
cd STM32-Adaptive-Cruise-Control
```

---

## Build Procedure

1. Open STM32CubeIDE.
2. Import the firmware project.
3. Connect the STM32 NUCLEO-G071RB board.
4. Build the firmware.
5. Flash using the onboard ST-LINK programmer.
6. Open a serial terminal at **115200 baud** to monitor runtime data.

---

# Testing Summary

| Test | Result |
|-------|--------|
| STM32 Boot | ✅ Pass |
| HC-SR04 Sensor | ✅ Pass |
| ADC Reading | ✅ Pass |
| PWM Generation | ✅ Pass |
| Motor Driver | ✅ Pass |
| UART Communication | ✅ Pass |
| Adaptive Cruise Control | ✅ Pass |
| Collision Avoidance | ✅ Pass |

---

# Applications

- Adaptive Cruise Control
- Collision Avoidance
- Automotive Embedded Systems
- Autonomous Vehicle Prototyping
- Embedded Control Systems
- Robotics
- Academic Embedded Systems Projects

---

# Future Improvements

Future versions of this project may include:

- CAN Bus Communication
- CAN FD Support
- FreeRTOS Integration
- PID Speed Control
- Wheel Encoder Feedback
- Camera-Based Obstacle Detection
- Sensor Fusion
- OBD-II Diagnostics
- Bluetooth Monitoring
- Wi-Fi Connectivity
- Cloud Dashboard
- Mobile Application Support
- AUTOSAR-Compatible Software Architecture

---

# Skills Demonstrated

This project demonstrates practical experience in:

## Embedded Systems

- Embedded C
- STM32 HAL
- STM32CubeIDE
- STM32CubeMX
- GPIO
- ADC
- PWM
- Timers
- UART
- Interrupts

## Automotive Systems

- Adaptive Cruise Control
- Collision Avoidance
- Motor Control
- Sensor Interfacing
- Real-Time Embedded Systems
- Automotive Firmware Development

## Software Engineering

- Modular Firmware Design
- Finite State Machines
- Hardware Abstraction Layer
- Technical Documentation
- Git
- GitHub

---

# Documentation

Additional documentation is available inside the **docs** folder.

| Document | Description |
|----------|-------------|
| PROJECT_OVERVIEW.md | Project objectives and overview |
| HARDWARE.md | Hardware description |
| SOFTWARE.md | Firmware architecture |
| ALGORITHM.md | Adaptive Cruise Control algorithm |
| TESTING.md | Validation and testing |

---

# Author

## Shashi Kiran A M

M.Tech – Automotive Electronics

Embedded Systems Engineer

### Technical Skills

- Embedded C
- STM32
- STM32CubeIDE
- STM32CubeMX
- GPIO
- ADC
- PWM
- Timers
- UART
- Automotive Electronics
- Embedded Systems

GitHub

https://github.com/shashikiranam

---

# License

This project is licensed under the MIT License.

Please refer to the **LICENSE** file for complete license information.

---

# Citation

If you use this project for academic or educational purposes, please cite it using the metadata available in **CITATION.cff**.

---

# Acknowledgements

Special thanks to:

- STMicroelectronics
- STM32CubeIDE Development Team
- STM32 HAL Library
- Open Source Embedded Systems Community

This project was developed as an educational and portfolio demonstration of Embedded Systems, Automotive Electronics, and real-time firmware development.

---

<p align="center">

⭐ If you found this project useful, please consider giving it a star on GitHub.

</p>
