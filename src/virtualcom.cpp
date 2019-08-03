/*
 * virtualcom.cpp
 *
 *  Created on: 03.08.2019
 *      Author: Marcin
 */

#include "virtualcom.h"

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
void VirtualCom::handler()
{

}
