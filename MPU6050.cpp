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
		//Device not found at address
		if (Debug)Serial.println("MPU not found");
		return false;
	}

	if (result == 4)
	{
		//Unknown device at address
		if (Debug)Serial.println("Unknown device found");
		return false;
	}

	if (Debug)Serial.println("Mpu Found");
	return true;
}

void MPU6050::SetGyroScale(int gyroScale)
{
	switch (gyroScale)
	{
	case 250:
		GyroCfg = MPU_GYRO_CFG_250DEG;
		GyroScale = MPU_GYRO_READINGSCALE_250DEG;
		break;
	case 500:
		GyroCfg = MPU_GYRO_CFG_500DEG;
		GyroScale = MPU_GYRO_READINGSCALE_500DEG;
		break;
	case 1000:
		GyroCfg = MPU_GYRO_CFG_1000DEG;
		GyroScale = MPU_GYRO_READINGSCALE_1000DEG;
		break;
	case 2000:
		GyroCfg = MPU_GYRO_CFG_2000DEG;
		GyroScale = MPU_GYRO_READINGSCALE_2000DEG;
		break;
	default:
		//Scale value out of range, setting default";
		GyroCfg = MPU_GYRO_CFG_250DEG;
		GyroScale = MPU_GYRO_READINGSCALE_250DEG;
	}

	for (int i = 0; i < NumberOfSensors; i++)
	{
		Wire.beginTransmission(MPUAddress[i]);
		Wire.write(MPU_GYRO_CFG_REG);
		Wire.write(GyroCfg);
		Wire.endTransmission();
	}
}

void MPU6050::SetAccelScale(int accelScale)
{
	switch (accelScale)
	{
	case 2:
		AccelCfg = MPU_ACCEL_CFG_2G;
		AccelScale = MPU_ACCEL_READINGSCALE_2G;
		break;
	case 4:
		AccelCfg = MPU_ACCEL_CFG_4G;
		AccelScale = MPU_ACCEL_READINGSCALE_4G;
		break;
	case 8:
		AccelCfg = MPU_ACCEL_CFG_8G		;
		AccelScale = MPU_ACCEL_READINGSCALE_8G;
		break;
	case 16:
		AccelCfg = MPU_ACCEL_CFG_16G;
		AccelScale = MPU_ACCEL_READINGSCALE_16G;
		break;
	default:
		//Scale value out of range, setting default";
		AccelCfg = MPU_ACCEL_CFG_2G;
		AccelScale = MPU_ACCEL_READINGSCALE_2G;
	}

	for (int i = 0; i < NumberOfSensors; i++)
	{
		Wire.beginTransmission(MPUAddress[i]);
		Wire.write(MPU_ACCEL_CFG_REG);
		Wire.write(AccelCfg);
		Wire.endTransmission();
	}
}

void MPU6050::SetPowerMode(int powerMode)
{
	//TODO: Add power modes, right now just always on
	for (int i = 0; i < NumberOfSensors; i++)
	{
		Wire.beginTransmission(MPUAddress[i]);
		Wire.write(MPU_POWER_REG);
		Wire.write(PowerCfg);
		Wire.endTransmission();
	}
}

void MPU6050::SetAngleMode(int mode)
{
	switch (mode)
	{
	case TILT:
		AngleMode = TILT;
		break;
	case ROTATION:
		AngleMode = ROTATION;
		break;
	case DMP:
		AngleMode = DMP;
		break;
	default:
		//Invalid Mode, setting to default (TILT)";
		AngleMode = TILT;
		break;
	}
}

bool MPU6050::update()
{
	for (int i = 0; i < NumberOfSensors; i++)
	{
		if (Debug) Serial.println("Reading Gyro");
		MPUReadGyro(i);
		if (Debug) Serial.println("Reading Accel");
		MPUReadAccel(i);
		if (Debug) Serial.println("Calc Angle");
		calcAngle(i);
		if (Debug) Serial.println("Done");
	}
}

bool MPU6050::begin(int address)
{
	return begin(address, -1);
}

bool MPU6050::begin(int address, int address2)
{
	Serial.println("Start I2C");
	Wire.begin();

	Serial.println("Looking for MPU");
	if (!verifyDeviceAtAddress(address)) return false;
	NumberOfSensors = 1;
	MPUAddress[0] = address;
	Serial.println("Setup MPU");
	setupMPU(0);

	//second sensor?
	if (address2 != -1)
	{
		if (!verifyDeviceAtAddress(address))
		{
			//Unable to detect the second device.";
			return false;
		}
		MPUAddress[1] = address2;
		NumberOfSensors = 2;
		setupMPU(1);
	}
	return true;
}

