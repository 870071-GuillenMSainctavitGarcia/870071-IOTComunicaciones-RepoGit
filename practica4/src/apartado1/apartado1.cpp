#include <ArduinoBLE.h>

// 1. Definimos el UUID personalizado con las letras de tu nombre en HEX
// "GUILLEN" -> 47 55 49 4C 45 4E
#define SERVICE_UUID "4755494c-4c45-4e6c-d104-768a12141234"

// 2. Creación del servicio BLE usando ese UUID
BLEService miServicio(SERVICE_UUID); 

void setup() {
  Serial.begin(115200);
  
  if (!BLE.begin()) {
    Serial.println("¡Error al iniciar BLE!");
    while (1);
  }

  // LE PONGO NOMBRE AL DISPOSITIVO BLE, QUE ES EL QUE SE MOSTRARÁ EN EL MÓVIL CUANDO BUSQUE DISPOSITIVOS
  BLE.setLocalName("GUILLEN_Arduino");

  // Añadimos el UUID del servicio al paquete de Advertising
  BLE.setAdvertisedService(miServicio); // ESTO HACE QUE EL MÓVIL PUEDA VER EL UUID DEL SERVICIO CUANDO BUSQUE DISPOSITIVOS

  // Añadimos el servicio al servidor GATT interno
  BLE.addService(miServicio); // SE AÑADE EL SERVICIO AL GATT DEL NANO, PARA QUE EL MÓVIL PUEDA CONECTARSE Y LEERLO

  // Iniciamos la emisión de anuncios (Advertising)
  BLE.advertise(); // INICIO DE LA EMISIÓN DE PUBLICIDAD BLE

  Serial.println("Emitiendo publicidad BLE con UUID personalizado...");
}

void loop() {
  // El módulo estará emitiendo automáticamente en segundo plano
  BLEDevice central = BLE.central();  // COMPRUEBA SI HAY UN MÓVIL CONECTADO, SI NO HAY NINGUNO, DEVUELVE UN OBJETO VACÍO

  if (central) { // SI ESTA CONECTADO UN MÓVIL, ENTRA EN ESTE BLOQUE
    Serial.print("¡Móvil conectado! Dirección MAC: ");
    Serial.println(central.address()); // DIRECCIÓN MAC DEL MÓVIL CONECTADO

    // Mientras el móvil siga conectado
    while (central.connected()) {
      // Aquí podemos actualizar el valor o hacer cosas periódicas
      delay(1000);
    }

    Serial.println("Móvil desconectado. Volviendo a emitir publicidad...");
   //BLE.advertise(); opcional, no hace falta volver a lanzarlo, hasta ahora me lo ha detectado siempre, pero por si acaso lo dejo comentado
   
  }
  Serial.println("Esperando a que un móvil se conecte...");
}