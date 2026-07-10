# Pin Connections / Signal Map

All pin assignments below are also defined symbolically in
`firmware/Core/Inc/system_config.h` — this table is the human-readable source of truth that the
header comments reference.

## 1. HC-SR04 Ultrasonic Sensor

| HC-SR04 Pin | NUCLEO-G071RB Pin | Function | Notes |
|---|---|---|---|
| VCC | 5V | Power | From NUCLEO 5V rail (CN6/CN10) |
| GND | GND | Ground | Common ground |
| Trig | PA1 | GPIO Output | `system_config.h: US_TRIG_GPIO_PORT/PIN` |
| Echo | PA0 | TIM2_CH1 (Input Capture) | `system_config.h: US_ECHO_GPIO_PORT/PIN`, AF2 |

## 2. L298N Motor Driver

| L298N Pin | NUCLEO-G071RB Pin | Function | Notes |
|---|---|---|---|
| ENA | PA6 | TIM3_CH1 (PWM Output) | `system_config.h: PWM_TIM_CHANNEL`, AF1 |
| IN1 | PB0 | GPIO Output | `system_config.h: MOTOR_IN1_GPIO_PORT/PIN` |
| IN2 | PB1 | GPIO Output | `system_config.h: MOTOR_IN2_GPIO_PORT/PIN` |
| OUT1 | Motor Terminal 1 | — | |
| OUT2 | Motor Terminal 2 | — | |
| 12V | External 12V Supply (+) | Power | Dedicated motor rail |
| GND | External 12V Supply (−) AND NUCLEO GND | Power/Ground | **Must share ground with MCU** |
| 5V OUT | Not connected | — | Onboard regulator output unused in this design |

## 3. Potentiometer

| Potentiometer Pin | NUCLEO-G071RB Pin | Function | Notes |
|---|---|---|---|
| Terminal 1 | 3.3V | Power | |
| Wiper | PA4 | ADC1_IN4 | `system_config.h: ADC_POT_CHANNEL` |
| Terminal 2 | GND | Ground | |

## 4. Status LEDs

| LED | NUCLEO-G071RB Pin | Function | Series Resistor |
|---|---|---|---|
| Green (Cruise) | PB3 | GPIO Output | 330Ω to GND |
| Yellow (Slow-Down) | PB4 | GPIO Output | 330Ω to GND |
| Red (Brake/E-Stop) | PB5 | GPIO Output | 330Ω to GND |

## 5. UART Debug

| Signal | NUCLEO-G071RB Pin | Function | Notes |
|---|---|---|---|
| USART2_TX | PA2 | UART Output | Routed to ST-LINK Virtual COM Port automatically on NUCLEO boards |
| USART2_RX | PA3 | UART Input | Routed to ST-LINK VCP (unused by firmware, reserved) |

No external USB-serial adapter is required — the ST-LINK on the NUCLEO board exposes USART2 as a
virtual COM port over the same USB cable used for programming.

## 6. Full Signal Summary Table

| Peripheral | Pin | AF/Mode | Direction |
|---|---|---|---|
| TIM2_CH1 (US Echo capture) | PA0 | AF2, Input Capture | In |
| GPIO (US Trigger) | PA1 | Output Push-Pull | Out |
| USART2_TX | PA2 | AF1 | Out |
| USART2_RX | PA3 | AF1 | In |
| ADC1_IN4 (Potentiometer) | PA4 | Analog | In |
| TIM3_CH1 (Motor PWM / ENA) | PA6 | AF1, PWM Output | Out |
| GPIO (Motor IN1) | PB0 | Output Push-Pull | Out |
| GPIO (Motor IN2) | PB1 | Output Push-Pull | Out |
| GPIO (LED Green) | PB3 | Output Push-Pull | Out |
| GPIO (LED Yellow) | PB4 | Output Push-Pull | Out |
| GPIO (LED Red) | PB5 | Output Push-Pull | Out |

## 7. Wiring Diagram

See the [wiring Mermaid diagram in the root README](../README.md#wiring-diagram) for a visual
representation of this table.
