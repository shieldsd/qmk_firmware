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
#include "matrix.h"
#include "i2c_master.h"
#include <string.h>
#include "model100.h"

/* If no key events have occurred, the scanners will time out on reads.
 * So we don't want to be too permissive here. */
#define SCANNER_I2C_TIMEOUT     10

#define ROWS_PER_HAND (MATRIX_ROWS / 2)

static void i2c_read_hand(int hand, matrix_row_t current_matrix[]) {
  uint8_t buf[5];
  i2c_status_t ret = i2c_receive(I2C_ADDR(hand), buf, sizeof(buf), SCANNER_I2C_TIMEOUT);
  if (ret != I2C_STATUS_SUCCESS)
    return;

  if (buf[0] != TWI_REPLY_KEYDATA)
    return;

  int start_row = hand ? ROWS_PER_HAND : 0;
  memcpy(&current_matrix[start_row], &buf[1], ROWS_PER_HAND);
}

static void i2c_set_keyscan_interval(int hand, int delay) {
  uint8_t buf[] = {TWI_CMD_KEYSCAN_INTERVAL, delay};
  i2c_transmit(I2C_ADDR(hand), buf, sizeof(buf), SCANNER_I2C_TIMEOUT);
}

void matrix_init_custom(void) {
  /* Turn on the switched 5V network that powers the scanners. On the Model 100
   * this is an open-drain output driven LOW to enable (unlike the Model 01's
   * push-pull C7). The scanner power must come up before I2C is usable, else
   * neither hand will answer. */
  gpio_set_pin_output_open_drain(B9);
  gpio_write_pin_low(B9);
  wait_ms(70);

  i2c_init();
  i2c_set_keyscan_interval(LEFT, 2);
  i2c_set_keyscan_interval(RIGHT, 2);
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
  matrix_row_t last_matrix[MATRIX_ROWS];
  memcpy(last_matrix, current_matrix, sizeof(last_matrix));

  i2c_read_hand(LEFT, current_matrix);
  i2c_read_hand(RIGHT, current_matrix);

  return memcmp(last_matrix, current_matrix, sizeof(last_matrix)) != 0;
}

/* vim: set ts=2 sw=2 et: */
