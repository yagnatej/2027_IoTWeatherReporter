#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2);
const int buzzerPin = 13; // Change this to the pin where your buzzer is connected

const char* ssid = "MIG";
const char* password = "Mohit@306";

String URL = "http://api.openweathermap.org/data/2.5/weather?";
String ApiKey = "7f19e1d3cdaefd3a2c48f3f9efd80958";

String lat = "17.550925240914996";
String lon = "78.33602830021218";

void setup() {
  Serial.begin(115200);
  pinMode(buzzerPin, OUTPUT); // Set the buzzer pin as an output

  lcd.init();
  lcd.backlight();
  lcd.clear();

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http;
    http.begin(URL + "lat=" + lat + "&lon=" + lon + "&units=metric&appid=" + ApiKey);

    int httpCode = http.GET();

    if (httpCode > 0) {
      String JSON_Data = http.getString();
      Serial.println(JSON_Data);

      DynamicJsonDocument doc(2048);
      deserializeJson(doc, JSON_Data);
      JsonObject obj = doc.as<JsonObject>();

      const char* description = obj["weather"][0]["description"].as<const char*>();
      const float temp = obj["main"]["temp"].as<float>();
      const float temp_min = obj["main"]["temp_min"].as<float>();
      const float temp_max = obj["main"]["temp_max"].as<float>();
      const float humidity = obj["main"]["humidity"].as<float>();
      const int clouds = obj["clouds"]["all"].as<int>();
      const int visibility = obj["visibility"].as<int>();
      const int weatherId = obj["weather"][0]["id"].as<int>();

      lcd.clear();
      lcd.print("Weather: ");
      lcd.setCursor(0, 1);
      lcd.print(description);
      delay(2000);

      lcd.clear();
      lcd.print("Temperature: ");
      lcd.setCursor(0, 1);
      lcd.print(temp);
      delay(2000);

      lcd.clear();
      lcd.print("Temp Range: ");
      lcd.setCursor(0, 1);
      lcd.print(temp_min);
      lcd.print("-");
      lcd.print(temp_max);
      delay(2000);

      lcd.clear();
      lcd.print("Humidity: ");
      lcd.setCursor(0, 1);
      lcd.print(humidity);
      lcd.print("%");
      delay(2000);

      lcd.clear();
      lcd.print("Clouds Coverage: ");
      lcd.setCursor(0, 1);
      lcd.print(clouds);
      lcd.print("%");
      delay(2000);

      lcd.clear();
      lcd.print("Visibility: ");
      lcd.setCursor(0, 1);
      lcd.print(visibility);
      lcd.print("m");
      delay(2000);

      lcd.clear();
      lcd.print("Rain: ");
      lcd.setCursor(0, 1);
      if (weatherId >= 500 && weatherId <= 531) {  // Weather codes for rain
        lcd.print("Expected!");
        buzzBuzzer(2500); // Buzz the buzzer for 2.5 seconds
      } else {
        lcd.print("Not Expected");
      }
      delay(2000);
    }

    http.end();
  }
  

  delay(2000);
}

void buzzBuzzer(unsigned int duration) {
  digitalWrite(buzzerPin, HIGH);
  delay(duration);
  digitalWrite(buzzerPin, LOW);
}
