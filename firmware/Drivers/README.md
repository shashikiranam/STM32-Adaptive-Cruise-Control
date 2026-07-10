# Drivers (STM32G0xx HAL + CMSIS)

This directory is intentionally **empty in version control** (see `.gitignore`). It is populated
by STM32CubeMX when you generate code from [`../STM32CubeMX/Project.ioc`](../STM32CubeMX/Project.ioc),
and will contain:

```
Drivers/
├── CMSIS/
│   ├── Include/                       ← ARM CMSIS core headers
│   └── Device/ST/STM32G0xx/           ← Device-specific CMSIS headers, startup files
└── STM32G0xx_HAL_Driver/
    ├── Inc/                           ← HAL peripheral driver headers
    └── Src/                           ← HAL peripheral driver sources
```

## Why isn't this checked in?

1. **License separation.** The STM32 HAL/CMSIS sources are distributed by STMicroelectronics
   under their own license (BSD-3-Clause, bundled with every STM32Cube firmware package). This
   repository's MIT license covers only the original application code in
   `firmware/Core/Inc` and `firmware/Core/Src`, plus the documentation. Not vendoring the HAL
   keeps the licensing boundary unambiguous.
2. **Size and churn.** The full STM32G0xx HAL package is several thousand files; vendoring it
   would dominate the repository size and generate noisy diffs on every ST firmware package
   update, for code this project doesn't modify.
3. **Reproducibility.** Regenerating from `Project.ioc` guarantees you get a HAL version that
   matches the pin/clock configuration actually described by the `.ioc` file, rather than a
   possibly-stale committed copy.

## How to populate it

1. Install **STM32CubeMX** (6.11.x or later) or use the version bundled with STM32CubeIDE.
2. Open `firmware/STM32CubeMX/Project.ioc`.
3. Click **Generate Code** (the gear/cog icon, or `Project → Generate Code`).
4. CubeMX will populate this `Drivers/` folder alongside the existing hand-written
   `Core/Inc`/`Core/Src` files. CubeMX is configured (via `Project.ioc`'s project settings) to
   **not** overwrite user code sections in `Core/Src/main.c`, but since this repository's
   `main.c` is fully hand-authored (see `docs/SOFTWARE.md`), review any regenerated `main.c`
   diff carefully before accepting it, or simply keep the hand-authored version in this
   repository and only take the `Drivers/` output from a fresh generation into an otherwise
   empty scratch folder.

## Expected HAL Package Version

This project was authored against **STM32G0xx HAL Driver v1.5.x**, part of the
**STM32CubeG0** firmware package. If CubeMX offers a newer HAL package version, it should remain
API-compatible for the HAL calls used in this project's driver modules (`HAL_TIM_IC_*`,
`HAL_TIM_PWM_*`, `HAL_ADC_*`, `HAL_UART_Transmit`, `HAL_GPIO_*`), but re-run the test plan in
`docs/TESTING.md` after any HAL package upgrade.
