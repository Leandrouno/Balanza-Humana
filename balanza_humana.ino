/*
 Setup your scale and start the sketch WITHOUT a weight on the scale
 Once readings are displayed place the weight on the scale
 Press +/- or a/z to adjust the calibration_factor until the output readings match the known weight
 Arduino pin 6 -> HX711 CLK
 Arduino pin 5 -> HX711 DOUT
 Arduino pin 5V -> HX711 VCC
 Arduino pin GND -> HX711 GND
    D0           Led_Wifi 
*/
#define LED D0
#include "HX711.h"
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

ESP8266WiFiMulti WiFiMulti;

HX711 scale(D3, D4);

float calibration_factor = 20223; 
float unidades = 0, peso[]={0,0,0,0,0}, suma = 0, promedio = 0;
int i = 0, ya_mando = 0;
long milisanteriores = 0, milisanteriores2 = 0;


void setup() {
                pinMode(LED, OUTPUT);     
                digitalWrite(LED, LOW);         
                Serial.begin(9600);
                delay(10);
                scale.set_scale();
                scale.tare();  
                WiFi.mode(WIFI_STA);
                //  WiFiMulti.addAP("Quien es Mi Idolo ?", "LioMessi");
                WiFiMulti.addAP("LosPuchus", "BastaMartina2017");
                Serial.println();
                Serial.print("Conectando al  WiFi... ");
                      while (WiFiMulti.run() != WL_CONNECTED) {
                                                                Serial.print(".");
                                                                delay(500);
                                                                
                                                              }
                Serial.println("");
                Serial.println("WiFi Conectado");                
                Serial.println("Direccion IP: ");
                Serial.println(WiFi.localIP());
                delay(500);
                long zero_factor = scale.read_average(); 
                Serial.print("Zero factor: "); 
                Serial.println(zero_factor);
              }

void loop() {
             if(millis() - milisanteriores >= 1000){     
                                                    milisanteriores = millis();
                                                    scale.set_scale(calibration_factor);
                                                    unidades = scale.get_units(), 10;
                                                     if (unidades <= 0) { unidades = 0.00; i = 0;}
                                                     if (unidades > 2) {
                                                                        peso [i]= unidades;
                                                                        if(i!=0){
                                                                                  if(unidades > peso [i-1]-2 && unidades < peso [i-1]+2)  
                                                                                  {i++;}
                                                                                  else {i = 0;}
                                                                                } else {i++;}
                                                    digitalWrite(LED, HIGH);  
                                                    Serial.print(i);
                                                    Serial.print(" - ");  
                                                    Serial.println(unidades); 
                                                    delay(500);
                                                    digitalWrite(LED, LOW);           
                                                    if(i>=5){
                                                              for(int j=0;j<5;j++){suma = suma + peso[j]; }
                                                              promedio = suma/5;
                                                              if(promedio!=0 && ya_mando==0){
                                                                                              Serial.print("Peso : ");             
                                                                                              Serial.print(promedio);
                                                                                              Serial.print(" kg"); 
                                                                                              Serial.println();
                                                                                              //delay(100);
                                                                                              enviar_datos();
                                                                                              ya_mando = 1;}
                                                              i = 0;
                                                              suma = 0;
                                                              promedio = 0;}
                                                                      }
        
                                                              if(Serial.available()){
                                                                                      char temp = Serial.read();
                                                                                      if(temp == '+' || temp == 'a')calibration_factor += 1;
                                                                                      else if(temp == '-' || temp == 'z')calibration_factor -= 1;
                                                                                    }
                                                  }

                                                  if(millis()-milisanteriores2 >= 30000){milisanteriores2 = millis();ya_mando=0;}
                  }

void enviar_datos() {
                        digitalWrite(LED,HIGH);
                        const uint16_t port = 80;
                        const char * host = "www.leandrouno.com.ar"; // ip or dns
                        Serial.print("Conectando a ");
                        Serial.println(host);
                        WiFiClient client;
                      if (!client.connect(host, port)) {
                                                        Serial.println("Conexion Fallida");
                                                        Serial.println("Esperando 5 Segundos");
                                                        delay(5000);
                                                        return;
                                                        }
                      client.print(String("GET /Balanza/addpeso.php?peso=") + promedio + " HTTP/1.1\r\n" +
                      "Host: " + host + "\r\n" +
                      "Connection: close\r\n\r\n");
                       while (client.available() == 0) {
                                                        static int count = 0;
                                                        Serial.print(".");
                                                        delay(250);
                                                        if (count > 12) //waiting more than 3 seconds
                                                        break;
                                                       }
                       Serial.println();
                       if (client.connected()) {Serial.println("Peso Agregado a la Base de Datos");}
                       else { Serial.println("Error de Conexion.");  }
                       Serial.println("Cerrando conexion");
                       client.stop();
                       digitalWrite(LED, LOW);
                     }
