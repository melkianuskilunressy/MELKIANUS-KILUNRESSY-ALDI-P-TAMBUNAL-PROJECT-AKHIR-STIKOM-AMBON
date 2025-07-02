# 🚗 Sistem Parkir Otomatis dengan RFID & ESP8266

Sebuah proyek sistem parkir otomatis berbasis **ESP8266** yang menggunakan **RFID** untuk otentikasi, **sensor inframerah** untuk deteksi ketersediaan slot, dan dilengkapi dengan **antarmuka web** untuk pemantauan real-time.

> ![Gambar Demo Sistem Parkir](#)  
> *(Disarankan untuk mengganti link di atas dengan GIF demo proyek Anda)*

---

## 📜 Deskripsi Proyek

Proyek ini dirancang sebagai solusi parkir cerdas yang efisien dan otomatis.  
Pengguna dengan kartu RFID yang telah terdaftar dapat masuk ke area parkir jika masih ada slot yang tersedia.  
Palang pintu akan terbuka secara otomatis setelah otentikasi berhasil.

Keunggulan utama dari sistem ini adalah kemampuannya untuk membuat Access Point (AP) Wi-Fi sendiri.  
Hal ini memungkinkan petugas atau pengguna untuk terhubung langsung ke sistem dan memantau status ketersediaan setiap slot parkir secara **real-time** melalui halaman web sederhana dari perangkat apa pun.

---

## ✨ Fitur Utama

- ✅ **Otentikasi RFID**: Akses masuk hanya untuk kartu RFID yang telah terdaftar.
- 🚘 **Deteksi Slot Parkir**: Menggunakan 3 sensor inframerah (IR) untuk deteksi kendaraan.
- ⚙️ **Kontrol Palang Otomatis**: Motor servo membuka/menutup palang secara otomatis.
- 🌐 **Monitoring Real-Time via Web**: Akses Wi-Fi lokal via ESP8266 (IP: `192.168.4.1`).
- ❌ **Notifikasi Parkir Penuh**: Sistem menolak akses jika slot penuh.

---

## 🛠️ Komponen yang Dibutuhkan

| Komponen             | Jumlah | Keterangan                            |
|----------------------|--------|----------------------------------------|
| ESP8266 (NodeMCU)    | 1      | Mikrokontroler utama                  |
| MFRC522 RFID Reader  | 1      | Untuk membaca kartu/tag RFID          |
| Kartu/Tag RFID       | 5+     | Frekuensi 13.56 MHz                   |
| Motor Servo          | 1      | SG90/MG90S untuk palang pintu         |
| Sensor Inframerah IR | 3      | Untuk mendeteksi kendaraan            |
| Kabel Jumper         | Secukupnya | Penghubung antar komponen          |
| Power Supply 5V      | 1      | Daya untuk sistem                     |

---

## 🔌 Skema dan Pengkabelan

| Komponen        | Pin Komponen | GPIO ESP8266 | Pin NodeMCU |
|-----------------|---------------|---------------|-------------|
| **MFRC522**     | SDA (SS)      | GPIO15        | D8          |
|                 | SCK           | GPIO14        | D5          |
|                 | MOSI          | GPIO13        | D7          |
|                 | MISO          | GPIO12        | D6          |
|                 | RST           | GPIO0         | D3          |
|                 | 3.3V          | 3.3V          | 3.3V        |
|                 | GND           | GND           | GND         |
| **Motor Servo** | Signal        | GPIO2         | D4          |
|                 | VCC           | 5V            | VIN         |
|                 | GND           | GND           | GND         |
| **IR Sensor x3**| OUT           | GPIO5,4,16    | D1,D2,D0     |
|                 | VCC           | 3.3V          | 3.3V        |
|                 | GND           | GND           | GND         |

> ![Gambar Skema Rangkaian](https://github.com/melkianuskilunressy/MELKIANUS-KILUNRESSY-ALDI-P-TAMBUNAL-PROJECT-AKHIR-STIKOM-AMBON/blob/1bb49743a4abaa350f433b64ff4dc6c849401f93/WhatsApp%20Image%202025-07-02%20at%2017.27.18_a0fafbe1.jpg)  
> *(Ganti link di atas dengan gambar wiring Anda)*

---

## 🚀 Instalasi dan Pengaturan

### 1. Clone Repositori

```bash
git clone https://github.com/username/nama-repositori.git
cd nama-repositori
