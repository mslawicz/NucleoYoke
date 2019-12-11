/*
 * HX711.h
 *
 *  Created on: 10.12.2019
 *      Author: Marcin
 */

#ifndef HX711_H_
#define HX711_H_

#include "GPIO.h"

enum HX711State
{
    HXS_wait_for_data_ready,
    HXS_clock_pulse,
    HXS_after_pulse
};

class HX711
{
public:
    HX711(GPIO_TypeDef* clockPort, uint32_t clockPin, GPIO_TypeDef* dataPort, uint32_t dataPin, uint8_t totalPulses = 25);
    void handler(void);
    uint32_t getData(void) const { return dataRegister; }
private:
    GPIO clockSignal;
    GPIO dataSignal;
    HX711State state;
    uint8_t pulseNumber;
    uint32_t dataBuffer;
    uint8_t totalPulses;
    uint32_t dataRegister;
};

#endif /* HX711_H_ */
