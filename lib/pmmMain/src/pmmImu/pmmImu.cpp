/* pmmImu.cpp
 * Code for the Inertial Measure Unit (IMU!)
 *
 * By Henrique Bruno Fantauzzi de Almeida (SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <Wire.h>

#include <MPU6050.h>
#include <HMC5883L.h>
#include <BMP085.h>

#include "pmmConsts.h"
#include "pmmImu/pmmImu.h"
#include "pmmErrorsCentral/pmmErrorsCentral.h"

PmmImu::PmmImu()
{
}


int PmmImu::initMpu()
{
    mMpu.initialize();

    if(mMpu.testConnection())
    {
        mPmmErrorsCentral->reportErrorByCode(ERROR_MAGNETOMETER_INIT);
        PMM_DEBUG_PRINTLN("PmmImu #1: MPU6050 INIT FAILED");
        return 1;
    }
    /*
    mpu.setXGyroscopeOffset(220);
    mpu.setYGyroscopeOffset(76);
    mpu.setZGyroscopeOffset(-85);
    mpu.setZAccelerometerOffset(1788); // 1688 factory default for my test chip*/
    PMM_DEBUG_PRINTLN_MORE("PmmImu [M]: MPU6050 initialized successfully!");
    return 0;

}



int PmmImu::initMagnetometer()
{
    mMagnetometer.initialize();

    if (mMagnetometer.testConnection())
    {
        PMM_DEBUG_PRINTLN("PmmImu #2: MAGNETOMETER INIT ERROR");
        return 1;
    }
    //mMagnetometer.setMode(HMC5883L_MODE_CONTINUOUS); // works without.
    PMM_DEBUG_PRINTLN_MORE("PmmImu [M]: Magnetometer initialized successfully!");
    return 0;
}



int PmmImu::initBmp()  //BMP085 Setup
{
    mBarometer.initialize();
    if(!mBarometer.testConnection())
    {
        PMM_DEBUG_PRINTLN("PmmImu #3: BAROMETER INIT ERROR");
        return 1;
    }
    PMM_DEBUG_PRINTLN_MORE("PmmImu [M]: BMP initialized successfully!");
    return 0;
}





int PmmImu::init(PmmErrorsCentral *pmmErrorsCentral)
{
    mNextMillisBarometer = 0;
    mPmmErrorsCentral = pmmErrorsCentral;
    mMagnetometerDeclinationRad = 369.4 / 1000; // https://www.meccanismocomplesso.org/en/arduino-magnetic-magnetic-magnetometer-hmc5883l/

    Wire.begin(); // as seen in the sensors example!

    int foundError = 0;

    if (initBmp())
    {
        mPmmErrorsCentral->reportErrorByCode(ERROR_BAROMETER_INIT);
        foundError = 1;
    }

    if (initMpu())
    {
        mPmmErrorsCentral->reportErrorByCode(ERROR_ACCELEROMETER_INIT);
        mPmmErrorsCentral->reportErrorByCode(ERROR_GYROSCOPE_INIT);
        foundError = 1;
    }

    if (initMagnetometer())
    {
        mPmmErrorsCentral->reportErrorByCode(ERROR_MAGNETOMETER_INIT);
        foundError = 1;
    }

    updateScales();

    return foundError? 1: 0;
}





int PmmImu::updateMpu()
{
    mMpu.getMotion6(&mAccelerometerRaw[0], &mAccelerometerRaw[1], &mAccelerometerRaw[2], &mGyroscopeRaw[0], &mGyroscopeRaw[1], &mGyroscopeRaw[2]);

    mPmmImuStruct.accelerometerArray[0] = mAccelerometerRaw[0] / mAccelerometerScale;
    mPmmImuStruct.accelerometerArray[1] = mAccelerometerRaw[1] / mAccelerometerScale;
    mPmmImuStruct.accelerometerArray[2] = mAccelerometerRaw[2] / mAccelerometerScale;

    mPmmImuStruct.gyroscopeArray[0] = mGyroscopeRaw[0] / mGyroscopeScale;
    mPmmImuStruct.gyroscopeArray[1] = mGyroscopeRaw[1] / mGyroscopeScale;
    mPmmImuStruct.gyroscopeArray[2] = mGyroscopeRaw[2] / mGyroscopeScale;

    return 0;
}



