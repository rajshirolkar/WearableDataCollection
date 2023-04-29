#include <Wire.h>
// Include other required libraries for sensors

// Sensor initialization and setup code

unsigned long baselineStartTime = 0;
const unsigned long baselineDuration = 60000; // 60 seconds for baseline measurement
bool baselineEstablished = false;
float baselineGSR = 0.0;
float normalizedGSR = 0.0;

void setup()
{
    Serial.begin(115200);

    // Initialize and setup your sensors

    Serial.println("Please relax and keep still for the baseline measurement.");
    baselineStartTime = millis();
}

void loop()
{
    // Read GSR sensor value
    int rawGSR = analogRead(GSR_PIN);

    if (!baselineEstablished)
    {
        if (millis() - baselineStartTime < baselineDuration)
        {
            // Calculate the average GSR value during the baseline measurement
            baselineGSR += rawGSR;
            baselineGSR /= 2;
        }
        else
        {
            Serial.println("Baseline established. Starting to monitor GSR values.");
            baselineEstablished = true;
        }
    }
    else
    {
        // Calculate the relative change in GSR value compared to the baseline
        normalizedGSR = (float)rawGSR / baselineGSR;

        Serial.print("Raw GSR: ");
        Serial.print(rawGSR);
        Serial.print(", Normalized GSR: ");
        Serial.println(normalizedGSR);
    }

    // Add code to read other sensors and perform required operations

    delay(1000);
}
