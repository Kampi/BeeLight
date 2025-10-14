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

#pragma once

#include <zb_ha_device_config.h>

/** @brief See https://zigbeealliance.org/wp-content/uploads/2021/10/07-5123-08-Zigbee-Cluster-Library.pdf for additional informations
 *  @details
 *          Server
 *              - @ref ZB_ZCL_BASIC \n
 *              - @ref ZB_ZCL_IDENTIFY \n
 *              - @ref ZB_ZCL_CLUSTER_ID_POWER_CONFIG \n
 *              - @ref ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT \n
 *              - @ref ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT \n
 *              - @ref ZB_ZCL_CLUSTER_ID_PRESSURE_MEASUREMENT \n
 *              - @ref ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT \n
 *              - @ref ZB_ZCL_CLUSTER_ID_CO2_MEASUREMENT \n
 *              - @ref ZB_ZCL_CLUSTER_ID_IAQ_MEASUREMENT \n
 *              - @ref ZB_ZCL_CLUSTER_ID_VOC_MEASUREMENT \n
 *
 *          Client
 *              -
 */

/** @brief Number of IN (server) clusters.
*/
#define ZB_ENV_SENSOR_IN_CLUSTER_NUM            10

/** @brief Number of OUT (client) clusters.
*/
#define ZB_ENV_SENSOR_OUT_CLUSTER_NUM           0

/** @brief Number of attributes.
*/
#define ZB_ENV_SENSOR_REPORT_ATTR_COUNT         10

/** @brief                              Declare the cluster list for the Environment Sensor device.
 *  @param cluster_list_name            Cluster list variable name
 *  @param basic_attr_list              Attribute list for Basic cluster
 *  @param identify_server_attr_list    Attribute list for Identify cluster (Server)
 *  @param power_attr_list              Attribute list for Power cluster
 *  @param illu_attr_list               Attribute list for Illuminance Measurement cluster
 *  @param temp_attr_list               Attribute list for Temperature Measurement cluster
 *  @param pres_attr_list               Attribute list for Pressure Measurement cluster
 *  @param hum_attr_list                Attribute list for Humidity Measurement cluster
 *  @param co2_attr_list                Attribute list for CO2 cluster
 *  @param iaq_attr_list                Attribute list for IAQ Measurement cluster
 *  @param voc_attr_list                Attribute list for VOC Measurement cluster
 */
#define ZB_DECLARE_ENV_SENSOR_CLUSTER_LIST(                                 \
    cluster_list_name,                                                      \
    basic_attr_list,                                                        \
    identify_server_attr_list,                                              \
    power_attr_list,                                                        \
    illu_attr_list,                                                         \
    temp_attr_list,                                                         \
    pres_attr_list,                                                         \
    hum_attr_list,                                                          \
    co2_attr_list,                                                          \
    iaq_attr_list,                                                          \
    voc_attr_list)                                                          \
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
            ZB_ZCL_ARRAY_SIZE(illu_attr_list, zb_zcl_attr_t),               \
            (illu_attr_list),                                               \
            ZB_ZCL_CLUSTER_SERVER_ROLE,                                     \
            ZB_ZCL_MANUF_CODE_INVALID                                       \
        ),                                                                  \
        ZB_ZCL_CLUSTER_DESC(                                                \
            ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,                             \
            ZB_ZCL_ARRAY_SIZE(temp_attr_list, zb_zcl_attr_t),               \
            (temp_attr_list),                                               \
            ZB_ZCL_CLUSTER_SERVER_ROLE,                                     \
            ZB_ZCL_MANUF_CODE_INVALID                                       \
        ),                                                                  \
        ZB_ZCL_CLUSTER_DESC(                                                \
            ZB_ZCL_CLUSTER_ID_PRESSURE_MEASUREMENT,                         \
            ZB_ZCL_ARRAY_SIZE(pres_attr_list, zb_zcl_attr_t),               \
            (pres_attr_list),                                               \
            ZB_ZCL_CLUSTER_SERVER_ROLE,                                     \
            ZB_ZCL_MANUF_CODE_INVALID                                       \
        ),                                                                  \
        ZB_ZCL_CLUSTER_DESC(                                                \
            ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT,                     \
            ZB_ZCL_ARRAY_SIZE(hum_attr_list, zb_zcl_attr_t),                \
            (hum_attr_list),                                                \
            ZB_ZCL_CLUSTER_SERVER_ROLE,                                     \
            ZB_ZCL_MANUF_CODE_INVALID                                       \
        ),                                                                  \
        ZB_ZCL_CLUSTER_DESC(                                                \
            ZB_ZCL_CLUSTER_ID_CO2_MEASUREMENT,                              \
            ZB_ZCL_ARRAY_SIZE(co2_attr_list, zb_zcl_attr_t),                \
            (co2_attr_list),                                                \
            ZB_ZCL_CLUSTER_SERVER_ROLE,                                     \
            ZB_ZCL_MANUF_CODE_INVALID                                       \
        ),                                                                  \
        ZB_ZCL_CLUSTER_DESC(                                                \
            ZB_ZCL_CLUSTER_ID_IAQ_MEASUREMENT,                              \
            ZB_ZCL_ARRAY_SIZE(iaq_attr_list, zb_zcl_attr_t),                \
            (iaq_attr_list),                                                \
            ZB_ZCL_CLUSTER_SERVER_ROLE,                                     \
            ZB_ZCL_MANUF_CODE_INVALID                                       \
        ),                                                                  \
        ZB_ZCL_CLUSTER_DESC(                                                \
            ZB_ZCL_CLUSTER_ID_VOC_MEASUREMENT,                              \
            ZB_ZCL_ARRAY_SIZE(voc_attr_list, zb_zcl_attr_t),                \
            (voc_attr_list),                                                \
            ZB_ZCL_CLUSTER_SERVER_ROLE,                                     \
            ZB_ZCL_MANUF_CODE_INVALID                                       \
        ),                                                                  \
    }

/** @brief                  Declare simple descriptor for the Environment Sensor device.
 *  @param ep_name          Endpoint variable name
 *  @param ep_id            Endpoint ID
 *  @param in_clust_num     Number of supported input clusters
 *  @param out_clust_num    Number of supported output clusters
 */
#define ZB_ZCL_DECLARE_HA_ENV_SENSOR_SIMPLE_DESC(ep_name, ep_id, in_clust_num, out_clust_num)   \
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
            ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,                                                 \
            ZB_ZCL_CLUSTER_ID_PRESSURE_MEASUREMENT,                                             \
            ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT,                                         \
            ZB_ZCL_CLUSTER_ID_CO2_MEASUREMENT,                                                  \
            ZB_ZCL_CLUSTER_ID_IAQ_MEASUREMENT,                                                  \
            ZB_ZCL_CLUSTER_ID_VOC_MEASUREMENT,                                                  \
        }                                                                                       \
    }
