# Keyboardio Model 100

A split keyboard.

* Keyboard Maintainer: QMK Community
* Hardware Supported: Keyboardio Model 100
* Hardware Availability: [Keyboardio](https://shop.keyboard.io)

Make example for this keyboard (after setting up your build environment):

    make keyboardio/model100:default

Flashing example for this keyboard:

    make keyboardio/model100:default:flash

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information.
Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## Bootloader

The Model 100 ships with a DFU bootloader. To enter it, press the `Prog` key
(upper left) or hold it while plugging the keyboard in. The `stm32duino`
(dfu-util) flasher is used.

It is *very* hard to brick a Model 100 short of overwriting the bootloader
region itself, which requires specialised hardware, so experimentation is safe.

## Porting notes

The Model 100 is architecturally almost identical to the Model 01: both halves
are ATtiny key scanners that the main MCU talks to over I2C, using the same wire
protocol (`wire-protocol-constants.h`), the same scanner addresses, and the same
64-LED (4 banks x 8, per hand) layout. The custom `matrix.c` / `leds.c` are
therefore lifted directly from the Model 01.

What differs is the **main MCU**:

| Function              | Model 01 (ATmega32U4) | Model 100 (GD32F303CG) |
| --------------------- | --------------------- | ---------------------- |
| MCU family            | AVR                   | ARM Cortex-M4          |
| Scanner power enable  | `C7`, push-pull, HIGH | `B9`, open-drain, LOW  |
| Power-sense inputs    | `B4`                  | `B14`, `B15`           |
| I2C to scanners       | AVR TWI               | I2C1 = `B6` / `B7`     |

The GD32F303 is register-compatible enough with the STM32F103 high-density line
that ChibiOS's existing STM32F1 HAL drives it. We therefore build it as
`STM32F103` with `board.h` selecting the high-density (`STM32F103xE`) variant,
following the precedent set by `keyboards/mlego/m65/rev2`. There is no need for a
dedicated GD32 ChibiOS port (the in-tree `GD32VF103` support is the unrelated
RISC-V part).

Pin assignments were taken from the Kaleidoscope Model 100 hardware plugin
(`kaleidoscope/device/keyboardio/Model100.cpp`) and the
`ArduinoCore-GD32-Keyboardio` `keyboardio_model_100` variant.

### Still to verify on real hardware

* The GD32F303 clock tree under the stm32duino board files — the ChibiOS
  `mcuconf.h` clock setup for a genuine STM32F103 may need tuning so USB and the
  I2C `TIMINGR`/prescaler land at the right frequency.
* I2C1 pin alternate-function / open-drain wiring for `B6`/`B7`.

The `Prog` key is at matrix `[0, 7]` (upper left), the same position as on the
Model 01, and the default keymap places `QK_BOOT` there for bootloader entry.

## Features

This implements the standard keymap, including mousekeys.

It doesn't do cursor warping - QMK does not support absolute mouse positioning.

LED support is limited. Gamma compensation and the high speed batch update
functions supported by the hardware are not implemented.
