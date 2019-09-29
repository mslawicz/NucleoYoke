/*
 * SPI.cpp
 *
 *  Created on: 11.09.2019
 *      Author: Marcin
 */

#include "SPI.h"
#include "System.h"
#include "timer.h" //XXX

SpiBus* SpiBus::pSpi2 = nullptr;
SpiBus* SpiBus::pSpi4 = nullptr;

SpiBus::SpiBus(SPI_TypeDef* instance) :
    instance(instance)
{
    if(instance == SPI2)
    {
        name = "SPI2";
        /* Peripheral clock enable */
        __HAL_RCC_SPI2_CLK_ENABLE();
        /* DMA controller clock enable */
        __HAL_RCC_DMA1_CLK_ENABLE();
        // MOSI pin
        GPIO(GPIOB, GPIO_PIN_15, GPIO_MODE_AF_PP, GPIO_PULLDOWN, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_AF5_SPI2);
        // SCK pin
        GPIO(GPIOB, GPIO_PIN_13, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_AF5_SPI2);

        hSpi.Instance = instance;
        hSpi.Init.Mode = SPI_MODE_MASTER;
        hSpi.Init.Direction = SPI_DIRECTION_2LINES;
        hSpi.Init.DataSize = SPI_DATASIZE_8BIT;
        hSpi.Init.CLKPolarity = SPI_POLARITY_LOW;
        hSpi.Init.CLKPhase = SPI_PHASE_1EDGE;
        hSpi.Init.NSS = SPI_NSS_SOFT;
        hSpi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
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

        /* SPI2_DMA TX Init */
        hDmaTx.Instance = DMA1_Stream4;
        hDmaTx.Init.Channel = DMA_CHANNEL_0;
        hDmaTx.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hDmaTx.Init.PeriphInc = DMA_PINC_DISABLE;
        hDmaTx.Init.MemInc = DMA_MINC_ENABLE;
        hDmaTx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hDmaTx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hDmaTx.Init.Mode = DMA_NORMAL;
        hDmaTx.Init.Priority = DMA_PRIORITY_LOW;
        hDmaTx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hDmaTx) == HAL_OK)
        {
            /* DMA1_Stream4_IRQn interrupt configuration */
            HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 0);
            HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);
            __HAL_LINKDMA(&hSpi,hdmatx,hDmaTx);
        }
        else
        {
            System::getInstance().getConsole()->sendMessage(Severity::Error, LogChannel::LC_SPI, name + " TX DMA initialization failed");
        }

        pPinCD = nullptr;
        pSpi2 = this;
    }
    if(instance == SPI4)
    {
        name = "SPI4";
        /* Peripheral clock enable */
        __HAL_RCC_SPI4_CLK_ENABLE();
        /* DMA controller clock enable */
        __HAL_RCC_DMA2_CLK_ENABLE();
        // MOSI pin
        GPIO(GPIOE, GPIO_PIN_14, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_AF5_SPI4);
        // SCK pin
        GPIO(GPIOE, GPIO_PIN_12, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_AF5_SPI4);

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

        /* SPI4_DMA TX Init */
        hDmaTx.Instance = DMA2_Stream1;
        hDmaTx.Init.Channel = DMA_CHANNEL_4;
        hDmaTx.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hDmaTx.Init.PeriphInc = DMA_PINC_DISABLE;
        hDmaTx.Init.MemInc = DMA_MINC_ENABLE;
        hDmaTx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hDmaTx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hDmaTx.Init.Mode = DMA_NORMAL;
        hDmaTx.Init.Priority = DMA_PRIORITY_LOW;
        hDmaTx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hDmaTx) == HAL_OK)
        {
            /* DMA2_Stream1_IRQn interrupt configuration */
            HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0, 0);
            HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
            __HAL_LINKDMA(&hSpi,hdmatx,hDmaTx);
        }
        else
        {
            System::getInstance().getConsole()->sendMessage(Severity::Error, LogChannel::LC_SPI, name + " TX DMA initialization failed");
        }

        pPinCD = new GPIO(SPI4_CD_PORT, SPI4_CD_PIN, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_LOW);
        pSpi4 = this;
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
        if(pPinCD != nullptr)
        {
            pPinCD->write(sendRequest.isCommand ? GPIO_PinState::GPIO_PIN_RESET : GPIO_PinState::GPIO_PIN_SET);
        }
        if(sendRequest.pDevice->autoSelect)
        {
            // set active CS signal
            sendRequest.pDevice->chipSelect.write(GPIO_PinState::GPIO_PIN_RESET);
        }
        // start DMA transmission
        pCurrentlyServedDevice = sendRequest.pDevice;
        busy = true;
        HAL_SPI_Transmit_DMA(&hSpi, &dataToSend[0], dataToSend.size());
    }
}

/*
 * SPI device constructor for a device with CS signal handling
 */
SpiDevice::SpiDevice(SpiBus* pBus, GPIO_TypeDef* portCS, uint32_t pinCS) :
        pBus(pBus),
        chipSelect(portCS, pinCS, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_VERY_HIGH)
{
    System::getInstance().getConsole()->sendMessage(Severity::Info, LogChannel::LC_SPI, "SPI device created, CS=" + Console::toHex(reinterpret_cast<uint32_t>(portCS)) + "/" + Console::toHex(pinCS));
    autoSelect = true;
    chipSelect.write(GPIO_PinState::GPIO_PIN_SET);
}

/*
 * SPI device constructor for a device without CS signal handling
 */
SpiDevice::SpiDevice(SpiBus* pBus) :
        pBus(pBus),
        chipSelect(nullptr, 0, GPIO_MODE_INPUT)
{
    System::getInstance().getConsole()->sendMessage(Severity::Info, LogChannel::LC_SPI, "SPI device created with no CS signal");
    autoSelect = false;
}

/*
 * sets CS inactive
 */
void SpiDevice::unselect(void)
{
    if(autoSelect)
    {
        // unselect only devices with autoSelected flag set
        chipSelect.write(GPIO_PinState::GPIO_PIN_SET);
    }
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
    if(hspi->Instance == SPI2)
    {
        // SPI2 doesn't need CS handling (1-wire transmission)
        // mark this SPI bus as free
        SpiBus::pSpi2->setBusy(false);
    }

    if(hspi->Instance == SPI4)
    {
        // unselect device
        SpiBus::pSpi4->pCurrentlyServedDevice->unselect();
        // mark this SPI bus as free
        SpiBus::pSpi4->setBusy(false);
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
