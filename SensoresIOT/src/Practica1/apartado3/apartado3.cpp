/* Includes ---------------------------------------------------------------- */
#include <P1_RPY_870071_inferencing.h>
#include <Arduino_LSM9DS1.h>

/* Constant defines -------------------------------------------------------- */
#define CONVERT_G_TO_MS2    9.80665f
#define MAX_ACCEPTED_RANGE  2.0f
#define UMBRAL_CONFIANZA    0.70f  // 70% de seguridad mínima
#define STRIDE_MS           250    // Actualiza cada 250 ms
#define UMBRAL_MOVIMIENTO   15.0f  // Grados/seg mínimos para considerar que NO está parada

/* Private variables ------------------------------------------------------- */
static bool debug_nn = false;
static float buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE] = { 0 };

float ax = 0, ay = 0, az = 0;
float gx = 0, gy = 0, gz = 0;
float mx = 0, my = 0, mz = 0;

/**
 * @brief Controla el LED RGB con PWM (lógica inversa: 255 = apagado, 0 = encendido)
 */
void ponerColorLED(int r, int g, int b) {
    analogWrite(LEDR, 255 - r);
    analogWrite(LEDG, 255 - g);
    analogWrite(LEDB, 255 - b);
}

void actualizarLED(String etiqueta) {
    if (etiqueta == "R_mas") {
        ponerColorLED(255, 0, 0);     // Rojo
    } else if (etiqueta == "R_menos") {
        ponerColorLED(255, 40, 120);  // Rosa
    } else if (etiqueta == "P_mas") {
        ponerColorLED(0, 255, 0);     // Verde
    } else if (etiqueta == "P_menos") {
        ponerColorLED(255, 180, 0);   // Amarillo
    } else if (etiqueta == "Y_mas") {
        ponerColorLED(0, 0, 255);     // Azul
    } else if (etiqueta == "Y_menos") {
        ponerColorLED(130, 0, 255);   // Morado
    } else {
        ponerColorLED(0, 0, 0);       // Apagado
    }
}

float ei_get_sign(float number) {
    return (number >= 0.0) ? 1.0 : -1.0;
}

void leerMuestraIMU(float *destino) {
    if (IMU.accelerationAvailable()) {
        IMU.readAcceleration(ax, ay, az);
        if (fabs(ax) > MAX_ACCEPTED_RANGE) ax = ei_get_sign(ax) * MAX_ACCEPTED_RANGE;
        if (fabs(ay) > MAX_ACCEPTED_RANGE) ay = ei_get_sign(ay) * MAX_ACCEPTED_RANGE;
        if (fabs(az) > MAX_ACCEPTED_RANGE) az = ei_get_sign(az) * MAX_ACCEPTED_RANGE;
    }
    if (IMU.gyroscopeAvailable()) {
        IMU.readGyroscope(gx, gy, gz);
    }
    if (IMU.magneticFieldAvailable()) {
        IMU.readMagneticField(mx, my, mz);
    }

    destino[0] = ax * CONVERT_G_TO_MS2;
    destino[1] = ay * CONVERT_G_TO_MS2;
    destino[2] = az * CONVERT_G_TO_MS2;
    destino[3] = gx;
    destino[4] = gy;
    destino[5] = gz;
    destino[6] = mx;
    destino[7] = my;
    destino[8] = mz;
}

/**
 * @brief Comprueba si la placa está parada mirando la actividad del giroscopio en el último tramo
 */
bool estaParado() {
    float energia_giro = 0.0f;
    // Revisar las últimas 25 muestras (los últimos 250 ms)
    size_t inicio = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE - (25 * EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME);
    for (size_t ix = inicio; ix < EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE; ix += EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME) {
        energia_giro += fabs(buffer[ix + 3]) + fabs(buffer[ix + 4]) + fabs(buffer[ix + 5]);
    }
    float promedio_giro = energia_giro / 25.0f;
    return (promedio_giro < UMBRAL_MOVIMIENTO);
}

void setup()
{
    Serial.begin(115200);
    while (!Serial);

    pinMode(LEDR, OUTPUT);
    pinMode(LEDG, OUTPUT);
    pinMode(LEDB, OUTPUT);
    ponerColorLED(0, 0, 0); // Empezar con el LED apagado

    if (!IMU.begin()) {
        while (1);
    }

    // Llenar el buffer inicial de 2 segundos en silencio
    for (size_t ix = 0; ix < EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE; ix += EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME) {
        uint64_t next_tick = micros() + (EI_CLASSIFIER_INTERVAL_MS * 1000);
        leerMuestraIMU(&buffer[ix]);
        int64_t espera = next_tick - micros();
        if (espera > 0) delayMicroseconds(espera);
    }
}

void loop()
{
    size_t muestras_nuevas = (STRIDE_MS / EI_CLASSIFIER_INTERVAL_MS) * EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME;
    if (muestras_nuevas >= EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
        muestras_nuevas = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;
    }

    // 1. Desplazar buffer y leer muestras nuevas
    memmove(buffer, buffer + muestras_nuevas, (EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE - muestras_nuevas) * sizeof(float));

    for (size_t ix = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE - muestras_nuevas; ix < EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE; ix += EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME) {
        uint64_t next_tick = micros() + (EI_CLASSIFIER_INTERVAL_MS * 1000);
        leerMuestraIMU(&buffer[ix]);
        int64_t espera = next_tick - micros();
        if (espera > 0) delayMicroseconds(espera);
    }

    // 2. SI ESTÁ PARADO: apagar LED y salir sin imprimir nada por pantalla
    if (estaParado()) {
        ponerColorLED(0, 0, 0);
        return;
    }

    // 3. Si hay movimiento, ejecutar el clasificador
    signal_t signal;
    if (numpy::signal_from_buffer(buffer, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal) != 0) return;

    ei_impulse_result_t result = { 0 };
    if (run_classifier(&signal, &result, debug_nn) != EI_IMPULSE_OK) return;

    float max_val = 0.0f;
    String mejor_etiqueta = "";
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
        if (result.classification[ix].value > max_val) {
            max_val = result.classification[ix].value;
            mejor_etiqueta = String(result.classification[ix].label);
        }
    }

    // 4. Solo encender LED y escribir en pantalla si supera el umbral de confianza
    if (max_val >= UMBRAL_CONFIANZA) {
        actualizarLED(mejor_etiqueta);

        if (mejor_etiqueta == "R_mas") {
            Serial.println("Roll positivo (Rojo)");
        } else if (mejor_etiqueta == "R_menos") {
            Serial.println("Roll negativo (Rosa)");
        } else if (mejor_etiqueta == "P_mas") {
            Serial.println("Pitch positivo (Verde)");
        } else if (mejor_etiqueta == "P_menos") {
            Serial.println("Pitch negativo (Amarillo)");
        } else if (mejor_etiqueta == "Y_mas") {
            Serial.println("Yaw positivo (Azul)");
        } else if (mejor_etiqueta == "Y_menos") {
            Serial.println("Yaw negativo (Morado)");
        }
    } else {
        ponerColorLED(0, 0, 0);
    }
}