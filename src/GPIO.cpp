/*
 * gpio.cpp
 *
 *  Created on: 02.08.2019
 *      Author: Marcin
 */

#include "GPIO.h"

GPIO::GPIO(GPIO_TypeDef* port, uint32_t pin, uint32_t mode, uint32_t pull, uint32_t speed, uint32_t alternate) :
    port(port),
    pin(pin)
{
    if(port == nullptr) { return; } else
    if(port == GPIOA) {__HAL_RCC_GPIOA_CLK_ENABLE();} else
    if(port == GPIOB) {__HAL_RCC_GPIOB_CLK_ENABLE();} else
    if(port == GPIOC) {__HAL_RCC_GPIOC_CLK_ENABLE();} else
    if(port == GPIOD) {__HAL_RCC_GPIOD_CLK_ENABLE();} else
    if(port == GPIOE) {__HAL_RCC_GPIOE_CLK_ENABLE();} else
    if(port == GPIOF) {__HAL_RCC_GPIOF_CLK_ENABLE();} else
    if(port == GPIOG) {__HAL_RCC_GPIOG_CLK_ENABLE();} else
    if(port == GPIOH) {__HAL_RCC_GPIOH_CLK_ENABLE();}
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Mode = mode;
    GPIO_InitStruct.Pull = pull;
    GPIO_InitStruct.Speed = speed;
    GPIO_InitStruct.Alternate = alternate;
    HAL_GPIO_Init(port, &GPIO_InitStruct);
}
