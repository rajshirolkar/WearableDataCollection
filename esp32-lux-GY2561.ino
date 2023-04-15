#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>

/* Create an instance of the sensor */
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

void setup()
{
    Serial.begin(115200);
    if (!tsl.begin())
    {
        Serial.println("No TSL2561 detected. Check your connections.");
        while (1)
            ;
    }
    tsl.enableAutoRange(true);                            // Enable auto-gain
    tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS); // Short integration time for more frequent readings
}

void loop()
{
    sensors_event_t event;
    tsl.getEvent(&event);
    if (event.light)
    {
        Serial.print("Light: ");
        Serial.print(event.light);
        Serial.println(" lux");
    }
    else
    {
        Serial.println("Sensor overload or malfunction");
    }
    delay(1000);
}