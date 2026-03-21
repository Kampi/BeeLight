/*
 * This file is part of the BeeLight project <https://github.com/Kampi/BeeLight>.
 * Copyright (c) 2026 Daniel Kampert.
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

#pragma once

/* Version of the application software (1 byte). */
#define INIT_BASIC_APP_VERSION                  1

/* Version of the implementation of the Zigbee stack (1 byte). */
#define INIT_BASIC_STACK_VERSION                10

/* Version of the hardware of the device (1 byte). */
#define INIT_BASIC_HW_VERSION                   1

/* Manufacturer name (32 bytes). */
#define INIT_BASIC_MANUF_NAME                   "Kampi"

/* Model number assigned by manufacturer (32-bytes long string). */
#define INIT_BASIC_MODEL_ID                     "BeeLight_v2"

/* First 8 bytes specify the date of manufacturer of the device
 * in ISO 8601 format (YYYYMMDD). The rest (8 bytes) are manufacturer specific.
 */
#ifdef CONFIG_DEBUG
#define INIT_BASIC_DATE_CODE                    "20251019_Debug"
#else
#define INIT_BASIC_DATE_CODE                    "20251019_Release"
#endif

/* Describes the physical location of the device (16 bytes).
 * May be modified during commissioning process.
 */
#define INIT_BASIC_LOCATION_DESC                "Office desk"

/* Button used to enter the Identify mode. */
#define IDENTIFY_MODE_BUTTON                    DK_BTN1_MSK

/* LED indicating that light switch successfully joined Zigbee network. */
#define ZIGBEE_NETWORK_STATE_LED                DK_LED1

/* Button to start Factory Reset. */
#define FACTORY_RESET_BUTTON                    IDENTIFY_MODE_BUTTON

/* Define 'bat_num' as empty in order to declare default battery set attributes. */
/* According to Table 3-17 of ZCL specification, defining 'bat_num' as 2 or 3 allows */
/* to declare battery set attributes for BATTERY2 and BATTERY3. */
#define bat_num

/** @brief Device endpoint, used to receive environment sensor results.
 */
#define BEELIGHT_ENDPOINT                       10

/** @brief The rated battery voltage in millivolts.
 */
#define BEELIGHT_RATED_VOLTAGE_MV               3300UL

/** @brief The lowest battery voltage in millivolts.
 */
#define BEELIGHT_EMPTY_VOLTAGE_MV               2700UL

/** @brief ZigBee device version.
*/
#define ZB_DEVICE_VER                           1

/** @brief  ZigBee profile ID used
 *          See ZIGBEE HOME AUTOMATION PUBLIC APPLICATION PROFILE table 5.1 for additional information.
*/
#define ZB_PROFILE_ID                           ZB_AF_HA_PROFILE_ID

/** @brief  ZigBee device ID
 *          See ZIGBEE HOME AUTOMATION PUBLIC APPLICATION PROFILE table 5.1 for additional information.
*/
#define ZB_DEVICE_ID                            0x106

/** @brief                  Declare endpoint for the Environment Sensor device.
 *  @param ep_name          Endpoint variable name
 *  @param ep_id            Endpoint ID
 *  @param cluster_list     Endpoint cluster list
 */
#define BEELIGHT_DECLARE_EP(ep_name, ep_id, cluster_list)                       \
    BEELIGHT_DECLARE_SIMPLE_DESC(                                               \
        ep_name,                                                                \
        ep_id,                                                                  \
        BEELIGHT_IN_CLUSTER_NUM,                                                \
        BEELIGHT_OUT_CLUSTER_NUM                                                \
    );                                                                          \
    ZBOSS_DEVICE_DECLARE_REPORTING_CTX(                                         \
        reporting_info##ep_name,                                                \
        BEELIGHT_REPORT_ATTR_COUNT                                              \
    );                                                                          \
    ZB_AF_DECLARE_ENDPOINT_DESC(                                                \
        ep_name,                                                                \
        ep_id,                                                                  \
        ZB_PROFILE_ID,                                                          \
        0,                                                                      \
        NULL,                                                                   \
        ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t),                 \
        cluster_list,                                                           \
        (zb_af_simple_desc_1_1_t *)&simple_desc_##ep_name,                      \
        BEELIGHT_REPORT_ATTR_COUNT,                                             \
        reporting_info##ep_name,                                                \
        0,                                                                      \
        NULL                                                                    \
    )

