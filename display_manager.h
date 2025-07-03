// display_manager.h (VERSI REVISI: Hapus _Y_DETECT)

#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>

extern TFT_eSPI tft;
extern XPT2046_Touchscreen touch;

#ifndef TOUCH_CS
#define TOUCH_CS 15 // Ganti dengan pin CS (Chip Select) Touchscreen Anda
#endif
#ifndef TOUCH_IRQ
#define TOUCH_IRQ -1 // Ganti dengan pin IRQ Touchscreen Anda (-1 jika tidak digunakan)
#endif

extern String _prevWeatherStr;
extern String _prevCalendarStr;
extern String _prevTimeStr;
extern bool _initialDisplay;

enum ScreenState {
  SCREEN_MAIN,
  SCREEN_WEATHER_DETAIL,
  SCREEN_CALENDAR_DETAIL
};

extern ScreenState currentScreen;

// --- Definisi Posisi dan Ukuran Elemen UI ---
// Waktu (Akan menempati sebagian besar area layar)
#define TIME_FONT_SIZE 4
#define TIME_RECT_W tft.width()
#define TIME_RECT_H (tft.height() - BUTTON_HEIGHT - 10)

// --- Definisi Tombol UI di Layar Utama (Visualnya di bawah, side-by-side) ---
#define BUTTON_WIDTH ((tft.width() / 2) - 10)
#define BUTTON_HEIGHT 35 // Tinggi tombol

// Tombol Cuaca Detail (Gunakan ini untuk menggambar tombol)
#define WEATHER_BUTTON_X 5
#define WEATHER_BUTTON_Y_VISUAL (tft.height() - BUTTON_HEIGHT - 5) // Y visual untuk menggambar
#define WEATHER_BUTTON_W BUTTON_WIDTH
#define WEATHER_BUTTON_H BUTTON_HEIGHT
#define WEATHER_BUTTON_TEXT "Cuaca"
#define WEATHER_BUTTON_FONT_SIZE 1

// Tombol Kalender Detail (Gunakan ini untuk menggambar tombol)
#define CALENDAR_BUTTON_X (tft.width() - BUTTON_WIDTH - 5)
#define CALENDAR_BUTTON_Y_VISUAL (tft.height() - BUTTON_HEIGHT - 5) // Y visual untuk menggambar
#define CALENDAR_BUTTON_W BUTTON_WIDTH
#define CALENDAR_BUTTON_H BUTTON_HEIGHT
#define CALENDAR_BUTTON_TEXT "Kalender"
#define CALENDAR_BUTTON_FONT_SIZE 1

// --- Definisi Tombol Kembali (di layar detail) ---
#define BACK_BUTTON_W 80
#define BACK_BUTTON_H 35
#define BACK_BUTTON_X (tft.width() - BACK_BUTTON_W - 5)
#define BACK_BUTTON_Y 5
#define BACK_BUTTON_TEXT "Kembali"
#define BACK_BUTTON_FONT_SIZE 1

// Deklarasi fungsi-fungsi manajemen display
void initDisplay();
void updateDisplay(const String& timeStr, const String& weatherStr = "", const String& calendarStr = "");
bool getTouchPoint(int &x, int &y);
void displayWeatherDetail(const String& weatherDetail);
void displayCalendarDetail(const String& calendarDetail);

#endif