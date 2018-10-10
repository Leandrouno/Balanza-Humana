/*
 Arduino pin 6 -> HX711 CLK
 Arduino pin 5 -> HX711 DOUT
 Arduino pin 5V -> HX711 VCC
 Arduino pin GND -> HX711 GND
    D2           Led_Wifi 
    D0           Led Envio Datos
*/
#define LED D0
#define LED_BUILTIN D2
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
                pinMode(LED_BUILTIN, OUTPUT);
                digitalWrite(LED_BUILTIN, LOW); 
                pinMode(LED, OUTPUT);     
                digitalWrite(LED, LOW);         
                Serial.begin(9600);
                delay(10);
                scale.set_scale();
                scale.tare();  
                WiFi.mode(WIFI_STA);
                WiFiMulti.addAP("Quien es Mi Idolo ?", "LioMessi");
              //  WiFiMulti.addAP("LosPuchus", "BastaMartina2017");
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
               if (WiFi.status() == WL_CONNECTED) {
                                                  digitalWrite(LED_BUILTIN, HIGH);
                                                   } else {digitalWrite(LED_BUILTIN, LOW);}
               if(millis() - milisanteriores >= 1000 && ya_mando == 0){     
                                                    milisanteriores = millis();
                                                    scale.set_scale(calibration_factor);
                                                    unidades = scale.get_units(), 10;
                                                     if (unidades <= 0) { unidades = 0.00; i = 0;}
                                                     if (unidades > 2) {
                                                                        peso [i]= unidades;
                                                                        if(i!=0){
                                                                                  if(unidades > peso [i-1]-1 && unidades < peso [i-1]+1)  
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
