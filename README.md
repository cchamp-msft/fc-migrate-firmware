# fc-migrate firmware

Unofficial Betaflight builds for flight controllers that Betaflight's cloud build does not support yet. [fc-migrate.com](https://fc-migrate.com) offers these builds when it migrates such a board. Everything needed to rebuild them is in this repository.

This is not a Betaflight project and is not endorsed by it. Each board here is meant to reach Betaflight's official channels: its config goes to [`betaflight/config`](https://github.com/betaflight/config) and any firmware change to [`betaflight/betaflight`](https://github.com/betaflight/betaflight). Once a board is in an official release, it is built by Betaflight and dropped from this repository.

## Boards

| Board | Config | Betaflight | Patches | Status |
|---|---|---|---|---|
| T-Motor Pacer G4 Alpha HD (STM32G473) | [`configs/COMM/TMOTORG4ALPHAHD`](configs/COMM/TMOTORG4ALPHAHD/config.h) | 2026.6.2 | ADC oversampling, LPUART1 backport | Flown. Not yet proposed upstream. |
| T-Motor Pacer G4 Alpha Analog (STM32G473, STM32G071 OSD chip) | [`configs/COMM/TMOTORG4ALPHAANALOG`](configs/COMM/TMOTORG4ALPHAANALOG/config.h) | 2026.6.2 | ADC oversampling, LPUART1 backport, MSP DisplayPort diff | Bench-tested on one board, not flown yet. Not yet proposed upstream. |

Both boards ship with closed firmware and a locked chip. Moving one to Betaflight is a one-way change, made with an ST-Link through [fc-migrate.com](https://fc-migrate.com).

The Analog board's OSD is a separate STM32G071 running FETtec's OSD firmware, which takes MSP DisplayPort on UART4. Its build uses the SD canvas (`USE_OSD_SD`, not `USE_OSD_HD`) and needs the MSP DisplayPort diff patch. After a `bl` command it did not come up as a USB DFU device, so it is updated over SWD.

### Manufacturer ID

Configs here use `COMM`, which Betaflight reserves for community-provided definitions of closed-source boards. The board maker did not submit them. A board running one of these builds reports manufacturer `COMM`, the same as the official target will.

## Patches

Each Betaflight release has its own folder under `patches/`. Every patch starts with a description of the problem it fixes and how it was tested.

| Patch | Why | Upstream |
|---|---|---|
| `stm32g4-adc-oversampling.patch` | Averages 256 conversions in the G4 ADC's hardware oversampler. Without it, the Alpha HD's noisy battery-sense pin never settles, so the cell count stays at 0 and low-battery warnings never fire. Results stay 12-bit, so `vbat_scale` is unchanged. | To be proposed. |
| `stm32g4-lpuart1-init.patch` | Starts LPUART1, the Alpha HD's ESC telemetry line, which 2026.6.2 never initialises. | Backport of Betaflight PR #15723, already on master. |
| `msp-displayport-diff.patch` | The Analog board's OSD chip acts on CLEAR_SCREEN as soon as it arrives, so Betaflight's clear on every OSD frame makes it flicker and hides some elements. With `USE_MSP_DISPLAYPORT_DIFF` defined, the FC sends only the characters that changed. Without the define, the file builds as before. Analog board only. | To be proposed. |

## How a build is made

[`builds.json`](builds.json) lists every build: the Betaflight release, the target, its patches and its build options. For each entry, [the workflow](.github/workflows/build.yml):

1. checks out Betaflight at the release tag;
2. applies that build's patches and commits them with fixed metadata, so the revision the firmware reports is the same on every run;
3. builds with Betaflight's own pinned ARM toolchain (`make arm_sdk_install`) and `CONFIG_DIR` pointing at this repository;
4. writes a JSON file beside the `.hex` with its SHA-256, the Betaflight commit, the revision it reports, the patches, the options and the toolchain.

Every push builds. A `v*` tag publishes a release with the `.hex` files, their JSON and `SHA256SUMS`.

To build one yourself, follow the same steps. For the Alpha HD:

```sh
git clone --branch 2026.6.2 https://github.com/betaflight/betaflight.git
cd betaflight
git apply ../fc-migrate-firmware/patches/2026.6.2/stm32g4-adc-oversampling.patch \
  ../fc-migrate-firmware/patches/2026.6.2/stm32g4-lpuart1-init.patch
git submodule update --init src/config lib/modules/dronecan/libcanard
make arm_sdk_install
make CONFIG=TMOTORG4ALPHAHD CONFIG_DIR="$PWD/../fc-migrate-firmware" \
  EXTRA_FLAGS="-DCLOUD_BUILD -DUSE_SERIALRX -DUSE_SERIALRX_CRSF -DUSE_SERIALRX_SBUS -DUSE_TELEMETRY -DUSE_TELEMETRY_CRSF -DUSE_OSD_HD -DUSE_DSHOT -DUSE_LED_STRIP -DUSE_PINIO -DUSE_VTX"
```

For the Alpha Analog, also apply `msp-displayport-diff.patch`, and build with `CONFIG=TMOTORG4ALPHAANALOG` and `-DUSE_OSD_SD` in place of `-DUSE_OSD_HD`. [`builds.json`](builds.json) lists each build's patches and options.

The firmware lands in `obj/`. Without the commit in step 2, it reports `norevision`.

## License

GPL-3.0-or-later, the same as Betaflight. The configs and patches are derived from Betaflight. Each release names the exact source it was built from: the Betaflight tag plus this repository at the release tag.
