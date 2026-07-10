# Firmware

This folder contains the STM32CubeIDE project for the Adaptive Cruise Control and Collision
Avoidance System.

## Structure

```
firmware/
├── STM32CubeMX/
│   ├── Project.ioc        ← CubeMX project file (pin config, clock tree, peripherals)
│   └── README.md
├── Drivers/                ← STM32G0xx HAL + CMSIS sources (regenerated via CubeMX, not tracked)
│   └── README.md
└── Core/
    ├── Inc/                ← Header files for every module
    └── Src/                ← Source files for every module
```

## Module Map

| Module | Header | Source | Responsibility |
|---|---|---|---|
| Central config | `Inc/system_config.h` | — | Every pin/timing/threshold constant |
| Application entry | `Inc/main.h` | `Src/main.c` | HAL/clock/peripheral init, main control loop |
| GPIO driver | `Inc/gpio.h` | `Src/gpio.c` | LEDs, ultrasonic trigger, motor direction pins |
| Timer driver | `Inc/timer.h` | `Src/timer.c` | TIM2 input-capture echo timing |
| PWM driver | `Inc/pwm.h` | `Src/pwm.c` | TIM3-based motor speed PWM |
| Ultrasonic driver | `Inc/ultrasonic.h` | `Src/ultrasonic.c` | HC-SR04 ranging |
| Motor driver | `Inc/motor.h` | `Src/motor.c` | L298N speed + direction composition |
| ACC application core | `Inc/acc.h` | `Src/acc.c` | FSM, control law, telemetry |

See [`../docs/SOFTWARE.md`](../docs/SOFTWARE.md) for the full architecture writeup and build
instructions, and [`../docs/ALGORITHM.md`](../docs/ALGORITHM.md) for the control theory behind
`acc.c`.

## Quick Build

```bash
# 1. Open firmware/STM32CubeMX/Project.ioc in STM32CubeMX, click "Generate Code"
#    (populates Drivers/ with STM32G0xx HAL + CMSIS)
# 2. Open STM32CubeIDE -> File -> Open Projects from File System -> select firmware/
# 3. Project -> Build Project
# 4. Connect NUCLEO-G071RB via USB, Run -> Debug (or Run -> Run)
```
