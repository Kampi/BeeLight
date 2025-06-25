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

#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "events/events.h"

static struct k_work_delayable periodic_24h_work;
static struct k_work_delayable periodic_5min_work;

ZBUS_CHAN_DEFINE(periodic_24h_chan,
                 struct periodic_event,
                 NULL,
                 &periodic_24h_work,
                 ZBUS_OBSERVERS_EMPTY,
                 ZBUS_MSG_INIT()
                 );

ZBUS_CHAN_DEFINE(periodic_5min_chan,
                 struct periodic_event,
                 NULL,
                 &periodic_5min_work,
                 ZBUS_OBSERVERS_EMPTY,
                 ZBUS_MSG_INIT()
                 );

ZBUS_CHAN_DEFINE(light_data_chan,
                 struct light_event,
                 NULL,
                 NULL,
                 ZBUS_OBSERVERS_EMPTY,
                 ZBUS_MSG_INIT()
                 );

ZBUS_CHAN_DEFINE(env_data_chan,
                 struct env_event,
                 NULL,
                 NULL,
                 ZBUS_OBSERVERS_EMPTY,
                 ZBUS_MSG_INIT()
                 );

ZBUS_CHAN_DEFINE(battery_data_chan,
                 struct battery_event,
                 NULL,
                 NULL,
                 ZBUS_OBSERVERS_EMPTY,
                 ZBUS_MSG_INIT()
                 );

LOG_MODULE_REGISTER(events, LOG_LEVEL_DBG);

static void handle_24h_timeout(struct k_work *item)
{
    struct periodic_event evt = {
    };
    struct k_work_delayable *work = NULL;

    LOG_DBG("24 h timeout");

    zbus_chan_claim(&periodic_24h_chan, K_FOREVER);
    work = (struct k_work_delayable *)zbus_chan_user_data(&periodic_24h_chan);
    k_work_reschedule(work, K_MSEC(24 * 60 * 60 * 1000UL));
    zbus_chan_finish(&periodic_24h_chan);

    zbus_chan_pub(&periodic_24h_chan, &evt, K_NO_WAIT);
}

static void handle_5min_timeout(struct k_work *item)
{
    struct periodic_event evt = {
    };
    struct k_work_delayable *work = NULL;

    LOG_DBG("5 min timeout");

    zbus_chan_claim(&periodic_5min_chan, K_FOREVER);
    work = (struct k_work_delayable *)zbus_chan_user_data(&periodic_5min_chan);
    k_work_reschedule(work, K_MSEC(5 * 60 * 1000UL));
    zbus_chan_finish(&periodic_5min_chan);

    zbus_chan_pub(&periodic_5min_chan, &evt, K_NO_WAIT);
}

static int beelight_events_init(void)
{
    struct k_work_delayable *work = NULL;

    work = (struct k_work_delayable *)zbus_chan_user_data(&periodic_24h_chan);
    k_work_init_delayable(work, handle_24h_timeout);
    k_work_reschedule(work, K_MSEC(10000));

    work = (struct k_work_delayable *)zbus_chan_user_data(&periodic_5min_chan);
    k_work_init_delayable(work, handle_5min_timeout);

    LOG_DBG("Events initialized");

    return k_work_reschedule(work, K_MSEC(10000));
}

SYS_INIT(beelight_events_init, APPLICATION, 1);