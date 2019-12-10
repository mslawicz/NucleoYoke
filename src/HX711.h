/*
 * HX711.h
 *
 *  Created on: 10.12.2019
 *      Author: Marcin
 */

#ifndef HX711_H_
#define HX711_H_

#include "GPIO.h"
#include <vector>

struct HX711board
{
    GPIO dOut;
    uint32_t data;
};

#define HX711_SCK_PORT  GPIOF   // XXX change!
#define HX711_SCK_PIN   GPIO_PIN_11  // XXX change!

enum HX711State
{
    HXS_wait_for_data_ready,
    HXS_read_bit
};

class HX711
{
public:
    HX711();
    void handler(void);
private:
    GPIO sckSignal;
    std::vector<HX711board> boards;
    HX711State state;
};

#endif /* HX711_H_ */
