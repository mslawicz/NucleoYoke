/*
 * yoke.h
 *
 *  Created on: 11.08.2019
 *      Author: Marcin
 */


#ifndef YOKE_H_
#define YOKE_H_

#include "USB.h"
#include "LSM6DS3.h"
#include <string>

enum YokeState
{
    YS_start,
    YS_wait_for_imu_data_ready,
    YS_wait_for_data_reception,
    YS_compute_imu_data
};

struct ForceFeedbackData
{
    float pitchForce;
    float rollForce;
    float yawForce;
};

struct ImuRawData
{
    int16_t gyroscopeX;
    int16_t gyroscopeY;
    int16_t gyroscopeZ;
    int16_t accelerometerX;
    int16_t accelerometerY;
    int16_t accelerometerZ;
};

class Yoke
{
public:
    Yoke();
    ~Yoke();
    void handler(void);
    USB::Device& getInterface(void) { return interface; }
    void forceFeedbackHandler(uint8_t* buffer);
private:
    USB::Device interface;
    ForceFeedbackData forceFeedbackData;
    LSM6DS3 imu;
    YokeState state;
    ImuRawData imuRawData;
};

#endif /* YOKE_H_ */
