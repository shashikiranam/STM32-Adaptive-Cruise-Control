# STM32CubeMX Project Configuration

`Project.ioc` is the STM32CubeMX project descriptor for this board configuration. It captures:

- **MCU**: STM32G071RBTx (NUCLEO-G071RB board preset)
- **Clock tree**: HSI (16 MHz) → PLL → 64 MHz SYSCLK/HCLK/PCLK1
- **Peripherals**:
  - `TIM2` — Input Capture, Channel 1 (PA0) — HC-SR04 echo timing
  - `TIM3` — PWM Generation, Channel 1 (PA6) — motor speed (L298N ENA)
  - `ADC1` — Single channel, IN4 (PA4) — potentiometer setpoint
  - `USART2` — Asynchronous, 115200-8N1 (PA2/PA3) — debug telemetry, routed to ST-LINK VCP
  - `GPIO` — PA1 (Trigger, output), PB0/PB1 (motor direction), PB3/PB4/PB5 (status LEDs)

## Opening the project

1. Install STM32CubeMX 6.11.x or later.
2. `File → Open Project` → select `Project.ioc`.
3. Review the **Pinout & Configuration** and **Clock Configuration** tabs against
   [`../../hardware/PIN_CONNECTIONS.md`](../../hardware/PIN_CONNECTIONS.md) if you are adapting
   this project to different pins or a different board variant.
4. `Project Manager` tab → confirm **Toolchain/IDE** is set to `STM32CubeIDE` and the project
   location points at the `firmware/` directory.
5. Click **GENERATE CODE**.

## Regeneration notes

- CubeMX will (re)populate `firmware/Drivers/` — see
  [`../Drivers/README.md`](../Drivers/README.md) for why that folder isn't tracked in git.
- This repository's `Core/Inc/*.h` and `Core/Src/*.c` files are **hand-authored**, not
  CubeMX-generated stubs — do not let CubeMX overwrite `main.c` without reviewing the diff, since
  the hand-authored version already contains the full peripheral init and application wiring
  documented in `docs/SOFTWARE.md`.
- If you change a pin assignment in CubeMX, mirror the change in
  `firmware/Core/Inc/system_config.h` and `hardware/PIN_CONNECTIONS.md` so all three stay in
  sync — `system_config.h` is the runtime source of truth the C code actually uses.

## Note on the `.ioc` file in this repository

The `Project.ioc` file included here is a complete, valid STM32CubeMX project descriptor
reflecting the pin/clock/peripheral configuration described above and in
`hardware/PIN_CONNECTIONS.md`. Open it in STM32CubeMX to inspect or regenerate the HAL/CMSIS
driver tree; the hand-authored application code in `Core/Inc`/`Core/Src` does not depend on
regenerating it to build, provided your own `Drivers/` tree matches the STM32G0xx HAL v1.5.x API
surface referenced throughout this project.
