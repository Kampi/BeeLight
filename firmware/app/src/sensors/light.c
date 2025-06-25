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

#include <zephyr/logging/log.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/pm/device_runtime.h>

#include "events.h"

static void zbus_5min_callback(const struct zbus_channel *chan);

const struct device *const apds9306 = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(apds9306));

ZBUS_CHAN_DECLARE(light_data_chan);
ZBUS_CHAN_DECLARE(periodic_5min_chan);
ZBUS_LISTENER_DEFINE(light_periodic_sample_event_lis, zbus_5min_callback);

LOG_MODULE_REGISTER(light, LOG_LEVEL_DBG);

static void zbus_5min_callback(const struct zbus_channel *chan)
{
    struct sensor_value sensor_val;

    if (device_is_ready(apds9306)) {
        if ((sensor_sample_fetch(apds9306) == 0) &&
            (sensor_channel_get(apds9306, SENSOR_CHAN_LIGHT, &sensor_val) == 0)) {
            struct light_event evt;

            memset(&evt, 0, sizeof(struct light_event));

            evt.light = sensor_val.val1;

            zbus_chan_pub(&light_data_chan, &evt, K_NO_WAIT);
            LOG_DBG("Publish new data...");
            LOG_DBG("   Light: %u", evt.light);
        }
    } else {
        LOG_ERR("Device \"%s\" is not ready!", apds9306->name);
    }
}

static int beelight_light_sensor_init(void)
{
    int err;

    pm_device_runtime_auto_enable(apds9306);

    err = zbus_chan_add_obs(&periodic_5min_chan, &light_periodic_sample_event_lis, K_NO_WAIT);
    if (err != 0) {
        return err;
    }

    return 0;
}

SYS_INIT(beelight_light_sensor_init, APPLICATION, 1);