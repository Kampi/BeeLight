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
#include <dk_buttons_and_leds.h>

#include <zboss_api.h>
#include <zboss_api_addons.h>
#include <zb_nrf_platform.h>
#include <zb_mem_config_med.h>
#include <zigbee/zigbee_app_utils.h>
#include <zigbee/zigbee_error_handler.h>

#include "events.h"
#include "beelight.h"

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

/** @brief Declare attribute list for Identify cluster (client).
 */
ZB_ZCL_DECLARE_IDENTIFY_CLIENT_ATTRIB_LIST(identify_client_attr_list);

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

/** @brief Carbon Dioxide cluster attributes additions data.
 */
ZB_ZCL_DECLARE_CO2_MEASUREMENT_ATTRIB_LIST(
    co2_sensor_attr_list,
    &dev_ctx.co2_attr.measurement_attr,
    &dev_ctx.co2_attr.min_attr,
    &dev_ctx.co2_attr.max_attr,
    &dev_ctx.co2_attr.tolerance);

/** @brief IAQ cluster attributes additions data.
 */
ZB_ZCL_DECLARE_IAQ_MEASUREMENT_ATTRIB_LIST(
    iaq_sensor_attr_list,
    &dev_ctx.iaq_attr.measurement_attr,
    &dev_ctx.iaq_attr.min_attr,
    &dev_ctx.iaq_attr.max_attr,
    &dev_ctx.iaq_attr.tolerance);

/** @brief VOC cluster attributes additions data.
 */
ZB_ZCL_DECLARE_VOC_MEASUREMENT_ATTRIB_LIST(
    voc_sensor_attr_list,
    &dev_ctx.voc_attr.measurement_attr,
    &dev_ctx.voc_attr.min_attr,
    &dev_ctx.voc_attr.max_attr,
    &dev_ctx.voc_attr.tolerance);

ZB_DECLARE_ENV_SENSOR_CLUSTER_LIST(
    env_sensor_clusters,
    basic_attr_list,
    identify_client_attr_list,
    identify_server_attr_list,
    power_attr_list,
    light_sensor_attr_list,
    temp_sensor_attr_list,
    pres_sensor_attr_list,
    hum_sensor_attr_list,
    co2_sensor_attr_list,
    iaq_sensor_attr_list,
    voc_sensor_attr_list);

