/* Copyright 2018 James Laird-Wah
 * Copyright 2025 QMK
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "leds.h"
#include "led_tables.h"
#include "rgb_matrix.h"
#include <string.h>
#include "model100.h"

/* LED writes go over the SAME I2C1 bus as the key scanners. QMK's ChibiOS DMA
 * i2c_transmit does not work on the GD32F303 and mixing it with the polled key
 * reads corrupts the bus, so use the shared polled i2c_poll_write (matrix.c). */

void set_all_leds_to(uint8_t r, uint8_t g, uint8_t b) {
  uint8_t buf[] = {
    TWI_CMD_LED_SET_ALL_TO,
    b, g, r
  };
  i2c_poll_write(I2C_ADDR(LEFT), buf, sizeof(buf));
  i2c_poll_write(I2C_ADDR(RIGHT), buf, sizeof(buf));
}

void set_led_to(int led, uint8_t r, uint8_t g, uint8_t b) {
  uint8_t buf[] = {
    TWI_CMD_LED_SET_ONE_TO,
    led & 0x1f,
    b, g, r
  };
  int hand = (led >= 32) ? RIGHT : LEFT;
  i2c_poll_write(I2C_ADDR(hand), buf, sizeof(buf));
}

#ifdef RGB_MATRIX_ENABLE

static struct {
  uint8_t b;
  uint8_t g;
  uint8_t r;
} __attribute__((packed)) led_state[64];

static void set_color(int index, uint8_t r, uint8_t g, uint8_t b) {
  led_state[index].r = r;
  led_state[index].g = g;
  led_state[index].b = b;
}

static void set_color_all(uint8_t r, uint8_t g, uint8_t b) {
  for (int i=0; i<RGB_MATRIX_LED_COUNT; i++)
    set_color(i, r, g, b);
}

static void init(void) {
  /* The Model 100 gates power to the two 5V scanner/LED nets via B9, which is
   * enabled in matrix_init(). Unlike the Model 01 there is no separate
   * high-current LED enable line to drive here. B14/B15 are power-sense inputs. */
  gpio_set_pin_input(B14);
  gpio_set_pin_input(B15);
}

/* The green boot beacon lit and stayed green -> hardware + write path are fine,
 * and rgb_matrix's flush() never overwrote it, i.e. rgb_matrix is disabled at
 * runtime. RGB_MATRIX_DEFAULT_ON only applies when the (emulated-flash) EEPROM
 * is reset, which it may not be. Force it on at boot regardless of EEPROM. */
void keyboard_post_init_kb(void) {
  rgb_matrix_enable_noeeprom();
  rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
  rgb_matrix_sethsv_noeeprom(0, 0, 128);   /* white, mid brightness */
  keyboard_post_init_user();
}

static void flush(void) {
  /* TEMP: led_state has data (blue test) but the per-bank 0x80+bank path is dark
   * while SET_ALL (0x03) works. Drive the per-bank path with HARDCODED non-zero
   * data (0x20 everywhere). If LEDs light, the 0x80+bank command works and the
   * bug is the led_state->bank byte layout; if dark, the per-bank command itself
   * isn't lighting LEDs for us. */
  uint8_t command[1 + 8*3];
  for (int hand=0; hand<2; hand++) {
    int addr = I2C_ADDR(hand);
    for (int bank=0; bank<4; bank++) {
      command[0] = TWI_CMD_LED_BASE + bank;
      memset(&command[1], 0x20, 8*3);
      i2c_poll_write(addr, command, sizeof(command));
    }
  }
  return;

  uint8_t *bank_data = (uint8_t*)&led_state[0];
  for (int hand=0; hand<2; hand++) {
    int addr = I2C_ADDR(hand);

    for (int bank=0; bank<4; bank++) {
      command[0] = TWI_CMD_LED_BASE + bank;
      memcpy(&command[1], bank_data, 8*3);
      i2c_poll_write(addr, command, sizeof(command));

      bank_data += 8*3;
    }
  }
}

const rgb_matrix_driver_t rgb_matrix_driver = {
  .init = init,
  .flush = flush,
  .set_color = set_color,
  .set_color_all = set_color_all
};

#endif

/* vim: set ts=2 sw=2 et: */
