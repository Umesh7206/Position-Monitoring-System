#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <MPU6050.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
MPU6050 mpu;

const char* ssid = "Wokwi-GUEST";
const char* password = "";

const char* server = "http://api.thingspeak.com/update";
String apiKey = "5VKY33TP48RXQCKI";
void setup() {
  Serial.begin(115200);
  Wire.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    while (true);
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed");
    while (true);
  }

  WiFi.begin(ssid, password);
  display.setCursor(0, 0);
  display.println("Connecting WiFi...");
  display.display();

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  display.println("WiFi connected!");
  display.display();
  delay(1000);
}

void loop() {
  int16_t accelX_raw, accelY_raw, accelZ_raw;
  int16_t gyroX_raw, gyroY_raw, gyroZ_raw;

  mpu.getMotion6(&accelX_raw, &accelY_raw, &accelZ_raw, &gyroX_raw, &gyroY_raw, &gyroZ_raw);

  float accelX_g = accelX_raw / 16384.0;
  float accelY_g = accelY_raw / 16384.0;
  float accelZ_g = accelZ_raw / 16384.0;

  float gyroX_dps = gyroX_raw / 131.0;
  float gyroY_dps = gyroY_raw / 131.0;
  float gyroZ_dps = gyroZ_raw / 131.0;

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("AX: "); display.print(accelX_g, 2); display.println(" g");
  display.print("AY: "); display.print(accelY_g, 2); display.println(" g");
  display.print("AZ: "); display.print(accelZ_g, 2); display.println(" g");
  display.print("GX: "); display.print(gyroX_dps, 2); display.println(" dps");
  display.print("GY: "); display.print(gyroY_dps, 2); display.println(" dps");
  display.print("GZ: "); display.print(gyroZ_dps, 2); display.println(" dps");
  display.display();

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(server) +
                 "?api_key=" + apiKey +
                 "&field1=" + String(accelX_g) +
                 "&field2=" + String(accelY_g) +
                 "&field3=" + String(accelZ_g) +
                 "&field4=" + String(gyroX_dps) +
                 "&field5=" + String(gyroY_dps) +
                 "&field6=" + String(gyroZ_dps);
    http.begin(url);
    int httpCode = http.GET();
    http.end();

    Serial.println("Data sent to ThingSpeak");
  }

  delay(15000);
}
