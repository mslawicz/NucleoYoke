/*
 * ADC.cpp
 *
 *  Created on: 07.09.2019
 *      Author: Marcin
 */

#include "ADC.h"
#include "System.h"

ADConverter::ADConverter()
{
    hADC.Instance = ADC1;
    hADC.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hADC.Init.Resolution = ADC_RESOLUTION_12B;
    hADC.Init.ScanConvMode = ENABLE;
    hADC.Init.ContinuousConvMode = DISABLE;
    hADC.Init.DiscontinuousConvMode = DISABLE;
    hADC.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hADC.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hADC.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hADC.Init.NbrOfConversion = 1;
    hADC.Init.DMAContinuousRequests = ENABLE;
    hADC.Init.EOCSelection = ADC_EOC_SEQ_CONV;
    if (HAL_ADC_Init(&hADC) == HAL_OK)
    {
        System::getInstance().getConsole()->sendMessage(Severity::Info, LogChannel::LC_ADC, "ADC1 initialized");
    }
    else
    {
        System::getInstance().getConsole()->sendMessage(Severity::Error, LogChannel::LC_ADC, "ADC1 initialization failed");
    }
}

