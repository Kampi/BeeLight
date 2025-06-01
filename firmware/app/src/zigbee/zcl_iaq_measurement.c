#include "zb_common.h"
#include "zb_zcl.h"
#include "zb_aps.h"
#include "zcl/zb_zcl_common.h"

#include "zb_zcl_iaq_cluster.h"

zb_ret_t check_value_iaq_measurement_server(zb_uint16_t attr_id, zb_uint8_t endpoint, zb_uint8_t *value);

void zb_zcl_iaq_measurement_init_server(void)
{
    zb_zcl_add_cluster_handlers(ZB_ZCL_CLUSTER_ID_IAQ_MEASUREMENT,
                                ZB_ZCL_CLUSTER_SERVER_ROLE,
                                check_value_iaq_measurement_server,
                                (zb_zcl_cluster_write_attr_hook_t)NULL,
                                (zb_zcl_cluster_handler_t)NULL);
}

void zb_zcl_iaq_measurement_init_client(void)
{
    zb_zcl_add_cluster_handlers(ZB_ZCL_CLUSTER_ID_IAQ_MEASUREMENT,
                                ZB_ZCL_CLUSTER_CLIENT_ROLE,
                                (zb_zcl_cluster_check_value_t)NULL,
                                (zb_zcl_cluster_write_attr_hook_t)NULL,
                                (zb_zcl_cluster_handler_t)NULL);
}

zb_ret_t check_value_iaq_measurement_server(zb_uint16_t attr_id, zb_uint8_t endpoint, zb_uint8_t *value)
{
    zb_ret_t ret = RET_OK;
    zb_int16_t val = ZB_ZCL_ATTR_GETS16(value);

    TRACE_MSG(TRACE_ZCL1, "> check_value_iaq_measurement, attr_id %d, val %d", (FMT__D_D, attr_id, val));

    switch (attr_id)
    {
        case ZB_ZCL_ATTR_IAQ_MEASUREMENT_VALUE_ID:
            if (ZB_ZCL_ATTR_IAQ_MEASUREMENT_VALUE_UNKNOWN == val)
            {
                ret = RET_OK;
            }
            else
            {
                zb_zcl_attr_t *attr_desc = zb_zcl_get_attr_desc_a(endpoint,
                    ZB_ZCL_CLUSTER_ID_IAQ_MEASUREMENT,
                    ZB_ZCL_CLUSTER_SERVER_ROLE,
                    ZB_ZCL_ATTR_IAQ_MEASUREMENT_MIN_VALUE_ID);
                ZB_ASSERT(attr_desc);

                ret = (ZB_ZCL_GET_ATTRIBUTE_VAL_S16(attr_desc) == ZB_ZCL_ATTR_IAQ_MEASUREMENT_MIN_VALUE_INVALID ||
                    ZB_ZCL_GET_ATTRIBUTE_VAL_S16(attr_desc) <= val)
                    ? RET_OK : RET_ERROR;

                if (ret)
                {
                    attr_desc = zb_zcl_get_attr_desc_a(endpoint,
                        ZB_ZCL_CLUSTER_ID_IAQ_MEASUREMENT,
                        ZB_ZCL_CLUSTER_SERVER_ROLE,
                        ZB_ZCL_ATTR_IAQ_MEASUREMENT_MAX_VALUE_ID);
                    ZB_ASSERT(attr_desc);

                    ret = ZB_ZCL_GET_ATTRIBUTE_VAL_S16(attr_desc) == ZB_ZCL_ATTR_IAQ_MEASUREMENT_MAX_VALUE_INVALID ||
                            val <= ZB_ZCL_GET_ATTRIBUTE_VAL_S16(attr_desc)
                        ? RET_OK : RET_ERROR;
                }
            }
            break;

        case ZB_ZCL_ATTR_IAQ_MEASUREMENT_MIN_VALUE_ID:
            ret = ((ZB_ZCL_ATTR_IAQ_MEASUREMENT_MIN_VALUE_MIN_VALUE <= val) &&
                    (val <= ZB_ZCL_ATTR_IAQ_MEASUREMENT_MIN_VALUE_MAX_VALUE)) ||
                    (ZB_ZCL_ATTR_IAQ_MEASUREMENT_MIN_VALUE_INVALID == val)
                    ? RET_OK : RET_ERROR;
            break;

        case ZB_ZCL_ATTR_IAQ_MEASUREMENT_MAX_VALUE_ID:
            ret = ( (ZB_ZCL_ATTR_IAQ_MEASUREMENT_MAX_VALUE_MIN_VALUE <= ZB_ZCL_ATTR_GET16(value)) &&
                    (ZB_ZCL_ATTR_GET16(value) <= ZB_ZCL_ATTR_IAQ_MEASUREMENT_MAX_VALUE_MAX_VALUE) )
                    ? RET_OK : RET_ERROR;
            break;

        default:
            break;
    }

    TRACE_MSG(TRACE_ZCL1, "< check_value_IAQ_measurement ret %hd", (FMT__H, ret));

    return ret;
}