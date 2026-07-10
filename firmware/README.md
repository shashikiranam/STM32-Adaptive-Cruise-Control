# Firmware

## Overview

This directory contains the reference firmware implementation for the STM32-Based Adaptive Cruise Control and Collision Avoidance System.

The firmware is organized into modular components responsible for sensor interfacing, motor control, Adaptive Cruise Control (ACC) logic, GPIO configuration, timer configuration, and PWM generation.

## Firmware Modules

| Module | Description |
|---------|-------------|
| main | System initialization and main execution loop |
| ultrasonic | Distance measurement using HC-SR04 |
| motor | DC motor control using L298N |
| pwm | PWM generation for speed control |
| acc | Adaptive Cruise Control decision logic |
| gpio | GPIO initialization |
| timer | Timer configuration |
| system_config | Global configuration parameters |

## Development Environment

- STM32CubeIDE
- STM32CubeMX
- STM32 HAL Library
- Embedded C

## Directory Structure

```text
Core/
├── Inc/
└── Src/

Drivers/
STM32CubeMX/
```
