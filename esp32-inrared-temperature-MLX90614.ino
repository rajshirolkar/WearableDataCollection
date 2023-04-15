#include <Wire.h>
#include <Adafruit_MLX90614.h>

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  mlx.begin();
}

void loop()
{
  double ambientTemp = mlx.readAmbientTempC();
  double objectTemp = mlx.readObjectTempC();

  Serial.print("Ambient temperature: ");
  Serial.print(ambientTemp);
  Serial.print(" °C");
  Serial.print("\tObject temperature: ");
  Serial.print(objectTemp);
  Serial.println(" °C");

  delay(1000);
}
