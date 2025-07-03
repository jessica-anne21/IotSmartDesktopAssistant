// calendar_api.h (VERSI DIPERBAIKI)

#ifndef CALENDAR_API_H
#define CALENDAR_API_H

#include <ESP8266WiFi.h>             // <--- TAMBAHKAN INI untuk WiFi.status()
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ArduinoJson.h>

// Deklarasi variabel global sebagai extern
// Ini memberitahu kompiler bahwa variabel ini akan didefinisikan di file .cpp
extern String _currentCalendarSummary; // Untuk ringkasan di layar utama
extern String _detailedCalendarInfo;   // Untuk detail di halaman terpisah

// Deklarasi fungsi
void getCalendarSchedule();
String getCalendarInfo();            // Mengembalikan ringkasan
String getDetailedCalendarInfo();    // Mengembalikan detail

#endif