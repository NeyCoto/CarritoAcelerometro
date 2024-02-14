#include <Arduino.h>

SemaphoreHandle_t mutex;

#include <BluetoothSerial.h>
bool emparejado = false;
BluetoothSerial SerialBT;

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// Variable para recibir y alamacenar datos 
char received;

// Pines del control de puente H
int in1 = 12 ;
int in2 = 14 ;
int in3 = 27 ;
int in4 = 26 ;

// Parametros del control de velocidad
int resolution = 16;
int vel = 0;

void connect_successful(){// Esta función solo es para el debbuging
  while (!emparejado)
  {
    if (!SerialBT.connected())
    {
      Serial.println("Esperando emparejamiento...");
      delay(1000);
    }
    else
    {
      Serial.println("Emparejado con el esclavo");
      emparejado = true;
    }
  }
}

void TaskBluetooth(void *pvParameters) {
  (void) pvParameters;

  SerialBT.begin("ESP32_Esclavo");  // Nombre del dispositivo Bluetooth del esclavo
  Serial.println("ESP32 esclavo iniciado");

  while (1) {

    connect_successful(); 

    if (SerialBT.available()){
      xSemaphoreTake(mutex, portMAX_DELAY);
      received = SerialBT.read();
      xSemaphoreGive(mutex);
      Serial.println(received);
    }

    vTaskDelay(pdMS_TO_TICKS(1));
  }

}

void TaskMotorControl(void *pvParameters) {
  (void) pvParameters;

  // Asiganción de pines como salidas
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  // Configurar la resolución del PWM
  ledcSetup(0, 1000, resolution);  // Canal 0, frecuencia de 5 kHz
  ledcSetup(1, 1000, resolution);  // Canal 1, frecuencia de 5 kHz
  ledcSetup(2, 1000, resolution);  // Canal 2, frecuencia de 5 kHz
  ledcSetup(3, 1000, resolution);  // Canal 3, frecuencia de 5 kHz

  // Asigancion de canales
  ledcAttachPin(in1, 0);
  ledcAttachPin(in2, 1);
  ledcAttachPin(in3, 2);
  ledcAttachPin(in4, 3);

  char localReceived;

  while (1) {

    xSemaphoreTake(mutex, portMAX_DELAY);
    localReceived = received;
    xSemaphoreGive(mutex);

    switch (localReceived){

      case 'B':
      ledcWrite(0, vel); 
      ledcWrite(1, 0); 
      ledcWrite(2, vel); 
      ledcWrite(3, 0); 
      break;

      case 'F':
      ledcWrite(0, 0); 
      ledcWrite(1, vel); 
      ledcWrite(2, 0); 
      ledcWrite(3, vel); 
      break;

      case 'L':
      ledcWrite(0, 0); 
      ledcWrite(1, 0); 
      ledcWrite(2, 0); 
      ledcWrite(3, vel); 
      break;

      case 'R':
      ledcWrite(0, 0); 
      ledcWrite(1, vel); 
      ledcWrite(2, 0); 
      ledcWrite(3, 0); 
      break;

      case 'S':
      ledcWrite(0, 0); 
      ledcWrite(1, 0); 
      ledcWrite(2, 0); 
      ledcWrite(3, 0); 
      break;

      case '0':
      vel = 0;
      break;

      case '1':
      vel = 6554;
      break;

      case '2':
      vel = 13107;
      break;

      case '3':
      vel = 19661;
      break;

      case '4':
      vel = 26214;
      break;

      case '5':
      vel = 32768;
      break;

      case '6':
      vel = 39321;
      break;

      case '7':
      vel = 45875;
      break;

      case '8':
      vel = 52428;
      break;

      case '9':
      vel = 58982;
      break;

      case 'q':
      vel = 65535;
      break;

      default:
      break;
    }

    vTaskDelay(pdMS_TO_TICKS(1));
  }
}

void setup(){

  Serial.begin(115200);

  mutex = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(TaskBluetooth, "TaskBluetooth", 10000, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(TaskMotorControl, "TaskMotorControl", 10000, NULL, 1, NULL, 1);

}

void loop(){

}



