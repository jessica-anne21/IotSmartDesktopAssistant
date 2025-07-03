// ntp_time.cpp (FILE BARU)

#include "ntp_time.h" // Include header file ini
#include <Arduino.h>  // Sertakan jika ada fungsi Arduino dasar seperti Serial.println

// Definisi objek yang dideklarasikan sebagai extern di header
// Ini adalah satu-satunya tempat objek-objek ini didefinisikan secara aktual
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 7 * 3600, 60000); // GMT+7

void syncNTPTime() {
  Serial.println("[Time] Starting NTP client...");
  timeClient.begin();
  // NTPClient.update() dapat memblokir selama beberapa detik jika belum disinkronkan.
  // Pastikan WiFi sudah terkoneksi sebelum memanggil ini.
  timeClient.update();
  Serial.println("[Time] NTP time synced: " + timeClient.getFormattedTime());
}

String getFormattedTime() {
  // Update waktu setiap kali dipanggil, ini akan otomatis memeriksa interval update
  timeClient.update();
  String currentTime = timeClient.getFormattedTime();
  // Serial.println("[Time] Current Time: " + currentTime); // Opsional: Matikan ini untuk mengurangi spam serial
  return currentTime;
}