# Mimari Dokümantasyon

## Genel Bakış

Ses Dengeleyici, Windows WASAPI (Windows Audio Session API) kullanan gerçek zamanlı ses işleme uygulamasıdır.

## Sistem Mimarisi

```
┌─────────────────────────────────────────────────────────┐
│                    Kullanıcı Arayüzü                     │
│                    (System Tray Icon)                    │
└──────────────────────┬──────────────────────────────────┘
                       │
                       ▼
┌─────────────────────────────────────────────────────────┐
│                    Main Controller                       │
│                       (main.cpp)                         │
└──────┬────────────────┬────────────────┬────────────────┘
       │                │                │
       ▼                ▼                ▼
┌──────────────┐ ┌──────────────┐ ┌──────────────┐
│ AudioCapture │ │AudioCompressor│ │  TrayIcon    │
│  (WASAPI)    │ │  (Algorithm)  │ │  (Win32 UI)  │
└──────────────┘ └──────────────┘ └──────────────┘
       │                │                │
       ▼                ▼                ▼
┌─────────────────────────────────────────────────────────┐
│                    Windows API Layer                     │
│            (WASAPI, Win32 API, Shell API)               │
└─────────────────────────────────────────────────────────┘
```

## Modül Detayları

### 1. AudioCapture (ses-yakalama)
**Dosyalar**: `AudioCapture.h`, `AudioCapture.cpp`

**Sorumluluklar**:
- WASAPI loopback mode ile sistem sesini yakalama
- Ses formatı dönüştürme (PCM/Float)
- Volume kontrol
- Thread-safe ses buffering

**Anahtar Teknolojiler**:
- COM (Component Object Model)
- WASAPI Loopback Capture
- Multi-threading (`std::thread`)

### 2. AudioCompressor (ses-işleme)
**Dosyalar**: `AudioCompressor.h`, `AudioCompressor.cpp`

**Sorumluluklar**:
- RMS (Root Mean Square) seviye hesaplama
- Dynamic range compression
- Loudness normalization
- Smoothing ve envelope following

**Algoritma**:
1. RMS seviye hesapla
2. dB'ye çevir
3. Threshold kontrolü
4. Compression ratio uygula
5. Attack/Release envelope
6. Volume adjustment
7. Limiting ve smoothing

### 3. TrayIcon (kullanıcı-arayüzü)
**Dosyalar**: `TrayIcon.h`, `TrayIcon.cpp`

**Sorumluluklar**:
- System tray icon yönetimi
- Context menu
- Tooltip güncelleme
- Bildirim gösterme

**Anahtar Teknolojiler**:
- Win32 Shell API
- Message loop handling
- Event callbacks

### 4. Main Controller
**Dosya**: `main.cpp`

**Sorumluluklar**:
- Uygulama yaşam döngüsü
- Modül koordinasyonu
- Event handling
- State management

## Veri Akışı

```
Sistem Ses Çıkışı
       │
       ▼
WASAPI Loopback Capture
       │
       ▼
Float Buffer (Frame-based)
       │
       ▼
RMS Analysis
       │
       ▼
Compression Algorithm
       │
       ▼
Volume Adjustment
       │
       ▼
WASAPI Volume Control
       │
       ▼
Hoparlör/Kulaklık
```

## Threading Model

```
Main Thread (UI Thread)
├─ TrayIcon message loop
└─ User interactions

Background Thread (Audio Thread)
├─ Audio capture loop
├─ Buffer processing
└─ Volume calculation

Status Thread
└─ Tooltip updates (500ms interval)
```

## Bellek Yönetimi

- **RAII Pattern**: Tüm kaynaklar constructor/destructor ile yönetilir
- **Smart Pointers**: Kullanılmaz (COM raw pointers gerekli)
- **Manual Release**: COM nesneleri manuel olarak release edilir
- **No Memory Leaks**: Valgrind/Dr. Memory ile test edilmiştir

## Performans Optimizasyonları

1. **Düşük Latency**:
   - 10ms audio buffer
   - Lock-free callback
   - Minimal processing

2. **CPU Efficiency**:
   - Efficient RMS calculation
   - Pre-calculated coefficients
   - Optimized loops

3. **Memory Efficiency**:
   - Small history buffer (10 samples)
   - Stack allocation preferred
   - No dynamic allocations in hot path

## Güvenlik Önlemleri

1. **Input Validation**:
   - Null pointer checks
   - Range validation
   - Type safety

2. **Exception Safety**:
   - RAII for resource management
   - No exceptions in callbacks
   - Graceful error handling

3. **Thread Safety**:
   - Atomic variables for state
   - No shared mutable state
   - Read-only callback data

## Platform Bağımlılıkları

- **Windows 10+**: WASAPI support
- **x64 Architecture**: 64-bit builds
- **MSVC Compiler**: Microsoft Visual C++
- **Windows SDK**: 10.0.22621.0 or later

## Genişletme Noktaları

1. **Yeni Compression Algorithms**:
   - `AudioCompressor` sınıfını extend edin
   - Yeni algoritmalar ekleyin

2. **UI Improvements**:
   - `TrayIcon` sınıfına özellikler ekleyin
   - Ayarlar penceresi ekleyin

3. **Configuration**:
   - JSON/INI config dosyası
   - Runtime parameter adjustment

## Build Sistemi

- **CMake**: Cross-platform build configuration
- **MSVC**: Native Windows compiler
- **Static Linking**: No external DLL dependencies

## Test Stratejisi

1. **Unit Tests**: Algoritma doğrulama
2. **Integration Tests**: Modül entegrasyonu
3. **Manual Tests**: Gerçek dünya senaryoları
4. **Performance Tests**: CPU/Memory profiling

## Bilinen Sınırlamalar

1. **Windows Only**: WASAPI Windows'a özel
2. **System-wide**: Uygulama bazlı kontrol yok
3. **Single Instance**: Tek instance çalışır
4. **Real-time**: Batch processing yok

