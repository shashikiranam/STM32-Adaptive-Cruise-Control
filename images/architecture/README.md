# Architecture Diagrams

This directory contains the high-level architecture diagrams used throughout the STM32 Adaptive Cruise Control and Collision Avoidance System project.

These diagrams explain the overall system design, firmware architecture, hardware organization, and project workflow.

---

# Contents

| File | Description |
|------|-------------|
| block_diagram.png | Overall hardware block diagram of the Adaptive Cruise Control system |
| system_architecture.png | Complete system architecture showing sensors, STM32, motor driver, DC motor, LCD, and power flow |
| software_architecture.png | Layered firmware architecture illustrating the Embedded C software stack |
| project_workflow.png | Complete project execution workflow from obstacle detection to motor control |

---

# Block Diagram

The block diagram illustrates the major hardware components and their interactions.

It includes:

- STM32 NUCLEO-G071RB
- HC-SR04 Ultrasonic Sensor
- PWM Motor Driver
- DC Motor
- LCD Display
- Status LEDs
- Power Supply

---

# System Architecture

The system architecture diagram explains the overall Adaptive Cruise Control implementation.

The STM32 continuously reads obstacle distance from the HC-SR04 ultrasonic sensor, processes the information using the Embedded C application, adjusts the PWM duty cycle, controls the motor speed through the L298N motor driver, updates the LCD display, and activates visual warnings using LEDs.

---

# Software Architecture

The software architecture illustrates the layered firmware implementation.

Software layers include:

- Adaptive Cruise Control Application
- Distance Processing
- PWM Controller
- Ultrasonic Driver
- LCD Driver
- GPIO Driver
- STM32 HAL
- CMSIS
- STM32 Hardware

The modular architecture simplifies maintenance and future expansion.

---

# Project Workflow

The workflow diagram summarizes the complete execution sequence.

```
Obstacle Detection
        │
        ▼
Distance Measurement
        │
        ▼
STM32 Processing
        │
        ▼
Adaptive Cruise Control Algorithm
        │
        ▼
PWM Generation
        │
        ▼
Motor Speed Control
        │
        ▼
LCD Update
        │
        ▼
Warning LEDs
```

---

# Design Goals

- Modular Firmware
- Real-Time Performance
- Low Latency
- Expandable Architecture
- Automotive-Oriented Design
- Educational Documentation

---

# Future Improvements

- CAN Communication
- FreeRTOS Integration
- Multiple Distance Sensors
- Camera-Based Detection
- AI Obstacle Prediction
- Adaptive Speed Profiles
- Automotive Gateway ECU Integration

---

These architecture diagrams provide a visual overview of the complete STM32 Adaptive Cruise Control and Collision Avoidance System and complement the detailed explanations provided in the project documentation.