int PmmImu::updateMagnetometer() // READ https://www.meccanismocomplesso.org/en/arduino-magnetic-magnetic-magnetometer-hmc5883l/
{
    mMagnetometer.getHeading(&mMagnetometerRaw[0], &mMagnetometerRaw[1], &mMagnetometerRaw[2]);

    mPmmImuStruct.magnetometerArray[0] = mMagnetometerRaw[0] * mMagnetometerScale;
    mPmmImuStruct.magnetometerArray[1] = mMagnetometerRaw[1] * mMagnetometerScale;
    mPmmImuStruct.magnetometerArray[2] = mMagnetometerRaw[2] * mMagnetometerScale;

    mPmmImuStruct.headingRadian = atan2(mMagnetometerRaw[1], mMagnetometerRaw[0]); // argument is (mY, mX).

    mPmmImuStruct.headingRadian += mMagnetometerDeclinationRad; // Adds the declination

    if(mPmmImuStruct.headingRadian < 0)
        mPmmImuStruct.headingRadian += 2 * M_PI;
    mPmmImuStruct.headingDegree = mPmmImuStruct.headingRadian * 180/M_PI;
    return 0;
}



int PmmImu::updateBmp()
{
    mBarometer.setControl(BMP085_MODE_TEMPERATURE);

    // read calibrated temperature value in degrees Celsius
    mPmmImuStruct.temperature = mBarometer.getTemperatureC();

    // request pressure (3x oversampling mode, high detail, 23.5ms delay)
    mBarometer.setControl(BMP085_MODE_PRESSURE_3);

    // read calibrated pressure value in Pascals (Pa)
    mPmmImuStruct.pressure = mBarometer.getPressure();

    // calculate absolute altitude in meters based on known pressure
    // (may pass a second "sea level pressure" parameter here,
    // otherwise uses the standard value of 101325 Pa)
    mPmmImuStruct.altitudePressure = mBarometer.getAltitude(mPmmImuStruct.pressure);
    return 0;
}

int PmmImu::update()
{

    if (mPmmErrorsCentral->getAccelerometerIsWorking())//accelIsWorking)
    {
        updateMpu();
        PMM_IMU_DEBUG_PRINT_MORE("PmmImu [M]: Mpu updated!");
    }
    /*
    if (mPmmErrorsCentral->getGyroscopeIsWorking())//gyroIsWorking)
        getGyroscope();
    */
    if (mPmmErrorsCentral->getMagnetometerIsWorking())//magnIsWorking)
    {
        updateMagnetometer();
        PMM_IMU_DEBUG_PRINT_MORE("PmmImu [M]: Magnetometer updated!");
    }

    if (millis() >= mNextMillisBarometer)
    {
        mNextMillisBarometer = millis() + DELAY_MS_BAROMETER;
        if (mPmmErrorsCentral->getBarometerIsWorking())//baroIsWorking)
        {
            updateBmp();
            PMM_IMU_DEBUG_PRINT_MORE("PmmImu [M]: Barometer updated!");
        }
    }
    return 0;
}





void PmmImu::getAccelerometer(float destinationArray[3])
{
    memcpy(destinationArray, mPmmImuStruct.accelerometerArray, 3);
}
void PmmImu::getGyroscope(float destinationArray[3])
{
    memcpy(destinationArray, mPmmImuStruct.gyroscopeArray, 3);
}
void PmmImu::getMagnetometer(float destinationArray[3])
{
    memcpy(destinationArray, mPmmImuStruct.magnetometerArray, 3);
}
float PmmImu::getBarometer()
{
    return mPmmImuStruct.pressure;
}
float PmmImu::getAltitudeBarometer()
{
    return mPmmImuStruct.altitudePressure;
}
float PmmImu::getTemperature()
{
    return mPmmImuStruct.temperature;
}
pmmImuStructType PmmImu::getImuStruct()
{
    return mPmmImuStruct;
}



float* PmmImu::getAccelerometerPtr()
{
    return mPmmImuStruct.accelerometerArray;
}
float* PmmImu::getGyroscopePtr()
{
    return mPmmImuStruct.gyroscopeArray;
}
float* PmmImu::getMagnetometerPtr()
{
    return mPmmImuStruct.magnetometerArray;
}
float* PmmImu::getBarometerPtr()
{
    return &mPmmImuStruct.pressure;
}
float* PmmImu::getAltitudeBarometerPtr()
{
    return &mPmmImuStruct.altitudePressure;
}
float* PmmImu::getTemperaturePtr()
{
    return &mPmmImuStruct.temperature;
}
pmmImuStructType* PmmImu::getImuStructPtr()
{
    return &mPmmImuStruct;
}
