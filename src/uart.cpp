/*
 * uart.cpp
 *
 *  Created on: 02.08.2019
 *      Author: Marcin
 */

#include "uart.h"

#include "GPIO.h"
#include "System.h"

UART* UART::pUSART3 = nullptr;

UART::UART(USART_TypeDef* instance, uint32_t baudRate) :
    instance(instance)
{
    if(instance == USART3)
    {
        __HAL_RCC_USART3_CLK_ENABLE();
        //UART TX pin
        GPIO(GPIOD, GPIO_PIN_9, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_AF7_USART3);
        //UART RX pin
        GPIO(GPIOD, GPIO_PIN_8, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_AF7_USART3);
    }

    hUart.Instance = instance;
    hUart.Init.BaudRate = baudRate;
    hUart.Init.WordLength = UART_WORDLENGTH_8B;
    hUart.Init.StopBits = UART_STOPBITS_1;
    hUart.Init.Parity = UART_PARITY_NONE;
    hUart.Init.Mode = UART_MODE_TX_RX;
    hUart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    hUart.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&hUart) != HAL_OK)
    {
      System::getInstance().errorHandler();
    }

    if(instance == USART3)
    {
        pUSART3 = this;
        /* USART3 interrupt Init */
        HAL_NVIC_SetPriority(USART3_IRQn, 1, 0);
        HAL_NVIC_EnableIRQ(USART3_IRQn);
    }
    busy = false;
    receptionBuffer = 0;
}

UART::~UART()
{
    if(instance == USART3)
    {
        /* USART3 interrupt DeInit */
        HAL_NVIC_DisableIRQ(USART3_IRQn);
    }
}


/**
  * @brief  Tx Transfer completed callbacks.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART3)
    {
        UART::pUSART3->clearBusyFlag();
    }
}

/**
  * @brief  Rx Transfer completed callbacks.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART3)
    {
        // move received character to the reception string
        UART::pUSART3->moveReceivedByte();
        // wait for the next character to receive
        UART::pUSART3->startReception();
    }
}

/*
 * UART transmit handler to be called periodically in a loop
 */
void UART::transmitHandler(void)
{
    if((!busy) && (!transmitQueue.empty()))
    {
        // UART is not busy and the transmit queue is not empty
        stringToSend = transmitQueue.front();
        transmitQueue.pop();
        busy = true;
        HAL_UART_Transmit_IT(&hUart, reinterpret_cast<uint8_t*>(&stringToSend[0]), stringToSend.length());
    }
}

/*
 * move received character to reception string
 * store complete string to the reception queue
 * to be called in HAL_UART_RxCpltCallback
 */
void UART::moveReceivedByte(void)
{
    if(receptionBuffer == 0x0A)
    {
        // LF character received
        receptionQueue.push(receivedString);
        receivedString.clear();
    }
    else
    {
        // any other character received
        receivedString.push_back(receptionBuffer);
        // send echo
        send(std::string(1, receptionBuffer));
    }
}

/*
 * get received string from the reception queue
 */
std::string UART::getReceivedString(void)
{
    __HAL_UART_DISABLE_IT(&hUart, UART_IT_RXNE);
    auto str = receptionQueue.front();
    receptionQueue.pop();
    __HAL_UART_ENABLE_IT(&hUart, UART_IT_RXNE);
    return str;
}
