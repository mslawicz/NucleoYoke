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

#define SPI4_CD_PORT    GPIOE
#define SPI4_CD_PIN     GPIO_PIN_13

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
    void setBusy(bool state) { busy = state; }
    bool isBusy(void) const { return busy; }
    void handler(void);
    static SpiBus* pSpi2;       // global pointer for SPI2 bus
    static SpiBus* pSpi4;       // global pointer for SPI4 bus
    SpiDevice* pCurrentlyServedDevice;
    friend SpiDevice;
private:
    SPI_HandleTypeDef hSpi;     // SPI handle structure
    SPI_TypeDef* instance;      // instance of SPI bus
    DMA_HandleTypeDef hDmaTx;   // SPI DMA handle structure
    std::string name;           // name of this SPI bus
    std::vector<uint8_t> dataToSend;        // buffer for data being sent
    std::queue<sendRequestContainer> sendRequestQueue;      // queue of send requests from devices
    GPIO* pPinCD;     // command/data pin for this SPI bus; set to nullptr if not used
    volatile bool busy;          // true if SPI is busy transmitting
};

class SpiDevice
{
public:
    void sendRequest(std::vector<uint8_t> data, bool itisCommand = false);
    void unselect(void);
    friend SpiBus;
protected:
    SpiDevice(SpiBus* pBus, GPIO_TypeDef* portCS, uint32_t pinCS);
    SpiDevice(SpiBus* pBus);
    virtual ~SpiDevice();
    SpiBus* pBus;       // SPI bus for this device
private:
    GPIO chipSelect;
    bool autoSelect;
};

#endif /* SPI_H_ */
