#include <Arduino_LSM9DS1.h> // Si usas la versión Rev2, cambia a <Arduino_BMI270_BMM150.h>

void setup() {
  Serial.begin(9600);
  delay(1000); // Pausa de estabilización

  if (!IMU.begin()) {
    while (1); // Si falla la IMU, se detiene
  }
}

void loop() {
  float x, y, z;

  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z); // Lectura de aceleración en 'g'

    // Formato CSV estándar: Ax;Ay;Az
    Serial.print(x, 4);
    Serial.print(";");
    Serial.print(y, 4);
    Serial.print(";");
    Serial.println(z, 4);
  }

  delay(50); // Muestreo exacto a 50 ms (20 Hz)
}