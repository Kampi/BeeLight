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

#include <zephyr/types.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/settings/settings.h>

#include <soc.h>

#include "events.h"

LOG_MODULE_REGISTER(app, LOG_LEVEL_DBG);

/** @brief
 */
static void zbus_on_light_callback(const struct zbus_channel *chan);

/** @brief
 */
static void zbus_on_env_callback(const struct zbus_channel *chan);

/** @brief
 */
static void zbus_on_battery_callback(const struct zbus_channel *chan);

ZBUS_CHAN_DECLARE(light_data_chan);
ZBUS_LISTENER_DEFINE(light_data_lis, zbus_on_light_callback);

ZBUS_CHAN_DECLARE(env_data_chan);
ZBUS_LISTENER_DEFINE(env_data_lis, zbus_on_env_callback);

ZBUS_CHAN_DECLARE(battery_data_chan);
ZBUS_LISTENER_DEFINE(battery_data_lis, zbus_on_battery_callback);

void zbus_on_light_callback(const struct zbus_channel *chan)
{
    const struct light_event *evt = zbus_chan_const_msg(chan);

    LOG_DBG("Value from light sensor: %u", evt->value);
}

static void zbus_on_env_callback(const struct zbus_channel *chan)
{
    const struct env_event *evt = zbus_chan_const_msg(chan);

    LOG_DBG("New temperature: %u degree", evt->temperature);
}

static void zbus_on_battery_callback(const struct zbus_channel *chan)
{
    const struct battery_event *evt = zbus_chan_const_msg(chan);

    LOG_DBG("New battery voltage: %u", evt->mV);
}

int main(void)
{
    zbus_chan_add_obs(&light_data_chan, &light_data_lis, K_MSEC(100));
    zbus_chan_add_obs(&env_data_chan, &env_data_lis, K_MSEC(100));
    zbus_chan_add_obs(&battery_data_chan, &battery_data_lis, K_MSEC(100));

    LOG_INF("BeeLight application started");

    return 0;
}
