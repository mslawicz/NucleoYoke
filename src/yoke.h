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
private:
    USB::Device interface;
};

#endif /* YOKE_H_ */
