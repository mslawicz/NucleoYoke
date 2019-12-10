/*
 * HX711.cpp
 *
 *  Created on: 10.12.2019
 *      Author: Marcin
 */

#include "HX711.h"

HX711::HX711() :
    sckSignal(HX711_SCK_PORT, HX711_SCK_PIN, GPIO_MODE_OUTPUT_PP, GPIO_PULLDOWN, GPIO_SPEED_MEDIUM)
{
    boards.push_back(HX711board{GPIO(GPIOA, GPIO_PIN_0, GPIO_MODE_INPUT, GPIO_PULLDOWN, GPIO_SPEED_MEDIUM), 0});  // yoke pitch tension converter board
}

void HX711::handler(void)
{
}
