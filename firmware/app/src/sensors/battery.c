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

#include <zephyr/pm/device.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/pm/device_runtime.h>

#include "events.h"

static void zbus_10s_callback(const struct zbus_channel *chan);

const struct device *const vbatt = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(vbatt));

ZBUS_CHAN_DECLARE(battery_data_chan);
ZBUS_CHAN_DECLARE(periodic_event_10s_chan);
ZBUS_LISTENER_DEFINE(batt_periodic_event_10s_lis, zbus_10s_callback);

LOG_MODULE_REGISTER(battery, CONFIG_BEELIGHT_SENSORS_LOG_LEVEL);

static void zbus_10s_callback(const struct zbus_channel *chan)
{
    struct sensor_value sensor_val;

    LOG_DBG("Battery sample event");

    if (device_is_ready(vbatt)) {
        pm_device_runtime_get(vbatt);
        if ((sensor_sample_fetch(vbatt) == 0) &&
            (sensor_channel_get(vbatt, SENSOR_CHAN_VOLTAGE, &sensor_val) == 0)) {
            struct battery_event evt;

            memset(&evt, 0, sizeof(struct battery_event));

            evt.mV = (sensor_val.val1 * 1000) + (sensor_val.val2 / 1000);

            zbus_chan_pub(&battery_data_chan, &evt, K_NO_WAIT);
            LOG_DBG("Publish new data...");
        }
        pm_device_runtime_put(vbatt);
    }
    else {
        LOG_ERR("Device \"%s\" is not ready!", vbatt->name);
    }
}

static int beelight_battery_sensor_init(void)
{
    int err;

    pm_device_runtime_enable(vbatt);

    err = zbus_chan_add_obs(&periodic_event_10s_chan, &batt_periodic_event_10s_lis, K_NO_WAIT);
    if (err != 0) {
        return err;
    }

    return 0;
}

SYS_INIT(beelight_battery_sensor_init, APPLICATION, 1);