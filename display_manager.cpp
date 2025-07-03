// display_manager.cpp (VERSI REVISI LENGKAP)

#include "display_manager.h"
#include "weather_api.h"
#include "calendar_api.h"
#include <Arduino.h>

TFT_eSPI tft = TFT_eSPI();
XPT2046_Touchscreen touch(TOUCH_CS, TOUCH_IRQ);

String _prevWeatherStr = "";
String _prevCalendarStr = "";
String _prevTimeStr = "";
bool _initialDisplay = true;
ScreenState currentScreen = SCREEN_MAIN;

void initDisplay() {
  Serial.println("[Display] Initializing TFT...");
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);

  if (TOUCH_CS != -1) {
    touch.begin();
    touch.setRotation(tft.getRotation());
    Serial.println("[Display] Touchscreen initialized.");
  }
  _initialDisplay = true;
  Serial.println("[Display] TFT & Touch initialized successfully.");
}

void updateDisplay(const String& timeStr, const String& weatherStr /* ignored */, const String& calendarStr /* ignored */) {
  if (currentScreen != SCREEN_MAIN && !_initialDisplay) {
    return;
  }

  tft.fillScreen(TFT_BLACK);

  // --- Gambar Waktu di Tengah Layar ---
  tft.setTextSize(TIME_FONT_SIZE);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextDatum(MC_DATUM);

  int time_area_height = tft.height() - BUTTON_HEIGHT - 10;
  int time_area_width = tft.width();

  tft.drawString(timeStr, time_area_width / 2, time_area_height / 2);
  _prevTimeStr = timeStr;

  // Garis pemisah di atas tombol
  tft.drawLine(0, CALENDAR_BUTTON_Y_VISUAL - 2, tft.width(), CALENDAR_BUTTON_Y_VISUAL - 2, TFT_DARKGREY);


  // --- Gambar Tombol Cuaca Detail (Gunakan Y_VISUAL) ---
  tft.fillRect(WEATHER_BUTTON_X, WEATHER_BUTTON_Y_VISUAL, WEATHER_BUTTON_W, WEATHER_BUTTON_H, TFT_BLUE);
  tft.drawRect(WEATHER_BUTTON_X, WEATHER_BUTTON_Y_VISUAL, WEATHER_BUTTON_W, WEATHER_BUTTON_H, TFT_WHITE);
  tft.setTextSize(WEATHER_BUTTON_FONT_SIZE);
  tft.setTextColor(TFT_WHITE);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(WEATHER_BUTTON_TEXT, WEATHER_BUTTON_X + WEATHER_BUTTON_W / 2, WEATHER_BUTTON_Y_VISUAL + WEATHER_BUTTON_H / 2);

  // --- Gambar Tombol Kalender Detail (Gunakan Y_VISUAL) ---
  tft.fillRect(CALENDAR_BUTTON_X, CALENDAR_BUTTON_Y_VISUAL, CALENDAR_BUTTON_W, CALENDAR_BUTTON_H, tft.color565(255, 165, 0)); // ORANGE
  tft.drawRect(CALENDAR_BUTTON_X, CALENDAR_BUTTON_Y_VISUAL, CALENDAR_BUTTON_W, CALENDAR_BUTTON_H, TFT_WHITE);
  tft.setTextSize(CALENDAR_BUTTON_FONT_SIZE);
  tft.setTextColor(TFT_WHITE);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(CALENDAR_BUTTON_TEXT, CALENDAR_BUTTON_X + CALENDAR_BUTTON_W / 2, CALENDAR_BUTTON_Y_VISUAL + CALENDAR_BUTTON_H / 2);

  if (_initialDisplay) {
    _initialDisplay = false;
  }
}

