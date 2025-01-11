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
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>

#include "events.h"

#define VBATT                               DT_PATH(vbatt)

/** @brief This board uses a divider that reduces max voltage to reference voltage (600 mV).
 */
#define BATTERY_ADC_GAIN                    ADC_GAIN_1

struct divider_config {
    struct io_channel_config {
        uint8_t channel;
    } io_channel;
    struct gpio_dt_spec power_gpios;
    /* output_ohm is used as a flag value: if it is nonzero then
     * the battery is measured through a voltage divider;
     * otherwise it is assumed to be directly connected to Vdd.
     */
    uint32_t output_ohm;
    uint32_t full_ohm;
};

static const struct divider_config divider_config = {
    .io_channel = {
        DT_IO_CHANNELS_INPUT(VBATT),
    },
    .power_gpios = GPIO_DT_SPEC_GET_OR(VBATT, power_gpios, {}),
    .output_ohm = DT_PROP(VBATT, output_ohms),
    .full_ohm = DT_PROP(VBATT, full_ohms),
};

struct divider_data {
    const struct device *adc;
    struct adc_channel_cfg adc_cfg;
    struct adc_sequence adc_seq;
    int16_t raw;
};

static struct divider_data divider_data = {
    .adc = DEVICE_DT_GET_OR_NULL(DT_IO_CHANNELS_CTLR(VBATT)),
};

static void handle_battery_sample_timeout(struct k_work *item);

K_WORK_DELAYABLE_DEFINE(battery_sample_work, handle_battery_sample_timeout);

LOG_MODULE_REGISTER(battery, LOG_LEVEL_DBG);

static void handle_battery_sample_timeout(struct k_work *item)
{
    LOG_DBG("Battery sample event");

    k_work_schedule(&battery_sample_work, K_SECONDS(1));
}

static int beelight_battery_sensor_init(void)
{
    //k_work_schedule(&battery_sample_work, K_SECONDS(1));

    return 0;
}

SYS_INIT(beelight_battery_sensor_init, APPLICATION, 1);
