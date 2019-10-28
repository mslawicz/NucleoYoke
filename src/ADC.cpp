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

ADConverter* ADConverter::pADC1 = nullptr;

ADConverter::ADConverter()
{
    __HAL_RCC_ADC1_CLK_ENABLE();

    hADC.State = HAL_ADC_STATE_RESET;
    hADC.Instance = ADC1;
    hADC.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hADC.Init.Resolution = ADC_RESOLUTION_12B;
    hADC.Init.ScanConvMode = ENABLE;
    hADC.Init.ContinuousConvMode = DISABLE;
    hADC.Init.DiscontinuousConvMode = DISABLE;
    hADC.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hADC.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hADC.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hADC.Init.NbrOfConversion = 7;
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
    channelRank = 1;
    registerChannel(ADC_CHANNEL_14);    // rudder
    registerChannel(ADC_CHANNEL_0);     // thrust
    registerChannel(ADC_CHANNEL_10);    // mixture
    registerChannel(ADC_CHANNEL_11);    // propeller
    registerChannel(ADC_CHANNEL_12);    // analog joystick X
    registerChannel(ADC_CHANNEL_13);    // analog joystick Y
    registerChannel(ADC_CHANNEL_1);     // autorudder

    /* ADC1 DMA Init */
    __HAL_RCC_DMA2_CLK_ENABLE();

    hDMA.Instance = DMA2_Stream0;
    hDMA.Init.Channel = DMA_CHANNEL_0;
    hDMA.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hDMA.Init.PeriphInc = DMA_PINC_DISABLE;
    hDMA.Init.MemInc = DMA_MINC_ENABLE;
    hDMA.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hDMA.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hDMA.Init.Mode = DMA_NORMAL;
    hDMA.Init.Priority = DMA_PRIORITY_LOW;
    hDMA.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hDMA) != HAL_OK)
    {
        System::getInstance().getConsole()->sendMessage(Severity::Error, LogChannel::LC_ADC, "ADC1 DMA initialization failed");
    }

    __HAL_LINKDMA(&hADC, DMA_Handle, hDMA);

    pADC1 = this;

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
            {ADC_CHANNEL_1, std::pair<GPIO_TypeDef*, uint32_t>(GPIOA, GPIO_PIN_1)},
            {ADC_CHANNEL_2, std::pair<GPIO_TypeDef*, uint32_t>(GPIOA, GPIO_PIN_2)},
            {ADC_CHANNEL_3, std::pair<GPIO_TypeDef*, uint32_t>(GPIOA, GPIO_PIN_3)},
            {ADC_CHANNEL_4, std::pair<GPIO_TypeDef*, uint32_t>(GPIOA, GPIO_PIN_4)},
            {ADC_CHANNEL_5, std::pair<GPIO_TypeDef*, uint32_t>(GPIOA, GPIO_PIN_5)},
            {ADC_CHANNEL_6, std::pair<GPIO_TypeDef*, uint32_t>(GPIOA, GPIO_PIN_6)},
            {ADC_CHANNEL_7, std::pair<GPIO_TypeDef*, uint32_t>(GPIOA, GPIO_PIN_7)},
            {ADC_CHANNEL_8, std::pair<GPIO_TypeDef*, uint32_t>(GPIOB, GPIO_PIN_0)},
            {ADC_CHANNEL_9, std::pair<GPIO_TypeDef*, uint32_t>(GPIOB, GPIO_PIN_1)},
            {ADC_CHANNEL_10, std::pair<GPIO_TypeDef*, uint32_t>(GPIOC, GPIO_PIN_0)},
            {ADC_CHANNEL_11, std::pair<GPIO_TypeDef*, uint32_t>(GPIOC, GPIO_PIN_1)},
            {ADC_CHANNEL_12, std::pair<GPIO_TypeDef*, uint32_t>(GPIOC, GPIO_PIN_2)},
            {ADC_CHANNEL_13, std::pair<GPIO_TypeDef*, uint32_t>(GPIOC, GPIO_PIN_3)},
            {ADC_CHANNEL_14, std::pair<GPIO_TypeDef*, uint32_t>(GPIOC, GPIO_PIN_4)},
            {ADC_CHANNEL_15, std::pair<GPIO_TypeDef*, uint32_t>(GPIOC, GPIO_PIN_5)}
    };

    ADC_ChannelConfTypeDef config = {channel, channelRank++, samplingTime, 0};
    if (HAL_ADC_ConfigChannel(&hADC, &config) == HAL_OK)
    {
        System::getInstance().getConsole()->sendMessage(Severity::Info, LogChannel::LC_ADC, "ADC1 channel " + std::to_string(channel) + " registered");
    }
    else
    {
        System::getInstance().getConsole()->sendMessage(Severity::Error, LogChannel::LC_ADC, "ADC1 channel " + std::to_string(channel) + " registration failed");
    }
    GPIO(ADCPorts.find(channel)->second.first, ADCPorts.find(channel)->second.second, GPIO_MODE_ANALOG, GPIO_NOPULL);
    convertedValues.push_back(0);
}


/**
  * @brief  Conversion complete callback in non blocking mode
  * @param  AdcHandle : AdcHandle handle
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hADC)
{
    // nothing must be done here
}

/*
 * start conversions of all registered channels in DMA mode
 */
void ADConverter::startConversions(void)
{
    if(HAL_ADC_Start_DMA(&hADC, reinterpret_cast<uint32_t*>(&convertedValues[0]), channelRank-1) != HAL_OK)
    {
        System::getInstance().getConsole()->sendMessage(Severity::Error, LogChannel::LC_ADC, "ADC1 start conversion error");
    }
}
