#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_TSL2561_U.h>
#include <Adafruit_MLX90614.h>
#include "MAX30105.h"
#include "heartRate.h"

Adafruit_BME280 bme;
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
// MAX30105 particleSensor;

// const byte RATE_SIZE = 4;
// byte rates[RATE_SIZE];
// byte rateSpot = 0;
// long lastBeat = 0;
// float beatsPerMinute;
// int beatAvg;

void setup()
{
    Serial.begin(115200);

    if (!bme.begin(0x76))
    {
        Serial.println("No BME280 detected. Check your connections.");
        while (1)
            ;
    }

    if (!tsl.begin())
    {
        Serial.println("No TSL2561 detected. Check your connections.");
        while (1)
            ;
    }

    tsl.enableAutoRange(true);
    tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);

    mlx.begin();

    // if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    //   Serial.println("MAX30102 was not found. Please check wiring/power.");
    //   while (1);
    // }

    // particleSensor.setup();
    // particleSensor.setPulseAmplitudeRed(0x0A);
    // particleSensor.setPulseAmplitudeGreen(0);
}

void readBME280()
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
}

void readTSL2561()
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
        Serial.println("Sensor overload");
    }
}

void readMLX90614()
{
    Serial.print("Ambient temperature = ");
    Serial.print(mlx.readAmbientTempC());
    Serial.println(" *C");

    Serial.print("Object temperature = ");
    Serial.print(mlx.readObjectTempC());
    Serial.println(" *C");
}

// void readMAX30102() {
//   long irValue = particleSensor.getIR();

//   if (checkForBeat(irValue) == true) {
//     long delta = millis() - lastBeat;
//     lastBeat = millis();

//     beatsPerMinute = 60 / (delta / 1000.0);

//     if (beatsPerMinute < 255 && beatsPerMinute > 20) {
//       rates[rateSpot++] = (byte)beatsPerMinute;
//       rateSpot %= RATE_SIZE;

//       beatAvg = 0;
//       for (byte x = 0; x < RATE_SIZE; x++)
//         beatAvg += rates[x];
//       beatAvg /= RATE_SIZE;
//     }
//   }

//   Serial.print("IR=");
//   Serial.print(irValue);
//   Serial.print(", BPM=");
//   Serial.print(beatsPerMinute);
//   Serial.print(", Avg BPM=");
//   Serial.print(beatAvg);
// }

void loop()
{
    readBME280();
    readTSL2561();
    readMLX90614();
    // readMAX30102();

    Serial.println();
    delay(1000);
}

// #include <Wire.h>
// #include <Adafruit_Sensor.h>
// #include <Adafruit_BME280.h>
// #include <Adafruit_TSL2561_U.h>
// #include <Adafruit_MLX90614.h>

// Adafruit_BME280 bme;
// Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
// Adafruit_MLX90614 mlx = Adafruit_MLX90614();

// void setup() {
//   Serial.begin(115200);

//   if (!bme.begin(0x76)) {
//     Serial.println("No BME280 detected. Check your connections.");
//     while (1);
//   }

//   if (!tsl.begin()) {
//     Serial.println("No TSL2561 detected. Check your connections.");
//     while (1);
//   }

//   tsl.enableAutoRange(true);
//   tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);

//   mlx.begin();
// }

// void readBME280() {
//   Serial.print("Temperature = ");
//   Serial.print(bme.readTemperature());
//   Serial.println(" *C");

//   Serial.print("Humidity = ");
//   Serial.print(bme.readHumidity());
//   Serial.println(" %");

//   Serial.print("Pressure = ");
//   Serial.print(bme.readPressure() / 100.0F);
//   Serial.println(" hPa");
// }

// void readTSL2561() {
//   sensors_event_t event;
//   tsl.getEvent(&event);
//   if (event.light)
//   {
//     Serial.print("Light: ");
//     Serial.print(event.light);
//     Serial.println(" lux");
//   }
//   else
//   {
//     Serial.println("Sensor overload");
//   }
// }

// void readMLX90614() {
//   Serial.print("Ambient temperature = ");
//   Serial.print(mlx.readAmbientTempC());
//   Serial.println(" *C");

//   Serial.print("Object temperature = ");
//   Serial.print(mlx.readObjectTempC());
//   Serial.println(" *C");
// }

// void loop() {
//   readBME280();
//   readTSL2561();
//   readMLX90614();
//   Serial.println();
//   delay(1000);
// }
