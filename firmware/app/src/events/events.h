/*
 * This file is part of the BeeLight project <https://github.com/Kampi/BeeLight>.
 * Copyright (c) 2025 Daniel Kampert.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <zephyr/zbus/zbus.h>

#include <stdint.h>

/** @brief
 */
struct periodic_event {
};

/** @brief
 */
struct light_event {
    uint16_t light;             /**< */
};

/** @brief
 */
struct env_event {
    float temperature;          /**< */
    float pressure;             /**< */
    float humidity;             /**< */
#if CONFIG_BME68X_IAQ
    struct{
        float value;            /**< */
        uint32_t tolerance;     /**< */
    } iaq;
    struct {
        float value;            /**< */
        uint32_t tolerance;     /**< */
    } voc;
    struct {
        float value;            /**< */
        uint32_t tolerance;     /**< */
    } co2;
#endif
};

/** @brief
 */
struct battery_event {
    uint32_t voltage;           /**< */
    uint8_t percent;            /**< */
};