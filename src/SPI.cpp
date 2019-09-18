/*
 * SPI.cpp
 *
 *  Created on: 11.09.2019
 *      Author: Marcin
 */

#include "SPI.h"
#include "System.h"
#include "timer.h" //XXX

SpiBus* SpiBus::pSpi3 = nullptr;

SpiBus::SpiBus(SPI_TypeDef* instance) :
    instance(instance)
{
    if(instance == SPI3)
    {
        name = "SPI3";
        /* Peripheral clock enable */
        __HAL_RCC_SPI3_CLK_ENABLE();
        /* DMA controller clock enable */
        __HAL_RCC_DMA1_CLK_ENABLE();
        // MOSI pin
        GPIO(GPIOC, GPIO_PIN_12, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_AF6_SPI3);
        // SCK pin
        GPIO(GPIOC, GPIO_PIN_10, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_AF6_SPI3);
    }

    hSpi.Instance = instance;
    hSpi.Init.Mode = SPI_MODE_MASTER;
    hSpi.Init.Direction = SPI_DIRECTION_2LINES;
    hSpi.Init.DataSize = SPI_DATASIZE_8BIT;
    hSpi.Init.CLKPolarity = SPI_POLARITY_LOW;
    hSpi.Init.CLKPhase = SPI_PHASE_1EDGE;
    hSpi.Init.NSS = SPI_NSS_SOFT;
    hSpi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;    // clock 666 kHz
    hSpi.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hSpi.Init.TIMode = SPI_TIMODE_DISABLE;
    hSpi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hSpi.Init.CRCPolynomial = 10;
    if (HAL_SPI_Init(&hSpi) == HAL_OK)
    {
        System::getInstance().getConsole()->sendMessage(Severity::Info, LogChannel::LC_SPI, name + " initialized");
    }
    else
    {
        System::getInstance().getConsole()->sendMessage(Severity::Error, LogChannel::LC_SPI, name + " initialization failed");
    }

    if(instance == SPI3)
    {
        /* DMA interrupt init */
        /* DMA1_Stream5_IRQn interrupt configuration */
        HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

        /* SPI3_DMA TX Init */
        hDmaTx.Instance = DMA1_Stream5;
        hDmaTx.Init.Channel = DMA_CHANNEL_0;
        hDmaTx.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hDmaTx.Init.PeriphInc = DMA_PINC_DISABLE;
        hDmaTx.Init.MemInc = DMA_MINC_ENABLE;
        hDmaTx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hDmaTx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hDmaTx.Init.Mode = DMA_NORMAL;
        hDmaTx.Init.Priority = DMA_PRIORITY_LOW;
        hDmaTx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hDmaTx) != HAL_OK)
        {
            System::getInstance().getConsole()->sendMessage(Severity::Error, LogChannel::LC_SPI, name + " TX DMA initialization failed");
        }
        __HAL_LINKDMA(&hSpi,hdmatx,hDmaTx);

        pPinCD = new GPIO(SPI3_CD_PORT, SPI3_CD_PIN, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_LOW);
        pSpi3 = this;
    }

    pCurrentlyServedDevice = nullptr;
    busy = false;
}

SpiBus::~SpiBus()
{
    delete pPinCD;
}

/*
 * handler of the SpiBus object
 * to be called periodically in the main loop
 */
void SpiBus::handler(void)
{
    if((!sendRequestQueue.empty()) && (!busy))
    {
        // there is something to send and SPI is free
        auto sendRequest = sendRequestQueue.front();
        sendRequestQueue.pop();
        // copy requested data to send buffer
        this->dataToSend = sendRequest.dataToSend;
        // set command/data signal
        pPinCD->write(sendRequest.isCommand ? GPIO_PinState::GPIO_PIN_RESET : GPIO_PinState::GPIO_PIN_SET);
        // set active CS signal
        sendRequest.pDevice->chipSelect.write(GPIO_PinState::GPIO_PIN_RESET);
        // start DMA transmission
        pCurrentlyServedDevice = sendRequest.pDevice;
        busy = true;
        HAL_SPI_Transmit_DMA(&hSpi, &dataToSend[0], dataToSend.size());
    }
}

SpiDevice::SpiDevice(SpiBus* pBus, GPIO_TypeDef* portCS, uint32_t pinCS) :
        pBus(pBus),
        chipSelect(portCS, pinCS, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_VERY_HIGH)
{
    System::getInstance().getConsole()->sendMessage(Severity::Info, LogChannel::LC_SPI, "SPI device created, CS=" + Console::toHex(reinterpret_cast<uint32_t>(portCS)) + "/" + Console::toHex(pinCS));

    chipSelect.write(GPIO_PinState::GPIO_PIN_SET);
}

SpiDevice::~SpiDevice() {}

/*
 * place request for command/data sending to SPI device
 */
void SpiDevice::sendRequest(std::vector<uint8_t> data, bool itisCommand)
{
    sendRequestContainer newRequest = {this, itisCommand, data};
    pBus->sendRequestQueue.push(newRequest);
}

/**
  * @brief  Tx Transfer completed callback.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if(hspi->Instance == SPI3)
    {
        // unselect device
        SpiBus::pSpi3->pCurrentlyServedDevice->unselect();
        // mark this SPI bus as free
        SpiBus::pSpi3->setBusy(false);
    }
}


/**
  * @brief  SPI error callback.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
    System::getInstance().getConsole()->sendMessage(Severity::Error, LogChannel::LC_SPI, "SPI error code=" + std::to_string(HAL_SPI_GetError(hspi)));
}
