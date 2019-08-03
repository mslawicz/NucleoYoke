/*
 * virtualcom.h
 *
 *  Created on: 03.08.2019
 *      Author: Marcin
 */

#ifndef VIRTUALCOM_H_
#define VIRTUALCOM_H_

#include "stm32f4xx_hal.h"
#include <vector>
#include <queue>
#include "usbd_def.h"

typedef std::queue<std::vector<uint8_t>> PacketQueue;

class VirtualCom
{
public:
    VirtualCom(USBD_HandleTypeDef* pUsbdHandle);
    ~VirtualCom();
    void handler(void);
private:
    PacketQueue sendQueue;
    PacketQueue receiveQueue;
    USBD_HandleTypeDef* pUsbdHandle;
};

#endif /* VIRTUALCOM_H_ */
