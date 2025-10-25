# Katkıda Bulunma Kılavuzu

Ses Dengeleyici projesine katkıda bulunmak istediğiniz için teşekkür ederiz! 🎉

## Başlamadan Önce

1. Projeyi fork edin
2. Yerel makinenize clone edin
3. Yeni bir branch oluşturun
4. Değişikliklerinizi yapın
5. Test edin
6. Pull request açın

## Kod Standartları

### C++ Stil Kılavuzu

- **C++17** standartlarını kullanın
- **Modern C++** özellikleri tercih edin (smart pointers, RAII, vb.)
- **Type-safe** kod yazın
- **const correctness** uygulayın

### Naming Conventions

```cpp
// Classes: PascalCase
class AudioCapture { };

// Functions: PascalCase
void ProcessAudio();

// Member variables: m_ prefix + camelCase
float m_targetVolume;

// Local variables: camelCase
float currentLevel;

// Constants: k prefix + PascalCase
const float kDefaultThreshold = -20.0f;
```

### Dosya Yapısı

```cpp
// Header guard
#pragma once

// System includes
#include <windows.h>
#include <vector>

// Project includes
#include "AudioCapture.h"

// Class definition
class MyClass {
public:
    // Public interface
    
private:
    // Private implementation
};
```

## Commit Mesajları

Anlamlı commit mesajları yazın:

```
feat: Add new audio compression algorithm
fix: Fix memory leak in AudioCapture
docs: Update README with new features
refactor: Improve code structure in TrayIcon
test: Add unit tests for AudioCompressor
```

## Pull Request Süreci

1. **Branch oluşturun**
   ```bash
   git checkout -b feature/amazing-feature
   ```

2. **Değişikliklerinizi yapın**
   - Kod yazın
   - Test edin
   - Dokümantasyon ekleyin

3. **Commit edin**
   ```bash
   git commit -m "feat: Add amazing feature"
   ```

4. **Push edin**
   ```bash
   git push origin feature/amazing-feature
   ```

5. **Pull Request açın**
   - Detaylı açıklama yazın
   - Ekran görüntüsü ekleyin (varsa)
   - İlgili issue'yu referans verin

## Test Etme

Değişikliklerinizi test edin:

```cmd
# Derleyin
build.bat

# Çalıştırın
build\bin\Release\SesDengeleyici.exe

# Test senaryoları:
# 1. Farklı müzikler çalın
# 2. Volume kontrolünü test edin
# 3. Aç/kapat fonksiyonunu test edin
```

## Sorun Bildirme

Issue açarken şunları ekleyin:

- **Sorun açıklaması**: Ne olması gerekiyor, ne oluyor?
- **Adımlar**: Sorunu nasıl tekrar edebiliriz?
- **Beklenen davranış**: Ne olmalı?
- **Gerçek davranış**: Ne oluyor?
- **Sistem bilgileri**: Windows versiyonu, VS versiyonu
- **Ekran görüntüsü**: Varsa ekleyin

## Öneri Bildirme

Yeni özellik önerisi için:

- Açık ve detaylı açıklama
- Kullanım senaryosu
- Örnek kod (varsa)
- Mock-up veya diagram (varsa)

## Sorularınız mı var?

Issue açarak sorabilirsiniz!

Teşekkürler! ❤️

