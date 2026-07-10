# Pin Connections

## HC-SR04

| Sensor Pin | STM32 Pin |
|------------|-----------|
| VCC | 5V |
| GND | GND |
| Trigger | PA0 |
| Echo | PA1 |

---

## L298N Motor Driver

| Driver Pin | STM32 Pin |
|------------|-----------|
| ENA | PA8 (PWM) |
| IN1 | PB0 |
| IN2 | PB1 |

---

## Motor

| Motor Terminal | Connection |
|----------------|------------|
| Motor + | L298N OUT1 |
| Motor - | L298N OUT2 |

---

## Power

STM32 is powered through USB.

Motor Driver receives external 12V supply.

Grounds are connected together.
