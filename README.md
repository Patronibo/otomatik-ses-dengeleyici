#  Ses Dengeleyici

Windows için otomatik ses seviyesi dengeleyici. Bilgisayardan çıkan seslerin seviyesini gerçek zamanlı olarak dengeler - yüksek sesleri düşürür, düşük sesleri yükseltir.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform](https://img.shields.io/badge/platform-Windows%2010%2B-blue)](https://www.microsoft.com/windows)
[![C++](https://img.shields.io/badge/C%2B%2B-17-00599C?logo=c%2B%2B)](https://isocpp.org/)

##  Özellikler

-  **Otomatik Ses Dengeleme**: Gerçek zamanlı ses seviyesi kontrolü
-  **Yüksek Sesleri Düşürür**: Ani yüksek sesler rahatsız etmez
-  **Düşük Sesleri Yükseltir**: Sessiz bölümleri daha iyi duyarsınız
-  **Arka Plan Çalışma**: System tray'de sessizce çalışır
-  **Düşük Kaynak Kullanımı**: %1-2 CPU, ~10 MB RAM
-  **Kolay Kontrol**: Çift tıklayarak açıp-kapatabilirsiniz

##  Ekran Görüntüsü

```
System Tray'de çalışan uygulama:
┌──────────────────────────────┐
│  Ses Dengeleyici - Aktif     │
│  Seviye: 45%                 │
│  Volume: 82%                 │
└──────────────────────────────┘
```

##  Kullanım Senaryoları

- **Gece Müzik Dinlerken**: Yüksek sesler uyanmanıza neden olmaz
- **Film İzlerken**: Diyaloglar net duyulur, patlama sesleri kontrollü
- **Oyun Oynarken**: Ses dengesizlikleri giderilir
- **Podcast Dinlerken**: Tüm konuşmacılar aynı seviyede

##  Kurulum

### Gereksinimler

- Windows 10 veya üzeri
- Visual Studio 2019/2022 (Community Edition yeterli)
- CMake 3.15+ (Visual Studio ile birlikte gelir)

### Derleme

#### Yöntem 1: Otomatik (Önerilen)

```cmd
build.bat
```

#### Yöntem 2: Manuel

**Developer Command Prompt** açın:

```cmd
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

### Çalıştırma

```cmd
build\bin\Release\SesDengeleyici.exe
```

##  Kullanım

1. Uygulamayı başlatın
2. System tray'de (saat yanında) icon görünecek
3. Uygulama otomatik olarak sesleri dengeleyecek

### Kontroller

- **Çift Tık (Tray Icon)**: Etkinleştir/Devre Dışı Bırak
- **Sağ Tık Menüsü**:
  - Etkinleştir/Devre Dışı Bırak
  - Hakkında
  - Çıkış

##  Ayarlar

`src/main.cpp` dosyasındaki parametreleri değiştirerek özelleştirebilirsiniz:

```cpp
compressor.SetThreshold(-20.0f);  // Compression başlangıç seviyesi (dB)
compressor.SetRatio(3.0f);        // Compression oranı
compressor.SetAttackTime(5.0f);   // Attack süresi (ms)
compressor.SetReleaseTime(50.0f); // Release süresi (ms)
compressor.SetTargetLevel(0.65f); // Hedef ses seviyesi (0.0-1.0)
```

### Örnek Profiller

**Yumuşak Dengeleme (Müzik):**
```cpp
SetThreshold(-25.0f);
SetRatio(2.0f);
SetTargetLevel(0.6f);
```

**Agresif Dengeleme (Film):**
```cpp
SetThreshold(-15.0f);
SetRatio(4.0f);
SetTargetLevel(0.7f);
```

##  Proje Yapısı

```
ses-dengeliyici/
├── include/              # Header dosyaları
│   ├── AudioCapture.h    # WASAPI ses yakalama
│   ├── AudioCompressor.h # Ses dengeleme algoritması
│   └── TrayIcon.h        # System tray icon
├── src/                  # Kaynak dosyaları
│   ├── main.cpp
│   ├── AudioCapture.cpp
│   ├── AudioCompressor.cpp
│   └── TrayIcon.cpp
├── resources/            # Resource dosyaları
│   └── app.rc
├── CMakeLists.txt        # Build yapılandırması
└── build.bat             # Hızlı derleme scripti
```

##  Teknik Detaylar

### Teknolojiler

- **C++17**: Modern C++ özellikleri
- **WASAPI**: Windows Audio Session API (Loopback mode)
- **Dynamic Range Compression**: Ses seviyesi kontrolü
- **RMS Level Detection**: Gerçek zamanlı ses analizi
- **Attack/Release Envelope**: Yumuşak geçişler

### Algoritma

1. **Ses Yakalama**: WASAPI loopback ile sistem sesini yakalar
2. **RMS Analizi**: Ses seviyesini hesaplar (Root Mean Square)
3. **Compression**: Yüksek sesleri compress eder
4. **Normalization**: Genel ses seviyesini hedef değerde tutar
5. **Smoothing**: Ani değişimleri yumuşatır

##  Performans

- **CPU Kullanımı**: %1-2
- **RAM Kullanımı**: ~10 MB
- **Gecikme**: < 10ms (fark edilmez)
- **Dosya Boyutu**: ~50-100 KB

##  Güvenlik

-  Modern C++ standartları
-  Type-safe kod
-  Memory-safe (RAII pattern)
-  Exception-safe
-  No external dependencies

##  Katkıda Bulunma

1. Fork yapın
2. Feature branch oluşturun (`git checkout -b feature/AmazingFeature`)
3. Değişikliklerinizi commit edin (`git commit -m 'Add some AmazingFeature'`)
4. Branch'inizi push edin (`git push origin feature/AmazingFeature`)
5. Pull Request açın

##  Lisans

Bu proje MIT lisansı altında lisanslanmıştır. Detaylar için [LICENSE](LICENSE) dosyasına bakın.

##  Sorun Bildirme

Sorun bulursanız veya öneriniz varsa [issue](../../issues) açabilirsiniz.

##  SSS

**S: Ses kalitesi düşüyor mu?**  
C: Hayır, sadece ses seviyesi ayarlanır, kalite korunur.

**S: Hangi programlarda çalışır?**  
C: Tüm Windows uygulamalarında (Spotify, YouTube, oyunlar, vb.)

**S: Sistem performansını etkiler mi?**  
C: Hayır, çok düşük kaynak kullanır (%1-2 CPU).

**S: Otomatik başlatabilir miyim?**  
C: Evet, `Win+R` → `shell:startup` ile başlangıç klasörüne kısayol atın.

##  Teşekkürler

- Microsoft WASAPI dokümantasyonu
- C++ topluluğu

---

** Projeyi beğendiyseniz yıldız vermeyi unutmayın!**

Made with  using C++
