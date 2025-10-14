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

/* Version of the application software (1 byte). */
#define INIT_BASIC_APP_VERSION                  01

/* Version of the implementation of the Zigbee stack (1 byte). */
#define INIT_BASIC_STACK_VERSION                10

/* Version of the hardware of the device (1 byte). */
#define INIT_BASIC_HW_VERSION                   1

/* Manufacturer name (32 bytes). */
#define INIT_BASIC_MANUF_NAME                   "Kampi"

/* Model number assigned by manufacturer (32-bytes long string). */
#define INIT_BASIC_MODEL_ID                     "BeeLight_v2.0"

/* First 8 bytes specify the date of manufacturer of the device
 * in ISO 8601 format (YYYYMMDD). The rest (8 bytes) are manufacturer specific.
 */
#define INIT_BASIC_DATE_CODE                    "20200329"

/* Describes the physical location of the device (16 bytes).
 * May be modified during commissioning process.
 */
#define INIT_BASIC_LOCATION_DESC                "Office desk"

/* Button used to enter the Identify mode. */
#define IDENTIFY_MODE_BUTTON                    DK_BTN1_MSK

/* LED indicating that light switch successfully joind Zigbee network. */
#define ZIGBEE_NETWORK_STATE_LED                DK_LED1

/* Button to start Factory Reset. */
#define FACTORY_RESET_BUTTON                    IDENTIFY_MODE_BUTTON

/* Define 'bat_num' as empty in order to declare default battery set attributes. */
/* According to Table 3-17 of ZCL specification, defining 'bat_num' as 2 or 3 allows */
/* to declare battery set attributes for BATTERY2 and BATTERY3. */
#define bat_num

/** @brief Device endpoint, used to receive environment sensor results.
 */
#define SENSOR_ENDPOINT                         10

/** @brief The ratet battery voltage in millivolts.
 */
#define SENSOR_RATET_VOLTAGE_MV                 3000UL

/** @brief The lowest battery voltage in millivolts.
 */
#define SENSOR_EMPTY_VOLTAGE_MV                 2700UL

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

#if CONFIG_BME68X_IAQ
#include "beelight_bsec2.h"
#else
#include "beelight_nobsec2.h"
#endif /* CONFIG_BME68X_IAQ */
