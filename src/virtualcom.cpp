/*
 * virtualcom.cpp
 *
 *  Created on: 03.08.2019
 *      Author: Marcin
 */

#include "virtualcom.h"
#include "system.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"

VirtualCom::VirtualCom(USBD_HandleTypeDef* pUsbdHandle) :
    pUsbdHandle(pUsbdHandle)
{


}

VirtualCom::~VirtualCom()
{
    // TODO Auto-generated destructor stub
}

/*
 * virtual com handler to be executed in a loop
 */
void VirtualCom::handler(void)
{
    if(!sendQueue.empty() && (pUsbdHandle->pClassData != nullptr) && (!((USBD_CDC_HandleTypeDef*)pUsbdHandle->pClassData)->TxState))
    {
        // send queue not empty and USB send is not busy
        CDC_Transmit_FS(&sendQueue.front()[0], sendQueue.front().size());
        sendQueue.pop();
    }
    if(!receiveQueue.empty())
    {
        // receive queue is not empty
        // do sth with the received data
        receiveQueue.pop();
    }
}

void moveDataToReceiveQueue(uint8_t* data, uint32_t length)
{
    //System::getInstance().getVirtualCom()->putDataToReceiveQueue(data, length);
}
