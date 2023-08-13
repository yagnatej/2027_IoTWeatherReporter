#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2);
const int buzzerPin = 13;

const char* ssid = "MIG";
const char* password = "Mohit@306";

String URL = "http://api.openweathermap.org/data/2.5/forecast?";
String ApiKey = "7f19e1d3cdaefd3a2c48f3f9efd80958";

String lat = "17.550925240914996";
String lon = "78.33602830021218";

bool showHumidity = false;

void setup() {
  Serial.begin(115200);
  pinMode(buzzerPin, OUTPUT);
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


// ... (other code remains the same)

// ... (other code remains the same)

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(URL + "lat=" + lat + "&lon=" + lon + "&units=metric&appid=" + ApiKey);
    int httpCode = http.GET();

    if (httpCode > 0) {
      String JSON_Data = http.getString();

      DynamicJsonDocument doc(2048);
      deserializeJson(doc, JSON_Data);
      JsonObject obj = doc.as<JsonObject>();

      lcd.clear();
      lcd.print("Weather Forecast");

      JsonArray forecasts = obj["list"].as<JsonArray>();

      for (JsonVariant forecast : forecasts) {
        JsonObject forecastObj = forecast.as<JsonObject>();
        long forecastTime = forecastObj["dt"].as<long>();
        const char* description = forecastObj["weather"][0]["description"].as<const char*>();
        float temp = forecastObj["main"]["temp"].as<float>();
        float temp_min = forecastObj["main"]["temp_min"].as<float>();
        float temp_max = forecastObj["main"]["temp_max"].as<float>();
        float humidity = forecastObj["main"]["humidity"].as<float>();
        int clouds = forecastObj["clouds"]["all"].as<int>();
        int visibility = forecastObj["visibility"].as<int>();
        int weatherId = forecastObj["weather"][0]["id"].as<int>();

        // Convert Unix timestamp to human-readable date and time
        struct tm * timeinfo;
        timeinfo = localtime(&forecastTime);
        char formattedDateTime[20];
        strftime(formattedDateTime, sizeof(formattedDateTime), "%H:%M %d-%m-%Y", timeinfo);

        // Scroll "Weather Forecast" message
        scrollText("Weather Forecast", 1000);

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(formattedDateTime);
        lcd.setCursor(0, 1);
        lcd.print(description);
        delay(2000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Temperature:");
        lcd.setCursor(0, 1);
        lcd.print(temp);
        lcd.print("C");
        delay(2000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Temp Range:");
        lcd.setCursor(0, 1);
        lcd.print(temp_min);
        lcd.print("-");
        lcd.print(temp_max);
        delay(2000);

        // Show humidity on next "page"
        if (showHumidity) {
          lcd.clear();
          lcd.print("Humidity:");
          lcd.setCursor(0, 1);
          lcd.print(humidity);
          lcd.print("%");
          delay(2000);
          showHumidity = false;
        }

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Clouds:");
        lcd.setCursor(0, 1);
        lcd.print(clouds);
        lcd.print("%");
        delay(2000);

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Visibility:");
        lcd.setCursor(0, 1);
        lcd.print(visibility);
        lcd.print("m");
        delay(2000);

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Rain:");
        lcd.setCursor(0, 1);
        if (weatherId >= 500 && weatherId <= 531) {  // Weather codes for rain
          lcd.print("Expected!");
          buzzBuzzer(2500); // Buzz the buzzer for 2.5 seconds
        } else {
          lcd.print("Not Expected");
        }
        delay(2000);
      }
    }
    http.end();
  }
  delay(60000);
}


void scrollText(const char* text, unsigned int duration) {
  int textLength = strlen(text);
  for (int i = 0; i <= textLength - 16; i++) {
    lcd.setCursor(0, 0);
    lcd.print(text + i);
    delay(duration);
  }
}

void buzzBuzzer(unsigned int duration) {
  digitalWrite(buzzerPin, HIGH);
  delay(duration);
  digitalWrite(buzzerPin, LOW);
}
