#define BLYNK_TEMPLATE_ID "TMPL6u8aoJ_YR"
#define BLYNK_TEMPLATE_NAME "ESP32 DHT11 Monitor"
#define BLYNK_AUTH_TOKEN "v4drlqDzd83izaa8XLmpQXhEzEhTo8E5"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include "DHT.h"

// ================= WIFI =================
char ssid[] = "TP-Link_F7FD";
char pass[] = "60162324";

// ================= OLED =================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

// ================= DHT11 =================
#define DHTPIN 23
#define DHTTYPE DHT11

// ================= MQ-135 =================
#define MQ135_PIN 32
#define AIR_GOOD_MAX 300
#define AIR_WARN_MAX 800

// ================= NGƯỠNG =================
float TEMP_LOW_THRESHOLD  = 25.0;
float TEMP_HIGH_THRESHOLD = 27.0;
float HUM_LOW_THRESHOLD   = 45.0;
float HUM_HIGH_THRESHOLD  = 70.0;
int   LIGHT_LOW_THRESHOLD = 899;
int   LIGHT_HIGH_THRESHOLD = 3199;

// ================= LED =================
#define LED_TEMP_LOW    33
#define LED_TEMP_HIGH   19
#define LED_HUM_LOW     18
#define LED_HUM_HIGH     4
#define LED_LIGHT_LOW   26
#define LED_LIGHT_HIGH  12
#define LED_WIFI        14
#define LED_AIR_BAD     13  // LED sáng khi không khí KÉM

// ================= BUZZER =================
#define BUZZER_TEMP 25

// ================= ÁNH SÁNG =================
#define LIGHT_D0 27
#define LIGHT_A0 34

// ================= OBJECT =================
DHT dht(DHTPIN, DHTTYPE);
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
BlynkTimer timer;

// ================= BIẾN =================
String airStatus = "TOT";
bool airBad = false;

// Flag để tránh spam event (gửi chỉ 1 lần khi vượt ngưỡng)
bool notifiedTempHigh = false;
bool notifiedTempLow  = false;
bool notifiedHumHigh  = false;
bool notifiedHumLow   = false;
bool notifiedLightHigh = false;
bool notifiedLightLow  = false;
bool notifiedAirBad   = false;

