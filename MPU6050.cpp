// 
// 
// 

#include "MPU6050.h"

bool MPU6050::verifyDeviceAtAddress(int address)
{
	//This function is used to verify the address provided to
	//the begin function. It is also a good step to verify
	//your wiring. If this returns flase, check the value of
	//last error, for a description of the failure.

	int result = -1;

	Wire.beginTransmission(address);
	result = Wire.endTransmission();

	if (!(result == 0 || result == 4))
	{
		lastError = "Device not found at address";
		return false;
	}

	if (result == 4)
	{
		lastError = "Unknown device at address";
		return false;
	}
	return true;
}

bool MPU6050::update()
{
	return false;
}

bool MPU6050::begin(int address, int sda=-1, int scl=-1)
{
	if (sda == -1 || scl == -1)
	{
		lastError = "SDA,SCL not provided, assuming defaults";
		Wire.begin();
	}
	else
	{
		Wire.begin(sda, scl);
	}

	if (!verifyDeviceAtAddress(address)) return false;

	setupMPU();

}

void MPU6050::setupMPU(int address)
{
	Wire.beginTransmission(address);
	Wire.write(MPU_POWER_REG);
	Wire.write(MPU_POWER_CYCLE);
	Wire.endTransmission();
	Wire.beginTransmission(address);
	Wire.write(MPU_GYRO_CFG_REG);
	Wire.write(MPU_GYRO_CFG_250DEG);
	Wire.endTransmission();
	Wire.beginTransmission(address);
	Wire.write(MPU_ACCEL_CFG_REG);
	Wire.write(MPU_ACCEL_CFG_2G);
	Wire.endTransmission();
}
