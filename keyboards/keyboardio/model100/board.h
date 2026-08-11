// Copyright 2025 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include_next <board.h>

/* The GD32F303CG is a high-density part with 1MB flash / 96KB RAM. The
 * stm32duino board files default to the medium-density STM32F103xB; select the
 * high-density variant so the correct memory map and peripherals are used. */
#undef STM32F103xB
#define STM32F103xE
