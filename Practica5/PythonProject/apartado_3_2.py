import serial
import time

# Configura según tu sistema
PUERTO = 'COM4'  # Cambiar por tu puerto (ej: 'COM3' en Windows o '/dev/ttyACM0' en Linux/Mac)
BAUDIOS = 115200  # Debe coincidir con el Serial.begin() de tu Arduino

try:
    ser = serial.Serial(PUERTO, BAUDIOS, timeout=1)
    time.sleep(2)  # Espera de estabilización del puerto

    with open("datos_acelerometro.txt", "w", encoding="utf-8") as f:
        # Cabecera
        f.write("Ax;Ay;Az\n")
        print("Capturando datos... Presiona Ctrl+C para detener.")

        while True:
            linea = ser.readline().decode('utf-8', errors='ignore').strip()
            if linea:
                # Si Arduino envía valores separados por coma o espacio, los estandarizamos a ';'
                valores = linea.replace(',', ';').replace(' ', ';').split(';')
                # Eliminar elementos vacíos si hubiese espacios extra
                valores = [v for v in valores if v]

                if len(valores) >= 3:
                    linea_fmt = f"{valores[0]};{valores[1]};{valores[2]}\n"
                    f.write(linea_fmt)
                    f.flush()  # Fuerza la escritura inmediata en el disco duro
                    print(f"Guardado: {linea_fmt.strip()}")

except serial.SerialException as e:
    print(f"Error en la conexión serie: {e}")
except KeyboardInterrupt:
    print("\nCaptura finalizada.")
    if 'ser' in locals() and ser.is_open:
        ser.close()