#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>


// Sustituir con datos de vuestra red
const char* ssid     = "Richi wifi";
const char* password = "28042991";
const char* mqttServer = "192.168.128.225";
const int mqttPort = 1883;
const char* mqttUser = "";
const char* mqttPassword = "";

WiFiClient espClient;
PubSubClient client(espClient);

//Sensor DTH11
#define DHTPIN 2 
#define DHTTYPE DHT11
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

void setup()
{
  Serial.begin(9600);
  delay(10);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Conectando a:\t");
  Serial.println(ssid); 

  // Esperar a que nos conectemos
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(200);
  Serial.print('.');
  }

  // Mostrar mensaje de exito y dirección IP asignada
  Serial.println();
  Serial.print("Conectado a:\t");
  Serial.println(WiFi.SSID()); 
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());

  //Iniciamos el sensor DHT11
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;

  //Conectamos con el servidor mqtt
  client.setServer(mqttServer, mqttPort);
  while (!client.connected())
  {    Serial.println("Connecting to MQTT...");
       if (client.connect("ESP32Client", mqttUser, mqttPassword ))
           Serial.println("connected");
       else
       {   Serial.print("failed with state ");
           Serial.print(client.state());
           delay(2000);
       }
  }
}

void loop() 
{
 
  float temperatura;
  //Medimos temperatura y humedad con el sensor DHT11
  // Delay between measurements.
  delay(delayMS);
  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  }
  else {
    Serial.print(F("Temperature: "));
    temperatura = event.temperature;
    Serial.print(temperatura);
    Serial.println(F("°C"));
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }
  else {
    Serial.print(F("Humidity: "));
    Serial.print(event.relative_humidity);
    Serial.println(F("%"));
  }   

   //publicamos en el servidor mqtt
  client.loop();
  char str[16];
  //sprintf(str, "%u", random(100));
  //sprintf(str, "%u", temperatura);
  dtostrf(temperatura, 10,2,str);

  client.publish("/ESP32/comedor/temperatura", str);
  Serial.println(str);
  delay(5000);
}