#pragma once

#include <zb_ha_device_config.h>

/** @defgroup ZB_DEFINE_DEVICE_HUMIDITY_SENSOR Humidity Sensor
 *  @brief See https://zigbeealliance.org/wp-content/uploads/2021/10/07-5123-08-Zigbee-Cluster-Library.pdf for additional informations
 *  @{
 *  @details
 *          Server
 *          - @ref ZB_ZCL_BASIC \n
 *          - @ref ZB_ZCL_IDENTIFY \n
 *          - @ref ZB_ZCL_CLUSTER_ID_POWER_CONFIG \n
 *          - @ref ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT \n
 *
 *          Client
 *          - @ref ZB_ZCL_IDENTIFY \n
 */

/** @brief Device endpoint, used to receive light sensor results.
 */
#define LIGHT_SENSOR_ENDPOINT                   10

/** @brief
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

/** @brief Number of IN (server) clusters.
*/
#define ZB_LIGHT_SENSOR_IN_CLUSTER_NUM          4

/** @brief Number of OUT (client) clusters.
*/
#define ZB_LIGHT_SENSOR_OUT_CLUSTER_NUM         1

/** @brief Number of attributes.
*/
#define ZB_LIGHT_SENSOR_REPORT_ATTR_COUNT       (ZB_ZCL_ILLUMINANCE_MEASUREMENT_REPORT_ATTR_COUNT + 7)

/** @brief Continuous value change attribute count.
*/
#define ZB_LIGHT_SENSOR_CVC_ATTR_COUNT          1

/* */
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

/* */
typedef struct {
    zb_uint16_t measurement_attr;       /**< Attribute 4.2.2.2.1 */
    zb_uint16_t min_attr;               /**< Attribute 4.2.2.2.2 */
    zb_uint16_t max_attr;               /**< Attribute 4.2.2.2.3 */
} zb_zcl_illuminance_attrs_t;

/** @brief Main application customizable context. Stores all settings and static values.
 */
typedef struct {
    zb_zcl_basic_attrs_ext_t basic_attr;
    zb_zcl_identify_attrs_t identify_attr;
    zb_zcl_groups_attrs_t groups_attr;
    zb_zcl_power_attrs_t power_attr;
    zb_zcl_illuminance_attrs_t illuminance_attr;
} device_ctx_t;

/** @brief                              Declare the cluster list for the Light Sensor device.
 *  @param cluster_list_name            Cluster list variable name
 *  @param basic_attr_list              Attribute list for Basic cluster
 *  @param identify_client_attr_list    Attribute list for Identify cluster (Client)
 *  @param identify_server_attr_list    Attribute list for Identify cluster (Server)
 *  @param power_attr_list              Attribute list for Power cluster
 *  @param measure_attr_list            Attribute list for Illuminance Measurement cluster
 */
#define ZB_DECLARE_LIGHT_SENSOR_CLUSTER_LIST(                               \
    cluster_list_name,                                                      \
    basic_attr_list,                                                        \
	identify_client_attr_list,                                              \
	identify_server_attr_list,                                              \
    power_attr_list,                                                        \
    measure_attr_list)                                                      \
    zb_zcl_cluster_desc_t cluster_list_name[] =                             \
    {                                                                       \
        ZB_ZCL_CLUSTER_DESC(                                                \
            ZB_ZCL_CLUSTER_ID_BASIC,                                        \
            ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),              \
            (basic_attr_list),                                              \
            ZB_ZCL_CLUSTER_SERVER_ROLE,                                     \
            ZB_ZCL_MANUF_CODE_INVALID                                       \
        ),                                                                  \
        ZB_ZCL_CLUSTER_DESC(                                                \
            ZB_ZCL_CLUSTER_ID_IDENTIFY,                                     \
            ZB_ZCL_ARRAY_SIZE(identify_server_attr_list, zb_zcl_attr_t),    \
            (identify_server_attr_list),                                    \
            ZB_ZCL_CLUSTER_SERVER_ROLE,                                     \
            ZB_ZCL_MANUF_CODE_INVALID                                       \
        ),                                                                  \
        ZB_ZCL_CLUSTER_DESC(                                                \
            ZB_ZCL_CLUSTER_ID_POWER_CONFIG,                                 \
            ZB_ZCL_ARRAY_SIZE(power_attr_list, zb_zcl_attr_t),              \
            (power_attr_list),                                              \
            ZB_ZCL_CLUSTER_SERVER_ROLE,                                     \
            ZB_ZCL_MANUF_CODE_INVALID                                       \
        ),                                                                  \
        ZB_ZCL_CLUSTER_DESC(                                                \
            ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT,                      \
            ZB_ZCL_ARRAY_SIZE(measure_attr_list, zb_zcl_attr_t),            \
            (measure_attr_list),                                            \
            ZB_ZCL_CLUSTER_SERVER_ROLE,                                     \
            ZB_ZCL_MANUF_CODE_INVALID                                       \
        ),                                                                  \
        ZB_ZCL_CLUSTER_DESC(                                                \
            ZB_ZCL_CLUSTER_ID_IDENTIFY,                                     \
            ZB_ZCL_ARRAY_SIZE(identify_client_attr_list, zb_zcl_attr_t),    \
            (identify_client_attr_list),                                    \
            ZB_ZCL_CLUSTER_CLIENT_ROLE,                                     \
            ZB_ZCL_MANUF_CODE_INVALID                                       \
        )                                                                   \
    }

/** @brief                  Declare simple descriptor for Light Sensor device.
 *  @param ep_name          Endpoint variable name
 *  @param ep_id            Endpoint ID
 *  @param in_clust_num     Number of supported input clusters
 *  @param out_clust_num    Number of supported output clusters
 */
#define ZB_ZCL_DECLARE_HA_LIGHT_SENSOR_SIMPLE_DESC(ep_name, ep_id, in_clust_num, out_clust_num) \
    ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                                        \
    ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =                 \
    {                                                                                           \
        ep_id,                                                                                  \
        ZB_PROFILE_ID,                                                                          \
        ZB_DEVICE_ID,                                                                           \
        ZB_DEVICE_VER,                                                                          \
        0,                                                                                      \
        in_clust_num,                                                                           \
        out_clust_num,                                                                          \
        {                                                                                       \
            ZB_ZCL_CLUSTER_ID_BASIC,                                                            \
            ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                         \
            ZB_ZCL_CLUSTER_ID_POWER_CONFIG,                                                     \
            ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT,                                          \
            ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                         \
        }                                                                                       \
    }

/** @brief                  Declare endpoint for the Light Sensor device.
 *  @param ep_name          Endpoint variable name
 *  @param ep_id            Endpoint ID
 *  @param cluster_list     Endpoint cluster list
 */
#define ZB_DECLARE_LIGHT_SENSOR_EP(ep_name, ep_id, cluster_list)                \
    ZB_ZCL_DECLARE_HA_LIGHT_SENSOR_SIMPLE_DESC(                                 \
        ep_name,                                                                \
        ep_id,                                                                  \
        ZB_LIGHT_SENSOR_IN_CLUSTER_NUM,                                         \
        ZB_LIGHT_SENSOR_OUT_CLUSTER_NUM                                         \
    );                                                                          \
    ZBOSS_DEVICE_DECLARE_REPORTING_CTX(                                         \
        reporting_info##ep_name,                                                \
        ZB_LIGHT_SENSOR_REPORT_ATTR_COUNT                                       \
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
        ZB_LIGHT_SENSOR_REPORT_ATTR_COUNT,                                      \
        reporting_info##ep_name,                                                \
        0,                                                                      \
        NULL                                                                    \
    )
