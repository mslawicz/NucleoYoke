/*
 * yoke.h
 *
 *  Created on: 11.08.2019
 *      Author: Marcin
 */


#ifndef YOKE_H_
#define YOKE_H_

#include "usb.h"

class Yoke
{
public:
    Yoke();
    ~Yoke();
    USB::Device& getInterface(void) { return interface; }
    void forceFeedbackHandler(uint8_t* buffer);
private:
    void deviceControl();
    USB::Device interface;
    uint8_t* receivedData;
};

#endif /* YOKE_H_ */