// ======================================================
void setup() {
  Serial.begin(115200);

  // LED
  pinMode(LED_TEMP_LOW, OUTPUT);
  pinMode(LED_TEMP_HIGH, OUTPUT);
  pinMode(LED_HUM_LOW, OUTPUT);
  pinMode(LED_HUM_HIGH, OUTPUT);
  pinMode(LED_LIGHT_LOW, OUTPUT);
  pinMode(LED_LIGHT_HIGH, OUTPUT);
  pinMode(LED_WIFI, OUTPUT);
  pinMode(LED_AIR_BAD, OUTPUT);

  // Buzzer
  pinMode(BUZZER_TEMP, OUTPUT);
  digitalWrite(BUZZER_TEMP, LOW);

  // Sensors
  pinMode(LIGHT_D0, INPUT);
  pinMode(LIGHT_A0, INPUT);
  pinMode(MQ135_PIN, INPUT);

  // WiFi + Blynk
  WiFi.begin(ssid, pass);
  Blynk.config(BLYNK_AUTH_TOKEN);

  // DHT + OLED
  dht.begin();
  Wire.begin(21, 22);

  if (!display.begin(SCREEN_ADDRESS, true)) {
    Serial.println("LOI OLED!");
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println("Khoi dong he thong...");
  display.display();

  // Test buzzer
  digitalWrite(BUZZER_TEMP, HIGH);
  delay(1500);
  digitalWrite(BUZZER_TEMP, LOW);

  timer.setInterval(2000L, readAndSendSensors);
}

// ======================================================
void readAndSendSensors() {
  digitalWrite(LED_WIFI, WiFi.status() == WL_CONNECTED);

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int lightA = analogRead(LIGHT_A0);
  int airADC = analogRead(MQ135_PIN);

  if (isnan(h) || isnan(t)) {
    Serial.println("LOI DHT11");
    return;
  }

  // ===== TRẠNG THÁI =====
  bool tempLow   = t < TEMP_LOW_THRESHOLD;
  bool tempHigh  = t > TEMP_HIGH_THRESHOLD;
  bool humLow    = h < HUM_LOW_THRESHOLD;
  bool humHigh   = h > HUM_HIGH_THRESHOLD;
  bool lightLow  = lightA < LIGHT_LOW_THRESHOLD;
  bool lightHigh = lightA > LIGHT_HIGH_THRESHOLD;

  if (airADC < AIR_GOOD_MAX) {
    airStatus = "TOT";
    airBad = false;
  } else if (airADC < AIR_WARN_MAX) {
    airStatus = "TB";
    airBad = false;
  } else {
    airStatus = "KEM";
    airBad = true;
  }

  // ===== ĐIỀU KHIỂN LED & BUZZER (giữ nguyên) =====
  digitalWrite(LED_TEMP_LOW, tempLow);
  digitalWrite(LED_TEMP_HIGH, tempHigh);
  digitalWrite(LED_HUM_LOW, humLow);
  digitalWrite(LED_HUM_HIGH, humHigh);
  digitalWrite(LED_LIGHT_LOW, lightLow);
  digitalWrite(LED_LIGHT_HIGH, lightHigh);
  digitalWrite(LED_AIR_BAD, airBad);
  digitalWrite(BUZZER_TEMP, tempLow || tempHigh);

  // ===== TRIGGER EVENTS (chỉ gửi khi vượt ngưỡng và chưa notify) =====
  if (tempHigh && !notifiedTempHigh) {
    Blynk.logEvent("canh_bao_high_temp", "Nhiệt độ cao: " + String(t, 1) + "°C");
    notifiedTempHigh = true;
  } else if (!tempHigh) {
    notifiedTempHigh = false;
  }

  if (tempLow && !notifiedTempLow) {
    Blynk.logEvent("canh_bao_low_temp", "Nhiệt độ thấp: " + String(t, 1) + "°C");  // Nếu bạn tạo event này
    notifiedTempLow = true;
  } else if (!tempLow) {
    notifiedTempLow = false;
  }

  if (humHigh && !notifiedHumHigh) {
    Blynk.logEvent("canh_bao_high_hum", "Độ ẩm cao: " + String(h, 1) + "%");
    notifiedHumHigh = true;
  } else if (!humHigh) {
    notifiedHumHigh = false;
  }

  if (humLow && !notifiedHumLow) {
    Blynk.logEvent("canh_bao_low_hum", "Độ ẩm thấp: " + String(h, 1) + "%");  // Nếu bạn tạo event low hum
    notifiedHumLow = true;
  } else if (!humLow) {
    notifiedHumLow = false;
  }

  if (lightHigh && !notifiedLightHigh) {
    Blynk.logEvent("canh_bao_high_light", "Ánh sáng cao: " + String(lightA));
    notifiedLightHigh = true;
  } else if (!lightHigh) {
    notifiedLightHigh = false;
  }

  if (lightLow && !notifiedLightLow) {
    Blynk.logEvent("canh_bao_low_light", "Ánh sáng thấp: " + String(lightA));
    notifiedLightLow = true;
  } else if (!lightLow) {
    notifiedLightLow = false;
  }

  if (airBad && !notifiedAirBad) {
    Blynk.logEvent("canh_bao_khongkhi_kem", "Không khí kém: " + String(airADC) + " (KEM)");
    notifiedAirBad = true;
  } else if (!airBad) {
    notifiedAirBad = false;
  }

  // ===== GỬI LÊN BLYNK (giữ nguyên) =====
  Blynk.virtualWrite(V0, t);           // NHIETDO - Double
  Blynk.virtualWrite(V1, h);           // DOAM - Double
  Blynk.virtualWrite(V2, lightA);      // LIGHT_A0 - Integer
  Blynk.virtualWrite(V3, airADC);      // AIR_ADC - Integer
  Blynk.virtualWrite(V4, tempLow ? 1 : 0);   // LED_TEMP_LOW
  Blynk.virtualWrite(V5, tempHigh ? 1 : 0);  // LED_TEMP_HIGH
  Blynk.virtualWrite(V6, humLow ? 1 : 0);    // LED_HUM_LOW
  Blynk.virtualWrite(V7, humHigh ? 1 : 0);   // LED_HUM_HIGH
  Blynk.virtualWrite(V8, lightLow ? 1 : 0);  // LED_LIGHT_LOW
  Blynk.virtualWrite(V9, lightHigh ? 1 : 0); // LED_LIGHT_HIGH
  Blynk.virtualWrite(V10, airBad ? 1 : 0);   // LED_KK_KEM

  // ===== OLED (giữ nguyên) =====
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  
  display.setCursor(0, 0);
  display.print("Nhiet do: ");
  display.print(t, 1);
  display.println(" *C");

  display.setCursor(0, 12);
  display.print("Do am: ");
  display.print(h, 1);
  display.println(" %");

  display.setCursor(0, 24);
  display.print("Anh sang: ");
  display.println(lightA);

  display.setCursor(0, 36);
  display.print("KK: ");
  display.print(airADC);
  display.print(" - ");
  display.println(airStatus);

  display.display();

  // ===== SERIAL (giữ nguyên) =====
  Serial.print("T=");
  Serial.print(t);
  Serial.print(" H=");
  Serial.print(h);
  Serial.print(" Light=");
  Serial.print(lightA);
  Serial.print(" Air=");
  Serial.print(airADC);
  Serial.print(" (");
  Serial.print(airStatus);
  Serial.println(")");
}

// ======================================================
void loop() {
  Blynk.run();
  timer.run();
}

