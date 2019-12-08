/*
 * Servo.h
 *
 *  Created on: 13.11.2019
 *      Author: Marcin
 */

#ifndef SERVO_H_
#define SERVO_H_

#include "stm32f4xx_hal.h"

/*
 * servo parameters: min, -90, -45, 0, 45, 90, max
 * RDS3235 450, 620, 1060, 1500, 2000, 2510, 2550
 * RDS3225 520, 851, 1150, 1500, 1850, 2150, 2480
 */

class Servo
{
public:
    Servo(TIM_HandleTypeDef* hTimer, uint32_t channel, GPIO_TypeDef* port, uint32_t pin, uint32_t alternateFunction, uint32_t startValue, uint32_t minValue = 1000, uint32_t maxValue = 2000);
    static void config(void);
    void setValue(float value);
    static TIM_HandleTypeDef hTim;
private:
    TIM_HandleTypeDef* hTimer;
    uint32_t channel;
    uint32_t minValue;
    uint32_t maxValue;
};

#endif /* SERVO_H_ */
