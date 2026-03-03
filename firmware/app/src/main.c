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
#include <zephyr/settings/settings.h>

#include <soc.h>
#include <ram_pwrdn.h>
#include <dk_buttons_and_leds.h>

#include <zboss_api.h>
#include <zboss_api_addons.h>
#include <zb_nrf_platform.h>
#include <zb_mem_config_med.h>
#include <zigbee/zigbee_app_utils.h>
#include <zigbee/zigbee_error_handler.h>

#include <math.h>
#include <stdlib.h>

#include "events.h"
#include "beelight/beelight.h"

/** @brief Zigbee device application context storage.
 */
static device_ctx_t dev_ctx;

/** @brief
 */
static void zbus_on_light_callback(const struct zbus_channel *chan);

/** @brief
 */
static void zbus_on_env_callback(const struct zbus_channel *chan);

/** @brief
 */
static void zbus_on_battery_callback(const struct zbus_channel *chan);

/* Declare attribute list for Identify cluster (client). */
ZB_ZCL_DECLARE_IDENTIFY_CLIENT_ATTRIB_LIST(
	identify_client_attr_list);

/** @brief Declare attribute list for Identify cluster (server).
 */
ZB_ZCL_DECLARE_IDENTIFY_SERVER_ATTRIB_LIST(
    identify_server_attr_list,
    &dev_ctx.identify_attr.identify_time);

/** @brief
 */
ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST_EXT(
    basic_attr_list,
    &dev_ctx.basic_attr.zcl_version,
    &dev_ctx.basic_attr.app_version,
    &dev_ctx.basic_attr.stack_version,
    &dev_ctx.basic_attr.hw_version,
    dev_ctx.basic_attr.mf_name,
    dev_ctx.basic_attr.model_id,
    dev_ctx.basic_attr.date_code,
    &dev_ctx.basic_attr.power_source,
    dev_ctx.basic_attr.location_id,
    &dev_ctx.basic_attr.ph_env,
    dev_ctx.basic_attr.sw_ver);

/** @brief Power cluster attributes additions data.
 */
ZB_ZCL_DECLARE_POWER_CONFIG_BATTERY_ATTRIB_LIST_EXT(
    power_attr_list,
    &dev_ctx.power_attr.voltage,
    &dev_ctx.power_attr.size,
    &dev_ctx.power_attr.quantity,
    &dev_ctx.power_attr.rated_voltage,
    &dev_ctx.power_attr.alarm_mask,
    &dev_ctx.power_attr.voltage_min_threshold,
    &dev_ctx.power_attr.percent_remaining,
    &dev_ctx.power_attr.voltage_threshold_1,
    &dev_ctx.power_attr.voltage_threshold_2,
    &dev_ctx.power_attr.voltage_threshold_3,
    &dev_ctx.power_attr.percent_min_threshold,
    &dev_ctx.power_attr.percent_threshold_1,
    &dev_ctx.power_attr.percent_threshold_2,
    &dev_ctx.power_attr.percent_threshold_3,
    &dev_ctx.power_attr.alarm_state);

/** @brief Illuminance cluster attributes additions data.
 */
ZB_ZCL_DECLARE_ILLUMINANCE_MEASUREMENT_ATTRIB_LIST(
    light_sensor_attr_list,
    &dev_ctx.illuminance_attr.measurement_attr,
    &dev_ctx.illuminance_attr.min_attr,
    &dev_ctx.illuminance_attr.max_attr);

/** @brief Temperature cluster attributes additions data.
 */
ZB_ZCL_DECLARE_TEMP_MEASUREMENT_ATTRIB_LIST(
    temp_sensor_attr_list,
    &dev_ctx.temperature_attr.measurement_attr,
    &dev_ctx.temperature_attr.min_attr,
    &dev_ctx.temperature_attr.max_attr,
    &dev_ctx.temperature_attr.tolerance);

/** @brief Pressure cluster attributes additions data.
 */
ZB_ZCL_DECLARE_PRESSURE_MEASUREMENT_ATTRIB_LIST(
    pres_sensor_attr_list,
    &dev_ctx.pressure_attr.measurement_attr,
    &dev_ctx.pressure_attr.min_attr,
    &dev_ctx.pressure_attr.max_attr,
    &dev_ctx.pressure_attr.tolerance);

