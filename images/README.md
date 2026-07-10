# Images

This repository does not ship fabricated screenshots, test-bench photos, or renders — none of
those exist for a reference implementation without a physically built unit. Instead, each image
referenced from the documentation has a **detailed generation/photography prompt** here so you
(or an AI image tool) can produce a real, appropriately-licensed asset once you build the
hardware, or generate an illustrative graphic for the README banner.

## `banner.png` (referenced at the top of the root README)

**Prompt:**
> A clean, modern technical banner image for a GitHub repository, 1584x396px, dark navy
> background (#03234B, ST Microelectronics blue), featuring a stylized top-down silhouette of a
> small robotics car/rover with sensor cones projecting forward, minimalist circuit-trace line
> art in the background, the text "STM32 Adaptive Cruise Control" in a clean geometric sans-serif
> font (white/light blue), and a small STM32-style chip icon. Flat vector illustration style, no
> photorealistic elements, no real product logos, high contrast, suitable for a README header.

## Subfolders

| Folder | Contents |
|---|---|
| [`architecture/`](architecture/README.md) | System/software block diagram illustrative prompts |
| [`hardware/`](hardware/README.md) | Physical build/wiring photo prompts |
| [`flowcharts/`](flowcharts/README.md) | FSM and workflow flowchart illustrative prompts |
| [`results/`](results/README.md) | Test/telemetry result plot prompts (no fabricated data) |

## Why prompts instead of files?

Producing a convincing fake oscilloscope capture, a fake serial terminal log, or a fake photo of
a breadboard would misrepresent this repository as containing empirical results it does not have.
The Mermaid diagrams embedded directly in `README.md` and `docs/*.md` already provide the
structural/flow visuals natively renderable on GitHub without needing a static image asset at
all — the prompts here are for the *optional* polish images (banner, photos) that benefit from a
raster format.
