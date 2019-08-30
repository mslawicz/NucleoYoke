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
        HAL_NVIC_SetPriority(I2C1_EV_IRQn, 1, 0);
        HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);

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
}
