/*
 * SH1106.h
 *
 *  Created on: 14.09.2019
 *      Author: Marcin
 */

#ifndef SH1106_H_
#define SH1106_H_

#include "SPI.h"

#define SH1106_CS_PORT    GPIOC
#define SH1106_CS_PIN     GPIO_PIN_8

class SH1106 : public SpiDevice
{
public:
    SH1106();
};

#endif /* SH1106_H_ */
