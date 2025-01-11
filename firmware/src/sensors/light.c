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

#include "events.h"

static void zbus_10s_callback(const struct zbus_channel *chan);

static struct light_event evt;

const struct device *const sensor = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(apds9306));

ZBUS_CHAN_DECLARE(light_data_chan);
ZBUS_CHAN_DECLARE(periodic_event_10s_chan);
ZBUS_LISTENER_DEFINE(periodic_event_10s_lis, zbus_10s_callback);
LOG_MODULE_REGISTER(light, CONFIG_BEELIGHT_SENSORS_LOG_LEVEL);

static void zbus_10s_callback(const struct zbus_channel *chan)
{
    struct sensor_value sensor_val;

    if (device_is_ready(sensor)) {
        if ((sensor_sample_fetch(sensor) == 0) &&
            (sensor_channel_get(sensor, SENSOR_CHAN_LIGHT, &sensor_val) == 0)) {

            evt.value = sensor_val.val1;

            if(sensor_val.val1 > evt.max) {
                evt.max = sensor_val.val1;
            }

            if(sensor_val.val1 < evt.min) {
                evt.min = sensor_val.val1;
            }

            zbus_chan_pub(&light_data_chan, &evt, K_MSEC(100));
            LOG_DBG("Publish new value: %u", sensor_val.val1);
        }
    }
    else {
        LOG_ERR("Device \"%s\" is not ready!", sensor->name);
    }
}

static int beelight_light_sensor_init(void)
{
    int err;

    err = zbus_chan_add_obs(&periodic_event_10s_chan, &periodic_event_10s_lis, K_MSEC(100));
    if (err != 0) {
        return err;
    }

    return 0;
}

SYS_INIT(beelight_light_sensor_init, APPLICATION, 1);
