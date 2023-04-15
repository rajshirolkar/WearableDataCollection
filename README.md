# Wearable Data Collection Device Project for ESP32

This repository contains individual sensor codes and a combined code for interfacing multiple sensors with an ESP32 microcontroller. The sensors used in this project are BME280, TSL2561, MLX90614, and MAX30102.

## Table of Contents

- [Wearable Data Collection Device Project for ESP32](#wearable-data-collection-device-project-for-esp32)
  - [Table of Contents](#table-of-contents)
  - [Sensor Overview](#sensor-overview)
  - [Files in the Repository](#files-in-the-repository)
  - [Connection Diagrams](#connection-diagrams)
    - [BME280 \& TSL2561](#bme280--tsl2561)
    - [MLX90614 \& MAX30102](#mlx90614--max30102)
  - [Usage](#usage)
  - [License](#license)

## Sensor Overview

1. **BME280**: A temperature, humidity, and pressure sensor.
2. **TSL2561**: An ambient light sensor that measures illuminance (lux).
3. **MLX90614**: A non-contact infrared temperature sensor.
4. **MAX30102**: A pulse oximeter and heart rate sensor.

## Files in the Repository

- `BME280.ino`: Code for interfacing the BME280 sensor with ESP32.
- `TSL2561.ino`: Code for interfacing the TSL2561 sensor with ESP32.
- `MLX90614.ino`: Code for interfacing the MLX90614 sensor with ESP32.
- `MAX30102.ino`: Code for interfacing the MAX30102 sensor with ESP32.
- `CombinedSensors.ino`: Combined code for interfacing all four sensors with ESP32.

## Connection Diagrams

### BME280 & TSL2561

- VCC: 3.3V
- GND: GND
- SDA: GPIO21 (SDA)
- SCL: GPIO22 (SCL)

### MLX90614 & MAX30102

- VCC: 3.3V
- GND: GND
- SDA: GPIO21 (SDA)
- SCL: GPIO22 (SCL)

## Usage

1. Clone or download this repository.
2. Open the desired `.ino` file with the Arduino IDE.
3. Connect the corresponding sensor(s) to the ESP32 according to the connection diagrams.
4. Upload the code to your ESP32 board.
5. Open the Serial Monitor to view the sensor data.

## License

This project is released under the MIT License.