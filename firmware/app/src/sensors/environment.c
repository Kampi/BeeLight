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
#include <drivers/bme68x_iaq.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/pm/device_runtime.h>

#include <string.h>

#include "events.h"

static void zbus_5min_callback(const struct zbus_channel *chan);

const struct device *const bme688 = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(bme688));

ZBUS_CHAN_DECLARE(env_data_chan);
ZBUS_CHAN_DECLARE(periodic_5min_chan);
ZBUS_LISTENER_DEFINE(env_periodic_sample_event_lis, zbus_5min_callback);

LOG_MODULE_REGISTER(env, LOG_LEVEL_DBG);

static void zbus_5min_callback(const struct zbus_channel *chan)
{
    int ret = 0;
    struct env_event evt;
    struct sensor_value sensor_val;

    if ((bme688 != NULL) && (device_is_ready(bme688))) {
        if ((sensor_sample_fetch(bme688) == 0)) {
            memset(&evt, 0, sizeof(struct env_event));

            if (sensor_channel_get(bme688, SENSOR_CHAN_AMBIENT_TEMP, &sensor_val) == 0) {
                evt.temperature = sensor_val.val1;
            }

            if (sensor_channel_get(bme688, SENSOR_CHAN_PRESS, &sensor_val) == 0) {
                evt.pressure = sensor_val.val1;
            }

            if (sensor_channel_get(bme688, SENSOR_CHAN_HUMIDITY, &sensor_val) == 0) {
                evt.humidity = sensor_val.val1;
            }

#if CONFIG_BME68X_IAQ
            if (sensor_channel_get(bme688, SENSOR_CHAN_CO2, &sensor_val) == 0) {
                evt.co2.value = sensor_val.val1;
                evt.co2.tolerance = sensor_val.val2;
            }

            if (sensor_channel_get(bme688, SENSOR_CHAN_VOC, &sensor_val) == 0) {
                evt.voc.value = sensor_val.val1;
                evt.voc.tolerance = sensor_val.val2;
            }

            if (sensor_channel_get(bme688, SENSOR_CHAN_IAQ, &sensor_val) == 0) {
                evt.iaq.value = sensor_val.val1;
                evt.iaq.tolerance = sensor_val.val2;
            }
#endif /* CONFIG_BME68X_IAQ */

            ret = zbus_chan_pub(&env_data_chan, &evt, K_NO_WAIT);

            LOG_DBG("Publish new data...");
            LOG_DBG("   Temperature: %f", (double)evt.temperature);
            LOG_DBG("   Pressure: %f", (double)evt.pressure);
            LOG_DBG("   Humidity: %f", (double)evt.humidity);
#if CONFIG_BME68X_IAQ
            LOG_DBG("   CO2: %f", (double)evt.co2.value);
            LOG_DBG("   VOC: %f", (double)evt.voc.value);
            LOG_DBG("   IAQ: %f", (double)evt.iaq.value);
#endif
        }
    } else {
        LOG_WRN("Device is not ready!");

#ifdef CONFIG_DEBUG
        static int i = 0;
        LOG_INF("Sending fake data...");
        evt.temperature = i++;
        evt.pressure = i++;
        evt.humidity = i++;
#if CONFIG_BME68X_IAQ
        evt.co2.value = i++;
        evt.voc.value = i++;
        evt.iaq.value = i++;
#endif
        ret = zbus_chan_pub(&env_data_chan, &evt, K_NO_WAIT);
#endif
    }

    if (ret != 0) {
        LOG_ERR("Failed to publish env data: %d", ret);
    }

}

static int beelight_env_sensor_init(void)
{
    int err;

    err = zbus_chan_add_obs(&periodic_5min_chan, &env_periodic_sample_event_lis, K_NO_WAIT);
    if (err != 0) {
        return err;
    }

    if (bme688 == NULL) {
        LOG_ERR("BME688 device node not found!");
        return -ENODEV;
    }

    if (device_is_ready(bme688) == false) {
        LOG_ERR("BME688 device is not ready!");
        return -ENODEV;
    }

#ifdef CONFIG_PM_DEVICE
    err = pm_device_runtime_auto_enable(bme688);
    if (err != 0) {
        LOG_ERR("Failed to enable runtime PM: %d!", err);
        return err;
    }
#endif

    return 0;
}

SYS_INIT(beelight_env_sensor_init, APPLICATION, 1);
