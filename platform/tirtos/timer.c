/*
 * Copyright 2015-2016 Texas Instruments Incorporated. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#include <xdc/runtime/Assert.h>
#include <ti/sysbios/knl/Clock.h>
#include <timer_interface.h>

static inline uint32_t ticksToMs(uint32_t ticks)
{
    if (Clock_tickPeriod == 1000) {
        return (ticks);
    }
    else {
        return ((ticks * Clock_tickPeriod) / 1000);
    }
}

static inline uint32_t msToTicks(uint32_t ms)
{
    if (Clock_tickPeriod == 1000) {
        return (ms);
    }
    else {
        return ((ms * 1000) / Clock_tickPeriod);
    }
}

uint32_t left_ms(Timer *timer)
{
    uint32_t diff = Clock_getTicks() - timer->start;

    if (diff >= timer->length) {
        timer->length = 0;
        diff = 0;
    }
    else {
        diff = ticksToMs(timer->length - diff);
    }

    return (diff);
}

bool has_timer_expired(Timer *timer)
{
    return (!left_ms(timer));
}

void countdown_ms(Timer *timer, uint32_t tms)
{
    Assert_isTrue(tms < TIRTOS_MAX_TIMEOUT_MS, NULL);
    timer->length = msToTicks(tms);
    timer->start = Clock_getTicks();
}

void countdown_sec(Timer *timer, uint32_t tsec)
{
    Assert_isTrue(tsec < TIRTOS_MAX_TIMEOUT_SEC, NULL);
    countdown_ms(timer, tsec * 1000);
}

void init_timer(Timer *timer)
{
    timer->start = 0;
    timer->length = 0;
}