/** @brief Humidity cluster attributes additions data.
 */
ZB_ZCL_DECLARE_REL_HUMIDITY_MEASUREMENT_ATTRIB_LIST(
    hum_sensor_attr_list,
    &dev_ctx.humidity_attr.measurement_attr,
    &dev_ctx.humidity_attr.min_attr,
    &dev_ctx.humidity_attr.max_attr);

#if CONFIG_BME68X_IAQ
/** @brief Carbon Dioxide cluster attributes additions data.
 */
ZB_BEELIGHT_DECLARE_CO2_MEASUREMENT_ATTRIB_LIST(
    co2_sensor_attr_list,
    &dev_ctx.co2_attr.measurement_attr,
    &dev_ctx.co2_attr.min_attr,
    &dev_ctx.co2_attr.max_attr,
    &dev_ctx.co2_attr.tolerance);

/** @brief IAQ cluster attributes additions data.
 */
ZB_BEELIGHT_DECLARE_IAQ_MEASUREMENT_ATTRIB_LIST(
    iaq_sensor_attr_list,
    &dev_ctx.iaq_attr.measurement_attr,
    &dev_ctx.iaq_attr.min_attr,
    &dev_ctx.iaq_attr.max_attr,
    &dev_ctx.iaq_attr.tolerance);

/** @brief VOC cluster attributes additions data.
 */
ZB_BEELIGHT_DECLARE_VOC_MEASUREMENT_ATTRIB_LIST(
    voc_sensor_attr_list,
    &dev_ctx.voc_attr.measurement_attr,
    &dev_ctx.voc_attr.min_attr,
    &dev_ctx.voc_attr.max_attr,
    &dev_ctx.voc_attr.tolerance);
#endif /* CONFIG_BME68X_IAQ */

BEELIGHT_DECLARE_CLUSTER_LIST(
    env_sensor_clusters,
    basic_attr_list,
	identify_client_attr_list,
	identify_server_attr_list,
    power_attr_list,
    light_sensor_attr_list,
    temp_sensor_attr_list,
    pres_sensor_attr_list,
#if CONFIG_BME68X_IAQ
    hum_sensor_attr_list,
    co2_sensor_attr_list,
    iaq_sensor_attr_list,
    voc_sensor_attr_list
#else
    hum_sensor_attr_list
#endif /* CONFIG_BME68X_IAQ */
);

BEELIGHT_DECLARE_EP(
    env_sensor_ep,
    BEELIGHT_ENDPOINT,
    env_sensor_clusters);

ZBOSS_DECLARE_DEVICE_CTX_1_EP(
    env_sensor_ctx,
    env_sensor_ep);

ZBUS_CHAN_DECLARE(light_data_chan);
ZBUS_LISTENER_DEFINE(light_data_lis, zbus_on_light_callback);

ZBUS_CHAN_DECLARE(env_data_chan);
ZBUS_LISTENER_DEFINE(env_data_lis, zbus_on_env_callback);

ZBUS_CHAN_DECLARE(battery_data_chan);
ZBUS_LISTENER_DEFINE(battery_data_lis, zbus_on_battery_callback);

LOG_MODULE_REGISTER(app, LOG_LEVEL_DBG);

/** @brief          Function to toggle the identify LED.
 *  @param bufid    Unused parameter, required by ZBOSS scheduler API
 */
static void toggle_identify_led(zb_bufid_t bufid)
{
    ZB_SCHEDULE_APP_ALARM(toggle_identify_led, bufid, ZB_MILLISECONDS_TO_BEACON_INTERVAL(100));
}

/** @brief          Starts identifying the device.
 *  @param bufid    Unused parameter, required by ZBOSS scheduler API
 */