// Fungsi getTouchPoint (TETAP SAMA, HARUS DIKALIBRASI AKURAT)
bool getTouchPoint(int &x, int &y) {
  if (TOUCH_CS != -1 && touch.touched()) {
    TS_Point p = touch.getPoint();

    Serial.print("RAW Touch (X,Y,Z): ("); Serial.print(p.x); Serial.print(", "); Serial.print(p.y); Serial.print(", "); Serial.print(p.z); Serial.println(")");

    // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>> PENTING! GANTI NILAI KALIBRASI INI DENGAN YANG ANDA DAPATKAN DARI SKETCH Touch_calibrate !!! <<<
    // Contoh kalibrasi disesuaikan dari data yang Anda berikan.
    // Jika Cuaca RAW (X=2491, Y=450) -> MAPPED (X=19, Y=81)
    // Jika Kalender RAW (X=1080, Y=488) -> MAPPED (X=22, Y=182)
    const int RAW_X_MIN_ESTIMATED = 400;   // Estimasi dari data Anda
    const int RAW_X_MAX_ESTIMATED = 3500;  // Estimasi dari data Anda
    const int RAW_Y_MIN_ESTIMATED = 400;   // Estimasi dari data Anda
    const int RAW_Y_MAX_ESTIMATED = 3500;  // Estimasi dari data Anda

    // MAPPING:
    // x layar (width) dari p.y (raw Y dari touch sensor)
    x = map(p.y, RAW_Y_MIN_ESTIMATED, RAW_Y_MAX_ESTIMATED, 0, tft.width());
    
    // y layar (height) dari p.x (raw X dari touch sensor), perhatikan urutan max ke min untuk membalik
    y = map(p.x, RAW_X_MAX_ESTIMATED, RAW_X_MIN_ESTIMATED, 0, tft.height()); 
    // ^^^^^ PASTIKAN INI SESUAI DENGAN PENGUJIAN KALIBRASI ANDA ^^^^^

    x = constrain(x, 0, tft.width() - 1);
    y = constrain(y, 0, tft.height() - 1);

    Serial.print("MAPPED Touch (X,Y): ("); Serial.print(x); Serial.print(", "); Serial.print(y); Serial.println(")");
    Serial.println("---");
    
    return true;
  }
  return false;
}

// Fungsi untuk menampilkan detail cuaca
void displayWeatherDetail(const String& weatherDetail) {
  tft.fillScreen(TFT_BLACK);

  tft.setTextSize(2);
  tft.setTextColor(TFT_CYAN);
  tft.setTextDatum(TL_DATUM);
  tft.setCursor(5, 5);
  tft.print("Detail Cuaca:");

  tft.fillRect(BACK_BUTTON_X, BACK_BUTTON_Y, BACK_BUTTON_W, BACK_BUTTON_H, TFT_RED);
  tft.drawRect(BACK_BUTTON_X, BACK_BUTTON_Y, BACK_BUTTON_W, BACK_BUTTON_H, TFT_WHITE);
  tft.setTextSize(BACK_BUTTON_FONT_SIZE);
  tft.setTextColor(TFT_WHITE);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(BACK_BUTTON_TEXT, BACK_BUTTON_X + BACK_BUTTON_W / 2, BACK_BUTTON_Y + BACK_BUTTON_H / 2);

  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setTextDatum(TL_DATUM);
  int y_pos = BACK_BUTTON_Y + BACK_BUTTON_H + 10;

  int start = 0;
  int end = weatherDetail.indexOf('\n', start);
  while (end != -1 && y_pos < tft.height() - 5) {
    tft.setCursor(5, y_pos);
    tft.print(weatherDetail.substring(start, end));
    start = end + 1;
    y_pos += tft.fontHeight() + 2;
    end = weatherDetail.indexOf('\n', start);
  }
  if (y_pos < tft.height() - 5) {
    tft.setCursor(5, y_pos);
    tft.print(weatherDetail.substring(start));
  }
}

// Fungsi untuk menampilkan detail kalender
void displayCalendarDetail(const String& calendarDetail) {
  tft.fillScreen(TFT_BLACK);

  tft.setTextSize(2);
  tft.setTextColor(TFT_CYAN);
  tft.setTextDatum(TL_DATUM);
  tft.setCursor(5, 5);
  tft.print("Detail Kalender:");

  tft.fillRect(BACK_BUTTON_X, BACK_BUTTON_Y, BACK_BUTTON_W, BACK_BUTTON_H, TFT_RED);
  tft.drawRect(BACK_BUTTON_X, BACK_BUTTON_Y, BACK_BUTTON_W, BACK_BUTTON_H, TFT_WHITE);
  tft.setTextSize(BACK_BUTTON_FONT_SIZE);
  tft.setTextColor(TFT_WHITE);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(BACK_BUTTON_TEXT, BACK_BUTTON_X + BACK_BUTTON_W / 2, BACK_BUTTON_Y + BACK_BUTTON_H / 2);

  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setTextDatum(TL_DATUM);
  int y_pos = BACK_BUTTON_Y + BACK_BUTTON_H + 10;

  int start = 0;
  int end = calendarDetail.indexOf('\n', start);
  while (end != -1 && y_pos < tft.height() - 5) {
    tft.setCursor(5, y_pos);
    tft.print(calendarDetail.substring(start, end));
    start = end + 1;
    y_pos += tft.fontHeight() + 2;
    end = calendarDetail.indexOf('\n', start);
  }
  if (y_pos < tft.height() - 5) {
    tft.setCursor(5, y_pos);
    tft.print(calendarDetail.substring(start));
  }
}