#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_TSL2561_U.h>
#include <Adafruit_MLX90614.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLECharacteristic *pCharacteristic;
Adafruit_BME280 bme;
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
MAX30105 particleSensor;

const int GSRPin = A0;
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
unsigned long heartRateDuration = 20000; // 30 seconds
bool heartRateActive = true;

unsigned long otherSensorsStartMillis;
unsigned long otherSensorsDuration = 1000; // Read other sensors for 1 second
bool otherSensorsActive = false;

void setup()
{
    Serial.begin(115200);
    Wire.begin();

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

    heartRateStartMillis = millis();

    // Create the BLE Device
    BLEDevice::init("ESP32-SensorHub");

    // Create the BLE Server
    BLEServer *pServer = BLEDevice::createServer();

    // Create the BLE Service
    BLEService *pService = pServer->createService(SERVICE_UUID);

    // Create a BLE Characteristic
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE |
            BLECharacteristic::PROPERTY_NOTIFY |
            BLECharacteristic::PROPERTY_INDICATE);

    // Start the service
    pService->start();

    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x06); // Functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
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
        delay(2000);
        readBME280();
        readTSL2561();
        readGSR();
        readMLX90614();

        if (currentMillis - otherSensorsStartMillis >= otherSensorsDuration)
        {
            otherSensorsActive = false;
            delay(2000);
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
    Serial.println(beatAvg);
}

void printAllSensorReadings()
{

    String sensorData = "START";
    pCharacteristic->setValue(sensorData.c_str());
    pCharacteristic->notify();
    Serial.println(sensorData);
    delay(20);

    // Print BME280 readings
    // sensorData = "AmbTemp=" + String(bme.readTemperature()) + " *C";
    // pCharacteristic->setValue(sensorData.c_str());
    // pCharacteristic->notify();
    // delay(20);

    // sensorData = "Humidity=" + String(bme.readHumidity()) + " %";
    // pCharacteristic->setValue(sensorData.c_str());
    // pCharacteristic->notify();
    // delay(20);

    // sensorData = "Pressure=" + String(bme.readPressure() / 100.0F) + " hPa";
    // pCharacteristic->setValue(sensorData.c_str());
    // pCharacteristic->notify();
    // delay(20);

    // Print TSL2561 readings
    sensors_event_t event;
    tsl.getEvent(&event);
    if (event.light)
    {
        sensorData = "Light=" + String(event.light) + " lux";
        pCharacteristic->setValue(sensorData.c_str());
        pCharacteristic->notify();
        delay(20);
    }

    // Print GSR readings
    int GSRValue = analogRead(GSRPin);
    float percentage = ((float)(GSRValue - minValue) / (float)(maxValue - minValue)) * 100;
    sensorData = "GSR=" + String(GSRValue);
    pCharacteristic->setValue(sensorData.c_str());
    pCharacteristic->notify();
    Serial.println(sensorData);
    delay(20);

    sensorData = "GSRPercent=" + String(percentage);
    pCharacteristic->setValue(sensorData.c_str());
    pCharacteristic->notify();
    Serial.println(sensorData);
    delay(20);

    // Print MLX90614 readings
    sensorData = "AmbTemp=" + String(mlx.readAmbientTempC()) + " *C";
    pCharacteristic->setValue(sensorData.c_str());
    pCharacteristic->notify();
    Serial.println(sensorData);
    delay(20);

    sensorData = "ObjTemp=" + String(mlx.readObjectTempC()) + " *C";
    pCharacteristic->setValue(sensorData.c_str());
    pCharacteristic->notify();
    Serial.println(sensorData);
    delay(20);

    // Print MAX30102 readings
    sensorData = "Infrared=" + String(particleSensor.getIR());
    pCharacteristic->setValue(sensorData.c_str());
    pCharacteristic->notify();
    Serial.println(sensorData);
    delay(20);

    sensorData = "BPM=" + String(beatsPerMinute);
    pCharacteristic->setValue(sensorData.c_str());
    pCharacteristic->notify();
    Serial.println(sensorData);
    delay(20);

    sensorData = "AvgBPM=" + String(beatAvg);
    pCharacteristic->setValue(sensorData.c_str());
    pCharacteristic->notify();
    Serial.println(sensorData);
    delay(20);

    sensorData = "STOP";
    pCharacteristic->setValue(sensorData.c_str());
    pCharacteristic->notify();
    Serial.println(sensorData);
    delay(20);
}