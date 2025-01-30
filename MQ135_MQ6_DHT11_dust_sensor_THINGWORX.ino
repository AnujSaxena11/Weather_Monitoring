#include <WiFi.h>
#include <DHT.h>

const char* ssid = "Rachit";
const char* password = "rachit08";
const char* host = "thingworx.scetngp.com";
const char* appKey = "b8aad42f-f587-44f0-a8fd-7d850dc069ab";

const char* MethaneThing = "kshitiz_thimg";
const char* MethaneProperty = "Mq135";
const char* MQ6Thing = "kshitiz_thimg";
const char* MQ6Property = "Mq6";
const char* DustThing = "kshitiz_thimg";
const char* DustProperty = "Dust";

const char* DHTThing = "kshitiz_thimg";
const char* TempProperty = "Temp";
const char* HumidityProperty = "Hum";

#define MQ135PIN 35   
#define MQ6PIN 33   
#define DHTPIN 32    
#define DUSTPIN 34    
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.println("Connected to WiFi.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  dht.begin();
}

void loop() {
  float methane = readMethane();
  float mq6 = readMQ6();
  float temperature = readTemperature();
  float humidity = readHumidity();
  float dustDensity = readDustDensity();

  
  Serial.print("Methane: ");
  Serial.print(methane);
  Serial.println(" PPM");
  put(MethaneThing, MethaneProperty, methane);

  Serial.print("MQ6: ");
  Serial.print(mq6);
  Serial.println(" PPM");
  put(MQ6Thing, MQ6Property, mq6);

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");
  put(DHTThing, TempProperty, temperature);

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");
  put(DHTThing, HumidityProperty, humidity);

  Serial.print("Dust Density: ");
  Serial.print(dustDensity);
  Serial.println(" µg/m³");
  put(DustThing, DustProperty, dustDensity);

  delay(1000);
}

float readMethane() {
  int sensorValue = analogRead(MQ135PIN);
  float ppm = sensorValue * 0.1;
  return ppm;
}

float readMQ6() {
  int sensorValue = analogRead(MQ6PIN);
  float ppm = sensorValue * 0.05;
  return ppm;
}

float readTemperature() {
  return dht.readTemperature();
}

float readHumidity() {
  return dht.readHumidity();
}

float readDustDensity() {
  int sensorValue = analogRead(DUSTPIN);
  float density = sensorValue * 0.1;
  return density;
}

void put(const char* ThingName, const char* ThingProperty, float Value) {
  WiFiClient client;
  
  if (!client.connect(host, 80)) {
    Serial.println("Connection failed.");
    return;
  }

  String url = "/Thingworx/Things/" + String(ThingName) + "/Properties/" + String(ThingProperty);
  String data = "{\"" + String(ThingProperty) + "\":" + String(Value) + "}";

  client.print(String("PUT ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "appKey: " + appKey + "\r\n" +
               "x-thingworx-session: false\r\n" +
               "Content-Type: application/json\r\n" +
               "Content-Length: " + data.length() + "\r\n" +
               "Connection: close\r\n" +
               "\r\n" + data);
  
  Serial.println("Data sent to ThingWorx.");
}
