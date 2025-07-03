// SmartDesktopAssistant.ino (REVISI PADA BAGIAN LOOP UNTUK HARDCODE KOORDINAT)

#include <WiFi.h> // For ESP32, use <WiFi.h> instead of <ESP8266WiFi.h>
#include "wifi_manager.h"
#include "ntp_time.h"
#include "weather_api.h"
#include "calendar_api.h"
#include "display_manager.h" // Mengacu pada display_manager.h yang dimodifikasi

// Timer tracking
unsigned long lastTimeUpdate = 0;
unsigned long lastWeatherUpdate = 0;
unsigned long lastCalendarUpdate = 0;
unsigned long lastTouchCheck = 0;
const unsigned long touchCheckInterval = 50; // Periksa sentuhan setiap 50ms

extern ScreenState currentScreen;

// Define TFT and Touch Screen Pins for ESP32
// Note: Some pins might be shared (e.g., MOSI, SCK, MISO) if your setup uses SPI for both.
// TFT Pins (Using VSPI - typically GPIO 18, 19, 23 for SCK, MISO, MOSI)
#define TFT_CS   5
#define TFT_RST  18
#define TFT_DC   19
#define TFT_MOSI 23 // This is actually the SDA/SDI pin on some displays, usually GPIO23 on ESP32
#define TFT_SCK  18 // This is actually the SCL/SCK pin, usually GPIO18 on ESP32
// TFT_LED is usually connected to 3.3V directly or via a resistor, no GPIO needed
// TFT_MISO is typically GPIO19 on ESP32, but for display output, it's often not used.
// If your display module explicitly uses MISO, ensure it's defined and used in display_manager.h

// Touch Screen Pins (XPT2046 based, typically uses SPI)
#define T_CLK    18 // SCK - Shared with TFT_SCK if using same SPI bus
#define T_CS     21
#define T_DIN    23 // MOSI - Shared with TFT_MOSI if using same SPI bus
#define T_OUT    19 // MISO - Shared with TFT_MISO if using same SPI bus

void setup() {
  Serial.begin(115200);
  delay(1000);

  // You will need to adapt initWiFi, initDisplay, getTouchPoint, updateDisplay
  // and other display-related functions in their respective files (e.g., wifi_manager.h, display_manager.h)
  // to use ESP32 specific libraries and pin definitions.
  // For display, typically Adafruit_GFX and a specific display driver (e.g., TFT_eSPI) are used.
  // For touch, you might use XPT2046_Touchscreen library.

  // The initDisplay function in display_manager.h will need to be updated with the new pin definitions.
  // Example for TFT_eSPI (you'll need to configure User_Setup.h or pass pins directly):
  // tft.begin();
  // tft.setRotation(desired_rotation); // Set your display rotation

  initWiFi("kepin", "kepinsukawarnaputih"); // Make sure this function uses ESP32 WiFi library
  initDisplay(); // This function needs to be updated for ESP32 and new pins

  syncNTPTime(); // Ensure this is compatible with ESP32 NTP libraries

  getWeatherData(); // Ensure underlying HTTP client is ESP32 compatible
  getCalendarSchedule(); // Ensure underlying HTTP client is ESP32 compatible

  currentScreen = SCREEN_MAIN;
  String initialTime = getFormattedTime();
  updateDisplay(initialTime); // This function needs to be updated for ESP32

  lastWeatherUpdate = millis();
  lastCalendarUpdate = millis();
  lastTimeUpdate = millis();

  Serial.println("\n--- Smart Desktop Assistant Started ---");
  Serial.print("Initial screen state: "); Serial.println(currentScreen == SCREEN_MAIN ? "MAIN" : "UNKNOWN");
  Serial.print("Display width: "); Serial.println(tft.width());
  Serial.print("Display height: "); Serial.println(tft.height());

  // Debugging koordinat tombol (akan dicetak sekali di awal)
  Serial.println("Visual Button Coords (at screen bottom):");
  Serial.print("Weather Button: X=["); Serial.print(WEATHER_BUTTON_X); Serial.print(" to "); Serial.print(WEATHER_BUTTON_X + WEATHER_BUTTON_W);
  Serial.print("], Y=["); Serial.print(WEATHER_BUTTON_Y_VISUAL); Serial.print(" to "); Serial.print(WEATHER_BUTTON_Y_VISUAL + WEATHER_BUTTON_H); Serial.println("]");

  Serial.print("Calendar Button: X=["); Serial.print(CALENDAR_BUTTON_X); Serial.print(" to "); Serial.print(CALENDAR_BUTTON_X + CALENDAR_BUTTON_W);
  Serial.print("], Y=["); Serial.print(CALENDAR_BUTTON_Y_VISUAL); Serial.print(" to "); Serial.print(CALENDAR_BUTTON_Y_VISUAL + CALENDAR_BUTTON_H); Serial.println("]");

  Serial.print("Back Button: X=["); Serial.print(BACK_BUTTON_X); Serial.print(" to "); Serial.print(BACK_BUTTON_X + BACK_BUTTON_W);
  Serial.print("], Y=["); Serial.print(BACK_BUTTON_Y); Serial.print(" to "); Serial.print(BACK_BUTTON_Y + BACK_BUTTON_H); Serial.println("]");

  // Informasi koordinat deteksi yang akan digunakan (ini yang di-hardcode)
  Serial.println("\n--- HARDCODED TOUCH DETECTION RANGES ---");
  Serial.println("Weather Touch Area: X=[5 to 155], Y=[64 to 98] (around your reported MAPPED Y=81)");
  Serial.println("Calendar Touch Area: X=[165 to 315], Y=[165 to 199] (around your reported MAPPED Y=182)");
  Serial.println("----------------------------------------");
}

