/*
 * gpio.h
 *
 *  Created on: 02.08.2019
 *      Author: Marcin
 */

#ifndef GPIO_H_
#define GPIO_H_

#include "stm32f4xx_hal.h"

class GPIO
{
public:
    GPIO(GPIO_TypeDef* port, uint32_t pin, uint32_t mode, uint32_t pull = GPIO_NOPULL, uint32_t speed = GPIO_SPEED_FREQ_VERY_HIGH, uint32_t alternate = 0);
    void reset(void) { HAL_GPIO_DeInit(port, pin); }
    GPIO_PinState read(void) const { return HAL_GPIO_ReadPin(port, pin); }
    void write(GPIO_PinState state) const { HAL_GPIO_WritePin(port, pin ,state); }
    void toggle(void) const { HAL_GPIO_TogglePin(port, pin); }
private:
    GPIO_TypeDef* port;
    uint32_t pin;
};

#endif /* GPIO_H_ */
