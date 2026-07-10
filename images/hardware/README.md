# Hardware Build Images

These are photography guidance notes (not AI-generation prompts) for documenting your own
physical build once assembled, since a real photo is far more valuable here than a synthetic one.

## `breadboard_overview.jpg`

**Guidance:** Top-down photo of the full breadboard build in good, even lighting (avoid harsh
shadows that obscure wire colors). Frame to include the NUCLEO board, L298N module, HC-SR04,
potentiometer, and LED cluster all in one shot. Use a plain, uncluttered background (e.g. a
cutting mat or solid-color desk). A ruler or coin in-frame for scale is a nice touch for a
portfolio piece.

## `wiring_closeup_hcsr04.jpg`

**Guidance:** Close-up of the HC-SR04 Trig/Echo/VCC/GND connections to the NUCLEO header pins,
sharp focus, labels visible if you've added a wire-labeling scheme. Cross-reference against
[`../../hardware/PIN_CONNECTIONS.md`](../../hardware/PIN_CONNECTIONS.md) to confirm correctness
before photographing (a wiring photo that doesn't match the documented pinout is worse than no
photo).

## `l298n_motor_wiring.jpg`

**Guidance:** Close-up of the L298N module showing ENA/IN1/IN2 wired to the NUCLEO, OUT1/OUT2
wired to the motor terminals, and the external 12V supply leads clearly visible and distinct from
the NUCLEO's USB cable, to visually reinforce the separate-power-domain design decision described
in [`../../hardware/POWER_REQUIREMENTS.md`](../../hardware/POWER_REQUIREMENTS.md).

## `status_led_demo.jpg` or `.gif`

**Guidance:** A short video/GIF capture of the three status LEDs transitioning
Green → Yellow → Red as an obstacle is manually moved toward the sensor is one of the most
compelling portfolio assets you can produce for this project — consider prioritizing this over
static photos.

---

If you prefer AI-generated illustrative (non-photographic) hardware graphics instead of real
photos, use a prompt such as:

> A clean technical illustration (not photorealistic) of a breadboard electronics prototype
> featuring a small blue microcontroller board, an ultrasonic sensor module, a motor driver
> board, and a small DC motor, connected with colored jumper wires, isometric view, soft studio
> lighting, minimal background, in the style of a modern hardware product documentation
> illustration.
