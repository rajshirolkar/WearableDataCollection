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
MAX30105 particleSensor;

const int GSRPin = 34;
const int minValue = 0;
const int maxValue = 4095;

const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
float beatsPerMinute;
int beatAvg;

unsigned long currentMillis;
unsigned long heartRateStartMillis;
unsigned long heartRateDuration = 30000; // 30 seconds
bool heartRateActive = true;

unsigned long otherSensorsStartMillis;
unsigned long otherSensorsDuration = 1000; // Read other sensors for 1 second
bool otherSensorsActive = false;

// TwoWire Wire1(1);

void setup()
{
    Serial.begin(115200);
    Wire.begin();
    // Wire1.begin(21, 22);

    if (!bme.begin(0x76))
    {
        Serial.println("No BME280 detected. Check your connections.");
        while (1)
            ;
    }
    mlx.begin();

    if (!tsl.begin())
    {
        Serial.println("No TSL2561 detected. Check your connections.");
        while (1)
            ;
    }

    tsl.enableAutoRange(true);
    tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);

    pinMode(GSRPin, INPUT);

    if (!particleSensor.begin(Wire, I2C_SPEED_STANDARD))
    {
        Serial.println("MAX30102 was not found. Please check wiring/power.");
        while (1)
            ;
    }

    particleSensor.setup();
    particleSensor.setSampleRate(200); // Set sample rate 200 samples per second
    particleSensor.setADCRange(4096);  // Set ADC range to 4096
    particleSensor.setPulseAmplitudeRed(0x0A);
    particleSensor.setPulseAmplitudeGreen(0);

    // particleSensor.softReset();
    // delay(100);

    heartRateStartMillis = millis();
}

void loop()
{
    currentMillis = millis();

    if (heartRateActive)
    {
        readMAX30102();

        if (currentMillis - heartRateStartMillis >= heartRateDuration)
        {
            heartRateActive = false;
            otherSensorsActive = true;
            otherSensorsStartMillis = currentMillis;
        }
    }
    else if (otherSensorsActive)
    {
        readBME280();
        readTSL2561();
        readGSR();
        readMLX90614();

        if (currentMillis - otherSensorsStartMillis >= otherSensorsDuration)
        {
            otherSensorsActive = false;
            printAllSensorReadings();

            // Reset and start the next cycle
            heartRateStartMillis = currentMillis;
            heartRateActive = true;
        }
    }
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

void readGSR()
{
    int GSRValue = analogRead(GSRPin);
    float percentage = ((float)(GSRValue - minValue) / (float)(maxValue - minValue)) * 100;

    Serial.print("GSR Value: ");
    Serial.print(GSRValue);
    Serial.print(", Percentage: ");
    Serial.println(percentage);
}

// void readMAX30205() {
//   Wire.beginTransmission(MAX30205_ADDR);
//   Wire.write(MAX30205_TEMP);
//   Wire.endTransmission();
//   Wire.requestFrom(MAX30205_ADDR, 2);

//   if (Wire.available() == 2) {
//     uint8_t msb = Wire.read();
//     uint8_t lsb = Wire.read();
//     float temperature = ((msb << 8) | lsb) * 0.00390625;

//     Serial.print("Temperature: ");
//     Serial.print(temperature);
//     Serial.println(" °C");
//   }
// }
void readMAX30102()
{
    long irValue = particleSensor.getIR();

    if (checkForBeat(irValue) == true)
    {
        long delta = millis() - lastBeat;
        lastBeat = millis();

        beatsPerMinute = 60 / (delta / 1000.0);

        if (beatsPerMinute < 255 && beatsPerMinute > 20)
        {
            rates[rateSpot++] = (byte)beatsPerMinute;
            rateSpot %= RATE_SIZE;

            beatAvg = 0;
            for (byte x = 0; x < RATE_SIZE; x++)
                beatAvg += rates[x];
            beatAvg /= RATE_SIZE;
        }
        // delay(50);
    }

    Serial.print("IR=");
    Serial.print(irValue);
    Serial.print(", BPM=");
    Serial.print(beatsPerMinute);
    Serial.print(", Avg BPM=");
    Serial.print(beatAvg);
}

void printAllSensorReadings()
{
    // Print BME280 readings
    Serial.print("BME280 Temperature Humidity Sensor readings: ");
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.print(" *C, Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.print(" %, Pressure = ");
    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    // Print TSL2561 readings
    sensors_event_t event;
    tsl.getEvent(&event);
    Serial.print("TSL2561 Luminosity Sensor readings: ");
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

    // Print GSR readings
    int GSRValue = analogRead(GSRPin);
    float percentage = ((float)(GSRValue - minValue) / (float)(maxValue - minValue)) * 100;
    Serial.print("GSR readings: ");
    Serial.print("GSR Value: ");
    Serial.print(GSRValue);
    Serial.print(", Percentage: ");
    Serial.println(percentage);

    // Print MLX90614 readings
    Serial.print("MLX90614 Body temperature sensor readings: ");
    Serial.print("Ambient temperature = ");
    Serial.print(mlx.readAmbientTempC());
    Serial.print(" *C, Object temperature = ");
    Serial.print(mlx.readObjectTempC());
    Serial.println(" *C");

    // Print MAX30102 readings
    Serial.print("MAX30102 Heart Rate sensor readings: ");
    Serial.print("IR=");
    Serial.print(particleSensor.getIR());
    Serial.print(", BPM=");
    Serial.print(beatsPerMinute);
    Serial.print(", Avg BPM=");
    Serial.println(beatAvg);
}