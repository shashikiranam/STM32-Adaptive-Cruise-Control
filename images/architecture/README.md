# Architecture Images

The primary architecture diagrams for this project are implemented as **Mermaid diagrams**
directly in [`../../README.md`](../../README.md#system-architecture) and
[`../../docs/SOFTWARE.md`](../../docs/SOFTWARE.md), which render natively on GitHub without
needing a static image file. The prompts below are for an optional polished illustrative version.

## `system_block_diagram.png`

**Prompt:**
> A professional embedded-systems block diagram, flat vector style, light background, showing
> four labeled columns left-to-right: "Sensing" (ultrasonic sensor icon, potentiometer icon),
> "STM32G071RB MCU" (chip icon in the center with small labeled sub-blocks for ADC, Timer/Input
> Capture, PWM, UART), "Actuation" (H-bridge module icon, DC motor icon), and "Feedback" (three
* colored LED icons, a terminal/monitor icon). Arrows connect each column left to right showing
> signal flow. Use a restricted palette of navy blue, white, and one accent orange. No text other
> than the labels described. Technical documentation style, similar to a datasheet block diagram.

## `layered_software_architecture.png`

**Prompt:**
> A layered software architecture diagram, flat vector style, showing four horizontal
> bands stacked top to bottom: "Application Layer (ACC Finite State Machine)", "Driver Layer
> (Ultrasonic, Motor, PWM, GPIO, Timer)", "STM32 HAL", "CMSIS / Hardware Registers". Each band a
> different shade of blue, with thin connecting arrows showing dependency direction pointing
> downward only. Minimalist, textbook-diagram style, suitable for embedded software documentation.
