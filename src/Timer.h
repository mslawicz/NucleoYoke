/*
 * timer.h
 *
 *  Created on: 02.08.2019
 *      Author: Marcin
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "stm32f4xx_hal.h"

class Timer
{
public:
    Timer();
    static TIM_HandleTypeDef hTim;
    static void config(void);
    void reset(void) { latchedValue = __HAL_TIM_GET_COUNTER(&Timer::hTim); }
    uint32_t elapsed(void) const { return __HAL_TIM_GET_COUNTER(&Timer::hTim) - latchedValue; }
    bool elapsed(uint32_t elapsedValue) const { return ((__HAL_TIM_GET_COUNTER(&Timer::hTim) - latchedValue) >= elapsedValue); }
private:
    uint32_t latchedValue;
};

#endif /* TIMER_H_ */