static void start_identifying(zb_bufid_t bufid)
{
    ZVUNUSED(bufid);

    if (ZB_JOINED()) {
        /* Check if endpoint is in identifying mode, if not, put desired endpoint in identifying mode. */
        if (dev_ctx.identify_attr.identify_time == ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE) {

            zb_ret_t zb_err_code = zb_bdb_finding_binding_target(BEELIGHT_ENDPOINT);

            if (zb_err_code == RET_OK) {
                LOG_INF("Enter identify mode");
            } else if (zb_err_code == RET_INVALID_STATE) {
                LOG_WRN("RET_INVALID_STATE - Cannot enter identify mode");
            } else {
                ZB_ERROR_CHECK(zb_err_code);
            }
        } else {
            LOG_INF("Cancel identify mode");
            zb_bdb_finding_binding_target_cancel();
        }
    } else {
        LOG_WRN("Device not in a network - cannot enter identify mode");
    }
}

static void on_Identify_Handler(zb_bufid_t bufid)
{
    zb_ret_t err = RET_OK;

    if (bufid) {
        /* Schedule a self-scheduling function that will toggle the LED */
        err = ZB_SCHEDULE_APP_CALLBACK(toggle_identify_led, bufid);
        if (err) {
            LOG_ERR("Failed to schedule app callback: %d", err);
        } else {
            LOG_INF("Enter identify mode");
        }
    } else {
        /* Cancel the toggling function alarm and turn off LED */
        err = ZB_SCHEDULE_APP_ALARM_CANCEL(toggle_identify_led,
                            ZB_ALARM_ANY_PARAM);
        if (err) {
            LOG_ERR("Failed to schedule app alarm cancel: %d", err);
        } else {
            dk_set_led_off(ZIGBEE_NETWORK_STATE_LED);
            LOG_INF("Cancel identify mode");
        }
    }
}

/** @brief              Callback for button events.
 *  @param button_state Bitmask containing the state of the buttons
 *  @param has_changed  Bitmask containing buttons that have changed their state
 */
static void on_button_changed_handler(uint32_t button_state, uint32_t has_changed)
{
    LOG_DBG("Button pressed");

    if (IDENTIFY_MODE_BUTTON & has_changed) {
        if (IDENTIFY_MODE_BUTTON & button_state) {
        }
        /* Button changed its state to pressed */
        else {
            /* Button changed its state to released */
            if (was_factory_reset_done()) {
                /* The long press was for Factory Reset */
                LOG_DBG("After Factory Reset - ignore button release");
            }
            /* Button released before Factory Reset */
            else {
                LOG_DBG("Start identifying");

                /* Start identification mode */
                ZB_SCHEDULE_APP_CALLBACK(start_identifying, 0);
            }
        }
    }

    check_factory_reset_button(button_state, has_changed);
}

/** @brief Function for initializing all clusters attributes.
 */
