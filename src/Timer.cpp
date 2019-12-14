/*
 * timer.cpp
 *
 *  Created on: 02.08.2019
 *      Author: Marcin
 */

#include "Timer.h"
#include "System.h"

TIM_HandleTypeDef Timer::hTim = {0};

Timer::Timer()
{
    latchedValue = __HAL_TIM_GET_COUNTER(&Timer::hTim);
}

/*
 * configuration of the timer parameters
 */
void Timer::config(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    __HAL_DBGMCU_FREEZE_TIM5();
    __TIM5_CLK_ENABLE();
    Timer::hTim.Instance = TIM5;
    Timer::hTim.Init.Prescaler = 95;
    Timer::hTim.Init.CounterMode = TIM_COUNTERMODE_UP;
    Timer::hTim.Init.Period = 0xFFFFFFFF;
    Timer::hTim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    Timer::hTim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&Timer::hTim) != HAL_OK)
    {
      System::getInstance().errorHandler(ErrorCode::TimBase);
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&Timer::hTim, &sClockSourceConfig) != HAL_OK)
    {
        System::getInstance().errorHandler(ErrorCode::TimConfigClockSource);
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&Timer::hTim, &sMasterConfig) != HAL_OK)
    {
        System::getInstance().errorHandler(ErrorCode::TimMasterConfigSynchronization);
    }
    HAL_TIM_Base_Start(&Timer::hTim);
}
