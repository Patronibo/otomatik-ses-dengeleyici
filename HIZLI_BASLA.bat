@echo off
REM Hızlı test scripti - Zaten derlenmiş programı çalıştırır

if exist "build\bin\Release\SesDengeleyici.exe" (
    echo Program baslatiliyor...
    start "" "build\bin\Release\SesDengeleyici.exe"
    echo.
    echo Program basladi! System tray'e bakin ^(saat yaninda^)
    timeout /t 2 /nobreak >nul
) else if exist "build\bin\Debug\SesDengeleyici.exe" (
    echo Debug versiyonu baslatiliyor...
    start "" "build\bin\Debug\SesDengeleyici.exe"
    echo.
    echo Program basladi! System tray'e bakin ^(saat yaninda^)
    timeout /t 2 /nobreak >nul
) else (
    echo HATA: Program henuz derlenmemis!
    echo.
    echo Once build.bat dosyasini calistirin.
    pause
)

