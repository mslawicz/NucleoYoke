/*
 * I2C.cpp
 *
 *  Created on: 30.08.2019
 *      Author: Marcin
 */

#include "I2C.h"
#include "GPIO.h"
#include "System.h"

// this static pointer is used by interrupts
I2cBus* I2cBus::pI2c1 = nullptr;
I2cBus* I2cBus::pI2c2 = nullptr;

I2cBus::I2cBus(I2C_TypeDef* instance)
{
    if(instance == I2C1)
    {
        name = "I2C1";

        // SCL pin
        GPIO(GPIOB, GPIO_PIN_8, GPIO_MODE_AF_OD, GPIO_PULLUP, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_AF4_I2C1);
        // SDA pin
        GPIO(GPIOB, GPIO_PIN_9, GPIO_MODE_AF_OD, GPIO_PULLUP, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_AF4_I2C1);
        /* Peripheral clock enable */
        __HAL_RCC_I2C1_CLK_ENABLE();
        __HAL_RCC_DMA1_CLK_ENABLE();
        /* Peripheral interrupt init */
        HAL_NVIC_SetPriority(I2C1_EV_IRQn, 2, 1);
        HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
        HAL_NVIC_SetPriority(I2C1_ER_IRQn, 1, 3);
        HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);

        /* DMA interrupt init */
        /* DMA1_Stream0_IRQn interrupt configuration */
        HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
        /* DMA1_Stream1_IRQn interrupt configuration */
        HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);

        /* Peripheral DMA init*/

        hDmaI2cTx.Instance = DMA1_Stream1;
        hDmaI2cTx.Init.Channel = DMA_CHANNEL_0;
        hDmaI2cTx.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hDmaI2cTx.Init.PeriphInc = DMA_PINC_DISABLE;
        hDmaI2cTx.Init.MemInc = DMA_MINC_ENABLE;
        hDmaI2cTx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hDmaI2cTx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hDmaI2cTx.Init.Mode = DMA_NORMAL;
        hDmaI2cTx.Init.Priority = DMA_PRIORITY_LOW;
        hDmaI2cTx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hDmaI2cTx) != HAL_OK)
        {
            System::getInstance().getConsole()->sendMessage(Severity::Error, LogChannel::LC_I2C, name + " TX DMA initialization failed");
        }
        __HAL_LINKDMA(&hI2c,hdmatx,hDmaI2cTx);

        hDmaI2cRx.Instance = DMA1_Stream0;
        hDmaI2cRx.Init.Channel = DMA_CHANNEL_1;
        hDmaI2cRx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hDmaI2cRx.Init.PeriphInc = DMA_PINC_DISABLE;
        hDmaI2cRx.Init.MemInc = DMA_MINC_ENABLE;
        hDmaI2cRx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hDmaI2cRx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hDmaI2cRx.Init.Mode = DMA_NORMAL;
        hDmaI2cRx.Init.Priority = DMA_PRIORITY_LOW;
        hDmaI2cRx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hDmaI2cRx) != HAL_OK)
        {
            System::getInstance().getConsole()->sendMessage(Severity::Error, LogChannel::LC_I2C, name + "  RX DMA initialization failed");
        }

        __HAL_LINKDMA(&hI2c,hdmarx,hDmaI2cRx);

        pI2c1 = this;
    }

    if(instance == I2C2)
    {
        name = "I2C2";

        // SCL pin
        GPIO(GPIOF, GPIO_PIN_1, GPIO_MODE_AF_OD, GPIO_PULLUP, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_AF4_I2C2);
        // SDA pin
        GPIO(GPIOF, GPIO_PIN_0, GPIO_MODE_AF_OD, GPIO_PULLUP, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_AF4_I2C2);
        /* Peripheral clock enable */
        __HAL_RCC_I2C2_CLK_ENABLE();
        __HAL_RCC_DMA1_CLK_ENABLE();
        /* Peripheral interrupt init */
        HAL_NVIC_SetPriority(I2C2_EV_IRQn, 2, 1);
        HAL_NVIC_EnableIRQ(I2C2_EV_IRQn);
        HAL_NVIC_SetPriority(I2C2_ER_IRQn, 1, 3);
        HAL_NVIC_EnableIRQ(I2C2_ER_IRQn);

        /* DMA interrupt init */
        /* DMA1_Stream7_IRQn interrupt configuration */
        HAL_NVIC_SetPriority(DMA1_Stream7_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(DMA1_Stream7_IRQn);
        /* DMA1_Stream2_IRQn interrupt configuration */
        HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);

        /* Peripheral DMA init*/

        hDmaI2cTx.Instance = DMA1_Stream7;
        hDmaI2cTx.Init.Channel = DMA_CHANNEL_7;
        hDmaI2cTx.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hDmaI2cTx.Init.PeriphInc = DMA_PINC_DISABLE;
        hDmaI2cTx.Init.MemInc = DMA_MINC_ENABLE;
        hDmaI2cTx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hDmaI2cTx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hDmaI2cTx.Init.Mode = DMA_NORMAL;
        hDmaI2cTx.Init.Priority = DMA_PRIORITY_LOW;
        hDmaI2cTx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hDmaI2cTx) != HAL_OK)
        {
            System::getInstance().getConsole()->sendMessage(Severity::Error, LogChannel::LC_I2C, name + " TX DMA initialization failed");
        }
        __HAL_LINKDMA(&hI2c,hdmatx,hDmaI2cTx);

        hDmaI2cRx.Instance = DMA1_Stream2;
        hDmaI2cRx.Init.Channel = DMA_CHANNEL_7;
        hDmaI2cRx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hDmaI2cRx.Init.PeriphInc = DMA_PINC_DISABLE;
        hDmaI2cRx.Init.MemInc = DMA_MINC_ENABLE;
        hDmaI2cRx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hDmaI2cRx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hDmaI2cRx.Init.Mode = DMA_NORMAL;
        hDmaI2cRx.Init.Priority = DMA_PRIORITY_LOW;
        hDmaI2cRx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hDmaI2cRx) != HAL_OK)
        {
            System::getInstance().getConsole()->sendMessage(Severity::Error, LogChannel::LC_I2C, name + "  RX DMA initialization failed");
        }

        __HAL_LINKDMA(&hI2c,hdmarx,hDmaI2cRx);

        pI2c2 = this;
    }

    hI2c.Instance = instance;
    hI2c.Init.ClockSpeed = 400000;
    hI2c.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hI2c.Init.OwnAddress1 = 0;
    hI2c.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hI2c.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hI2c.Init.OwnAddress2 = 0;
    hI2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hI2c.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hI2c) == HAL_OK)
    {
        System::getInstance().getConsole()->sendMessage(Severity::Info, LogChannel::LC_I2C, name + " initialized");
    }
    else
    {
        System::getInstance().getConsole()->sendMessage(Severity::Error, LogChannel::LC_I2C, name + " initialization failed");
    }
    if(HAL_I2CEx_ConfigAnalogFilter(&hI2c, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
    {
        System::getInstance().getConsole()->sendMessage(Severity::Error, LogChannel::LC_I2C, name + " filter configuration failed");
    }

    pLastServedDevice = nullptr;
}

/*
 * constructor of I2C device
 * every device should have an I2C bus assigned and should have a proper address declared
 */
I2cDevice::I2cDevice(I2cBus* pBus, DeviceAddress deviceAddress) :
        pBus(pBus),
        deviceAddress(deviceAddress)
{
    System::getInstance().getConsole()->sendMessage(Severity::Info, LogChannel::LC_I2C, "I2C device created, addr=" + Console::toHex(deviceAddress, 2));
    newDataReceived = false;
}


/**
  * @brief  Memory Tx Transfer completed callback.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval None
  */
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hI2c)
{
    if(hI2c->Instance == I2C1)
    {

    }
}

