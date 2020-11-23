#ifndef _MPU6050_h
#define _MPU6050_h


#include "arduino.h"
#include <wire.h>

#define MPU_POWER_REG                   0x6B
#define MPU_POWER_CYCLE_STAYAWAKE       0b00000000
#define MPU_READ_TIMEOUT                2000
#define MPU_SAMP_FREQ                   250

#define MPU_GYRO_CFG_REG                0x1B
#define MPU_GYRO_READ_REG               0x43
#define MPU_GYRO_READ_REG_SIZE          6
#define MPU_GYRO_CFG_250DEG             0b00000000
#define MPU_GYRO_READINGSCALE_250DEG    131.0
#define MPU_GYRO_CFG_500DEG             0b00001000
#define MPU_GYRO_READINGSCALE_500DEG    65.5
#define MPU_GYRO_CFG_1000DEG            0b00010000
#define MPU_GYRO_READINGSCALE_1000DEG   32.8
#define MPU_GYRO_CFG_2000DEG            0b00011000
#define MPU_GYRO_READINGSCALE_2000DEG   16.4
#define MPU_CALIBRATE_READING_NUM       2000

#define MPU_TEMP_READ_REG               0x41
#define MPU_TEMP_READ_REG_SIZE          2

#define MPU_ACCEL_CFG_REG               0x1C
#define MPU_ACCEL_READ_REG              0x3B
#define MPU_ACCEL_READ_REG_SIZE         6
#define MPU_ACCEL_CFG_2G                0b00000000
#define MPU_ACCEL_READINGSCALE_2G       16384.0
#define MPU_ACCEL_CFG_4G                0b00001000
#define MPU_ACCEL_READINGSCALE_4G       8192.0
#define MPU_ACCEL_CFG_8G                0b00010000
#define MPU_ACCEL_READINGSCALE_8G       4096.0
#define MPU_ACCEL_CFG_16G               0b00011000
#define MPU_ACCEL_READINGSCALE_16G      2048.0

class MPUReadings
{
public:
	double gForceX, gForceY, gForceZ;
	double rateRotX, rateRotY, rateRotZ;
	double calibX, calibY, calibZ;
	double pitch, roll, yaw;
};

class MPU6050
{
private:
	bool Debug = true;
	int NumberOfSensors = 1;
	int MPUAddress[2] = { 0x0, 0x0 };

	int GyroCfg = MPU_GYRO_CFG_250DEG;
	int AccelCfg = MPU_ACCEL_CFG_2G;

	int PowerCfg = MPU_POWER_CYCLE_STAYAWAKE;

	float GyroScale = MPU_GYRO_READINGSCALE_250DEG;
	float AccelScale = MPU_ACCEL_READINGSCALE_2G;

	bool verifyDeviceAtAddress(int);
	void setupMPU(int);
	bool MPUReadGyro(int);
	bool MPUReadAccel(int);
	void calibrateGyro(int);
	bool calcAngle(int);
	void applyCalibration(int);

public:
	MPUReadings Readings[2];
	
	enum AngleModes { TILT, ROTATION, DMP }AngleMode = TILT;
	
	//setup functions
	void SetGyroScale(int);
	void SetAccelScale(int);
	void SetPowerMode(int);
	void SetAngleMode(int);
	bool begin(int);
	bool begin(int, int);

	void calibrate();

	bool update();


}; extern MPU6050;

#endif

