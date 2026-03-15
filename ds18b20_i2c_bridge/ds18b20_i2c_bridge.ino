/**
 * DS18B20 → BLE-Bridge
 * =====================
 * Plattform : ESP32-C3
 * 1-Wire    : GPIO1  (DS18B20 Data, 4.7 kΩ Pull-Up nach 3.3 V)
 * Sensor-VCC: GPIO0  (OUTPUT HIGH → 3.3 V) — Strapping-Pin, muss HIGH bleiben!
 * Sensor-GND: GPIO2  (OUTPUT LOW  → GND)
 * Direktanschluss DS18B20: Pin0=3V3, Pin1=Data, Pin2=GND
 * Onboard-LED: GPIO8 (blau) — 2× kurz blinken bei gültiger Messung
 *
 * BLE Advertising: BTHome v2, Service UUID 0xFCD2, Non-Connectable
 * Service-Data-Format:
 *   Byte 0 : 0x40  (BTHome v2, kein Encryption)
 *   Byte 1 : 0x02  (Object-ID: Temperature)
 *   Byte 2-3: int16 LE × 0.01 °C  →  25.0 °C = 0xC4 0x09
 *
 * Board-Manager : esp32 by Espressif  3.x.x  (IDF 5.x)
 * Board         : "ESP32C3 Dev Module"
 * Libraries     : OneWire, DallasTemperature, NimBLE-Arduino, WiFi, ArduinoOTA
 *
 * OTA-Flash     : Arduino IDE → Werkzeuge → Port → "temp_bridge (ESP32-C3)" (Netzwerk)
 *                 Passwort: siehe OTA_PASSWORD unten
 *
 * ── ESPHome Gegenseite (BTHome v2) ───────────────────────────────────────────
 *
 *   esp32_ble_tracker:
 *     scan_parameters:
 *       active: false
 *
 *   sensor:
 *     - platform: bthome
 *       mac_address: "XX:XX:XX:XX:XX:XX"   # ← MAC aus Serial-Monitor eintragen
 *       temperature:
 *         id: sensor_temp_becken
 *         name: "Temperatur Becken"
 *         on_value:
 *           - lvgl.widget.refresh: lbl_temp_becken
 * ─────────────────────────────────────────────────────────────────────────────
 */

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <NimBLEDevice.h>
#include <WiFi.h>
#include <ArduinoOTA.h>

// ── Konfiguration ─────────────────────────────────────────────────────────────
#define WIFI_SSID       "DEIN_WLAN_NAME"   // ← anpassen
#define WIFI_PASSWORD   "DEIN_WLAN_PASSWORT" // ← anpassen
#define OTA_HOSTNAME    "temp_bridge"
#define OTA_PASSWORD    "ota1234"           // ← anpassen

#define ONE_WIRE_PIN    1        // DS18B20 Datenleitung
#define PIN_SENSOR_VCC  0        // OUTPUT HIGH → 3.3 V für Sensor (Strapping-Pin: MUSS HIGH bleiben!)
#define PIN_SENSOR_GND  2        // OUTPUT LOW  → GND für Sensor
#define UPDATE_MS       3000     // Mess- + BLE-Update-Intervall [ms]
#define PIN_LED           8      // Onboard-LED (blau, active LOW)
// ─────────────────────────────────────────────────────────────────────────────

OneWire           oneWire(ONE_WIRE_PIN);
DallasTemperature ds18b20(&oneWire);

NimBLEAdvertising* pAdv = nullptr;