/** @brief 3.3 Power Attributes
*/
typedef struct {
    zb_uint8_t voltage;                 /**< Attribute 3.3.2.2.3.1 */
    zb_uint8_t size;                    /**< Attribute 3.3.2.2.4.2 */
    zb_uint8_t quantity;                /**< Attribute 3.3.2.2.4.4 */
    zb_uint8_t rated_voltage;           /**< Attribute 3.3.2.2.4.5 */
    zb_uint8_t alarm_mask;              /**< Attribute 3.3.2.2.4.6 */
    zb_uint8_t voltage_min_threshold;   /**< Attribute 3.3.2.2.4.7 */
    zb_uint8_t percent_remaining;       /**< Attribute 3.3.2.2.3.1
                                             NOTE: A value of 200  is equal to 100%! */
    zb_uint8_t voltage_threshold_1;     /**< Attribute 3.3.2.2.4.8 */
    zb_uint8_t voltage_threshold_2;     /**< Attribute 3.3.2.2.4.8 */
    zb_uint8_t voltage_threshold_3;     /**< Attribute 3.3.2.2.4.8 */
    zb_uint8_t percent_min_threshold;   /**< Attribute 3.3.2.2.4.9 */
    zb_uint8_t percent_threshold_1;     /**< Attribute 3.3.2.2.4.10 */
    zb_uint8_t percent_threshold_2;     /**< Attribute 3.3.2.2.4.10 */
    zb_uint8_t percent_threshold_3;     /**< Attribute 3.3.2.2.4.10 */
    zb_uint32_t alarm_state;            /**< Attribute 3.3.2.2.4.11 */
} zb_zcl_power_attrs_t;

/** @brief 4.2 Illuminance Measurement
*/
typedef struct {
    zb_uint16_t measurement_attr;       /**< Attribute 4.2.2.2.1 */
    zb_uint16_t min_attr;               /**< Attribute 4.2.2.2.2 */
    zb_uint16_t max_attr;               /**< Attribute 4.2.2.2.3 */
} zb_zcl_illuminance_attrs_t;

/** @brief 4.4 Temperature Measurement
*/
typedef struct {
    zb_uint16_t measurement_attr;       /**< Attribute 4.4.2.2.1 */
    zb_uint16_t min_attr;               /**< Attribute 4.4.2.2.2 */
    zb_uint16_t max_attr;               /**< Attribute 4.4.2.2.3 */
    zb_uint16_t tolerance;              /**< Attribute 4.4.2.2.4 */
} zb_zcl_temperature_attrs_t;

/** @brief 4.5 Pressure Measurement
*/
typedef struct {
    zb_uint16_t measurement_attr;       /**< Attribute 4.5.2.2.1 */
    zb_uint16_t min_attr;               /**< Attribute 4.5.2.2.2 */
    zb_uint16_t max_attr;               /**< Attribute 4.5.2.2.3 */
    zb_uint16_t tolerance;              /**< Attribute 4.5.2.2.4 */
} zb_zcl_pressure_attrs_t;

/** @brief 4.7 Relative Humidity Measurement
*/
typedef struct {
    zb_uint16_t measurement_attr;       /**< Attribute 4.7.2.2.1 */
    zb_uint16_t min_attr;               /**< Attribute 4.7.2.2.2 */
    zb_uint16_t max_attr;               /**< Attribute 4.7.2.2.3 */
    zb_uint16_t tolerance;              /**< Attribute 4.7.2.2.4 */
} zb_zcl_humidity_attrs_t;

#if CONFIG_BME68X_IAQ
/** @brief IAQ Measurement
*/
typedef struct {
    zb_uint16_t measurement_attr;       /**< */
    zb_uint16_t min_attr;               /**< */
    zb_uint16_t max_attr;               /**< */
    zb_uint8_t tolerance;               /**< */
} zb_zcl_iaq_attrs_t;

