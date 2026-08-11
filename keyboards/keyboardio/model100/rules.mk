CUSTOM_MATRIX = yes
I2C_DRIVER_REQUIRED = yes
SRC += leds.c \
	   matrix.c

# The stm32duino bootloader gives us the correct flash layout (application at
# 0x08002000), but the Model 100 ships Keyboardio's DAPBoot DFU bootloader,
# which enumerates as 3496:0005 rather than the Maple default 1EAF:0003.
# Override the dfu-util identifiers so `make ...:flash` targets the device.
DFU_ARGS = -d 3496:0005 -a 0 -s 0x08002000:leave
DFU_SUFFIX_ARGS = -v 3496 -p 0005
