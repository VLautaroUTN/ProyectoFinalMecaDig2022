#include <OneWire.h>
#include <ESP82>
OneWire  ds(D1);  // on pin D1 (a 4.7K resistor is necessary)


// ----- Prototipos de funcion -----
float medirTemperatura();
float comunicacionWeb();


// Variables globales
const char* ssid = "RED";
const char* password = "CONTRASEÑA";

WiFiServer server(80);

String header;

unsigned long lastTime, timeout = 2000;
const int outputPin = 0; // pin de salida
String outputState = "apagado"; //almacenar el estado actual de la salida


void setup(void) {
  Serial.begin(9600);
    pinMode(outputPin, OUTPUT);
  digitalWrite(outputPin, LOW);
  
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid,password);

  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Dispositivo conectado.");
  Serial.println("Direccion IP: ");
  Serial.print(WiFi.localIP());

  server.begin();

}


void loop(void) {
 
}


// ----- Implementacion de funciones -----

//Mide la temperatura con el sensor DS18B20 y devuelve un float
float medirTemperatura(){
  byte i;
  byte present = 0;
  byte data[9];
  byte addr[8];
  float celsius;
  
  if ( !ds.search(addr)) {
    ds.reset_search();
    delay(250);
    return;
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return;
  }
 
  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
  }

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];

  byte cfg = (data[4] & 0x60);
  // at lower res, the low bits are undefined, so let's zero them
  if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
  else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
  else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
  //// default is 12 bit resolution, 750 ms conversion time
  
  celsius = (float)raw / 16.0;
  return celsius;
}

// Comunica el NodeMCU 
void comunicacionWeb(){

  WiFiClient client = server.available();

  if(client)
  {
    lastTime = millis();
    
    Serial.println("Nuevo cliente");
    String currentLine = "";

    while(client.connected() && millis() - lastTime <= timeout)
    {

      if(client.available())
      {
        
        char c = client.read();
        Serial.write(c);
        header += c;

        if(c == '\n')
        {
          
          if(currentLine.length() == 0)
          {

            ////////// ENCABEZADO HTTP ////////////

            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();


            if (header.indexOf("GET /on") >= 0) {
              Serial.println("GPIO encendido");
              outputState = "encendido";
              digitalWrite(outputPin, HIGH);
            }                         
            else if (header.indexOf("GET /off") >= 0) {
              Serial.println("GPIO apagado");
              outputState = "apagado";
              digitalWrite(outputPin, LOW);
            } 
            

             //////// PAGINA WEB //////////////
             
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("</head>");

            client.println("<body></body>");

            client.println("</html>");

            client.println();
            break;

            
            /////////////////////////////////////
          }
          else
          {
            currentLine = "";
          }
        }
        else if ( c != '\r')
        {
          currentLine += c;
        }
        
        
      }

      
    }

    header = "";
    client.stop();
    Serial.println("Cliente desconectado.");
    Serial.println("");
  }

}