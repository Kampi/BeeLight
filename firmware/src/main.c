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
#include <zb_nrf_platform.h>
#include <zboss_api_addons.h>
#include <zb_mem_config_med.h>
#include <zigbee/zigbee_app_utils.h>
#include <zigbee/zigbee_error_handler.h>

#include "events.h"
#include "beelight.h"

/* LED indicating that light switch successfully joind Zigbee network. */
#define ZIGBEE_NETWORK_STATE_LED        DK_LED1

/* Version of the application software (1 byte). */
#define INIT_BASIC_APP_VERSION          01

/* Version of the implementation of the Zigbee stack (1 byte). */
#define INIT_BASIC_STACK_VERSION        10

/* Version of the hardware of the device (1 byte). */
#define INIT_BASIC_HW_VERSION           01

/* Manufacturer name (32 bytes). */
#define INIT_BASIC_MANUF_NAME           "Kampi"

/* Model number assigned by manufacturer (32-bytes long string). */
#define INIT_BASIC_MODEL_ID             "BeeLight_v1.0"

/* First 8 bytes specify the date of manufacturer of the device
 * in ISO 8601 format (YYYYMMDD). The rest (8 bytes) are manufacturer specific.
 */
#define INIT_BASIC_DATE_CODE            "20200329"

/* Describes the physical location of the device (16 bytes).
 * May be modified during commissioning process.
 */
#define INIT_BASIC_LOCATION_DESC        "Office desk"

/* Button used to enter the Identify mode. */
#define IDENTIFY_MODE_BUTTON            DK_BTN1_MSK

/* Button to start Factory Reset. */
#define FACTORY_RESET_BUTTON            IDENTIFY_MODE_BUTTON

/* Define 'bat_num' as empty in order to declare default battery set attributes. */
/* According to Table 3-17 of ZCL specification, defining 'bat_num' as 2 or 3 allows */
/* to declare battery set attributes for BATTERY2 and BATTERY3. */
#define bat_num

LOG_MODULE_REGISTER(app, LOG_LEVEL_DBG);

/** @brief Zigbee device application context storage.
 */
static device_ctx_t dev_ctx;

/** @brief
 */
static void zbus_on_light_callback(const struct zbus_channel *chan);

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

ZB_DECLARE_LIGHT_SENSOR_CLUSTER_LIST(
    light_sensor_clusters,
    basic_attr_list,
    identify_client_attr_list,
    identify_server_attr_list,
    power_attr_list,
    light_sensor_attr_list);

ZB_DECLARE_LIGHT_SENSOR_EP(
    light_sensor_ep,
    LIGHT_SENSOR_ENDPOINT,
    light_sensor_clusters);

ZBOSS_DECLARE_DEVICE_CTX_1_EP(
    light_sensor_ctx,
    light_sensor_ep);

ZBUS_CHAN_DECLARE(light_data_chan);
ZBUS_LISTENER_DEFINE(light_data_lis, zbus_on_light_callback);

ZBUS_CHAN_DECLARE(battery_data_chan);
ZBUS_LISTENER_DEFINE(battery_data_lis, zbus_on_battery_callback);

/** @brief          Starts identifying the device.
 *  @param bufid    Unused parameter, required by ZBOSS scheduler API
 */
