# Copyright (c) 2025, Daniel Kampert
#
# SPDX-License-Identifier: Apache-2.0
#

add_subdirectory_ifdef(CONFIG_APDS9306 apds9306)
add_subdirectory_ifdef(CONFIG_BME68X_EXT_IAQ bme68x_iaq)