static void app_clusters_attr_init(void)
{
    /* Basic cluster attributes data */
    dev_ctx.basic_attr.zcl_version = ZB_ZCL_VERSION;
    dev_ctx.basic_attr.app_version = INIT_BASIC_APP_VERSION;
    dev_ctx.basic_attr.stack_version = INIT_BASIC_STACK_VERSION;

#ifdef CONFIG_BOARD_REVISION
    dev_ctx.basic_attr.hw_version = (zb_uint8_t)atoi(CONFIG_BOARD_REVISION);
#else
    dev_ctx.basic_attr.hw_version = INIT_BASIC_HW_VERSION;
#endif /* CONFIG_BOARD_REVISION */

    /* Use ZB_ZCL_SET_STRING_VAL to set strings, because the first byte */
    /* should contain string length without trailing zero. */
    /* For example "test" string will be encoded as: */
    /*   [(0x4), 't', 'e', 's', 't'] */
    ZB_ZCL_SET_STRING_VAL(
        dev_ctx.basic_attr.mf_name,
        INIT_BASIC_MANUF_NAME,
        ZB_ZCL_STRING_CONST_SIZE(INIT_BASIC_MANUF_NAME)
    );

    ZB_ZCL_SET_STRING_VAL(
        dev_ctx.basic_attr.model_id,
        INIT_BASIC_MODEL_ID,
        ZB_ZCL_STRING_CONST_SIZE(INIT_BASIC_MODEL_ID)
    );

    ZB_ZCL_SET_STRING_VAL(
        dev_ctx.basic_attr.date_code,
        INIT_BASIC_DATE_CODE,
        ZB_ZCL_STRING_CONST_SIZE(INIT_BASIC_DATE_CODE)
    );

    dev_ctx.basic_attr.power_source = ZB_ZCL_BASIC_POWER_SOURCE_BATTERY;

    ZB_ZCL_SET_STRING_VAL(
        dev_ctx.basic_attr.location_id,
        INIT_BASIC_LOCATION_DESC,
        ZB_ZCL_STRING_CONST_SIZE(INIT_BASIC_LOCATION_DESC)
    );

    dev_ctx.basic_attr.ph_env = ZB_ZCL_BASIC_ENV_UNSPECIFIED;

    /* Identify cluster attributes data. */
    dev_ctx.identify_attr.identify_time = ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE;

#if CONFIG_BME68X_IAQ
    /* Initialize IAQ/VOC/CO2 min and max with their INVALID sentinels so that
     * check_value_iaq_measurement_server does not reject measured values > 0. */
    dev_ctx.iaq_attr.min_attr = ZB_BEELIGHT_ATTR_IAQ_MEASUREMENT_MIN_VALUE_INVALID;
    dev_ctx.iaq_attr.max_attr = ZB_BEELIGHT_ATTR_IAQ_MEASUREMENT_MAX_VALUE_INVALID;
#endif /* CONFIG_BME68X_IAQ */

    /* Initialize the values for the Battery cluster attributes */
    dev_ctx.power_attr.size = ZB_ZCL_POWER_CONFIG_BATTERY_SIZE_CR2;
    dev_ctx.power_attr.quantity = 1;
    dev_ctx.power_attr.rated_voltage = BEELIGHT_RATED_VOLTAGE_MV / 100;

    ZB_ZCL_SET_ATTRIBUTE(
        BEELIGHT_ENDPOINT,
        ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_ID,
        (zb_uint8_t *)&dev_ctx.power_attr.voltage,
        ZB_FALSE
    );

    ZB_ZCL_SET_ATTRIBUTE(
        BEELIGHT_ENDPOINT,
        ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_REMAINING_ID,
        (zb_uint8_t *)&dev_ctx.power_attr.percent_remaining,
        ZB_FALSE
    );
}

/** @brief          Callback function for handling ZCL commands.
 *  @param bufid    Reference to Zigbee stack buffer used to pass received data
 */
static void on_zcl_cb_handler(zb_bufid_t bufid)
{
    zb_zcl_device_callback_param_t *device_cb_param = ZB_BUF_GET_PARAM(bufid, zb_zcl_device_callback_param_t);

    if (device_cb_param->device_cb_id == ZB_ZCL_OTA_UPGRADE_VALUE_CB_ID) {
#ifdef CONFIG_ZIGBEE_FOTA
        zigbee_fota_zcl_cb(bufid);
#endif /* CONFIG_ZIGBEE_FOTA */
    } else {
        device_cb_param->status = RET_NOT_IMPLEMENTED;
    }
}

/** @brief          Zigbee stack event handler.
 *  @param bufid    Reference to the Zigbee stack buffer used to pass signal
 */
void zboss_signal_handler(zb_bufid_t bufid)
{
    zb_zdo_app_signal_hdr_t *p_sg_p = NULL;
    zb_zdo_app_signal_type_t sig = zb_get_app_signal(bufid, &p_sg_p);
    zb_ret_t status = ZB_GET_APP_SIGNAL_STATUS(bufid);

    /* Update network status LED. */
    switch (sig) {
        case ZB_SIGNAL_JOIN_DONE: {
            dk_set_led_off(ZIGBEE_NETWORK_STATE_LED);
            if (status == RET_OK) {
                for (uint8_t i = 0; i < 4; i++) {
                    dk_set_led_on(ZIGBEE_NETWORK_STATE_LED);
                    k_msleep(100);
                    dk_set_led_off(ZIGBEE_NETWORK_STATE_LED);
                    k_msleep(100);
                }
            }

            break;
        }
        case ZB_ZDO_SIGNAL_LEAVE: {
            for (uint8_t i = 0; i < 2; i++) {
                dk_set_led_on(ZIGBEE_NETWORK_STATE_LED);
                k_msleep(100);
                dk_set_led_off(ZIGBEE_NETWORK_STATE_LED);
                k_msleep(100);
            }

            break;
        }
        case ZB_ZDO_SIGNAL_SKIP_STARTUP: {
            LOG_DBG("Add observers for sensors");
            zbus_chan_add_obs(&battery_data_chan, &battery_data_lis, K_MSEC(100));
            zbus_chan_add_obs(&light_data_chan, &light_data_lis, K_MSEC(100));
            zbus_chan_add_obs(&env_data_chan, &env_data_lis, K_MSEC(100));
        }
        default: {
            break;
        }
    }

    /* No application-specific behavior is required. Call default signal handler. */
    ZB_ERROR_CHECK(zigbee_default_signal_handler(bufid));

    /* All callbacks should either reuse or free passed buffers. */
    /* If bufid == 0, the buffer is invalid (not passed). */
    if (bufid) {
        zb_buf_free(bufid);
    }
}

