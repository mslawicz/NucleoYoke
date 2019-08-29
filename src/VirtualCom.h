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

#ifdef __cplusplus
 extern "C" {
#endif

void moveDataToReceiveQueue(uint8_t* data, uint32_t length);

#ifdef __cplusplus
}
#endif

typedef std::queue<std::vector<uint8_t>> PacketQueue;

class VirtualCom
{
public:
    VirtualCom(USBD_HandleTypeDef* pUsbdHandle);
    ~VirtualCom();
    void handler(void);
    void send(std::vector<uint8_t> data) { sendQueue.push(data); }
    void putDataToReceiveQueue(uint8_t* data, uint32_t length) { receiveQueue.push(std::vector<uint8_t>(data, data+length)); }
private:
    PacketQueue sendQueue;
    PacketQueue receiveQueue;
    USBD_HandleTypeDef* pUsbdHandle;
};

#endif /* VIRTUALCOM_H_ */
