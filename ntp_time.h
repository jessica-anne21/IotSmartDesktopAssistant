// ntp_time.h (VERSI DIPERBAIKI)

#ifndef NTP_TIME_H
#define NTP_TIME_H

#include <NTPClient.h>
#include <WiFiUdp.h>

// Deklarasikan objek sebagai extern
// Ini memberitahu kompiler bahwa objek ini didefinisikan di tempat lain (di ntp_time.cpp)
extern WiFiUDP ntpUDP;
extern NTPClient timeClient;

// Deklarasi fungsi (tetap sama)
void syncNTPTime();
String getFormattedTime();

#endif