void zbus_on_light_callback(const struct zbus_channel *chan)
{
    const struct light_event *evt = zbus_chan_const_msg(chan);

    /* Compute ZCL-scaled value; clamp 0 lux per spec */
    /* See chapter 4.2.2.2.1 if the ZCL Specification */
    zb_uint16_t measured = (evt->light == 0)
                           ? 0
                           : (zb_uint16_t)(10000 * log10((double)evt->light) + 1);

    /* Only update when values differ */
    if (measured == dev_ctx.illuminance_attr.measurement_attr) {
        return;
    }

    dev_ctx.illuminance_attr.measurement_attr = measured;

    if (evt->light > dev_ctx.illuminance_attr.max_attr) {
        dev_ctx.illuminance_attr.max_attr = dev_ctx.illuminance_attr.measurement_attr;
    } else if (evt->light < dev_ctx.illuminance_attr.min_attr) {
        dev_ctx.illuminance_attr.min_attr = dev_ctx.illuminance_attr.measurement_attr;
    }

    LOG_DBG("Light value: %u", dev_ctx.illuminance_attr.measurement_attr);
    LOG_DBG("Light min: %u", dev_ctx.illuminance_attr.min_attr);
    LOG_DBG("Light max: %u", dev_ctx.illuminance_attr.max_attr);

    ZB_ZCL_SET_ATTRIBUTE(
        BEELIGHT_ENDPOINT,
        ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT,
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_ATTR_ILLUMINANCE_MEASUREMENT_MEASURED_VALUE_ID,
        (zb_uint8_t *)&dev_ctx.illuminance_attr.measurement_attr,
        ZB_FALSE
    );

    ZB_ZCL_SET_ATTRIBUTE(
        BEELIGHT_ENDPOINT,
        ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT,
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_ATTR_ILLUMINANCE_MEASUREMENT_MIN_MEASURED_VALUE_ID,
        (zb_uint8_t *)&dev_ctx.illuminance_attr.min_attr,
        ZB_FALSE
    );

    ZB_ZCL_SET_ATTRIBUTE(
        BEELIGHT_ENDPOINT,
        ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT,
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_ATTR_ILLUMINANCE_MEASUREMENT_MAX_MEASURED_VALUE_ID,
        (zb_uint8_t *)&dev_ctx.illuminance_attr.max_attr,
        ZB_FALSE
    );
}

