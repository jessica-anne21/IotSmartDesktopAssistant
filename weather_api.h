// weather_api.h (VERSI DIPERBAIKI)

#ifndef WEATHER_API_H
#define WEATHER_API_H

#include <ESP8266WiFi.h>             // <--- TAMBAHKAN INI untuk WiFi.status()
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ArduinoJson.h>

// Deklarasikan variabel global sebagai extern di header
// Ini memberitahu kompiler bahwa variabel ini didefinisikan di tempat lain (di file .cpp)
extern String temperature;
extern String condition;
extern String _detailedWeatherInfo; // Tambahkan ini jika Anda punya variabel ini untuk detail

// Deklarasi fungsi
String interpretWeatherCode(int code);
void getWeatherData();
String getWeatherInfo();
String getDetailedWeatherInfo(); // Pastikan ini dideklarasikan untuk fitur detail

#endif