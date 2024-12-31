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

static struct k_work_delayable periodic_sample_work;

ZBUS_CHAN_DEFINE(periodic_sample_event_chan,
                 struct periodic_event,
                 NULL,
                 &periodic_sample_work,
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

LOG_MODULE_REGISTER(events, CONFIG_BEELIGHT_EVENTS_LOG_LEVEL);

static void handle_sample_timeout(struct k_work *item)
{
    struct periodic_event evt = {
    };
    struct k_work_delayable *work = NULL;

    zbus_chan_claim(&periodic_sample_event_chan, K_FOREVER);
    work = (struct k_work_delayable *)zbus_chan_user_data(&periodic_sample_event_chan);
    k_work_reschedule(work, K_MSEC(CONFIG_BEELIGHT_CONFIG_SENSOR_SAMPLE_RATE_SECONDS * 1000));
    zbus_chan_finish(&periodic_sample_event_chan);

    zbus_chan_pub(&periodic_sample_event_chan, &evt, K_MSEC(250));
}

static int beelight_events_init(void)
{
    struct k_work_delayable *work = NULL;

    work = (struct k_work_delayable *)zbus_chan_user_data(&periodic_sample_event_chan);
    k_work_init_delayable(work, handle_sample_timeout);

    return k_work_reschedule(work, K_MSEC(10000));
}

SYS_INIT(beelight_events_init, APPLICATION, 1);