/**
  * @brief  Memory Rx Transfer completed callback.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval None
  */
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hI2c)
{
    if(hI2c->Instance == I2C1)
    {
        I2cBus::pI2c1->getLastServedDevice()->markNewDataReceived(true);
    }
}

/**
  * @brief  I2C error callback.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval None
  */
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    System::getInstance().getConsole()->sendMessage(Severity::Error, LogChannel::LC_I2C, " I2C error code=" + std::to_string(HAL_I2C_GetError(hi2c)));
}


/*
 * put send request structure into the I2cBus send queue
 * the actual transmission is initiated in I2C bus handler
 */
void I2cDevice::writeRequest(DeviceAddress deviceAddress, uint8_t deviceRegister, std::vector<uint8_t> data)
{
    I2cRequest dataToSend =
    {
            ActionType::I2C_WRITE,
            deviceAddress,
            deviceRegister,
            data,
            0,
            this
    };
    pBus->sendRequestQueue.push(dataToSend);
}

/*
 * put send request structure into the I2cBus send queue
 * the actual transmission is initiated in I2C bus handler
 */
void I2cDevice::readRequest(DeviceAddress deviceAddress, uint8_t deviceRegister, uint16_t size)
{
    I2cRequest dataToSend =
    {
            ActionType::I2C_READ,
            deviceAddress,
            deviceRegister,
            std::vector<uint8_t>(),
            size,
            this
    };
    pBus->sendRequestQueue.push(dataToSend);
}

/*
 * I2C bus handler to be executed in a loop
 */
void I2cBus::handler(void)
{
    if(sendRequestQueue.empty())
    {
        // there is no data to transmit
        return;
    }

    if( HAL_I2C_GetState(&hI2c)!= HAL_I2C_STATE_READY)
    {
        // I2C in not ready for next transmission
        return;
    }

    I2cRequest currentRequest = sendRequestQueue.front();
    sendRequestQueue.pop();
    pLastServedDevice = currentRequest.pDevice;
    if(currentRequest.Action == ActionType::I2C_WRITE)
    {
        // copy data to send buffer to have it in scope
        sendBuffer = currentRequest.Data;
        // write to device
        if(HAL_I2C_Mem_Write_DMA(&hI2c, currentRequest.Address, currentRequest.Register, I2C_MEMADD_SIZE_8BIT, &sendBuffer[0], sendBuffer.size()) != HAL_OK)
        {
            auto error = HAL_I2C_GetError(&hI2c);
            if(error == HAL_I2C_ERROR_DMA)
            {
                HAL_DMA_Init(&hDmaI2cTx);
            }
            System::getInstance().getConsole()->sendMessage(Severity::Error, LogChannel::LC_I2C, name + " write error=" + Console::toHex(error));
        }
    }
    else
    {
        // mark that new data is not yet available
        currentRequest.pDevice->newDataReceived = false;
        // initialize receive buffer in the device object
        currentRequest.pDevice->receiveBuffer = std::vector<uint8_t>(currentRequest.NoOfBytesToRead, 0);
        // read from device
        if(HAL_I2C_Mem_Read_DMA(&hI2c, currentRequest.Address, currentRequest.Register, I2C_MEMADD_SIZE_8BIT, &currentRequest.pDevice->receiveBuffer[0], currentRequest.NoOfBytesToRead) != HAL_OK)
        {
            auto error = HAL_I2C_GetError(&hI2c);
            if(error == HAL_I2C_ERROR_DMA)
            {
                HAL_DMA_Init(&hDmaI2cRx);
            }
            System::getInstance().getConsole()->sendMessage(Severity::Error, LogChannel::LC_I2C, name + " read error=" + Console::toHex(error));
        }
    }
}