static void start_identifying(zb_bufid_t bufid)
{
    ZVUNUSED(bufid);

    if (ZB_JOINED()) {
        /* Check if endpoint is in identifying mode, if not, put desired endpoint in identifying mode. */
        if (dev_ctx.identify_attr.identify_time == ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE) {

            zb_ret_t zb_err_code = zb_bdb_finding_binding_target(LIGHT_SENSOR_ENDPOINT);

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
    static int blink_status;

    // light_bulb_set_brightness(((++blink_status) % 2) ? (255U) : (0U));
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
    dev_ctx.power_attr.voltage = 30;
    dev_ctx.power_attr.percent_remaining = 200;
    dev_ctx.power_attr.quantity = 1;
    dev_ctx.power_attr.rated_voltage = 30;

    ZB_ZCL_SET_ATTRIBUTE(
        LIGHT_SENSOR_ENDPOINT,
        ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_REMAINING_ID,
        (zb_uint8_t *)&dev_ctx.power_attr.percent_remaining,
        ZB_FALSE
    );

    /* Initialize the values for the Illuminance measurement cluster attributes */
/*
    dev_ctx.illuminance_min_attr = 11;
    ZB_ZCL_SET_ATTRIBUTE(
        LIGHT_SENSOR_ENDPOINT,
        ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT,
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_ATTR_ILLUMINANCE_MEASUREMENT_MIN_MEASURED_VALUE_ID,
        (zb_uint8_t *)&dev_ctx.illuminance_min_attr,
        ZB_FALSE
    );

    dev_ctx.illuminance_max_attr = 200;
    ZB_ZCL_SET_ATTRIBUTE(
        LIGHT_SENSOR_ENDPOINT,
        ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT,
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_ATTR_ILLUMINANCE_MEASUREMENT_MAX_MEASURED_VALUE_ID,
        (zb_uint8_t *)&dev_ctx.illuminance_max_attr,
        ZB_FALSE
    );
*/
}

/** @brief          Callback function for handling ZCL commands.
 *  @param bufid    Reference to Zigbee stack buffer used to pass received data
 */
static void zcl_device_cb(zb_bufid_t bufid)
{
    zb_uint8_t attr_id;
    zb_uint8_t cluster_id;
    zb_zcl_device_callback_param_t *device_cb_param = ZB_BUF_GET_PARAM(bufid, zb_zcl_device_callback_param_t);

    LOG_INF("%s id %hd", __func__, device_cb_param->device_cb_id);

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

    LOG_INF("%s status: %hd", __func__, device_cb_param->status);
}

/** @brief          Zigbee stack event handler.
 *  @param bufid    Reference to the Zigbee stack buffer used to pass signal
 */
void zboss_signal_handler(zb_bufid_t bufid)
{
    /* Update network status LED. */
    zigbee_led_status_update(bufid, ZIGBEE_NETWORK_STATE_LED);

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

    LOG_DBG("Value from light sensor: %u", evt->value);

    dev_ctx.illuminance_attr.measurement_attr = evt->value;
    ZB_ZCL_SET_ATTRIBUTE(
        LIGHT_SENSOR_ENDPOINT,
        ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT,
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_ATTR_ILLUMINANCE_MEASUREMENT_MEASURED_VALUE_ID,
        (zb_uint8_t *)&dev_ctx.illuminance_attr.measurement_attr,
        ZB_FALSE
    );
}

static void zbus_on_battery_callback(const struct zbus_channel *chan)
{
    const struct battery_event *evt = zbus_chan_const_msg(chan);

    LOG_DBG("Value from battery sensor: %u", evt->value);

    /* Initialize the values for the Battery cluster attributes */
    dev_ctx.power_attr.voltage = 30;
    dev_ctx.power_attr.percent_remaining = 200;
    dev_ctx.power_attr.rated_voltage = 30;

    ZB_ZCL_SET_ATTRIBUTE(
        LIGHT_SENSOR_ENDPOINT,
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

    /* Register dimmer switch device context (endpoints). */
    ZB_AF_REGISTER_DEVICE_CTX(&light_sensor_ctx);

    clusters_attr_init();

    /* Register handler to identify notifications. */
    ZB_AF_SET_IDENTIFY_NOTIFICATION_HANDLER(LIGHT_SENSOR_ENDPOINT, identify_cb);

    /* Start Zigbee default thread. */
    zigbee_enable();

    zbus_chan_add_obs(&light_data_chan, &light_data_lis, K_MSEC(100));
    zbus_chan_add_obs(&battery_data_chan, &battery_data_lis, K_MSEC(100));

    LOG_INF("BeeLight application started");

    return 0;
}