static void zbus_on_env_callback(const struct zbus_channel *chan)
{
    const struct env_event *evt = zbus_chan_const_msg(chan);

    /* Only update when values differ */
    if ((evt->temperature * 100) != dev_ctx.temperature_attr.measurement_attr) {
        /* See chapter 4.4.2.2.1 if the ZCL Specification */
        dev_ctx.temperature_attr.measurement_attr = (zb_uint16_t)(evt->temperature * 100);

        // TODO: Add Min / Max

        ZB_ZCL_SET_ATTRIBUTE(
            BEELIGHT_ENDPOINT,
            ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,
            ZB_ZCL_CLUSTER_SERVER_ROLE,
            ZB_ZCL_ATTR_TEMP_MEASUREMENT_VALUE_ID,
            (zb_uint8_t *)&dev_ctx.temperature_attr.measurement_attr,
            ZB_FALSE
        );

        LOG_DBG("Temperature: %u", dev_ctx.temperature_attr.measurement_attr);
    }

    if ((evt->pressure / 100) != dev_ctx.pressure_attr.measurement_attr) {
        /* See chapter 4.5.2.2.1 if the ZCL Specification */
        dev_ctx.pressure_attr.measurement_attr = (zb_uint16_t)(evt->pressure / 100);

        // TODO: Add Min / Max

        ZB_ZCL_SET_ATTRIBUTE(
            BEELIGHT_ENDPOINT,
            ZB_ZCL_CLUSTER_ID_PRESSURE_MEASUREMENT,
            ZB_ZCL_CLUSTER_SERVER_ROLE,
            ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_VALUE_ID,
            (zb_uint8_t *)&dev_ctx.pressure_attr.measurement_attr,
            ZB_FALSE
        );

        LOG_DBG("Pressure: %u", dev_ctx.pressure_attr.measurement_attr);
    }

    if ((evt->humidity * 100) != dev_ctx.humidity_attr.measurement_attr) {
        /* See chapter 4.7.2.1.1 if the ZCL Specification */
        dev_ctx.humidity_attr.measurement_attr = (zb_uint16_t)(evt->humidity * 100);

        // TODO: Add Min / Max

        ZB_ZCL_SET_ATTRIBUTE(
            BEELIGHT_ENDPOINT,
            ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT,
            ZB_ZCL_CLUSTER_SERVER_ROLE,
            ZB_ZCL_ATTR_REL_HUMIDITY_MEASUREMENT_VALUE_ID,
            (zb_uint8_t *)&dev_ctx.humidity_attr.measurement_attr,
            ZB_FALSE
        );

        LOG_DBG("Humidity: %u", dev_ctx.humidity_attr.measurement_attr);
    }

#if CONFIG_BME68X_IAQ
    /* TODO: Add tolerance */
    if ((evt->co2.value != dev_ctx.co2_attr.measurement_attr)) {
        dev_ctx.co2_attr.measurement_attr = (zb_uint16_t)evt->co2.value;
        dev_ctx.co2_attr.tolerance = (zb_uint8_t)evt->co2.tolerance;

        ZB_ZCL_SET_ATTRIBUTE(
            BEELIGHT_ENDPOINT,
            ZB_BEELIGHT_CLUSTER_ID_CO2_MEASUREMENT,
            ZB_ZCL_CLUSTER_SERVER_ROLE,
            ZB_BEELIGHT_ATTR_CO2_MEASUREMENT_VALUE_ID,
            (zb_uint8_t *)&dev_ctx.co2_attr.measurement_attr,
            ZB_FALSE
        );

        LOG_DBG("CO2: %u", dev_ctx.co2_attr.measurement_attr);
    }

    /* TODO: Add tolerance */
    if ((evt->iaq.value != dev_ctx.iaq_attr.measurement_attr)) {
        dev_ctx.iaq_attr.measurement_attr = (zb_uint16_t)evt->iaq.value;
        dev_ctx.iaq_attr.tolerance = (zb_uint8_t)evt->iaq.tolerance;

        ZB_ZCL_SET_ATTRIBUTE(
            BEELIGHT_ENDPOINT,
            ZB_BEELIGHT_CLUSTER_ID_IAQ_MEASUREMENT,
            ZB_ZCL_CLUSTER_SERVER_ROLE,
            ZB_BEELIGHT_ATTR_IAQ_MEASUREMENT_VALUE_ID,
            (zb_uint8_t *)&dev_ctx.iaq_attr.measurement_attr,
            ZB_FALSE
        );

        LOG_DBG("IAQ: %u", dev_ctx.iaq_attr.measurement_attr);
    }

    /* TODO: Add tolerance */
    if ((evt->voc.value != dev_ctx.voc_attr.measurement_attr)) {
        dev_ctx.voc_attr.measurement_attr = (zb_uint16_t)evt->voc.value;
        dev_ctx.voc_attr.tolerance = (zb_uint8_t)evt->voc.tolerance;

        ZB_ZCL_SET_ATTRIBUTE(
            BEELIGHT_ENDPOINT,
            ZB_BEELIGHT_CLUSTER_ID_VOC_MEASUREMENT,
            ZB_ZCL_CLUSTER_SERVER_ROLE,
            ZB_BEELIGHT_ATTR_VOC_MEASUREMENT_VALUE_ID,
            (zb_uint8_t *)&dev_ctx.voc_attr.measurement_attr,
            ZB_FALSE
        );

        LOG_DBG("VOC: %u", dev_ctx.voc_attr.measurement_attr);
    }
#endif /* CONFIG_BME68X_IAQ */
}

