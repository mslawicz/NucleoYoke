/*
 * I2C.h
 *
 *  Created on: 30.08.2019
 *      Author: Marcin
 */

#ifndef I2C_H_
#define I2C_H_

#include "stm32f4xx_hal.h"
#include <string>
#include <vector>
#include <queue>

// I2C device address
enum DeviceAddress
{
    LSM9DS1_AG_ADD = 0xD6
};

enum ActionType
{
    I2C_WRITE,
    I2C_READ
};

struct DataToSend
{
    ActionType Action;  // action device write/read
    DeviceAddress Address;   // I2C device address shifted left
    uint8_t Register;   // I2C device register to write/read
    std::vector<uint8_t> Data;    // data to send/buffer for reading
};

class I2cDevice;

class I2cBus
{
public:
    I2cBus(I2C_TypeDef* instance);
    I2C_HandleTypeDef* getHandle(void) const { return const_cast<__I2C_HandleTypeDef*>(&hI2c); }
    DMA_HandleTypeDef* getDmaTxHandle(void) const { return const_cast<DMA_HandleTypeDef*>(&hDmaI2cTx); }
    DMA_HandleTypeDef* getDmaRxHandle(void) const { return const_cast<DMA_HandleTypeDef*>(&hDmaI2cRx); }
    void markAsFree(void) { busy = false; }
    void startTransmission(void);
    static I2cBus* pI2c1;
    friend I2cDevice;

    void tempXXX(void); //XXX
private:
    I2C_HandleTypeDef hI2c;
    std::string name;
    DMA_HandleTypeDef hDmaI2cTx;
    DMA_HandleTypeDef hDmaI2cRx;
    std::queue<DataToSend> sendQueue;
    bool busy;      // true if transmission is ongoing
    IRQn_Type eventIRQn;    // remembers event interrupt name
};

class I2cDevice
{
public:
    void writeRequest(DeviceAddress deviceAddress, uint8_t deviceRegister, std::vector<uint8_t> data);
    void readRequest(DeviceAddress deviceAddress, uint8_t deviceRegister, uint16_t size);
    void test(void);
protected:
public://XXX
    I2cDevice(I2cBus* pBus, DeviceAddress deviceAddress);
private:
    I2cBus* pBus;       // I2C bus for this device
    DeviceAddress deviceAddress;        // I2C device address (7-bit left aligned)
};

#endif /* I2C_H_ */
