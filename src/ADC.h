/*
 * ADC.h
 *
 *  Created on: 07.09.2019
 *      Author: Marcin
 */

#ifndef ADC_H_
#define ADC_H_

#include "stm32f4xx_hal.h"

class ADConverter
{
public:
    ADConverter();
private:
    ADC_HandleTypeDef hADC;
    DMA_HandleTypeDef hDMA;
};

#endif /* ADC_H_ */
