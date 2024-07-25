#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL69oAjFYLC"
#define BLYNK_TEMPLATE_NAME "MycoMonitor"
#define BLYNK_AUTH_TOKEN "VLbW9RagL11ZPeT1NSistW9OHYmgB-lu"

#include <BlynkSimpleEsp32.h>
#include <WiFi.h>
#include <Wire.h>
#include <DHT.h> // sensor suhu dan kelembaban

#define DHTPIN 4 // Pin data DHT22 terhubung ke pin Arduino 2
#define DHTTYPE DHT22 // Tipe sensor DHT22

DHT dht(DHTPIN, DHTTYPE); // Membuat objek DHT

#define PIRPIN 5 // PIR sensor pin di ESP32
#define LED_TEMP 13
#define LED_MOTION 12

int motion = LOW; // declare motion variable globally

const char* ssid = "BIMAYURAISYAH"; // nama wifi
const char* password = "rinisukemi"; // pass wifi

void setup() {
  // Initialize serial communication at 115200 baud rate
  Serial.begin(115200);

  // Initialize Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);

  // Initialize DHT sensor
  dht.begin();

  // Initialize PIR sensor
  pinMode(PIRPIN, INPUT);

  // Initialize LEDs
  pinMode(LED_TEMP, OUTPUT);
  pinMode(LED_MOTION, OUTPUT);
  digitalWrite(LED_TEMP, LOW); // Matikan lampu suhu secara default
  digitalWrite(LED_MOTION, LOW); // Matikan lampu gerakan secara default

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

  // Kontrol LED berdasarkan suhu
  if (temperature < 24 || temperature > 27) {
    digitalWrite(LED_TEMP, HIGH); // Nyalakan lampu suhu
    Blynk.virtualWrite(V4, 1); // Kirim status lampu suhu ke pin virtual V4
  } else {
    digitalWrite(LED_TEMP, LOW); // Matikan lampu suhu
    Blynk.virtualWrite(V4, 0); // Kirim status lampu suhu ke pin virtual V4
  }

  // Add a delay filter to reduce false positives
  if (motion == HIGH) {
    delay(100); // Tunggu 100 milidetik untuk konfirmasi gerakan
    if (digitalRead(PIRPIN) == HIGH) {
      digitalWrite(LED_MOTION, HIGH); // Nyalakan lampu gerakan
      Blynk.virtualWrite(V5, 1); // Kirim status lampu gerakan ke pin virtual V5
    } else {
      motion = LOW;
      digitalWrite(LED_MOTION, LOW); // Matikan lampu gerakan
      Blynk.virtualWrite(V5, 0); // Kirim status lampu gerakan ke pin virtual V5
    }
  } else {
    digitalWrite(LED_MOTION, LOW); // Matikan lampu gerakan
    Blynk.virtualWrite(V5, 0); // Kirim status lampu gerakan ke pin virtual V5
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
