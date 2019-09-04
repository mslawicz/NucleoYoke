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
            System::getInstance().testPin1.write(GPIO_PinState::GPIO_PIN_SET); //XXX
            state = YS_wait_for_data_reception;
        }
        break;
    case YS_wait_for_data_reception:
        if(imu.isNewDataReceived())
        {
            imu.markNewDataReceived(false);
            System::getInstance().testPin1.write(GPIO_PinState::GPIO_PIN_RESET); //XXX
            memcpy(&imuRawData, &imu.getReceivedData()[0], imu.getReceivedData().size());
            state = YS_compute_imu_data;
        }
        break;
    case YS_compute_imu_data:
        {
            static uint32_t cnt = 0; //XXX
            if(++cnt % 20 == 0)
            {
                std::string xyz; //XXX
                xyz += std::to_string(imuRawData.gyroscopeX); xyz+=" ";
                xyz += std::to_string(imuRawData.gyroscopeY); xyz+=" ";
                xyz += std::to_string(imuRawData.gyroscopeZ); xyz+=" ";
                xyz += std::to_string(imuRawData.accelerometerX); xyz+=" ";
                xyz += std::to_string(imuRawData.accelerometerY); xyz+=" ";
                xyz += std::to_string(imuRawData.accelerometerZ); xyz+=" ";
                System::getInstance().getConsole()->getInterface().send(xyz+"     \r\n");
            }
            state = YS_wait_for_imu_data_ready;
        }
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


