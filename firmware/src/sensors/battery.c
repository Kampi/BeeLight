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

struct adc_data_t {
    const struct adc_dt_spec adc_channel;
    const struct gpio_dt_spec power_gpios;
    struct adc_sequence adc_seq;
    uint32_t output_ohm;
    uint32_t full_ohm;
    uint16_t raw;
};

static struct adc_data_t adc_data = {
    .adc_channel = ADC_DT_SPEC_GET(DT_PATH(vbatt)),
    .power_gpios = GPIO_DT_SPEC_GET_OR(DT_PATH(vbatt), power_gpios, {}),
    //.output_ohm = DT_PROP(DT_PATH(vbatt), output_ohms),
    //.full_ohm = DT_PROP(DT_PATH(vbatt), full_ohms),
};

struct battery_level_point {
    /** Remaining life at #lvl_mV. */
    uint16_t lvl_pptt;

    /** Battery voltage at #lvl_pptt remaining life. */
    uint16_t lvl_mV;
};

/** A discharge curve specific to the power source. */
static const struct battery_level_point levels[] = {
    /*
    Battery supervisor cuts power at 3500mA so treat that as 0%
    This is very basic and the percentage will not be exact.
    */
    { 10000, 3200 },
    { 0, 2700 },
};

static void battery_sample_callback(struct k_work *item);

ZBUS_CHAN_DECLARE(battery_data_chan);
K_WORK_DELAYABLE_DEFINE(battery_sample_work, battery_sample_callback);

LOG_MODULE_REGISTER(battery, LOG_LEVEL_DBG);

static int battery_sample(int32_t* val_mv)
{
    int err;

    gpio_pin_set_dt(&adc_data.power_gpios, true);

    adc_sequence_init_dt(&adc_data.adc_channel, &adc_data.adc_seq);

    err = adc_read_dt(&adc_data.adc_channel, &adc_data.adc_seq);
    if (err == 0) {
        *val_mv = (int32_t)adc_data.raw;

        adc_raw_to_millivolts_dt(&adc_data.adc_channel, val_mv);

        //if (adc_data.output_ohm != 0) {
        //    LOG_INF("raw %u ~ %u mV => %u mV", adc_data.raw, *val_mv, (uint32_t)((*val_mv) * (uint64_t)adc_data.full_ohm / adc_data.output_ohm));
        //} else {
            LOG_INF("raw %u ~ %u mV", adc_data.raw, *val_mv);
        //}
    }

    gpio_pin_set_dt(&adc_data.power_gpios, false);

    return err;
}

static uint32_t battery_level_pptt(uint32_t batt_mV, const struct battery_level_point *curve)
{
    const struct battery_level_point *pb = curve;

    if (batt_mV >= pb->lvl_mV) {
        /* Measured voltage above highest point, cap at maximum. */
        return pb->lvl_pptt;
    }

    /* Go down to the last point at or below the measured voltage. */
    while ((pb->lvl_pptt > 0) && (batt_mV < pb->lvl_mV)) {
        ++pb;
    }

    if (batt_mV < pb->lvl_mV) {
        /* Below lowest point, cap at minimum */
        return pb->lvl_pptt;
    }

    /* Linear interpolation between below and above points. */
    const struct battery_level_point *pa = pb - 1;

    return pb->lvl_pptt
           + ((pa->lvl_pptt - pb->lvl_pptt)
              * (batt_mV - pb->lvl_mV)
              / (pa->lvl_mV - pb->lvl_mV));
}

static int get_battery_status(int32_t *mV, int32_t *percent)
{
    uint32_t batt_pptt;

    // From https://github.com/zephyrproject-rtos/zephyr/blob/main/samples/boards/nrf/battery/src/main.c
    battery_sample(mV);

    if (*mV < 0) {
        LOG_ERR("Failed to read battery voltage: %d", *mV);
        return -1;
    }

    //batt_pptt = battery_level_pptt(*mV, levels);

    //LOG_DBG("%d mV; %u pptt", *mV, batt_pptt);
    //*percent = batt_pptt / 100;

    return 0;
}

static void battery_sample_callback(struct k_work *item)
{
    struct battery_event evt;

    LOG_DBG("Battery sample callback");


    if (get_battery_status(&evt.mV, &evt.percent) == 0) {
        zbus_chan_pub(&battery_data_chan, &evt, K_MSEC(5));
    }

    k_work_schedule(&battery_sample_work,  K_SECONDS(1));
}

static int battery_init(void)
{
    int err;

    if (!gpio_is_ready_dt(&adc_data.power_gpios)) {
        LOG_ERR("GPIO device %s not ready!", adc_data.adc_channel.dev->name);
        return -1;
    }

    gpio_pin_configure_dt(&adc_data.power_gpios, GPIO_OUTPUT | GPIO_ACTIVE_LOW);
    gpio_pin_set_dt(&adc_data.power_gpios, false);

    if (!adc_is_ready_dt(&adc_data.adc_channel)) {
        LOG_ERR("ADC controller device %s not ready!", adc_data.adc_channel.dev->name);
        return -1;
    }

    err = adc_channel_setup_dt(&adc_data.adc_channel);
    if (err < 0) {
        LOG_ERR("Could not setup channel! Error: %d", err);
        return err;
    }

    adc_data.adc_seq.buffer = &adc_data.raw;
    adc_data.adc_seq.buffer_size = sizeof(adc_data.raw);

    k_work_schedule(&battery_sample_work, K_SECONDS(1));

    return 0;
}

SYS_INIT(battery_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
