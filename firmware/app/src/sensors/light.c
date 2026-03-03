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
    int ret = 0;
    struct light_event evt;
    struct sensor_value sensor_val;

    if ((apds9306 != NULL) && (device_is_ready(apds9306))) {
        if ((sensor_sample_fetch(apds9306) == 0) &&
            (sensor_channel_get(apds9306, SENSOR_CHAN_LIGHT, &sensor_val) == 0)) {

            evt.light = sensor_val.val1;
            ret = zbus_chan_pub(&light_data_chan, &evt, K_NO_WAIT);

            LOG_DBG("Publish new data...");
            LOG_DBG("   Light: %u", evt.light);
        }
    } else {
        LOG_WRN("Device is not ready!");

#ifdef CONFIG_DEBUG
        static int i = 0;
        LOG_INF("Sending fake data...");
        evt.light = i++;
        ret = zbus_chan_pub(&light_data_chan, &evt, K_NO_WAIT);
#endif
    }

    if (ret != 0) {
        LOG_ERR("Failed to publish light data: %d", ret);
    }
}

static int beelight_light_sensor_init(void)
{
    int err;

    err = zbus_chan_add_obs(&periodic_5min_chan, &light_periodic_sample_event_lis, K_NO_WAIT);
    if (err != 0) {
        return err;
    }

    if (apds9306 == NULL) {
        LOG_ERR("APDS9306 device node not found!");
        return -ENODEV;
    }

    if (device_is_ready(apds9306) == false) {
        LOG_ERR("APDS9306 device is not ready!");
        return -ENODEV;
    }

#ifdef CONFIG_PM_DEVICE
    err = pm_device_runtime_auto_enable(apds9306);
    if (err != 0) {
        LOG_ERR("Failed to enable runtime PM: %d!", err);
        return err;
    }
#endif

    return 0;
}

SYS_INIT(beelight_light_sensor_init, APPLICATION, 1);
