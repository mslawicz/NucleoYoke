/*
 * uart.h
 *
 *  Created on: 02.08.2019
 *      Author: Marcin
 */

#ifndef UART_H_
#define UART_H_

#include "stm32f4xx_hal.h"
#include <string>
#include <queue>

class UART
{
public:
    UART(USART_TypeDef* instance, uint32_t baudRate);
    ~UART();
    void transmitHandler(void);
    void clearBusyFlag(void) { busy = false; }
    bool isBusy(void) const { return busy; }
    void startReception(void) { HAL_UART_Receive_IT(&hUart, &receptionBuffer, 1); }
    void send(std::string message) { transmitQueue.push(message); }
    void moveReceivedByte(void);
    bool stringReceived(void) const { return !receptionQueue.empty(); }
    std::string getReceivedString(void);
    UART_HandleTypeDef* getUartHandle(void) { return &hUart; }
    static UART* pUSART3;
private:
    UART_HandleTypeDef hUart;   // structure defining UART
    USART_TypeDef* instance;    // USART instance
    std::string stringToSend;   // string currently being sent
    volatile bool busy {false};         // true if transmission is ongoing
    std::queue<std::string> transmitQueue;      // queue of strings to transmit
    uint8_t receptionBuffer {0};    // 1-byte reception buffer
    std::string receivedString; // string gathering received characters
    std::queue<std::string> receptionQueue;     // queue of received strings
};

#endif /* UART_H_ */
