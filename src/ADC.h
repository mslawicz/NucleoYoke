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
    void registerChannel(uint32_t channel, uint32_t samplingTime = ADC_SAMPLETIME_112CYCLES);
    ADC_HandleTypeDef hADC;
    DMA_HandleTypeDef hDMA;
    static uint32_t channelRank;
};

#endif /* ADC_H_ */
