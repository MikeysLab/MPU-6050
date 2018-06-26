#define STATUS_PIN                      13

#define MPU_POWER_REG                   0x6B
#define MPU_POWER_CYCLE                 0b00000000
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

#define MPU1_I2C_ADDRESS                0b1101000
#define MPU2_I2C_ADDRESS                0b1101001

#include <Wire.h>

float gForceX, gForceY, gForceZ;
float rotX, rotY, rotZ;
float calibX, calibY, calibZ;
//float pitch, roll, yaw;
float mpuTemp;
long loopTimer = 0;

void setup() {
  Serial.begin(57600);
  pinMode(STATUS_PIN,OUTPUT);
  digitalWrite(STATUS_PIN, LOW);
  Wire.begin();
  SetupMPU();
  delay(3000);
  calibrateGyro();
  Serial.println("setup complete.");
}

void SetupMPU()
{
  Wire.beginTransmission(MPU1_I2C_ADDRESS);
  Wire.write(MPU_POWER_REG);
  Wire.write(MPU_POWER_CYCLE); 
  Wire.endTransmission();
  Wire.beginTransmission(MPU1_I2C_ADDRESS);
  Wire.write(MPU_GYRO_CFG_REG);
  Wire.write(MPU_GYRO_CFG_250DEG); 
  Wire.endTransmission();
  Wire.beginTransmission(MPU1_I2C_ADDRESS);
  Wire.write(MPU_ACCEL_CFG_REG);
  Wire.write(MPU_ACCEL_CFG_2G); 
  Wire.endTransmission();
}

void loop() {
  if(ReadMPU())
  {                          
    while(micros() - loopTimer < 4000);
    loopTimer = micros();
  }
}

bool ReadMPU()
{
  if(MPUReadAccel() && MPUReadGyro())
  {
//    calcRotation();
    printData();
    return true;
  }
  return false;
}

bool MPUReadAccel()
{
  Wire.beginTransmission(MPU1_I2C_ADDRESS);
  Wire.write(MPU_ACCEL_READ_REG);
  Wire.endTransmission();
  Wire.requestFrom(MPU1_I2C_ADDRESS, MPU_ACCEL_READ_REG_SIZE);
  long timeout = millis() + MPU_READ_TIMEOUT;
  while(Wire.available() < MPU_ACCEL_READ_REG_SIZE && timeout < millis());
  if (timeout <= millis()) return false;
  gForceX = (long)(Wire.read() << 8 | Wire.read()) / MPU_ACCEL_READINGSCALE_2G;
  gForceY = (long)(Wire.read() << 8 | Wire.read()) / MPU_ACCEL_READINGSCALE_2G;
  gForceZ = (long)(Wire.read() << 8 | Wire.read()) / MPU_ACCEL_READINGSCALE_2G;
  return true;
}

bool MPUReadTemp()
{
  Wire.beginTransmission(MPU1_I2C_ADDRESS);
  Wire.write(MPU_TEMP_READ_REG);
  Wire.endTransmission();
  Wire.requestFrom(MPU1_I2C_ADDRESS, MPU_TEMP_READ_REG_SIZE);
  long timeout = millis() + MPU_READ_TIMEOUT;
  while(Wire.available() < MPU_TEMP_READ_REG_SIZE && timeout < millis());
  if (timeout <= millis()) return false;
  mpuTemp = (long)(Wire.read() << 8 | Wire.read())/340 + 36.53;
  return true;
}

bool MPUReadGyro()
{
  Wire.beginTransmission(MPU1_I2C_ADDRESS);
  Wire.write(MPU_GYRO_READ_REG);
  Wire.endTransmission();
  Wire.requestFrom(MPU1_I2C_ADDRESS, MPU_GYRO_READ_REG_SIZE);
  long timeout = millis() + MPU_READ_TIMEOUT;
  while(Wire.available() < MPU_ACCEL_READ_REG_SIZE && timeout < millis());
  if (timeout <= millis()) return false;
  rotX = (long)(Wire.read() << 8 | Wire.read()) / MPU_GYRO_READINGSCALE_250DEG;
  rotY = (long)(Wire.read() << 8 | Wire.read()) / MPU_GYRO_READINGSCALE_250DEG;
  rotZ = (long)(Wire.read() << 8 | Wire.read()) / MPU_GYRO_READINGSCALE_250DEG;
  
  return true;
}

void calibrateGyro()
{
  loopTimer = 0;
  
  digitalWrite(STATUS_PIN, HIGH);
  Serial.println("Calibrating Gyro");
  
  calibX = 0;
  calibY = 0;
  calibZ = 0;
  
  
  for(int i=0; i<MPU_CALIBRATE_READING_NUM;i++)
  {
    if(MPUReadGyro())
    {
      calibX += rotX;
      calibY += rotY;
      calibZ += rotZ;

      //wait for the next sample cycle
      while(micros() - loopTimer < 4000);
      loopTimer = micros();
    }
    else
    {
      i--;
    }
  }
  calibX = calibX / MPU_CALIBRATE_READING_NUM;
  calibY = calibY / MPU_CALIBRATE_READING_NUM;
  calibZ = calibZ / MPU_CALIBRATE_READING_NUM;

  Serial.print("x: ");
  Serial.print(calibX);
  Serial.print("y: ");
  Serial.print(calibY);
  Serial.print("z: ");
  Serial.println(calibZ);

  Serial.println("Calibration Done.");
  
  digitalWrite(STATUS_PIN, LOW);
}

//void calcRotation()
//{
//  pitch += (rotX - calibX) * (1/(MPU_SAMP_FREQ/MPU_GYRO_READINGSCALE_250DEG));
//  roll += (rotY - calibY) * (1/(MPU_SAMP_FREQ/MPU_GYRO_READINGSCALE_250DEG));
//  yaw += (rotZ - calibZ) * (1/(MPU_SAMP_FREQ/MPU_GYRO_READINGSCALE_250DEG));
//}

void printData() 
{
  Serial.print("Temp (deg c) ");
  Serial.print(mpuTemp);
  Serial.print(" Gyro (deg/s)");
  Serial.print(" X=");
  Serial.print(rotX - calibX);
  Serial.print(" Y=");
  Serial.print(rotY - calibY);
  Serial.print(" Z=");
  Serial.print(rotZ - calibZ);
  Serial.print(" Accel (g)");
  Serial.print(" X=");
  Serial.print(gForceX);
  Serial.print(" Y=");
  Serial.print(gForceY);
  Serial.print(" Z=");
  Serial.println(gForceZ);
//  Serial.print(" RT:");
//  Serial.print(" Roll:");
//  Serial.print(roll);
//  Serial.print(" Pitch:");
//  Serial.print(pitch);
//  Serial.print(" Yaw:");
//  Serial.println(yaw);
  
}

