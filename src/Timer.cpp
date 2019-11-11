/*
 * timer.cpp
 *
 *  Created on: 02.08.2019
 *      Author: Marcin
 */

#include "Timer.h"
#include "System.h"

TIM_HandleTypeDef Timer::hTim = {0};
TIM_HandleTypeDef htim3; //QQQ

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
      System::getInstance().errorHandler(ErrorCode::EC_TimBase);
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&Timer::hTim, &sClockSourceConfig) != HAL_OK)
    {
        System::getInstance().errorHandler(ErrorCode::EC_TimConfigClockSource);
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&Timer::hTim, &sMasterConfig) != HAL_OK)
    {
        System::getInstance().errorHandler(ErrorCode::EC_TimMasterConfigSynchronization);
    }
    HAL_TIM_Base_Start(&Timer::hTim);

    //QQQ========= PWM test ==========================
    sClockSourceConfig = {0};
    sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    __HAL_RCC_TIM3_CLK_ENABLE();
    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 96-1;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 20000-1;
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
    {
        System::getInstance().errorHandler(ErrorCode::EC_TimBase);
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
    {
        System::getInstance().errorHandler(ErrorCode::EC_TimBase);
    }
    if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
    {
        System::getInstance().errorHandler(ErrorCode::EC_TimBase);
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
    {
        System::getInstance().errorHandler(ErrorCode::EC_TimBase);
    }
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**TIM3 GPIO Configuration    PC8     ------> TIM3_CH3 */
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    // PWM set and start
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 1500;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
    {
        System::getInstance().errorHandler(ErrorCode::EC_TimBase);
    }
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
}
