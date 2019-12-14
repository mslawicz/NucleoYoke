/*
 * Servo.cpp
 *
 *  Created on: 13.11.2019
 *      Author: Marcin
 */

#include "Servo.h"
#include "GPIO.h"
#include "System.h"

TIM_HandleTypeDef Servo::hTim = {0};

/*
 * Servo constructor
 * hTimer - pointer to timer handle
 * channel TIM_CHANNEL_1 ... TIM_CHANNEL_4
 * PWM port/pin, e.g. GPIOA / GPIO_PIN_6
 * alternateFunction, e.g. GPIO_AF2_TIM3
 * start value in the range minValue..maxValue
 */
Servo::Servo(TIM_HandleTypeDef* hTimer, uint32_t channel, GPIO_TypeDef* port, uint32_t pin, uint32_t alternateFunction, uint32_t startValue, uint32_t minValue, uint32_t maxValue) :
        hTimer(hTimer),
        channel(channel),
        minValue(minValue),
        maxValue(maxValue)
{
    // set PWM pin
    GPIO(port, pin, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, alternateFunction);

    if(startValue < minValue)
    {
        startValue = minValue;
    }
    else if (startValue > maxValue)
    {
        startValue = maxValue;
    }

    // PWM set and start
    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = startValue;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(hTimer, &sConfigOC, channel) != HAL_OK)
    {
        System::getInstance().getConsole()->sendMessage(Severity::Error,LogChannel::LC_SERVO, "PWM channel " + std::to_string(channel) + " configuration failed");
    }
    else
    {
        System::getInstance().getConsole()->sendMessage(Severity::Info,LogChannel::LC_SERVO, "PWM channel " + std::to_string(channel) + " configured");
    }
    HAL_TIM_PWM_Start(hTimer, channel);
}

/*
 * Configuration for all servos
 * it must be called before any servo creation
 */
void Servo::config(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    __HAL_RCC_TIM3_CLK_ENABLE();

    Servo::hTim.Instance = TIM3;
    Servo::hTim.Init.Prescaler = 96-1;     // 1 MHz timer clock
    Servo::hTim.Init.CounterMode = TIM_COUNTERMODE_UP;
    Servo::hTim.Init.Period = 20000-1;     // timer period 20 ms with 1 us resolution
    Servo::hTim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    Servo::hTim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&Servo::hTim) != HAL_OK)
    {
        System::getInstance().getConsole()->sendMessage(Severity::Error,LogChannel::LC_SERVO, "Servo timer initialization failed");
        System::getInstance().errorHandler(ErrorCode::TimBase);
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&Servo::hTim, &sClockSourceConfig) != HAL_OK)
    {
        System::getInstance().getConsole()->sendMessage(Severity::Error,LogChannel::LC_SERVO, "Servo timer clock source error");
        System::getInstance().errorHandler(ErrorCode::TimConfigClockSource);
    }
    if (HAL_TIM_PWM_Init(&Servo::hTim) != HAL_OK)
    {
        System::getInstance().getConsole()->sendMessage(Severity::Error,LogChannel::LC_SERVO, "Servo timer PWM initialization failed");
        System::getInstance().errorHandler(ErrorCode::TimPwmInit);
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&Servo::hTim, &sMasterConfig) != HAL_OK)
    {
        System::getInstance().getConsole()->sendMessage(Severity::Error,LogChannel::LC_SERVO, "Servo timer master synchronization failed");
        System::getInstance().errorHandler(ErrorCode::TimMasterConfigSynchronization);
    }
}

/*
 * sets the servo value in the input range <0.0f..1.0f>
 */
void Servo::setValue(float value)
{
    __HAL_TIM_SET_COMPARE(hTimer, channel, (scaleValue<float, uint16_t>(0.0f, 1.0f, minValue, maxValue, value)));
}
