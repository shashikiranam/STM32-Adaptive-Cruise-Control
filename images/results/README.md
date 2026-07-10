# Results Images

**Important:** No test data ships with this repository. This project is a reference
implementation, not a built-and-benchmarked unit; any "results" screenshot would necessarily be
fabricated. Below are the recommended captures to produce once you build the hardware, plus a
placeholder-illustration prompt if you want a generic graphic in the meantime.

## Recommended real captures (post-build)

| File | How to produce it |
|---|---|
| `telemetry_plot.png` | Log UART frames per `docs/TESTING.md §6`, plot `distance_mm` and `duty_pct` vs. time in `matplotlib` while sweeping a real obstacle through all four zones. |
| `serial_terminal_capture.png` | Screenshot of a real serial terminal (e.g. Tera Term, PuTTY, or the CubeIDE terminal) showing live `[ACC] state=... dist_mm=...` frames during a test run. |
| `oscilloscope_pwm_capture.png` | Real oscilloscope/logic-analyzer capture of the `ENA` PWM signal at two or three different commanded duty cycles, to document actual PWM_SetDutyCycle() behavior. |
| `fsm_transition_log.png` | Annotated plot or table showing timestamped FSM state transitions during an INT-01 through INT-06 test run (see `docs/TESTING.md`). |

## Placeholder illustrative graphic (non-data)

If you want a generic "results" hero image before you have real data, use a prompt like:

> A minimalist flat-vector illustration of an abstract line chart trending downward-then-upward,
> paired with a small dashboard-style status panel showing three colored indicator dots (green,
> yellow, red), light background, clean geometric style, no readable numeric data or fabricated
> figures, purely decorative/illustrative — suitable as a placeholder "results" section graphic
> that does not imply specific measured values.

Do not caption any placeholder illustration as if it were real captured data — label it clearly
as illustrative in any documentation that uses it.
