#define STATUS_PIN                      13

#define MPU1_I2C_ADDRESS                0b1101000
#define MPU2_I2C_ADDRESS                0b1101001

#define OUTPUT_THROTTLE                 250

#include "MPU6050.h"
#include <Wire.h>

MPU6050 Gyro;
bool Debug=true;

void setup() 
{
    Serial.begin(57600);
    if(Debug)identify();
    pinMode(STATUS_PIN,OUTPUT);
    digitalWrite(STATUS_PIN, LOW);

    if(Debug)Serial.println("Starting Config");
    Gyro.SetGyroScale(250);
    if (Debug)Serial.println("Set Gyro Scale");
    Gyro.SetAngleMode(Gyro.TILT);
    if (Debug)Serial.println("Set Angle Mode");
    Gyro.SetAccelScale(2);
    if (Debug)Serial.println("Set Accel Scale");
    Gyro.SetPowerMode(0);
    if (Debug)Serial.println("Set Power mode");
  
    if (Debug)Serial.print("Starting Gyro");
    if (Debug && Gyro.begin(MPU1_I2C_ADDRESS))
    {
        Serial.println("... started");
    }
    else if (Debug)
    {
        Serial.println("...failed");
    }

    if (Debug)Serial.println("setup complete.");
  
    Serial.println("ms,gfx,gfy,gfz,rrx,rry,rrz,yaw,pit,rol");
}

void loop() 
{
    if (Debug)Serial.print("Free Memory: ");
    if (Debug)Serial.println(freeMemory());
    if (Debug)Serial.print(millis());
    if (Debug)Serial.print(" - Begin Read...");
    Gyro.update();
    if (Debug)Serial.println("Read Done");

    toggleStatus();
    delay(1000);
}

void identify()
{
    Serial.print("MPU6050 pt2 V 0.1 ");
    Serial.print(__DATE__);
    Serial.print(" ");
    Serial.println( __TIME__);
}

void toggleStatus()
{
    digitalWrite(STATUS_PIN, !digitalRead(STATUS_PIN));
}

int freeMemory()
{
    extern char* __brkval;
    char top;
#ifdef __arm__
    return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
    return &top - __brkval;
#else  // __arm__
    return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}


