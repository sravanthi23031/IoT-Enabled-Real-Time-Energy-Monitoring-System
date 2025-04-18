#include <WiFi.h>
#include <HTTPClient.h>
#include <PZEM004Tv30.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// WiFi Credentials
const char* ssid = "Medicharla";
const char* password = "Sravanthi";

// ThingSpeak API Info
const char* server = "http://api.thingspeak.com/update";
const char* apiKey = "I8JQN2QBNRPGK81O"; // Replace with your API Key

// PZEM-004T Configuration (TX=26, RX=25)
PZEM004Tv30 pzem(&Serial1, 25, 26);

// DHT11 Configuration
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nConnected to WiFi!");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Read Data from Sensors
    float voltage = pzem.voltage();
    float current = pzem.current();
    float power = pzem.power();
    float energy = pzem.energy();
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    // Check for Valid Data
    bool sensorError = false;

    if (isnan(voltage)) {
      Serial.println("Error: Failed to read Voltage!");
      sensorError = true;
    }
    if (isnan(current)) {
      Serial.println("Error: Failed to read Current!");
      sensorError = true;
    }
    if (isnan(power)) {
      Serial.println("Error: Failed to read Power!");
      sensorError = true;
    }
    if (isnan(energy)) {
      Serial.println("Error: Failed to read Energy!");
      sensorError = true;
    }
    if (isnan(humidity)) {
      Serial.println("Error: Failed to read Humidity!");
      sensorError = true;
    }
    if (isnan(temperature)) {
      Serial.println("Error: Failed to read Temperature!");
      sensorError = true;
    }

    // If any sensor failed, do not proceed
    if (sensorError) {
      Serial.println("Error: One or more sensors failed to read data!");
      return;
    }

    // Create API Request URL
    String url = String(server) + "?api_key=" + apiKey +
                 "&field1=" + String(voltage) +
                 "&field2=" + String(current) +
                 "&field3=" + String(power) +
                 "&field4=" + String(energy) +
                 "&field5=" + String(temperature) +
                 "&field6=" + String(humidity);

    // Send Data to ThingSpeak
    http.begin(url);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      Serial.println("Data Sent to ThingSpeak!");
      Serial.println(http.getString()); // Print server response
    } else {
      Serial.print("Error sending data: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  }

  delay(15000); // Send data every 15 seconds
}
