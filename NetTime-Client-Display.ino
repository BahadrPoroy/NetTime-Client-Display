#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Adafruit_SH110X.h>
#include <time.h>
#include <dht11.h>
#include "secrets.h"
#include "language.h"
#include <Fonts/FreeSansBold24pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>

// --- Display Settings ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define DHT11PIN 14
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// --- Communication & Sync Settings ---
WiFiUDP udp;
unsigned int localUdpPort = 4210;
int failCounter = 0;
const int MAX_RETRIES = 3;
bool isLocalMode = true;
unsigned long lastLocalCheck = 0;
const unsigned long LOCAL_CHECK_INTERVAL = 300000;  // 5 minutes

// --- Global Data Variables ---
String displayTime = "--:--";
String displayDate = "--/--/--";
bool isConnected = false;
bool showNoWifi = true;
unsigned long lastBlinkTime = 0;

dht11 DHT;

// --- Time Settings (GMT+3) ---
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3 * 3600;
struct tm timeinfo;

// --- Page & Animation Management ---
int currentPage = 0;
const int totalPages = 2;
unsigned long lastPageSwitch = 0;
const unsigned long pageInterval = 2500;

void setupOTA() {
  ArduinoOTA.setHostname("NetTime-Env-Client");
  ArduinoOTA.setPassword(YOUR_OTA_PASS);

  ArduinoOTA.onStart([]() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(10, 10);
    display.println("Guncelleniyor...");
    display.display();
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    int yuzde = progress / (total / 100);

    display.clearDisplay();
    display.setFont(&FreeSansBold9pt7b);
    display.setTextColor(SH110X_WHITE);

    display.setCursor(5, 25);
    display.print("Guncelleniyor");

    display.setCursor(40, 40);
    display.print("%");
    display.print(yuzde);

    display.setFont();
    display.drawRect(5, 52, 118, 10, SH110X_WHITE);
    display.fillRect(5, 52, map(yuzde, 0, 100, 0, 118), 10, SH110X_WHITE);

    display.display();
  });

  ArduinoOTA.onEnd([]() {
    display.clearDisplay();
    display.setTextColor(SH110X_WHITE);

    display.setFont(&FreeSansBold9pt7b);
    display.setTextSize(1);

    display.setCursor(10, 25);
    display.print("BASARILI!");

    display.setFont();
    display.setTextSize(1);

    display.setCursor(10, 45);
    display.print("Yeniden basliyor...");

    display.display();
  });

  ArduinoOTA.onError([](ota_error_t error) {
    display.clearDisplay();
    display.setCursor(10, 10);
    display.print("ERROR: ");
    display.println(error);
    display.display();
  });

  ArduinoOTA.begin();
}

void setup() {
  Serial.begin(115200);

  if (!display.begin(0x3C, true)) {
    Serial.println("OLED failed!");
  }
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);

  // WiFi Connection
  WiFi.begin(YOUR_SSID, YOUR_PASS);

  setupOTA();

  udp.begin(localUdpPort);

  // Initial NTP Setup (Always active in background)
  configTime(gmtOffset_sec, 0, ntpServer);

  Serial.println("\nClient Started in Hybrid Mode.");

  displayBrandLogo();
  delay(2000);
}

void loop() {
  ArduinoOTA.handle();
  handleHybridSync();  // Burada isConnected güncelleniyor

  DHT.read(DHT11PIN);

  // Sayfa değiştirme zamanlaması
  if (millis() - lastPageSwitch >= pageInterval) {
    currentPage = (currentPage + 1) % totalPages;
    lastPageSwitch = millis();
  }

  // EKRAN ÇİZİMİ
  display.clearDisplay();

  if (!isConnected) {
    // WIFI YOKSA: Uyarı yanıp sönsün
    if (millis() - lastBlinkTime >= 500) {
      showNoWifi = !showNoWifi;
      lastBlinkTime = millis();
    }

    if (showNoWifi) {
      display.setFont();
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.print("! NO WIFI");
    }
  }
  // WIFI VARSA: Normal sayfaları göster (Sunucu olmasa bile NTP saati görünür)
  switch (currentPage) {
    case 0: displayTimePage(); break;
    case 1: displayWeatherPage(); break;
  }


  display.display();
}