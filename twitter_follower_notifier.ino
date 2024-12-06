#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Wi-Fi credentials and Bearer Token
const char* ssid = " ";              // Replace with your Wi-Fi SSID
const char* password = "";  // Replace with your Wi-Fi password
const char* bearerToken = ""; 

// Twitter API endpoint to get follower count
const char* twitterAPI = "";

// GPIO pin definitions
#define BUZZER_PIN 5    // Buzzer on GPIO5
#define PIN_RED 23      // Red LED on GPIO23
#define PIN_GREEN 22    // Green LED on GPIO22
#define PIN_BLUE 21     // Blue LED on GPIO21

int previousFollowerCount = 0;  // To store the previous follower count

void setup() {
  Serial.begin(115200);  // Initialize serial communication
  WiFi.begin(ssid, password);  // Connect to Wi-Fi

  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");

  // Initialize GPIO pins
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {  
    HTTPClient http;
    http.begin(twitterAPI);  // Start HTTP request
    http.addHeader("Authorization", String("Bearer ") + bearerToken);  // Add Bearer token for authorization

    int httpResponseCode = http.GET();  // Send GET request

    if (httpResponseCode == 200) {  // If request is successful
      String payload = http.getString();  // Get the JSON response
      Serial.println(payload);  // Print response for debugging

      // Parse the JSON to extract follower count
      StaticJsonDocument<512> doc;
      DeserializationError error = deserializeJson(doc, payload);

      if (error) {
        Serial.print(F("JSON Parsing failed: "));
        Serial.println(error.f_str());
      } else {
        int followerCount = doc["data"]["public_metrics"]["followers_count"];
        Serial.print("Current Follower Count: ");
        Serial.println(followerCount);

        // Check if follower count has changed
        if (followerCount != previousFollowerCount) {
          notifyChange();  // Trigger buzzer and LED notification
          previousFollowerCount = followerCount;  // Update the stored count
        }
      }
    } else {
      Serial.print("HTTP Request failed. Code: ");
      Serial.println(httpResponseCode);
    }

    http.end();  // Close the connection
  } else {
    Serial.println("Wi-Fi disconnected. Attempting to reconnect...");
    WiFi.reconnect();  // Try to reconnect
  }

  delay(60000);  // Wait for 1 minute before the next update
}

// Function to notify change using RGB LED and buzzer
void notifyChange() {
  // Flash RGB LED with different colors
  analogWrite(PIN_RED, 255);   // Red on
  analogWrite(PIN_GREEN, 0);   // Green off
  analogWrite(PIN_BLUE, 0);    // Blue off
  delay(500);

  analogWrite(PIN_RED, 0);     // Red off
  analogWrite(PIN_GREEN, 255); // Green on
  analogWrite(PIN_BLUE, 0);    // Blue off
  delay(500);

  analogWrite(PIN_RED, 0);     // Red off
  analogWrite(PIN_GREEN, 0);   // Green off
  analogWrite(PIN_BLUE, 255);  // Blue on
  delay(500);

  // Trigger the buzzer
  tone(BUZZER_PIN, 1000, 500);  // Beep at 1kHz for 500ms
  delay(500);
  noTone(BUZZER_PIN);  // Stop the buzzer
}
