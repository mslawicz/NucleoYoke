/*
 * LSM6DS3.h
 *
 *  Created on: 03.09.2019
 *      Author: Marcin
 */

#ifndef LSM6DS3_H_
#define LSM6DS3_H_

#include "I2C.h"
#include "GPIO.h"

#define LSM6DS3_INT1_PORT   GPIOF
#define LSM6DS3_INT1_PIN    GPIO_PIN_2

enum LSM6DS3Register
{
    LSM6DS3_INT1_CTRL=0x0D,
    LSM6DS3_CTRL1_XL=0x10,
    LSM6DS3_CTRL7_G = 0x16,
    LSM6DS3_OUTX_L_G=0x22
};

class LSM6DS3 : public I2cDevice
{
public:
    LSM6DS3(I2cBus* pBus);
    ~LSM6DS3();
    bool isDataReady(void) { return int1Pin.read() == GPIO_PinState::GPIO_PIN_SET; }
    void getData(void) { readRequest(DeviceAddress::LSM6DS3_ADD, LSM6DS3Register::LSM6DS3_OUTX_L_G, 12); }
private:
    GPIO int1Pin;
};

#endif /* LSM6DS3_H_ */
