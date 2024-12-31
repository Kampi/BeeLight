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

#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/sensor.h>

#include "events/events.h"

static void zbus_10s_callback(const struct zbus_channel *chan);

ZBUS_CHAN_DECLARE(light_data_chan);
ZBUS_CHAN_DECLARE(periodic_event_10s_chan);
ZBUS_LISTENER_DEFINE(beelight_light_sensor_lis, zbus_10s_callback);

const struct device *const sensor = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(bh1750));

LOG_MODULE_REGISTER(lightsensor, LOG_LEVEL_DBG);

static void zbus_10s_callback(const struct zbus_channel *chan)
{
    struct sensor_value sensor_val;

    if (device_is_ready(sensor)) {
        if ((sensor_sample_fetch(sensor) == 0) &&
            (sensor_channel_get(sensor, SENSOR_CHAN_LIGHT, &sensor_val) == 0)) {
            LOG_DBG("New value: %u", sensor_val.val1);

            struct light_event evt = {
                .light = sensor_val.val1,
            };

            zbus_chan_pub(&light_data_chan, &evt, K_MSEC(250));
        }
    }
    else {
        LOG_ERR("Device \"%s\" is not ready!", sensor->name);
    }
}

int beelight_sensor_init(void)
{
    int err;

    err = zbus_chan_add_obs(&periodic_event_10s_chan, &beelight_light_sensor_lis, K_MSEC(100));
    if (err != 0) {
        return err;
    }

    return 0;
}
