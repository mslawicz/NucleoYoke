/*
 * ADC.cpp
 *
 *  Created on: 07.09.2019
 *      Author: Marcin
 */

#include "ADC.h"
#include "System.h"
#include "GPIO.h"
#include <unordered_map>
#include <utility>

uint32_t ADConverter::channelRank = 0;

ADConverter::ADConverter()
{
    __HAL_RCC_ADC1_CLK_ENABLE();

    hADC.Instance = ADC1;
    hADC.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hADC.Init.Resolution = ADC_RESOLUTION_12B;
    hADC.Init.ScanConvMode = ENABLE;
    hADC.Init.ContinuousConvMode = DISABLE;
    hADC.Init.DiscontinuousConvMode = DISABLE;
    hADC.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hADC.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hADC.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hADC.Init.NbrOfConversion = 2;
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

    // register ADC channels
    registerChannel(ADC_CHANNEL_0);
    registerChannel(ADC_CHANNEL_8);

    __HAL_RCC_DMA2_CLK_ENABLE();
    /* DMA2_Stream0_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
}


/*
 * registers ADC channel
 */
void ADConverter::registerChannel(uint32_t channel, uint32_t samplingTime)
{
    const std::unordered_map<uint32_t, std::pair<GPIO_TypeDef*, uint32_t>> ADCPorts =
    {
            {ADC_CHANNEL_0, std::pair<GPIO_TypeDef*, uint32_t>(GPIOA, GPIO_PIN_0)},
            {ADC_CHANNEL_8, std::pair<GPIO_TypeDef*, uint32_t>(GPIOB, GPIO_PIN_0)}
    };

    ADC_ChannelConfTypeDef config = {channel, ADConverter::channelRank++, samplingTime, 0};
    if (HAL_ADC_ConfigChannel(&hADC, &config) == HAL_OK)
    {
        System::getInstance().getConsole()->sendMessage(Severity::Info, LogChannel::LC_ADC, "ADC1 channel " + std::to_string(channel) + " registered");
    }
    else
    {
        System::getInstance().getConsole()->sendMessage(Severity::Error, LogChannel::LC_ADC, "ADC1 channel " + std::to_string(channel) + " registration failed");
    }
    GPIO(ADCPorts.find(channel)->second.first, ADCPorts.find(channel)->second.second, GPIO_MODE_ANALOG, GPIO_NOPULL);
}
