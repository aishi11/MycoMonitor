#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL69oAjFYLC"
#define BLYNK_TEMPLATE_NAME "MycoMonitor"
#define BLYNK_AUTH_TOKEN "VLbW9RagL11ZPeT1NSistW9OHYmgB-lu"

#include <BlynkSimpleEsp32.h>
#include <WiFi.h>
#include <Wire.h>
#include <DHT.h> // sensor suhu dan kelembaban

#define DHTPIN 4 // Pin data DHT22 terhubung ke pin Arduino 4
#define DHTTYPE DHT22 // Tipe sensor DHT22

DHT dht(DHTPIN, DHTTYPE); // Membuat objek DHT

#define PIRPIN 5 // PIR sensor pin di ESP32
#define LED_INDICATOR 13 // Pin LED untuk indikator

int motion = LOW; // declare motion variable globally

const char* ssid = "YOUR SSID"; // nama wifi
const char* password = "YOUR PASS"; // pass wifi

void setup() {
  // Initialize serial communication at 115200 baud rate
  Serial.begin(115200);

  // Initialize Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);

  // Initialize DHT sensor
  dht.begin();

  // Initialize PIR sensor
  pinMode(PIRPIN, INPUT);

  // Initialize LED
  pinMode(LED_INDICATOR, OUTPUT);
  digitalWrite(LED_INDICATOR, LOW); // Matikan lampu indikator secara default

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.println("Connected to Wi-Fi");
  Serial.print("Alamat IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Run Blynk
  Blynk.run();

  // Membaca data suhu dan kelembaban
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Cek jika pembacaan data valid
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Gagal membaca data!");
    return;
  }

  // Tampilkan data suhu dan kelembaban
  Serial.print("Suhu: ");
  Serial.print(temperature);
  Serial.print(" C, Kelembaban: ");
  Serial.print(humidity);
  Serial.println("%");
  
  // Read PIR sensor
  motion = digitalRead(PIRPIN);

  // Send data to Flask server via HTTP POST
  sendDataToServer(temperature, humidity, motion);

  // Kontrol LED indikator berdasarkan kondisi
  if (temperature < 24) {
    // Suhu rendah
    indicate(1);
    Blynk.virtualWrite(V4, "Low Temp");
  } else if (temperature > 27) {
    // Suhu tinggi
    indicate(2);
    Blynk.virtualWrite(V4, "High Temp");
  } else if (humidity < 70) {
    // Kelembapan rendah
    indicate(3);
    Blynk.virtualWrite(V4, "Low Humidity");
  } else if (humidity > 90) {
    // Kelembapan tinggi
    indicate(4);
    Blynk.virtualWrite(V4, "High Humidity");
  } else {
    // Kondisi normal
    digitalWrite(LED_INDICATOR, LOW);
    Blynk.virtualWrite(V4, "Normal");
  }

  // Send sensor data to Blynk
  Blynk.virtualWrite(V1, temperature); // Kirim data suhu ke pin virtual V1
  Blynk.virtualWrite(V2, humidity); // Kirim data kelembaban ke pin virtual V2
  Blynk.virtualWrite(V3, motion); // Kirim data gerakan ke pin virtual V3

  // Tampilkan data PIR di Serial Monitor
  Serial.print("Motion: ");
  Serial.println((motion == HIGH) ? "Detected" : "Not Detected");

  delay(2000); // Tunggu 2 detik sebelum membaca kembali
}

void sendDataToServer(float temperature, float humidity, int motion) {
  WiFiClient client;

  if (client.connect("192.168.100.12", 5000)) { // Ganti dengan alamat IP server Flask Anda
    String data = "temperature=" + String(temperature) +
                  "&humidity=" + String(humidity) +
                  "&motion=" + String(motion);
    
    client.println("POST /get_sensor_data HTTP/1.1");
    client.println("Host: 192.168.100.12:5000"); // Ganti dengan alamat IP server Flask Anda
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(data.length());
    client.println();
    client.print(data);
    client.println();
  }
}

void indicate(int condition) {
  switch (condition) {
    case 1:
      // Suhu rendah
      blinkLED(500); // LED berkedip 500 ms
      break;
    case 2:
      // Suhu tinggi
      blinkLED(1000); // LED berkedip 1000 ms
      break;
    case 3:
      // Kelembapan rendah
      blinkLED(1500); // LED berkedip 1500 ms
      break;
    case 4:
      // Kelembapan tinggi
      blinkLED(2000); // LED berkedip 2000 ms
      break;
  }
}

void blinkLED(int duration) {
  digitalWrite(LED_INDICATOR, HIGH); // LED ON
  delay(duration);
  digitalWrite(LED_INDICATOR, LOW);  // LED OFF
  delay(duration);
}
