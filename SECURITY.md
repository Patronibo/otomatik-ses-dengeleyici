# Güvenlik Politikası

## Desteklenen Versiyonlar

| Versiyon | Destekleniyor |
| ------- | --------------- |
| 1.0.x   | ✅              |

## Güvenlik Önlemleri

### Kod Güvenliği

- ✅ Modern C++17 standartları
- ✅ Type-safe kod yapısı
- ✅ Memory-safe (RAII pattern)
- ✅ Exception-safe design
- ✅ Input validation
- ✅ No buffer overflows
- ✅ No external dependencies

### Runtime Güvenliği

- ✅ Minimum privilege gereksinimi
- ✅ Sandbox içinde çalışabilir
- ✅ No network access
- ✅ No file system modification (sadece okuma)
- ✅ Windows API best practices

### Build Güvenliği

- ✅ Statik analiz temiz
- ✅ Warning-free compilation
- ✅ No deprecated API usage
- ✅ Modern toolchain

## Güvenlik Açığı Bildirme

Güvenlik açığı bulursanız:

1. **Public issue AÇMAYIN**
2. Email gönderin (proje sahibine)
3. Detaylı açıklama yapın:
   - Sorun açıklaması
   - Tekrar adımları
   - Potansiyel etki
   - Önerilen çözüm (varsa)

## Güvenlik En İyi Uygulamaları

### Kullanıcılar İçin

- ✅ Sadece resmi kaynaklardan indirin
- ✅ İmzayı kontrol edin (varsa)
- ✅ Antivirus taraması yapın
- ✅ Güncellemeleri takip edin

### Geliştiriciler İçin

- ✅ Code review yapın
- ✅ Statik analiz kullanın
- ✅ Input validation ekleyin
- ✅ Memory leak kontrolü yapın
- ✅ Thread-safe kod yazın

## Bilinen Sınırlamalar

- Windows API sınırlamaları geçerlidir
- System-wide ses kontrolü gerektirir
- Admin yetkisi GEREKTIRMEZ

## Lisans

Bu proje MIT lisansı altındadır. "AS IS" olarak sunulur, garanti verilmez.

