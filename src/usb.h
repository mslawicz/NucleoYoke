/*
 * usb.h
 *
 *  Created on: 02.08.2019
 *      Author: Marcin
 */

#ifndef USB_H_
#define USB_H_

#include "usbd_def.h"

namespace USB
{

class Device
{
public:
    Device();
    ~Device();
    void test(void);
    static USBD_HandleTypeDef deviceHandle;  // device handle structure
private:
};

} /* namespace USB */

#endif /* USB_H_ */