static void zbus_on_battery_callback(const struct zbus_channel *chan)
{
    int32_t remaining;
    const struct battery_event *evt = zbus_chan_const_msg(chan);

    /* Only update when values differ */
    if (evt->voltage == dev_ctx.power_attr.voltage) {
        return;
    }

    dev_ctx.power_attr.voltage = evt->voltage / 100;

    /* We need the difference between empty and full for the remaining percent */
    /* A CR2032 counts as empty when a voltage of 2.7 V is reached */
    /* Formula: (Voltage - Min. Voltage) [V] / (Rated voltage - Min. Voltage) [V] * 100 */
    remaining = (evt->voltage - BEELIGHT_EMPTY_VOLTAGE_MV);

    if (remaining < 0) {
        remaining = 0;
    } else {
        remaining = (remaining * 100) / (BEELIGHT_RATED_VOLTAGE_MV - BEELIGHT_EMPTY_VOLTAGE_MV);
        if (remaining > 100) {
            remaining = 100;
        }
    }

    dev_ctx.power_attr.percent_remaining = remaining * 2;

    LOG_DBG("Battery voltage: %u", dev_ctx.power_attr.voltage);
    LOG_DBG("Battery remaining: %u", dev_ctx.power_attr.percent_remaining);

    ZB_ZCL_SET_ATTRIBUTE(
        BEELIGHT_ENDPOINT,
        ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_ID,
        (zb_uint8_t *)&dev_ctx.power_attr.voltage,
        ZB_FALSE
    );

    ZB_ZCL_SET_ATTRIBUTE(
        BEELIGHT_ENDPOINT,
        ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_REMAINING_ID,
        (zb_uint8_t *)&dev_ctx.power_attr.percent_remaining,
        ZB_FALSE
    );
}

int main(void)
{
    dk_leds_init();
    dk_buttons_init(on_button_changed_handler);

    register_factory_reset_button(FACTORY_RESET_BUTTON);

    zb_set_ed_timeout(ED_AGING_TIMEOUT_64MIN);
    zb_set_keepalive_timeout(ZB_MILLISECONDS_TO_BEACON_INTERVAL(1 * 60 * 1000));
    zb_zdo_pim_set_long_poll_interval(ZB_MILLISECONDS_TO_BEACON_INTERVAL(10 * 60 * 1000));

#ifdef CONFIG_ZIGBEE_FOTA
#error "Not supported yet!"
    /* Initialize Zigbee FOTA download service */
    zigbee_fota_init(on_zcl_ota_evt_handler);

    /* Mark the current firmware as valid */
    confirm_image();

    /* Register callback for handling ZCL commands */
    ZB_ZCL_REGISTER_DEVICE_CB(on_zcl_cb_handler);
#endif /* CONFIG_ZIGBEE_FOTA */

    /* Register device context (endpoints) */
    ZB_AF_REGISTER_DEVICE_CTX(&env_sensor_ctx);

    app_clusters_attr_init();

    /* Register handler to identify notifications */
    ZB_AF_SET_IDENTIFY_NOTIFICATION_HANDLER(BEELIGHT_ENDPOINT, on_Identify_Handler);

    /* Power off unused sections of RAM to lower device power consumption */
    zb_set_rx_on_when_idle(ZB_FALSE);
    if (IS_ENABLED(CONFIG_RAM_POWER_DOWN_LIBRARY)) {
        power_down_unused_ram();
    }

    /* Start Zigbee default thread */
    zigbee_enable();

    LOG_INF("BeeLight application started");

#if CONFIG_BME68X_IAQ
    LOG_INF("BME68X IAQ support enabled - ensure that the sensor is connected and configured correctly");
#endif

#if CONFIG_APDS9306
    LOG_INF("APDS9306 support enabled - ensure that the sensor is connected and configured correctly");
#endif

    /* Put the MCU into power down mode */
    while (1) {
        k_sleep(K_FOREVER);
    }

    return 0;
}
