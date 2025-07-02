#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#define SS_PIN 15  // SDA pin (GPIO15)
#define RST_PIN 0  // RST pin (GPIO0)
#define SERVO_PIN 2 // Servo signal pin (GPIO2)

// Pin untuk sensor IR
#define IR_PIN_1 5  // GPIO5 (D1)
#define IR_PIN_2 4  // GPIO4 (D2)
#define IR_PIN_3 16 // GPIO16 (D0)

MFRC522 mfrc522(SS_PIN, RST_PIN); // Buat instance MFRC522
Servo gateServo; // Buat instance Servo
ESP8266WebServer server(80); // Buat instance web server

// Daftar UID yang valid
String validUIDs[] = {
    "0409220afc6e80", // UID pertama
    "f6cc8d02",       // UID kedua
    "03b45716",       // UID ketiga
    "2356da0c",        // UID keempat
    "04594872fd6e80"
};
const int numValidUIDs = 5; // Jumlah UID yang valid

// Status slot parkir
bool slotStatus[] = {false, false, false}; // false = kosong, true = terisi

void setup() {
    Serial.begin(115200); // Inisialisasi Serial Monitor
    SPI.begin();           // Inisialisasi SPI
    mfrc522.PCD_Init();    // Inisialisasi MFRC522
    gateServo.attach(SERVO_PIN); // Inisialisasi Servo
    gateServo.write(90); // Set servo ke posisi awal (90 derajat, gerbang tertutup)

    // Inisialisasi pin sensor IR
    pinMode(IR_PIN_1, INPUT);
    pinMode(IR_PIN_2, INPUT);
    pinMode(IR_PIN_3, INPUT);

    // Buat access point
    WiFi.softAP("Parkir_System", "");
    Serial.println("Access Point Started");
    Serial.print("IP Address: ");
    Serial.println(WiFi.softAPIP());

    // Inisialisasi web server
    server.on("/", handleRoot); // Tampilkan halaman utama
    server.on("/status", handleStatus); // Endpoint untuk mendapatkan status parkir
    server.begin();
    Serial.println("HTTP Server Started");

    Serial.println("Tempelkan kartu RFID Anda ke reader...");
}

void loop() {
    server.handleClient(); // Handle request web server

    // Perbarui status slot parkir
    slotStatus[0] = digitalRead(IR_PIN_1) == LOW; // LOW = terisi
    slotStatus[1] = digitalRead(IR_PIN_2) == LOW;
    slotStatus[2] = digitalRead(IR_PIN_3) == LOW;

    // Cek apakah ada kartu baru yang terdeteksi
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
        // Tampilkan UID di Serial Monitor
        Serial.print("UID tag: ");
        String uid = "";
        for (byte i = 0; i < mfrc522.uid.size; i++) {
            uid += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
            uid += String(mfrc522.uid.uidByte[i], HEX);
            Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
            Serial.print(mfrc522.uid.uidByte[i], HEX);
        }
        Serial.println();
        Serial.println("UID: " + uid);

        // Cek apakah UID sesuai dengan yang diinginkan
        bool accessGranted = false;
        for (int i = 0; i < numValidUIDs; i++) {
            if (uid == validUIDs[i]) {
                accessGranted = true;
                break;
            }
        }

        if (accessGranted) {
            // Cek apakah parkir penuh
            if (slotStatus[0] && slotStatus[1] && slotStatus[2]) {
                Serial.println("Parkir Penuh, Gerbang Tidak Dibuka!");
            } else {
                Serial.println("UID Sesuai, Membuka Gerbang...");
                openGate(); // Panggil fungsi untuk membuka gerbang
            }
        } else {
            Serial.println("UID Tidak Dikenal, Akses Ditolak!");
        }

        // Hentikan pembacaan kartu
        mfrc522.PICC_HaltA();
        mfrc522.PCD_StopCrypto1();
    }
}

// Fungsi untuk membuka gerbang
void openGate() {
    gateServo.write(0); // Gerakkan servo ke posisi 0 derajat (membuka gerbang)
    Serial.println("Gerbang Terbuka");
    delay(5000); // Tunggu 3 detik
    closeGate(); // Tutup gerbang setelah 3 detik
}

// Fungsi untuk menutup gerbang
void closeGate() {
    gateServo.write(150); // Gerakkan servo kembali ke posisi 90 derajat (menutup gerbang)
    Serial.println("Gerbang Tertutup");
}

// Fungsi untuk menangani halaman utama web server
void handleRoot() {
    String html = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Status Parkir</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f4f4f9;
            color: #333;
            margin: 0;
            padding: 0;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            flex-direction: column;
        }
        h1 {
            color: #4CAF50;
        }
        table {
            border-collapse: collapse;
            width: 80%;
            max-width: 600px;
            background-color: #fff;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
            border-radius: 8px;
            overflow: hidden;
        }
        th, td {
            padding: 12px;
            text-align: center;
        }
        th {
            background-color: #4CAF50;
            color: white;
        }
        tr:nth-child(even) {
            background-color: #f9f9f9;
        }
        tr:hover {
            background-color: #f1f1f1;
        }
        .status {
            font-weight: bold;
        }
        .status.terisi {
            color: #e74c3c;
        }
        .status.kosong {
            color: #2ecc71;
        }
        .notification {
            margin-top: 20px;
            padding: 10px;
            background-color: #ffeb3b;
            border-radius: 5px;
            display: none; /* Sembunyikan secara default */
        }
    </style>
    <script>
        async function updateStatus() {
            const response = await fetch("/status");
            const data = await response.json();
            document.getElementById("slot1").textContent = data.slot1 ? "Terisi" : "Kosong";
            document.getElementById("slot2").textContent = data.slot2 ? "Terisi" : "Kosong";
            document.getElementById("slot3").textContent = data.slot3 ? "Terisi" : "Kosong";

            // Update class for styling
            document.getElementById("slot1").className = "status " + (data.slot1 ? "terisi" : "kosong");
            document.getElementById("slot2").className = "status " + (data.slot2 ? "terisi" : "kosong");
            document.getElementById("slot3").className = "status " + (data.slot3 ? "terisi" : "kosong");

            // Tampilkan notifikasi jika parkir penuh
            if (data.slot1 && data.slot2 && data.slot3) {
                document.getElementById("notification").style.display = "block";
            } else {
                document.getElementById("notification").style.display = "none";
            }
        }

        setInterval(updateStatus, 1000); // Update setiap 1 detik
    </script>
</head>
<body>
    <h1>Status Parkir</h1>
    <table>
        <tr>
            <th>Slot</th>
            <th>Status</th>
        </tr>
        <tr>
            <td>Slot 1</td>
            <td id="slot1" class="status">Loading...</td>
        </tr>
        <tr>
            <td>Slot 2</td>
            <td id="slot2" class="status">Loading...</td>
        </tr>
        <tr>
            <td>Slot 3</td>
            <td id="slot3" class="status">Loading...</td>
        </tr>
    </table>
    <div id="notification" class="notification">Parkir Penuh!</div>
</body>
</html>
)=====";
    server.send(200, "text/html", html);
}

// Fungsi untuk menangani request status parkir
void handleStatus() {
    String json = "{";
    json += "\"slot1\":" + String(slotStatus[0] ? "true" : "false") + ",";
    json += "\"slot2\":" + String(slotStatus[1] ? "true" : "false") + ",";
    json += "\"slot3\":" + String(slotStatus[2] ? "true" : "false") + ",";
    json += "\"parkirPenuh\":" + String(slotStatus[0] && slotStatus[1] && slotStatus[2] ? "true" : "false");
    json += "}";
    server.send(200, "application/json", json);
}