// ── Setup ──────────────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  delay(300);

  Serial.println("\n╔══════════════════════════════════════╗");
  Serial.println("║  DS18B20 → BLE-Bridge   (ESP32-C3)  ║");
  Serial.println("╚══════════════════════════════════════╝");
  Serial.printf("  1-Wire  : GPIO%d\n",       ONE_WIRE_PIN);
  Serial.printf("  Sens-VCC: GPIO%d (HIGH)\n", PIN_SENSOR_VCC);
  Serial.printf("  Sens-GND: GPIO%d (LOW)\n",  PIN_SENSOR_GND);
  Serial.printf("  Intervall: %d ms\n\n",     UPDATE_MS);

  // Sensor-Versorgung per GPIO
  pinMode(PIN_SENSOR_GND, OUTPUT);
  digitalWrite(PIN_SENSOR_GND, LOW);
  pinMode(PIN_SENSOR_VCC, OUTPUT);
  digitalWrite(PIN_SENSOR_VCC, HIGH);
  delay(10);  // Sensor hochlaufen lassen

  // Onboard-LED (active LOW → HIGH = aus)
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, HIGH);

  // 1-Wire initialisieren
  ds18b20.begin();
  ds18b20.setResolution(12);   // 12-bit → 0.0625 °C Auflösung

  int devCount = ds18b20.getDeviceCount();
  if (devCount == 0) {
    Serial.println("  WARNUNG: Kein DS18B20 gefunden – Pull-Up prüfen!");
  } else {
    Serial.printf("  DS18B20 gefunden: %d Sensor(en)\n", devCount);
    DeviceAddress addr;
    if (ds18b20.getAddress(addr, 0)) {
      Serial.printf("  ROM-Code: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\n",
                    addr[0], addr[1], addr[2], addr[3],
                    addr[4], addr[5], addr[6], addr[7]);
    }
  }

  // BLE Advertising (BTHome v2, Non-Connectable)
  NimBLEDevice::init("temp_bridge");
  pAdv = NimBLEDevice::getAdvertising();
  pAdv->setConnectableMode(0);         // 0 = BLE_GAP_CONN_MODE_NON (Non-Connectable)
  pAdv->setMinInterval(800);   // 500 ms × 1/0.625 = 800 Einheiten
  pAdv->setMaxInterval(800);
  pAdv->start();
  Serial.printf("  BLE MAC : %s\n", NimBLEDevice::getAddress().toString().c_str());
  Serial.println("  BLE     : BTHome v2 aktiv (UUID 0xFCD2)\n");

  // WiFi + OTA
  Serial.printf("  WiFi: verbinde mit '%s' …", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  uint8_t wifiTries = 0;
  while (WiFi.status() != WL_CONNECTED && wifiTries < 30) {
    delay(500);
    Serial.print(".");
    wifiTries++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf(" OK  IP: %s\n", WiFi.localIP().toString().c_str());
  } else {
    Serial.println(" FEHLER (kein OTA verfügbar, Slave läuft trotzdem)");
  }

  ArduinoOTA.setHostname(OTA_HOSTNAME);
  ArduinoOTA.setPassword(OTA_PASSWORD);
  ArduinoOTA.onStart([]() {
    Serial.println("  [OTA] Start – bitte warten …");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("  [OTA] Fertig – Reboot.");
  });
  ArduinoOTA.onError([](ota_error_t e) {
    Serial.printf("  [OTA] Fehler [%u]\n", e);
  });
  ArduinoOTA.begin();
  Serial.printf("  OTA  : aktiv als '%s'\n\n", OTA_HOSTNAME);
}

// ── Loop ───────────────────────────────────────────────────────────────────────
static uint32_t lastMs = 0;

void loop() {
  ArduinoOTA.handle();

  if (millis() - lastMs < UPDATE_MS) return;
  lastMs = millis();

  ds18b20.requestTemperatures();
  float t = ds18b20.getTempCByIndex(0);

  // 85.0 °C = Power-On-Reset-Wert → als Fehler behandeln
  if (t == DEVICE_DISCONNECTED_C || t == 85.0f) {
    Serial.println("  [ERR] Sensor nicht erreichbar oder Power-on-Default");
    return;
  }

  // BTHome v2 Advertisement aktualisieren
  int16_t ble_raw = (int16_t)roundf(t * 100.0f);  // 0.01 °C Auflösung
  uint8_t svcData[4] = {
    0x40,                              // BTHome v2, kein Encryption
    0x02,                              // Object-ID: Temperature
    (uint8_t)(ble_raw & 0xFF),         // LSB
    (uint8_t)((ble_raw >> 8) & 0xFF),  // MSB
  };
  NimBLEAdvertisementData advData;
  advData.setFlags(0x06);
  advData.setName("temp_bridge");
  advData.setServiceData(NimBLEUUID((uint16_t)0xFCD2),
                         std::string((char*)svcData, sizeof(svcData)));
  pAdv->setAdvertisementData(advData);
  pAdv->stop();
  pAdv->start();

  // 2× kurz blinken (active LOW)
  for (int i = 0; i < 2; i++) {
    digitalWrite(PIN_LED, LOW);  delay(40);
    digitalWrite(PIN_LED, HIGH); delay(80);
  }

  Serial.printf("  Temp: %+7.4f °C   BLE raw=%d (BTHome 0x%02X 0x%02X)\n",
                t, ble_raw,
                (uint8_t)(ble_raw & 0xFF),
                (uint8_t)((ble_raw >> 8) & 0xFF));
}
