#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Abcd";         // Change to your WiFi name
const char* password = "jatingoel"; // Change to your WiFi password
const char* serverUrl = "http://192.168.237.87:5000/data";  // Raspberry Pi IP

#define FORCE_SENSOR_PIN 34  // Force sensor (Analog input)
#define MQ3_SENSOR_PIN 35    // Alcohol sensor (Analog input)
#define BUZZER_PIN 13        // Buzzer for alerts

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi!");

    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);  // Buzzer OFF initially
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        // Read Force Sensor
        int forceValue = analogRead(FORCE_SENSOR_PIN);
        Serial.print("🔵 Force Sensor Value: ");
        Serial.println(forceValue);
        
        // Read Alcohol Sensor
        int mq3Value = analogRead(MQ3_SENSOR_PIN);
        Serial.print("🟡 MQ3 Sensor Value: ");
        Serial.println(mq3Value);

        // Check conditions
        String status;
        if (forceValue < 500) {
            status = "Helmet Not Worn";
            digitalWrite(BUZZER_PIN, HIGH);
            delay(1000);
            digitalWrite(BUZZER_PIN, LOW);
        } else if (mq3Value > 3025) {
            status = "Alcohol Detected";
            digitalWrite(BUZZER_PIN, HIGH);
            delay(1000);
            digitalWrite(BUZZER_PIN, LOW);
        } else {
            status = "Allow Start";  // Helmet detected and no alcohol detected
        }

        // Send Data to Raspberry Pi
        HTTPClient http;
        http.begin(serverUrl);
        http.addHeader("Content-Type", "application/json");

        String jsonData = "{\"force\":" + String(forceValue) + ", \"alcohol\":" + String(mq3Value) + ", \"status\":\"" + status + "\"}";
        int httpResponseCode = http.POST(jsonData);
        Serial.print("Sent Data: ");
        Serial.println(jsonData);
        http.end();
    }

    delay(250);  // Send data every 2 seconds
}