/** @brief VOC Measurement
*/
typedef struct {
    zb_uint16_t measurement_attr;       /**< */
    zb_uint16_t min_attr;               /**< */
    zb_uint16_t max_attr;               /**< */
    zb_uint8_t tolerance;               /**< */
} zb_zcl_voc_attrs_t;

/** @brief CO2 Measurement
*/
typedef struct {
    zb_uint16_t measurement_attr;       /**< */
    zb_uint16_t min_attr;               /**< */
    zb_uint16_t max_attr;               /**< */
    zb_uint8_t tolerance;               /**< */
} zb_zcl_co2_attrs_t;
#endif /* CONFIG_BME68X_IAQ */

/** @brief Main application customizable context. Stores all settings and static values.
 */
typedef struct {
    zb_zcl_basic_attrs_ext_t basic_attr;
    zb_zcl_identify_attrs_t identify_attr;
    zb_zcl_groups_attrs_t groups_attr;
    zb_zcl_power_attrs_t power_attr;
    zb_zcl_illuminance_attrs_t illuminance_attr;
    zb_zcl_temperature_attrs_t temperature_attr;
    zb_zcl_pressure_attrs_t pressure_attr;
    zb_zcl_humidity_attrs_t humidity_attr;
#if CONFIG_BME68X_IAQ
    zb_zcl_co2_attrs_t co2_attr;
    zb_zcl_iaq_attrs_t iaq_attr;
    zb_zcl_voc_attrs_t voc_attr;
#endif /* CONFIG_BME68X_IAQ */
} device_ctx_t;

#define BEELIGHT_CLUSTER_BASIC_DESC(basic_attr_list) \
    ZB_ZCL_CLUSTER_DESC( \
        ZB_ZCL_CLUSTER_ID_BASIC, \
        ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t), \
        (basic_attr_list), \
        ZB_ZCL_CLUSTER_SERVER_ROLE, \
        ZB_ZCL_MANUF_CODE_INVALID \
    ),

#define BEELIGHT_CLUSTER_IDENTIFY_SERVER_DESC(identify_server_attr_list) \
    ZB_ZCL_CLUSTER_DESC( \
        ZB_ZCL_CLUSTER_ID_IDENTIFY, \
        ZB_ZCL_ARRAY_SIZE(identify_server_attr_list, zb_zcl_attr_t), \
        (identify_server_attr_list), \
        ZB_ZCL_CLUSTER_SERVER_ROLE, \
        ZB_ZCL_MANUF_CODE_INVALID \
    ),

#define BEELIGHT_CLUSTER_POWER_DESC(power_attr_list) \
    ZB_ZCL_CLUSTER_DESC( \
        ZB_ZCL_CLUSTER_ID_POWER_CONFIG, \
        ZB_ZCL_ARRAY_SIZE(power_attr_list, zb_zcl_attr_t), \
        (power_attr_list), \
        ZB_ZCL_CLUSTER_SERVER_ROLE, \
        ZB_ZCL_MANUF_CODE_INVALID \
    ),

#define BEELIGHT_CLUSTER_ILLUMINANCE_DESC(illu_attr_list) \
    ZB_ZCL_CLUSTER_DESC( \
        ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT, \
        ZB_ZCL_ARRAY_SIZE(illu_attr_list, zb_zcl_attr_t), \
        (illu_attr_list), \
        ZB_ZCL_CLUSTER_SERVER_ROLE, \
        ZB_ZCL_MANUF_CODE_INVALID \
    ),

#define BEELIGHT_CLUSTER_TEMP_DESC(temp_attr_list) \
    ZB_ZCL_CLUSTER_DESC( \
        ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT, \
        ZB_ZCL_ARRAY_SIZE(temp_attr_list, zb_zcl_attr_t), \
        (temp_attr_list), \
        ZB_ZCL_CLUSTER_SERVER_ROLE, \
        ZB_ZCL_MANUF_CODE_INVALID \
    ),

