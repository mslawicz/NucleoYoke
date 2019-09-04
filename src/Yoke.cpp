/*
 * yoke.cpp
 *
 *  Created on: 11.08.2019
 *      Author: Marcin
 */

#include "usbd_customhid.h"
#include "Yoke.h"
#include "System.h"


Yoke::Yoke() :
    interface(),
    imu(I2cBus::pI2c1)
{
    state = YS_start;
}

Yoke::~Yoke()
{
    // TODO Auto-generated destructor stub
}

/*
 * yoke handler
 * to be called in a main loop
 */
void Yoke::handler(void)
{
    interface.test();  //XXX

    switch(state)
    {
    case YS_start:
        state = YS_wait_for_imu_data_ready;
        break;
    case YS_wait_for_imu_data_ready:
        if(imu.isDataReady())
        {
            imu.getData();
            state = YS_wait_for_data_reception;
        }
        break;
    case YS_wait_for_data_reception:
        break;
    case YS_compute_imu_data:
        break;
    default:
        break;
    }
}

/*
 * called in USB data reception callback function
 */
void Yoke::forceFeedbackHandler(uint8_t* buffer)
{
    if(buffer[0] == 0x03)
    {
        memcpy(&forceFeedbackData, buffer+1, sizeof(ForceFeedbackData));
    }
    else
    {
        System::getInstance().getConsole()->sendMessage(Severity::Warning,LogChannel::LC_USB, "Received unexpected HID report id=" + std::to_string(static_cast<int>(buffer[0])));
    }
}


