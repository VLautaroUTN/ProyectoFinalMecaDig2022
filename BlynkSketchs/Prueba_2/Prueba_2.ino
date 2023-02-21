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

// << PINES  >>
// control comida 
int pinSComida = D5;
int ledComida = D6;
int pinMotor = D4;

// control luces
int pinLuces = D2; // Transistor --+++++ ok
int fotocelula = D7;

// control temperatura
int pinSenTemp = D1; //--+++++++ ok
OneWire ourWire(pinSenTemp); //pin de datos del DS18B20
int pinReleTemp = D?; // Transistor
// >> PINES  <<

BlynkTimer temporizador;
DallasTemperature sensorTemp(&ourWire); //Declaracion de un objeto para el sensor, inicializa en void Setup
String Temperatura;
float Datos;
int calentadorEstado = 0;
int lightControl = 0;
int ciclos;
int cantCiclos = 10000;
int tiempoAlimento;
int comida;


// ---------------------- Funciones propias: ----------------------
void enviarDatos(){
  Blynk.virtualWrite(V0, Temperatura);
  Blynk.virtualWrite(V1, calentadorEstado);
  Blynk.virtualWrite(V5, comida)
  }

void medirTemperatura(){
  sensorTemp.requestTemperatures(); //Se envia el comando para leer la temperatura
  Datos = sensorTemp.getTempCByIndex(0); //Se obtiene la temperatura en °C
  Temperatura = String(Datos) + " °C";
  }

void dispenarComida(){
  comida = digitalRead(pinSComida);
  if (comida == 1){
    digitalWrite(ledComida, LOW);
    digitalWrite(pinMotor, HIGH);
    delay(tiempoAlimento);
    digitalWrite(pinMotor, LOW);
  }
  else{
    digitalWrite(ledComida, HIGH);
  }
  ciclos = 0;
}


//  ---------------------- VOID SETUP ----------------------
void setup(){
  pinMode(pinLuces, OUTPUT);
  digitalWrite(pinLuces, LOW);
  pinMode(fotocelula, INPUT);
  Serial.begin(115200);
  delay(100);
  sensorTemp.begin(); //sensor de temperatura inicializado
  BlynkEdgent.begin();
  temporizador.setInterval(1000L, enviarDatos);
}


//  ---------------------- VOID LOOP ----------------------

void loop(){
  BlynkEdgent.run();
  temporizador.run();
  medirTemperatura();
  if (lightControl == 1){
    int sensorOscuridad = digitalRead(fotocelula)
    digitalWrite(pinLuces, sensorOscuridad);
  }
  if (ciclos >= cantCiclos){
    dispenarComida();
  }
  ciclos += 1;
}


//  ---------------------- FUNCIONES BLYNK ----------------------

// Boton de encendido y apagadio manual de la luz
BLYNK_WRITE(V2){
  if (lightControl == 0){
    int lightSort = param.asInt();
    digitalWrite(pinLuces, lightSort);
  }
}

//  Boton de modo manual (0) o modo automatico (1)
BLYNK_WRITE(V3){
  lightControl = param.asInt();
}

//Boton Alimentar
BLYNK_WRITE(V4){
  dispenarComida();
}
