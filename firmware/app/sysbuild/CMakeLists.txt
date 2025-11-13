# Copyright (c) 2026 BeeLight Project
# SPDX-License-Identifier: Apache-2.0

# Add the app directory as BOARD_ROOT so that boards/kampi/beelight is found.
list(APPEND BOARD_ROOT ${APP_DIR})

find_package(Sysbuild REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(sysbuild LANGUAGES)