void loop() {
  unsigned long currentMillis = millis();

  // --- Pembacaan Input Sentuh ---
  if (currentMillis - lastTouchCheck >= touchCheckInterval) {
    lastTouchCheck = currentMillis;
    int touchX, touchY;

    // The getTouchPoint() function in display_manager.h needs to be updated for ESP32 and new pins
    if (getTouchPoint(touchX, touchY)) {
      // Serial.println() debugging koordinat ada di getTouchPoint() sekarang.

      if (currentScreen == SCREEN_MAIN) {
        // Cek sentuhan pada tombol "Detail Cuaca" (menggunakan hardcoded koordinat Anda)
        // X = WEATHER_BUTTON_X (5) sampai (WEATHER_BUTTON_X + BUTTON_WIDTH) (155)
        // Y = Sekitar Y=81. Kita gunakan range Y=81 +/- BUTTON_HEIGHT/2 (misal 35/2 = 17)
        // Jadi, Y range sekitar 81-17 = 64 sampai 81+17 = 98
        if (touchX >= 5 && touchX <= (5 + BUTTON_WIDTH) && // X range of visual button
            touchY >= (81 - (BUTTON_HEIGHT / 2)) && touchY <= (81 + (BUTTON_HEIGHT / 2))) { // Y range around reported 81
          Serial.println(">>> Tombol Cuaca Ditekan! Navigating to Weather Detail...");
          currentScreen = SCREEN_WEATHER_DETAIL;
          displayWeatherDetail(getDetailedWeatherInfo()); // This function needs to be updated for ESP32
          delay(300);
        }
        // Cek sentuhan pada tombol "Detail Kalender" (menggunakan hardcoded koordinat Anda)
        // X = CALENDAR_BUTTON_X (165) sampai (CALENDAR_BUTTON_X + BUTTON_WIDTH) (315)
        // Y = Sekitar Y=182. Kita gunakan range Y=182 +/- BUTTON_HEIGHT/2 (misal 35/2 = 17)
        // Jadi, Y range sekitar 182-17 = 165 sampai 182+17 = 199
        else if (touchX >= (tft.width() - BUTTON_WIDTH - 5) && touchX <= (tft.width() - 5) && // X range of visual button
                 touchY >= (182 - (BUTTON_HEIGHT / 2)) && touchY <= (182 + (BUTTON_HEIGHT / 2))) { // Y range around reported 182
          Serial.println(">>> Tombol Kalender Ditekan! Navigating to Calendar Detail...");
          currentScreen = SCREEN_CALENDAR_DETAIL;
          displayCalendarDetail(getDetailedCalendarInfo()); // This function needs to be updated for ESP32
          delay(300);
        }
      } else { // Jika berada di layar detail (Cuaca atau Kalender)
        // Cek sentuhan pada tombol "Kembali" (posisi tetap, karena umumnya ini akurat)
        if (touchX >= BACK_BUTTON_X && touchX <= (BACK_BUTTON_X + BACK_BUTTON_W) &&
            touchY >= BACK_BUTTON_Y && touchY <= (BACK_BUTTON_Y + BACK_BUTTON_H)) {
          Serial.println(">>> Tombol Kembali Ditekan! Returning to Main Screen...");
          currentScreen = SCREEN_MAIN;
          String currentTimeStr = getFormattedTime();
          updateDisplay(currentTimeStr); // This function needs to be updated for ESP32
          delay(300);
        }
      }
    }
  }

  // --- Logika Update Data dan Tampilan (tetap sama) ---
  if (currentMillis - lastWeatherUpdate >= 60000) {
    lastWeatherUpdate = currentMillis;
    Serial.println("[MainLoop] Updating weather data...");
    getWeatherData();
    if (currentScreen == SCREEN_WEATHER_DETAIL) {
      displayWeatherDetail(getDetailedWeatherInfo());
    }
  }

  if (currentMillis - lastCalendarUpdate >= 300000) {
    lastCalendarUpdate = currentMillis;
    Serial.println("[MainLoop] Updating calendar data...");
    getCalendarSchedule();
    if (currentScreen == SCREEN_CALENDAR_DETAIL) {
      displayCalendarDetail(getDetailedCalendarInfo());
    }
  }

  if (currentScreen == SCREEN_MAIN && currentMillis - lastTimeUpdate >= 1000) {
    lastTimeUpdate = currentMillis;
    String currentTimeStr = getFormattedTime();
    updateDisplay(currentTimeStr);
  }
}