ZB_DECLARE_ENV_SENSOR_EP(
    env_sensor_ep,
    SENSOR_ENDPOINT,
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

/** @brief          Starts identifying the device.
 *  @param bufid    Unused parameter, required by ZBOSS scheduler API
 */
static void start_identifying(zb_bufid_t bufid)
{
    ZVUNUSED(bufid);

    if (ZB_JOINED()) {
        /* Check if endpoint is in identifying mode, if not, put desired endpoint in identifying mode. */
        if (dev_ctx.identify_attr.identify_time == ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE) {

            zb_ret_t zb_err_code = zb_bdb_finding_binding_target(SENSOR_ENDPOINT);

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

/** @brief              Callback for button events.
 *  @param button_state Bitmask containing the state of the buttons
 *  @param has_changed  Bitmask containing buttons that have changed their state
 */
static void button_changed(uint32_t button_state, uint32_t has_changed)
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

/** @brief          Function to toggle the identify LED.
 *  @param bufid    Unused parameter, required by ZBOSS scheduler API
 */
static void toggle_identify_led(zb_bufid_t bufid)
{
    ZB_SCHEDULE_APP_ALARM(toggle_identify_led, bufid, ZB_MILLISECONDS_TO_BEACON_INTERVAL(100));
}

/** @brief          Function to handle identify notification events.
 *  @param bufid    Unused parameter, required by ZBOSS scheduler API
 */
static void identify_cb(zb_bufid_t bufid)
{
    if (bufid) {
        /* Schedule a self-scheduling function that will toggle the LED. */
        ZB_SCHEDULE_APP_CALLBACK(toggle_identify_led, bufid);
    } else {
        zb_ret_t zb_err_code;

        /* Cancel the toggling function alarm and restore current Zigbee LED state. */
        zb_err_code = ZB_SCHEDULE_APP_ALARM_CANCEL(toggle_identify_led, ZB_ALARM_ANY_PARAM);
        ZVUNUSED(zb_err_code);

        /*
        if (dev_ctx.on_off_attr.on_off) {
            light_bulb_set_brightness(dev_ctx.level_control_attr.current_level);
        } else {
            light_bulb_set_brightness(0U);
        }*/
    }
}

/** @brief Function for initializing all clusters attributes.
 */
static void clusters_attr_init(void)
{
    /* Basic cluster attributes data */
    dev_ctx.basic_attr.zcl_version = ZB_ZCL_VERSION;
    dev_ctx.basic_attr.app_version = INIT_BASIC_APP_VERSION;
    dev_ctx.basic_attr.stack_version = INIT_BASIC_STACK_VERSION;
    dev_ctx.basic_attr.hw_version = INIT_BASIC_HW_VERSION;

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

    /* Initialize the values for the Battery cluster attributes */
    dev_ctx.power_attr.size = ZB_ZCL_POWER_CONFIG_BATTERY_SIZE_CR2;
    dev_ctx.power_attr.quantity = 1;
    dev_ctx.power_attr.rated_voltage = SENSOR_RATET_VOLTAGE / 100;

    ZB_ZCL_SET_ATTRIBUTE(
        SENSOR_ENDPOINT,
        ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_ID,
        (zb_uint8_t *)&dev_ctx.power_attr.voltage,
        ZB_FALSE
    );

    ZB_ZCL_SET_ATTRIBUTE(
        SENSOR_ENDPOINT,
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
static void zcl_device_cb(zb_bufid_t bufid)
{
    zb_uint8_t attr_id;
    zb_uint8_t cluster_id;
    zb_zcl_device_callback_param_t *device_cb_param = ZB_BUF_GET_PARAM(bufid, zb_zcl_device_callback_param_t);

    LOG_INF("Callback ID: %hd", device_cb_param->device_cb_id);

    /* Set default response value. */
    device_cb_param->status = RET_OK;

    switch (device_cb_param->device_cb_id)
    {
        case ZB_ZCL_SET_ATTR_VALUE_CB_ID:
        {
            cluster_id = device_cb_param->cb_param.set_attr_value_param.cluster_id;
            attr_id = device_cb_param->cb_param.set_attr_value_param.attr_id;

            break;
        }
        default:
        {
            device_cb_param->status = RET_NOT_IMPLEMENTED;
            break;
        }
    }

    LOG_INF("Status: %hd", device_cb_param->status);
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
        case ZB_SIGNAL_JOIN_DONE:
            dk_set_led_off(ZIGBEE_NETWORK_STATE_LED);
            if (status == RET_OK) {
                for(uint8_t i = 0; i < 4; i++) {
                    dk_set_led_on(ZIGBEE_NETWORK_STATE_LED);
                    k_msleep(100);
                    dk_set_led_off(ZIGBEE_NETWORK_STATE_LED);
                    k_msleep(100);
                }
            }

            break;

        case ZB_ZDO_SIGNAL_LEAVE:
            for(uint8_t i = 0; i < 2; i++) {
                dk_set_led_on(ZIGBEE_NETWORK_STATE_LED);
                k_msleep(100);
                dk_set_led_off(ZIGBEE_NETWORK_STATE_LED);
                k_msleep(100);
            }
            break;

        default:
            break;
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

    dev_ctx.illuminance_attr.measurement_attr = evt->value;

    if(evt->value > dev_ctx.illuminance_attr.max_attr) {
        dev_ctx.illuminance_attr.max_attr = evt->value;
    }

    if(evt->value < dev_ctx.illuminance_attr.min_attr) {
        dev_ctx.illuminance_attr.min_attr = evt->value;
    }

    LOG_DBG("Light value: %u", dev_ctx.illuminance_attr.measurement_attr);
    LOG_DBG("Light min: %u", dev_ctx.illuminance_attr.min_attr);
    LOG_DBG("Light max: %u", dev_ctx.illuminance_attr.max_attr);

    ZB_ZCL_SET_ATTRIBUTE(
        SENSOR_ENDPOINT,
        ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT,
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_ATTR_ILLUMINANCE_MEASUREMENT_MEASURED_VALUE_ID,
        (zb_uint8_t *)&dev_ctx.illuminance_attr.measurement_attr,
        ZB_FALSE
    );

    ZB_ZCL_SET_ATTRIBUTE(
        SENSOR_ENDPOINT,
        ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT,
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_ATTR_ILLUMINANCE_MEASUREMENT_MIN_MEASURED_VALUE_ID,
        (zb_uint8_t *)&dev_ctx.illuminance_attr.min_attr,
        ZB_FALSE
    );

    ZB_ZCL_SET_ATTRIBUTE(
        SENSOR_ENDPOINT,
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

    dev_ctx.temperature_attr.measurement_attr = (zb_uint16_t)(evt->temperature * 100);

    dev_ctx.pressure_attr.measurement_attr = (zb_uint16_t)(evt->pressure / 100);

    dev_ctx.humidity_attr.measurement_attr = (zb_uint16_t)(evt->humidity * 100);

    dev_ctx.co2_attr.measurement_attr = (zb_uint16_t)evt->co2.value;
    dev_ctx.co2_attr.tolerance = (zb_uint8_t)evt->co2.tolerance;

    dev_ctx.iaq_attr.measurement_attr = (zb_uint16_t)evt->iaq.value;
    dev_ctx.iaq_attr.tolerance = (zb_uint8_t)evt->iaq.tolerance;

    dev_ctx.voc_attr.measurement_attr = (zb_uint16_t)evt->voc.value;
    dev_ctx.voc_attr.tolerance = (zb_uint8_t)evt->voc.tolerance;

    LOG_DBG("Temperature: %u", dev_ctx.temperature_attr.measurement_attr);
    LOG_DBG("Pressure: %u", dev_ctx.pressure_attr.measurement_attr);
    LOG_DBG("Humidity: %u", dev_ctx.humidity_attr.measurement_attr);
    LOG_DBG("CO2: %u", dev_ctx.co2_attr.measurement_attr);
    LOG_DBG("IAQ: %u", dev_ctx.iaq_attr.measurement_attr);
    LOG_DBG("VOC: %u", dev_ctx.voc_attr.measurement_attr);

    ZB_ZCL_SET_ATTRIBUTE(
        SENSOR_ENDPOINT,
        ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_ATTR_TEMP_MEASUREMENT_VALUE_ID,
        (zb_uint8_t *)&dev_ctx.temperature_attr.measurement_attr,
        ZB_FALSE
    );

    ZB_ZCL_SET_ATTRIBUTE(
        SENSOR_ENDPOINT,
        ZB_ZCL_CLUSTER_ID_PRESSURE_MEASUREMENT,
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_VALUE_ID,
        (zb_uint8_t *)&dev_ctx.pressure_attr.measurement_attr,
        ZB_FALSE
    );

    ZB_ZCL_SET_ATTRIBUTE(
        SENSOR_ENDPOINT,
        ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT,
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_ATTR_REL_HUMIDITY_MEASUREMENT_VALUE_ID,
        (zb_uint8_t *)&dev_ctx.humidity_attr.measurement_attr,
        ZB_FALSE
    );

    ZB_ZCL_SET_ATTRIBUTE(
        SENSOR_ENDPOINT,
        ZB_ZCL_CLUSTER_ID_CO2_MEASUREMENT,
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_ATTR_CO2_MEASUREMENT_VALUE_ID,
        (zb_uint8_t *)&dev_ctx.co2_attr.measurement_attr,
        ZB_FALSE
    );

    ZB_ZCL_SET_ATTRIBUTE(
        SENSOR_ENDPOINT,
        ZB_ZCL_CLUSTER_ID_IAQ_MEASUREMENT,
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_ATTR_IAQ_MEASUREMENT_VALUE_ID,
        (zb_uint8_t *)&dev_ctx.iaq_attr.measurement_attr,
        ZB_FALSE
    );

    ZB_ZCL_SET_ATTRIBUTE(
        SENSOR_ENDPOINT,
        ZB_ZCL_CLUSTER_ID_VOC_MEASUREMENT,
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_ATTR_VOC_MEASUREMENT_VALUE_ID,
        (zb_uint8_t *)&dev_ctx.voc_attr.measurement_attr,
        ZB_FALSE
    );
}

static void zbus_on_battery_callback(const struct zbus_channel *chan)
{
    const struct battery_event *evt = zbus_chan_const_msg(chan);

    dev_ctx.power_attr.voltage = evt->mV / 100;
    dev_ctx.power_attr.percent_remaining = (zb_uint8_t)((((float)evt->mV) / SENSOR_RATET_VOLTAGE) * 100 * 2);
    if(dev_ctx.power_attr.percent_remaining > 200) {
        dev_ctx.power_attr.percent_remaining = 200;
    }

    LOG_DBG("Battery voltage: %u", dev_ctx.power_attr.voltage);
    LOG_DBG("Battery remaining: %u", dev_ctx.power_attr.percent_remaining);

    ZB_ZCL_SET_ATTRIBUTE(
        SENSOR_ENDPOINT,
        ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_ID,
        (zb_uint8_t *)&dev_ctx.power_attr.voltage,
        ZB_FALSE
    );

    ZB_ZCL_SET_ATTRIBUTE(
        SENSOR_ENDPOINT,
        ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_REMAINING_ID,
        (zb_uint8_t *)&dev_ctx.power_attr.percent_remaining,
        ZB_FALSE
    );
}

int main(void)
{
    dk_buttons_init(button_changed);
    dk_leds_init();

    register_factory_reset_button(FACTORY_RESET_BUTTON);

    /* Register callback for handling ZCL commands. */
    ZB_ZCL_REGISTER_DEVICE_CB(zcl_device_cb);

    /* Register device context (endpoints). */
    ZB_AF_REGISTER_DEVICE_CTX(&env_sensor_ctx);

    clusters_attr_init();

    /* Register handler to identify notifications. */
    ZB_AF_SET_IDENTIFY_NOTIFICATION_HANDLER(SENSOR_ENDPOINT, identify_cb);

    /* Start Zigbee default thread. */
    zigbee_enable();

    zbus_chan_add_obs(&light_data_chan, &light_data_lis, K_MSEC(100));
    zbus_chan_add_obs(&battery_data_chan, &battery_data_lis, K_MSEC(100));
    zbus_chan_add_obs(&env_data_chan, &env_data_lis, K_MSEC(100));

    LOG_INF("BeeLight application started");

    return 0;
}
