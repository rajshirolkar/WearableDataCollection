#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLECharacteristic *pCharacteristic;

void setup()
{
    Serial.begin(115200);
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

void printAllSensorReadings()
{

    int dummy = 20;

    String sensorData = "START";
    pCharacteristic->setValue(sensorData.c_str());
    pCharacteristic->notify();
    Serial.println(sensorData);
    delay(20);

    // Print BME280 readings
    sensorData = "AmbTemp=" + String(dummy++) + " *C";
    pCharacteristic->setValue(sensorData.c_str());
    pCharacteristic->notify();
    Serial.println(sensorData);
    delay(20);

    sensorData = "Humidity=" + String(dummy++) + " %";
    pCharacteristic->setValue(sensorData.c_str());
    pCharacteristic->notify();
    Serial.println(sensorData);
    delay(20);

    sensorData = "Pressure=" + String(dummy++ / 100.0F) + " hPa";
    pCharacteristic->setValue(sensorData.c_str());
    pCharacteristic->notify();
    Serial.println(sensorData);
    delay(20);

    // Print TSL2561 readings
    sensorData = "Light=" + String(dummy++) + " lux";
    pCharacteristic->setValue(sensorData.c_str());
    pCharacteristic->notify();
    Serial.println(sensorData);
    delay(20);

    sensorData = "GSR=" + String(dummy++);
    pCharacteristic->setValue(sensorData.c_str());
    pCharacteristic->notify();
    Serial.println(sensorData);
    delay(20);

    sensorData = "BodyTemp=" + String(dummy++) + " *C";
    pCharacteristic->setValue(sensorData.c_str());
    pCharacteristic->notify();
    Serial.println(sensorData);
    delay(20);

    // Print MAX30102 readings
    sensorData = "Infrared=" + String(dummy++);
    pCharacteristic->setValue(sensorData.c_str());
    pCharacteristic->notify();
    Serial.println(sensorData);
    delay(20);

    sensorData = "BPM=" + String(dummy++);
    pCharacteristic->setValue(sensorData.c_str());
    pCharacteristic->notify();
    Serial.println(sensorData);
    delay(20);

    sensorData = "AvgBPM=" + String(dummy++);
    pCharacteristic->setValue(sensorData.c_str());
    pCharacteristic->notify();
    Serial.println(sensorData);
    delay(20);

    // Add timestamp
    time_t currentTime = time(NULL);
    sensorData = String(currentTime);
    pCharacteristic->setValue(sensorData.c_str());
    pCharacteristic->notify();
    Serial.println(sensorData);
    delay(20);

    sensorData = "STOP";
    pCharacteristic->setValue(sensorData.c_str());
    pCharacteristic->notify();
    Serial.println(sensorData);
    delay(20);

    Serial.println("Data sent");
}

void loop()
{
    printAllSensorReadings();
    delay(5000);
}