void MPU6050::calibrate()
{
	for (int i = 0; i < NumberOfSensors; i++)
	{
		calibrateGyro(i);
	}
}

void MPU6050::setupMPU(int sensor)
{
	Wire.beginTransmission(MPUAddress[sensor]);
	Wire.write(MPU_POWER_REG);
	Wire.write(PowerCfg);
	Wire.endTransmission();
	Wire.beginTransmission(MPUAddress[sensor]);
	Wire.write(MPU_GYRO_CFG_REG);
	Wire.write(GyroCfg);
	Wire.endTransmission();
	Wire.beginTransmission(MPUAddress[sensor]);
	Wire.write(MPU_ACCEL_CFG_REG);
	Wire.write(AccelCfg);
	Wire.endTransmission();
	calibrateGyro(sensor);
}

bool MPU6050::MPUReadGyro(int sensor)
{
	if (sensor < 0 || sensor > 1) return false;

	Wire.beginTransmission(MPUAddress[sensor]);
	Wire.write(MPU_GYRO_READ_REG);
	Wire.endTransmission();
	Wire.requestFrom(MPUAddress[sensor], MPU_GYRO_READ_REG_SIZE);

	long timeout = millis() + MPU_READ_TIMEOUT;

	while (Wire.available() < MPU_GYRO_READ_REG_SIZE && timeout < millis());

	Readings[sensor].rateRotX = (long)(Wire.read() << 8 | Wire.read()) / GyroScale;
	Readings[sensor].rateRotY = (long)(Wire.read() << 8 | Wire.read()) / GyroScale;
	Readings[sensor].rateRotZ = (long)(Wire.read() << 8 | Wire.read()) / GyroScale;

	applyCalibration(sensor);

	return true;
}

bool MPU6050::MPUReadAccel(int sensor)
{
	if (sensor < 0 || sensor > 1) return false;

	Wire.beginTransmission(MPUAddress[sensor]);
	Wire.write(MPU_ACCEL_READ_REG);
	Wire.endTransmission();
	Wire.requestFrom(MPUAddress[sensor], MPU_ACCEL_READ_REG_SIZE);
	
	long timeout = millis() + MPU_READ_TIMEOUT;
	while (Wire.available() < MPU_ACCEL_READ_REG_SIZE && timeout < millis());
	
	if (timeout <= millis())
	{
		//Unable to get Accel Data";
		return false;
	}

	Readings[sensor].gForceX = (long)(Wire.read() << 8 | Wire.read()) / AccelScale;
	Readings[sensor].gForceY = (long)(Wire.read() << 8 | Wire.read()) / AccelScale;
	Readings[sensor].gForceZ = (long)(Wire.read() << 8 | Wire.read()) / AccelScale;
	
	return true;
}

void MPU6050::calibrateGyro(int sensor)
{
	long loopTimer = 0;

	float newCalibX = 0;
	float newCalibY = 0;
	float newCalibZ = 0;


	for (int i = 0; i < MPU_CALIBRATE_READING_NUM; i++)
	{
		if (MPUReadGyro(sensor))
		{
			newCalibX += Readings[sensor].rateRotX;
			newCalibY += Readings[sensor].rateRotY;
			newCalibZ += Readings[sensor].rateRotZ;

			//wait for the next sample cycle
			while (micros() - loopTimer < 4000);
			loopTimer = micros();
		}
		else
		{
			i--;
		}
	}
	Readings[sensor].calibX = newCalibX / MPU_CALIBRATE_READING_NUM;
	Readings[sensor].calibY = newCalibY / MPU_CALIBRATE_READING_NUM;
	Readings[sensor].calibZ = newCalibZ / MPU_CALIBRATE_READING_NUM;
}

bool MPU6050::calcAngle(int sensor)
{
	switch (AngleMode)
	{
	case TILT:
		double x_Buff = Readings[sensor].gForceX;
		double y_Buff = Readings[sensor].gForceY;
		double z_Buff = Readings[sensor].gForceZ;

		Readings[sensor].roll = atan2(y_Buff, z_Buff) * 57.3;
		Readings[sensor].pitch = atan2((-x_Buff), sqrt(y_Buff * y_Buff + z_Buff * z_Buff)) * 57.3;
		Readings[sensor].yaw = NULL;

		break;
	
	case ROTATION:
		break;

	case DMP:
		break;

	default:
		//Unknown calculation mode";
		return false;
	}
	return true;
}

void MPU6050::applyCalibration(int sensor)
{
	Readings[sensor].rateRotX = Readings[sensor].rateRotX - Readings[sensor].calibX;
	Readings[sensor].rateRotY = Readings[sensor].rateRotY - Readings[sensor].calibY;
	Readings[sensor].rateRotZ = Readings[sensor].rateRotZ - Readings[sensor].calibZ;
}
