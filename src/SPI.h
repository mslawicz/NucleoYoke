/*
 * SPI.h
 *
 *  Created on: 11.09.2019
 *      Author: Marcin
 */

#ifndef SPI_H_
#define SPI_H_

#include "stm32f4xx_hal.h"
#include "GPIO.h"
#include <vector>
#include <string>
#include <queue>

class SpiDevice;

struct sendRequestContainer
{
    SpiDevice* pDevice;
    bool isCommand;
    std::vector<uint8_t> dataToSend;
};

class SpiBus
{
public:
    SpiBus(SPI_TypeDef* instance);
    ~SpiBus();
    SPI_HandleTypeDef* getHandle(void) const { return const_cast<__SPI_HandleTypeDef*>(&hSpi); }
    DMA_HandleTypeDef* getDmaTxHandle(void) const { return const_cast<DMA_HandleTypeDef*>(&hDmaTx); }
    void handler(void);
    static SpiBus* pSpi3;
    friend SpiDevice;
private:
    SPI_HandleTypeDef hSpi;
    SPI_TypeDef* instance;
    DMA_HandleTypeDef hDmaTx;
    std::string name;
    std::vector<uint8_t> dataToSend;
    std::queue<sendRequestContainer> sendRequestQueue;
};

class SpiDevice
{
public:
    void send(std::vector<uint8_t> data);
    void receiveRequest(uint16_t size);
    void sendReceiveRequest(std::vector<uint8_t> data);
    void select(void) { chipSelect.write(GPIO_PinState::GPIO_PIN_RESET); }
    void unselect(void) { chipSelect.write(GPIO_PinState::GPIO_PIN_SET); }
    friend SpiBus;
protected:
    SpiDevice(SpiBus* pBus, GPIO_TypeDef* portCS, uint32_t pinCS, bool autoCS = true);
    virtual ~SpiDevice();
    SpiBus* pBus;       // SPI bus for this device
    std::vector<uint8_t> receptionBuffer;    // vector of data being received
private:
    std::vector<uint8_t> dataToSend;    // vector of data to send
    GPIO chipSelect;
    bool newDataReady;
    bool autoCS;
};

#endif /* SPI_H_ */
