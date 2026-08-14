CUSTOM_MATRIX = lite
I2C_DRIVER_REQUIRED = yes
SRC += leds.c \
	   matrix.c

# We deliberately do NOT set `bootloader: stm32duino` in keyboard.json. That
# would pull in platforms/chibios/bootloader.mk's stm32duino block, which
# hard-assigns (with `=`, not `?=`) the Maple defaults
# `DFU_ARGS = -d 1EAF:0003 -a 2 -R` / `DFU_SUFFIX_ARGS = -v 1EAF -p 0003`
# *after* this file is read, clobbering any override we set here. Instead we set
# the same flags the stm32duino block would have (board is declared in
# keyboard.json) and supply our own DFU identifiers. Same approach as
# keyboards/handwired/onekey/bluepill_f103c6.
OPT_DEFS += -DBOOTLOADER_STM32DUINO
BOOTLOADER_TYPE = stm32duino

# The stm32duino board files give us the correct flash layout (application at
# 0x08002000), but the Model 100 ships Keyboardio's DAPBoot DFU bootloader,
# which enumerates as 3496:0005 rather than the Maple default 1EAF:0003.
# Point dfu-util (and the .bin's DFU suffix) at the real device.
DFU_ARGS = -d 3496:0005 -a 0 -s 0x08002000:leave
DFU_SUFFIX_ARGS = -v 3496 -p 0005
