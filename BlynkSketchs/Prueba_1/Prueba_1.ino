#include <OneWire.h>
#include <DallasTemperature.h>

#define BLYNK_TEMPLATE_ID "TMPLxzebwkh9"
#define BLYNK_DEVICE_NAME "ProyectoFinalMecaDig"
#define BLYNK_FIRMWARE_VERSION        "0.1.0"
#define BLYNK_PRINT Serial
#define APP_DEBUG
#define USE_NODE_MCU_BOARD
#include "BlynkEdgent.h"

// ---------------------- VARIABLES: ----------------------
BlynkTimer temporizador;
OneWire ourWire(D1); //pin de datos del DS18B20
DallasTemperature sensorTemp(&ourWire); //Declaracion de un objeto
//para el sensor, inicializa en void Setup
float temperatura;
String Datos;
int HeaterStatus = 0;



// ---------------------- Funciones propias: ----------------------
void enviarTemperatura(){
  Blynk.virtualWrite(V0, Datos);
  Blynk.virtualWrite(V1, HeaterStatus);
  }

void medirTemperatura(){
  sensorTemp.requestTemperatures(); //Se envia el comando para leer la temperatura
  temperatura = sensorTemp.getTempCByIndex(0); //Se obtiene la temperatura en °C
  Datos = String(temperatura) + " °C";
  }

//void btn_alimentar(){}

//void btn_luzAuto(){}

//void btn_OrdenLuz(){}

//  ---------------------- Funciones base ----------------------
void setup(){
  Serial.begin(115200);
  delay(100);
  sensorTemp.begin(); //sensor de temperatura inicializado
  BlynkEdgent.begin();

  temporizador.setInterval(1000L, enviarTemperatura);
}

void loop(){
  BlynkEdgent.run();
  temporizador.run();
  medirTemperatura();
  if (HeaterStatus == 0){HeaterStatus = 1;}
  else{HeaterStatus = 0;}
}
