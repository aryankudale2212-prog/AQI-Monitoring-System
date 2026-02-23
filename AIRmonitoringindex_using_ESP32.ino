#include <Wire.h>
#include <SensirionI2CSen5x.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

SensirionI2CSen5x sen5x;

#define TFT_CS 5
#define TFT_DC 2
#define TFT_RST 4

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

#define RELAY1 26
#define RELAY2 27

float pm25, pm10, temp, hum, voc;

int calculateAQI(float pm) {
  float Clow, Chigh, Ilow, Ihigh;

  if (pm <= 12.0) {
    Clow = 0; Chigh = 12.0; Ilow = 0; Ihigh = 50;
  }
  else if (pm <= 35.4) {
    Clow = 12.1; Chigh = 35.4; Ilow = 51; Ihigh = 100;
  }
  else if (pm <= 55.4) {
    Clow = 35.5; Chigh = 55.4; Ilow = 101; Ihigh = 150;
  }
  else if (pm <= 150.4) {
    Clow = 55.5; Chigh = 150.4; Ilow = 151; Ihigh = 200;
  }
  else if (pm <= 250.4) {
    Clow = 150.5; Chigh = 250.4; Ilow = 201; Ihigh = 300;
  }
  else {
    Clow = 250.5; Chigh = 500.4; Ilow = 301; Ihigh = 500;
  }

  return ((Ihigh - Ilow) / (Chigh - Clow)) * (pm - Clow) + Ilow;
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

  sen5x.begin(Wire);
  sen5x.startMeasurement();

  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);

  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);
}

void loop() {

  uint16_t error;
  char errorMessage[256];

  float pm1, pm4, nox;

  error = sen5x.readMeasuredValues(
    pm1, pm25, pm4, pm10,
    temp, hum, voc, nox
  );

  if (error) {
    Serial.print("Error: ");
    Serial.println(error);
  } else {

    int aqi = calculateAQI(pm25);

    tft.fillScreen(ILI9341_BLACK);
    tft.setTextSize(2);
    tft.setTextColor(ILI9341_WHITE);

    tft.setCursor(10,20);
    tft.print("PM2.5: ");
    tft.print(pm25);

    tft.setCursor(10,50);
    tft.print("AQI: ");
    tft.print(aqi);

    tft.setCursor(10,80);
    tft.print("Temp: ");
    tft.print(temp);

    tft.setCursor(10,110);
    tft.print("Hum: ");
    tft.print(hum);

    tft.setCursor(10,140);
    tft.print("VOC: ");
    tft.print(voc);

    // Relay control
    if (aqi > 150) {
      digitalWrite(RELAY1, HIGH);
      digitalWrite(RELAY2, HIGH);
    } else {
      digitalWrite(RELAY1, LOW);
      digitalWrite(RELAY2, LOW);
    }
  }

  delay(2000);
}