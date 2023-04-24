#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// Create an instance of the sensor
Adafruit_BME280 bme;

void setup()
{
    Serial.begin(115200);

    if (!bme.begin(0x76))
    {
        Serial.println("No BME280 detected. Check your connections.");
        while (1)
            ;
    }
}

void loop()
{
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");

    Serial.print("Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");

    Serial.print("Pressure = ");
    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.println();
    delay(1000);
}
