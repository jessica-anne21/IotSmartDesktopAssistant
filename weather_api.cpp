// weather_api.cpp (FILE BARU ATAU MODIFIKASI)

#include "weather_api.h" // Include header file ini

// Definisi variabel global yang dideklarasikan sebagai extern di header
String temperature = "??";
String condition = "??";
String _detailedWeatherInfo = "Tidak ada detail cuaca"; // Definisi untuk detail cuaca

// Implementasi fungsi interpretWeatherCode
String interpretWeatherCode(int code) {
  switch (code) {
    case 0: return "Cerah";
    case 1: return "Hampir Cerah";
    case 2: return "Sebagian Berawan";
    case 3: return "Berawan";
    case 45: case 48: return "Berkabut";
    case 51: case 53: case 55: return "Gerimis";
    case 61: case 63: case 65: return "Hujan";
    case 66: case 67: return "Hujan Beku";
    case 71: case 73: case 75: return "Salju";
    case 77: return "Butiran Salju";
    case 80: case 81: case 82: return "Hujan Lokal";
    case 85: case 86: return "Salju Lokal";
    case 95: return "Badai Petir";
    case 96: case 99: return "Badai Petir + Hujan Es";
    default: return "Tidak Diketahui";
  }
}

// Implementasi fungsi getWeatherData
void getWeatherData() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("[Weather] Connecting to API...");

    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
    client->setInsecure(); // Untuk HTTPS tanpa verifikasi sertifikat ketat

    HTTPClient http;
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

    String url = "https://api.open-meteo.com/v1/forecast?latitude=-6.9147&longitude=107.6098&current_weather=true&temperature_unit=celsius&windspeed_unit=ms&precipitation_unit=mm"; // Tambahkan unit untuk detail
    // Anda mungkin ingin menambahkan parameter lain seperti:
    // &hourly=temperature_2m,relativehumidity_2m,weathercode,precipitation,windspeed_10m
    // untuk mendapatkan data yang lebih kaya untuk detail cuaca

    if (http.begin(*client, url)) {
      int httpCode = http.GET();
      Serial.print("[Weather] HTTP Response Code: ");
      Serial.println(httpCode);

      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        DynamicJsonDocument doc(2048); // Tingkatkan ukuran buffer JSON jika Anda mengambil lebih banyak data

        DeserializationError error = deserializeJson(doc, payload);

        if (error) {
          Serial.print("[Weather] JSON Deserialization Error: ");
          Serial.println(error.c_str());
          temperature = "Error";
          condition = "Parse Err";
          _detailedWeatherInfo = "Error parsing weather data.";
        } else {
          if (doc.containsKey("current_weather") &&
              doc["current_weather"].is<JsonObject>()) {

            JsonObject currentWeather = doc["current_weather"];
            if (currentWeather.containsKey("temperature") &&
                currentWeather.containsKey("weathercode") &&
                currentWeather.containsKey("windspeed") && // Asumsi ini ada
                currentWeather.containsKey("winddirection")) { // Asumsi ini ada

              float temp_val = currentWeather["temperature"];
              int weathercode_val = currentWeather["weathercode"];
              float windspeed_val = currentWeather["windspeed"];
              int winddirection_val = currentWeather["winddirection"];

              temperature = String(temp_val, 1) + "°C";
              condition = interpretWeatherCode(weathercode_val);

              _detailedWeatherInfo = "Suhu: " + String(temp_val, 1) + "°C\n"
                                   + "Kondisi: " + interpretWeatherCode(weathercode_val) + "\n"
                                   + "Kecepatan Angin: " + String(windspeed_val, 1) + " m/s\n"
                                   + "Arah Angin: " + String(winddirection_val) + "°";
              // Anda bisa menambahkan kelembaban, tekanan, dll. jika Anda mengambilnya dari API

              Serial.println("[Weather] Temperature: " + temperature);
              Serial.println("[Weather] Condition: " + condition);
              Serial.println("[Weather] Detailed Info: " + _detailedWeatherInfo);
            } else {
              Serial.println("[Weather] Missing required data in current_weather JSON.");
              temperature = "Data Err";
              condition = "Missing";
              _detailedWeatherInfo = "Missing key weather data.";
            }
          } else {
            Serial.println("[Weather] Missing current_weather object in JSON.");
            temperature = "Struct Err";
            condition = "API";
            _detailedWeatherInfo = "API response structure error.";
          }
        }
      } else {
        Serial.println("[Weather] Failed to connect. HTTP error code: " + String(httpCode));
        temperature = "HTTP Err";
        condition = String(httpCode);
        _detailedWeatherInfo = "HTTP Error: " + String(httpCode);
      }
      http.end();
    } else {
      Serial.println("[Weather] HTTP client.begin() failed.");
      temperature = "Conn Err";
      condition = "Begin";
      _detailedWeatherInfo = "HTTP connection failed.";
    }
  } else {
    Serial.println("[Weather] WiFi not connected!");
    temperature = "No WiFi";
    condition = "";
    _detailedWeatherInfo = "Tidak ada koneksi WiFi.";
  }
}

// Implementasi fungsi getWeatherInfo
String getWeatherInfo() {
  if (temperature == "No WiFi" || temperature == "HTTP Err" || temperature == "Error" || temperature == "Data Err" || temperature == "Struct Err" || temperature == "Conn Err") {
    return temperature;
  }
  return temperature + " | " + condition;
}

// Implementasi fungsi getDetailedWeatherInfo
String getDetailedWeatherInfo() {
  return _detailedWeatherInfo;
}