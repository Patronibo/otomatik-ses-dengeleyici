@echo off
chcp 65001 >nul
echo.
echo ╔════════════════════════════════════════════════════╗
echo ║     Ses Dengeleyici - Build Script                ║
echo ╚════════════════════════════════════════════════════╝
echo.

REM Visual Studio versiyonunu algıla
set "VS_VERSION=Visual Studio 17 2022"
where /q cl.exe
if %errorlevel% neq 0 (
    echo Visual Studio 2022 bulunamadı, 2019 deneniyor...
    set "VS_VERSION=Visual Studio 16 2019"
)

REM Build klasörü oluştur
if not exist build (
    echo [1/4] Build klasörü oluşturuluyor...
    mkdir build
) else (
    echo [1/4] Build klasörü mevcut, temizleniyor...
)

cd build

echo [2/4] CMake yapılandırması yapılıyor...
cmake .. -G "%VS_VERSION%" -A x64
if errorlevel 1 (
    echo.
    echo ❌ HATA: CMake yapılandırması başarısız!
    echo.
    echo Olası çözümler:
    echo 1. Visual Studio 2019 veya 2022 kurulu olduğundan emin olun
    echo 2. "Desktop development with C++" yüklü olmalı
    echo 3. Developer Command Prompt kullanmayı deneyin
    echo.
    cd ..
    pause
    exit /b 1
)

echo [3/4] Proje derleniyor (Release mode)...
cmake --build . --config Release --parallel
if errorlevel 1 (
    echo.
    echo ❌ HATA: Derleme başarısız!
    echo.
    echo Yukarıdaki hata mesajlarını kontrol edin.
    echo.
    cd ..
    pause
    exit /b 1
)

echo [4/4] Kontrol ediliyor...
if exist "bin\Release\SesDengeleyici.exe" (
    echo.
    echo ╔════════════════════════════════════════════════════╗
    echo ║  ✅ Derleme BAŞARILI!                              ║
    echo ╚════════════════════════════════════════════════════╝
    echo.
    echo 📂 Çalıştırılabilir dosya:
    echo    %CD%\bin\Release\SesDengeleyici.exe
    echo.
    echo 🚀 Hemen çalıştırmak için: [E]vet / Çıkmak için: [H]ayır
    echo.
    choice /C EH /N /M "Seçiminiz (E/H): "
    if errorlevel 2 goto end
    if errorlevel 1 goto run
) else (
    echo.
    echo ❌ HATA: .exe dosyası bulunamadı!
    cd ..
    pause
    exit /b 1
)

:run
echo.
echo ▶️ Program başlatılıyor...
start "" "bin\Release\SesDengeleyici.exe"
echo.
echo ✅ Program başlatıldı!
echo 💡 System tray'e (saat yanına) bakın.
echo.
timeout /t 3 /nobreak >nul
goto end

:end
cd ..
echo.
pause

