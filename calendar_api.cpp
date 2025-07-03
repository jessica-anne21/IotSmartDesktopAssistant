// calendar_api.cpp (FILE BARU ATAU MODIFIKASI DARI ISI calendar_api.h LAMA)

#include "calendar_api.h" // Harus menyertakan header file ini

// Definisi variabel global yang dideklarasikan sebagai extern di header
// Ini adalah satu-satunya tempat variabel ini didefinisikan secara aktual
String _currentCalendarSummary = "Tidak ada jadwal";
String _detailedCalendarInfo = "Tidak ada detail jadwal";

void getCalendarSchedule() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("[Calendar] Connecting to API...");
    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
    client->setInsecure();

    HTTPClient http;
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

    String url = "https://script.google.com/macros/s/AKfycbxpKisSHvMKdNLG3hJdGOQCeS9EXKs0Gq2QMAvP-6CICcg-tpBfYe9uo1nQmsZIn0EJ/exec"; // URL GAS Anda

    Serial.print("[Calendar] Mencoba menghubungkan ke: ");
    Serial.println(url);

    if (http.begin(*client, url)) {
      Serial.println("[Calendar] HTTP client.begin() berhasil.");
      int httpCode = http.GET();

      Serial.printf("[Calendar] HTTP GET selesai, kode respons: %d\n", httpCode);

      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println("[Calendar] Respons Payload:");
        Serial.println(payload);
        Serial.printf("[Calendar] Panjang Payload: %d\n", payload.length());

        DynamicJsonDocument doc(2048); // Tingkatkan ukuran buffer JSON jika payload lebih besar
        DeserializationError error = deserializeJson(doc, payload);

        if (!error) {
          Serial.println("[Calendar] JSON berhasil di-deserialize.");
          if (doc.containsKey("status") && doc["status"].as<String>() == "success") {
            Serial.println("[Calendar] Status dari API adalah 'success'.");
            JsonArray events = doc["events"];

            if (!events.isNull()) {
              if (events.size() > 0) {
                String tempCalendarSummary = "";
                String tempCalendarDetail = ""; // Untuk menyimpan detail penuh
                int eventsToShowSummary = 0; // Batas acara untuk ringkasan (misal 1-2)
                int eventsToShowDetail = 0; // Batas acara untuk detail (misal 5)

                for (JsonObject event_item : events) {
                  // Batasi jumlah acara untuk ringkasan di layar utama
                  if (eventsToShowSummary < 2 && event_item.containsKey("title") && event_item.containsKey("start")) {
                     String title = event_item["title"].as<String>();
                     String startFull = event_item["start"].as<String>();
                     String startTime = startFull.substring(11, 16);

                     if (eventsToShowSummary > 0) {
                       tempCalendarSummary += "\n";
                     }
                     tempCalendarSummary += title + " (" + startTime + ")";
                     eventsToShowSummary++;
                  }

                  // Tambahkan semua acara atau batasi untuk detail jika terlalu banyak
                  if (eventsToShowDetail < 5 && event_item.containsKey("title") &&
                      event_item.containsKey("start") && event_item.containsKey("end") &&
                      event_item.containsKey("description") && event_item.containsKey("location")) {
                    String title = event_item["title"].as<String>();
                    String startFull = event_item["start"].as<String>();
                    String endFull = event_item["end"].as<String>();
                    String description = event_item["description"].as<String>();
                    String location = event_item["location"].as<String>();

                    if (eventsToShowDetail > 0) {
                      tempCalendarDetail += "\n---\n"; // Pemisah antar acara di detail
                    }
                    tempCalendarDetail += "Judul: " + title + "\n";
                    tempCalendarDetail += "Mulai: " + startFull + "\n";
                    tempCalendarDetail += "Selesai: " + endFull + "\n";
                    if (description.length() > 0) tempCalendarDetail += "Deskripsi: " + description + "\n";
                    if (location.length() > 0) tempCalendarDetail += "Lokasi: " + location + "\n";

                    eventsToShowDetail++;
                  } else if (eventsToShowDetail >= 5) {
                    // Berhenti memproses detail jika sudah mencapai batas
                    tempCalendarDetail += "\n...dan acara lainnya.";
                  }
                }

                if (eventsToShowSummary > 0) {
                  _currentCalendarSummary = tempCalendarSummary;
                } else {
                  _currentCalendarSummary = "Tidak ada jadwal";
                }

                if (eventsToShowDetail > 0) {
                  _detailedCalendarInfo = tempCalendarDetail;
                } else {
                  _detailedCalendarInfo = "Tidak ada detail jadwal.";
                }

                Serial.println("[Calendar] Ringkasan Acara: " + _currentCalendarSummary);
                Serial.println("[Calendar] Detail Acara: " + _detailedCalendarInfo);

              } else {
                Serial.println("[Calendar] Tidak ada acara ditemukan dalam JSON (array events kosong).");
                _currentCalendarSummary = "Tidak ada jadwal";
                _detailedCalendarInfo = "Tidak ada detail jadwal.";
              }
            } else {
              Serial.println("[Calendar] Field 'events' tidak ada atau null dalam JSON.");
              _currentCalendarSummary = "Struktur events tidak valid";
              _detailedCalendarInfo = "Struktur events tidak valid.";
            }
          } else {
            Serial.println("[Calendar] Status API bukan 'success' atau field 'status' tidak ada.");
            if(doc.containsKey("status")){
                Serial.print("[Calendar] doc[\"status\"]: ");
                Serial.println(doc["status"].as<String>());
            } else {
                Serial.println("[Calendar] Field 'status' tidak ditemukan dalam JSON.");
            }
            _currentCalendarSummary = "Error status API";
            _detailedCalendarInfo = "Error status API.";
          }
        } else {
          Serial.print(F("[Calendar] deserializeJson() gagal: "));
          Serial.println(error.f_str());
          Serial.println(F("[Calendar] Payload yang diterima (gagal parsing):"));
          Serial.println(payload);
          _currentCalendarSummary = "JSON Parse error";
          _detailedCalendarInfo = "JSON Parse error.";
        }
      } else {
        Serial.printf("[Calendar] Permintaan HTTP GET gagal, error: %s (HTTP code: %d)\n", http.errorToString(httpCode).c_str(), httpCode);
        _currentCalendarSummary = "HTTP error " + String(httpCode);
        _detailedCalendarInfo = "HTTP error " + String(httpCode) + " saat mengambil data kalender.";
      }
      http.end();
    } else {
      Serial.println("[Calendar] Koneksi HTTP gagal (http.begin() gagal).");
      _currentCalendarSummary = "HTTP begin error";
      _detailedCalendarInfo = "Koneksi HTTP ke API kalender gagal.";
    }
  } else {
    Serial.println("[Calendar] WiFi tidak terhubung.");
    _currentCalendarSummary = "WiFi disconnected";
    _detailedCalendarInfo = "WiFi tidak terhubung, tidak dapat mengambil jadwal kalender.";
  }
  Serial.println("[Calendar] calendarEvent (summary) diperbarui menjadi: " + _currentCalendarSummary);
}

String getCalendarInfo() {
  return _currentCalendarSummary;
}

String getDetailedCalendarInfo() {
  return _detailedCalendarInfo;
}