#define BEELIGHT_CLUSTER_PRESSURE_DESC(pres_attr_list) \
    ZB_ZCL_CLUSTER_DESC( \
        ZB_ZCL_CLUSTER_ID_PRESSURE_MEASUREMENT, \
        ZB_ZCL_ARRAY_SIZE(pres_attr_list, zb_zcl_attr_t), \
        (pres_attr_list), \
        ZB_ZCL_CLUSTER_SERVER_ROLE, \
        ZB_ZCL_MANUF_CODE_INVALID \
    ),

#define BEELIGHT_CLUSTER_HUMIDITY_DESC(hum_attr_list) \
    ZB_ZCL_CLUSTER_DESC( \
        ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT, \
        ZB_ZCL_ARRAY_SIZE(hum_attr_list, zb_zcl_attr_t), \
        (hum_attr_list), \
        ZB_ZCL_CLUSTER_SERVER_ROLE, \
        ZB_ZCL_MANUF_CODE_INVALID \
    ),

#ifdef CONFIG_BME68X_IAQ
#define BEELIGHT_CLUSTER_CO2_DESC(co2_attr_list) \
    ZB_ZCL_CLUSTER_DESC( \
        ZB_BEELIGHT_CLUSTER_ID_CO2_MEASUREMENT, \
        ZB_ZCL_ARRAY_SIZE(co2_attr_list, zb_zcl_attr_t), \
        (co2_attr_list), \
        ZB_ZCL_CLUSTER_SERVER_ROLE, \
        ZB_ZCL_MANUF_CODE_INVALID \
    ),

#define BEELIGHT_CLUSTER_IAQ_DESC(iaq_attr_list) \
    ZB_ZCL_CLUSTER_DESC( \
        ZB_BEELIGHT_CLUSTER_ID_IAQ_MEASUREMENT, \
        ZB_ZCL_ARRAY_SIZE(iaq_attr_list, zb_zcl_attr_t), \
        (iaq_attr_list), \
        ZB_ZCL_CLUSTER_SERVER_ROLE, \
        ZB_ZCL_MANUF_CODE_INVALID \
    ),

#define BEELIGHT_CLUSTER_VOC_DESC(voc_attr_list) \
    ZB_ZCL_CLUSTER_DESC( \
        ZB_BEELIGHT_CLUSTER_ID_VOC_MEASUREMENT, \
        ZB_ZCL_ARRAY_SIZE(voc_attr_list, zb_zcl_attr_t), \
        (voc_attr_list), \
        ZB_ZCL_CLUSTER_SERVER_ROLE, \
        ZB_ZCL_MANUF_CODE_INVALID \
    ),
#endif /* CONFIG_BME68X_IAQ */

#define BEELIGHT_CLUSTER_IDENTIFY_CLIENT_DESC(identify_client_attr_list) \
    ZB_ZCL_CLUSTER_DESC( \
        ZB_ZCL_CLUSTER_ID_IDENTIFY, \
        ZB_ZCL_ARRAY_SIZE(identify_client_attr_list, zb_zcl_attr_t),    \
        (identify_client_attr_list),    \
        ZB_ZCL_CLUSTER_CLIENT_ROLE, \
        ZB_ZCL_MANUF_CODE_INVALID   \
    ),  \

/** @brief Basic cluster IDs (for both variants)
 */
#define BEELIGHT_CLUSTER_IDS_BASIC \
    ZB_ZCL_CLUSTER_ID_BASIC, \
    ZB_ZCL_CLUSTER_ID_IDENTIFY, \
    ZB_ZCL_CLUSTER_ID_POWER_CONFIG, \
    ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT, \
    ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT, \
    ZB_ZCL_CLUSTER_ID_PRESSURE_MEASUREMENT, \
    ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT,

/** @brief Additional cluster IDs (for both variants)
 */
#define BEELIGHT_CLUSTER_IDS_EXTENDED \
    BEELIGHT_CLUSTER_IDS_BASIC \
    ZB_BEELIGHT_CLUSTER_ID_CO2_MEASUREMENT, \
    ZB_BEELIGHT_CLUSTER_ID_IAQ_MEASUREMENT, \
    ZB_BEELIGHT_CLUSTER_ID_VOC_MEASUREMENT,

#if CONFIG_BME68X_IAQ
#include "beelight_bsec2.h"
#else
#include "beelight_nobsec2.h"
#endif /* CONFIG_BME68X_IAQ */
