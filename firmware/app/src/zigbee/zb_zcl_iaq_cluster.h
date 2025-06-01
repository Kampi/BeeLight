#ifndef ZB_ZCL_IAQ_MEASUREMENT_H_
#define ZB_ZCL_IAQ_MEASUREMENT_H_

#include <zcl/zb_zcl_common.h>
#include <zcl/zb_zcl_commands.h>

#define ZB_ZCL_CLUSTER_ID_IAQ_MEASUREMENT                   0x1A0AU
#define ZB_IAQ_DEVICE_ID                        0x29A
#define ZB_IAQ_DEVICE_VERSION                   6

enum zb_zcl_iaq_cluster_attr_e
{
    ZB_ZCL_ATTR_IAQ_MEASUREMENT_VALUE_ID        = 0x0000,
    ZB_ZCL_ATTR_IAQ_MEASUREMENT_MIN_VALUE_ID    = 0x0001,
    ZB_ZCL_ATTR_IAQ_MEASUREMENT_MAX_VALUE_ID    = 0x0002,
};

/** @brief Default value for IAQ Measurement cluster revision global attribute */
#define ZB_ZCL_IAQ_MEASUREMENT_CLUSTER_REVISION_DEFAULT             ((zb_uint16_t)0x0001U)

/** @brief MeasuredValue attribute unknown value */
#define ZB_ZCL_ATTR_IAQ_MEASUREMENT_VALUE_UNKNOWN                   ((zb_int16_t)0x8000)

/** @brief MinMeasuredValue attribute minimum value */
#define ZB_ZCL_ATTR_IAQ_MEASUREMENT_MIN_VALUE_MIN_VALUE             ((zb_int16_t)0x8001)

/** @brief MinMeasuredValue attribute maximum value */
#define ZB_ZCL_ATTR_IAQ_MEASUREMENT_MIN_VALUE_MAX_VALUE             ((zb_int16_t)0x7ffe)

/** @brief MinMeasuredValue attribute invalid value */
#define ZB_ZCL_ATTR_IAQ_MEASUREMENT_MIN_VALUE_INVALID               ((zb_int16_t)0x8000)

/** @brief MaxMeasuredValue attribute minimum value */
#define ZB_ZCL_ATTR_IAQ_MEASUREMENT_MAX_VALUE_MIN_VALUE             ((zb_int16_t)0x8002)

/** @brief MaxMeasuredValue attribute maximum value */
#define ZB_ZCL_ATTR_IAQ_MEASUREMENT_MAX_VALUE_MAX_VALUE             ((zb_int16_t)0x7fff)

/** @brief MaxMeasuredValue attribute invalid value */
#define ZB_ZCL_ATTR_IAQ_MEASUREMENT_MAX_VALUE_INVALID               ((zb_int16_t)0x8000)

/** @brief Tolerance attribute minimum value */
#define ZB_ZCL_ATTR_IAQ_MEASUREMENT_TOLERANCE_MIN_VALUE             ((zb_uint16_t)0x0000)

/** @brief Tolerance attribute maximum value */
#define ZB_ZCL_ATTR_IAQ_MEASUREMENT_TOLERANCE_MAX_VALUE             ((zb_uint16_t)0x0800)

/** @brief Default value for Value attribute */
#define ZB_ZCL_ATTR_IAQ_MEASUREMENT_VALUE_DEFAULT_VALUE             ((zb_int16_t)0xFFFF)

/** @brief Default value for MinValue attribute */
#define ZB_ZCL_PATTR_RESSURE_MEASUREMENT_MIN_VALUE_DEFAULT_VALUE    ((zb_int16_t)0x8000)

/** @brief Default value for MaxValue attribute */
#define ZB_ZCL_PATTR_RESSURE_MEASUREMENT_MAX_VALUE_DEFAULT_VALUE    ((zb_int16_t)0x8000)

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_IAQ_VALUE_ID(data_ptr)                            \
{                                                                                       \
    ZB_ZCL_ATTR_IAQ_MEASUREMENT_VALUE_ID,                                               \
    ZB_ZCL_ATTR_TYPE_U8,                                                                \
    ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                       \
    (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                 \
    (void*) data_ptr                                                                    \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_IAQ_MIN_VALUE_ID(data_ptr)                        \
{                                                                                       \
    ZB_ZCL_ATTR_IAQ_MEASUREMENT_MIN_VALUE_ID,                                           \
    ZB_ZCL_ATTR_TYPE_U8,                                                                \
    ZB_ZCL_ATTR_ACCESS_WRITE_ONLY,                                                      \
    (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                 \
    (void*) data_ptr                                                                    \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_IAQ_MAX_VALUE_ID(data_ptr)                        \
{                                                                                       \
    ZB_ZCL_ATTR_IAQ_MEASUREMENT_MAX_VALUE_ID,                                           \
    ZB_ZCL_ATTR_TYPE_U8,                                                                \
    ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                      \
    (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                 \
    (void*) data_ptr                                                                    \
}

/** @brief Number of attributes mandatory for reporting in IAQ Measurement cluster */
#define ZB_ZCL_IAQ_MEASUREMENT_REPORT_ATTR_COUNT 1

/** @brief Declare attribute list for IAQ Measurement cluster - server side
    @param attr_list - attribute list name
    @param value - pointer to variable to store MeasuredValue attribute
    @param min_value - pointer to variable to store MinMeasuredValue attribute
    @param max_value - pointer to variable to store MAxMeasuredValue attribute
*/
#define ZB_ZCL_DECLARE_IAQ_MEASUREMENT_ATTRIB_LIST(attr_list, value, min_value, max_value)                  \
    ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_IAQ_MEASUREMENT)                    \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_IAQ_MEASUREMENT_VALUE_ID, (value))                                     \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_IAQ_MEASUREMENT_MIN_VALUE_ID, (min_value))                             \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_IAQ_MEASUREMENT_MAX_VALUE_ID, (max_value))                             \
    ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

void zb_zcl_iaq_measurement_init_server(void);
void zb_zcl_iaq_measurement_init_client(void);

#define ZB_ZCL_CLUSTER_ID_CUSTOM_SERVER_ROLE_INIT zb_zcl_iaq_measurement_init_server
#define ZB_ZCL_CLUSTER_ID_CUSTOM_CLIENT_ROLE_INIT zb_zcl_iaq_measurement_init_client

#endif /* ZB_ZCL_IAQ_MEASUREMENT_H_ */