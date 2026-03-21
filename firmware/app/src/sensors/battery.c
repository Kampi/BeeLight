/*
 * This file is part of the BeeLight project <https://github.com/Kampi/BeeLight>.
 * Copyright (c) 2026 Daniel Kampert.
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

static void zbus_24h_callback(const struct zbus_channel *chan);

const struct device *const vbatt = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(vbatt));

ZBUS_CHAN_DECLARE(battery_data_chan);
ZBUS_CHAN_DECLARE(periodic_24h_chan);
ZBUS_LISTENER_DEFINE(batt_periodic_sample_event_lis, zbus_24h_callback);

LOG_MODULE_REGISTER(battery, LOG_LEVEL_DBG);

static void zbus_24h_callback(const struct zbus_channel *chan)
{
    int ret = 0;
    struct battery_event evt;
    struct sensor_value sensor_val;

    if (vbatt == NULL) {
        LOG_ERR("VBATT device node not found");
        return;
    }

    if ((vbatt != NULL) && (device_is_ready(vbatt))) {
        pm_device_runtime_get(vbatt);
        if ((sensor_sample_fetch(vbatt) == 0) &&
            (sensor_channel_get(vbatt, SENSOR_CHAN_VOLTAGE, &sensor_val) == 0)) {

            memset(&evt, 0, sizeof(struct battery_event));

            evt.voltage = (sensor_val.val1 * 1000) + (sensor_val.val2 / 1000);
            ret = zbus_chan_pub(&battery_data_chan, &evt, K_NO_WAIT);

            LOG_DBG("Publish new data...");
            LOG_DBG("   Voltage: %u", evt.voltage);
        }
        pm_device_runtime_put(vbatt);
    } else {
        LOG_WRN("Device is not ready!");

#ifdef CONFIG_DEBUG
        static int i = 0;
        LOG_INF("Sending fake data...");
        evt.voltage = i++;
        ret = zbus_chan_pub(&battery_data_chan, &evt, K_NO_WAIT);
#endif

        if (ret != 0) {
            LOG_ERR("Failed to publish battery data: %d", ret);
        }
    }
}

static int beelight_battery_sensor_init(void)
{
    int err;

    err = zbus_chan_add_obs(&periodic_24h_chan, &batt_periodic_sample_event_lis, K_NO_WAIT);
    if (err != 0) {
        return err;
    }

    if (vbatt == NULL) {
        LOG_ERR("VBATT device node not found!");
        return -ENODEV;
    }

#ifdef CONFIG_PM_DEVICE
    err = pm_device_runtime_enable(vbatt);
    if (err != 0) {
        LOG_ERR("Failed to enable runtime PM: %d!", err);
        return err;
    }
#endif

    return 0;
}

SYS_INIT(beelight_battery_sensor_init, APPLICATION, 1);
