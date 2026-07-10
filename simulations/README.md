# Simulations

This directory contains simulation resources and reference material for the STM32-Based Adaptive Cruise Control and Collision Avoidance System.

## Objective

The project can be validated using software simulation before deployment on physical hardware.

## Supported Platforms

- Proteus Professional (optional)
- STM32CubeIDE Debugger
- STM32CubeMX
- Serial Monitor (PuTTY / Tera Term)

## Simulation Workflow

1. Configure STM32 peripherals.
2. Build and flash firmware.
3. Open serial monitor.
4. Simulate obstacle movement using the HC-SR04 sensor.
5. Observe PWM changes and motor speed.
6. Verify Adaptive Cruise Control behavior.

## Expected Behaviour

| Distance | System Action |
|-----------|---------------|
| > 60 cm | Maintain cruise speed |
| 25–60 cm | Reduce motor speed gradually |
| 14–25 cm | Slow vehicle significantly |
| ≤ 14 cm | Stop motor immediately |

## Validation Checklist

- GPIO initialization
- Timer configuration
- PWM generation
- Ultrasonic distance measurement
- Motor driver response
- Collision avoidance logic
- Serial monitor output

This folder may also include Proteus projects, waveform captures, or other simulation assets in future updates.
