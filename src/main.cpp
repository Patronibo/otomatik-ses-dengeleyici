#include "AudioCapture.h"
#include "AudioCompressor.h"
#include "TrayIcon.h"
#include <windows.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <atomic>
#include <thread>

std::atomic<bool> g_isActive{true};
std::atomic<bool> g_shouldExit{false};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Unused parameters
    (void)hPrevInstance;
    (void)lpCmdLine;
    (void)nCmdShow;
    
    // Console penceresi aç (debug için)
    #ifdef _DEBUG
    if (AllocConsole()) {
        FILE* fpStdout = nullptr;
        FILE* fpStderr = nullptr;
        freopen_s(&fpStdout, "CONOUT$", "w", stdout);
        freopen_s(&fpStderr, "CONOUT$", "w", stderr);
        std::cout << "Ses Dengeleyici baslatiliyor..." << std::endl;
    }
    #endif

    // Audio sistemi oluştur
    AudioCapture audioCapture;
    AudioCompressor compressor;

    // Compressor parametrelerini ayarla
    compressor.SetThreshold(-20.0f);  // -20 dB threshold
    compressor.SetRatio(3.0f);        // 3:1 compression
    compressor.SetAttackTime(5.0f);   // 5 ms attack
    compressor.SetReleaseTime(50.0f); // 50 ms release
    compressor.SetTargetLevel(0.65f); // %65 hedef seviye

    // Audio callback ayarla
    audioCapture.SetCallback([&](float* audioData, size_t frameCount, int channels) {
        if (!g_isActive) {
            return;
        }

        // Ses seviyesini analiz et ve uygun volume'u hesapla
        float targetVolume = compressor.ProcessAudio(audioData, frameCount, channels);
        
        // Volume'u uygula
        audioCapture.SetVolume(targetVolume);
    });

    // Audio capture'ı başlat
    if (!audioCapture.Initialize()) {
        #ifdef _DEBUG
        std::cerr << "Audio capture initialize edilemedi!" << std::endl;
        #endif
        MessageBoxW(nullptr, 
            L"Ses sistemi başlatılamadı!\n"
            L"Lütfen ses ayarlarınızı kontrol edin.",
            L"Hata", MB_OK | MB_ICONERROR);
        return 1;
    }

    if (!audioCapture.Start()) {
        #ifdef _DEBUG
        std::cerr << "Audio capture baslatilamadi!" << std::endl;
        #endif
        MessageBoxW(nullptr,
            L"Ses yakalama başlatılamadı!\n"
            L"Başka bir program ses cihazını kullanıyor olabilir.",
            L"Hata", MB_OK | MB_ICONERROR);
        return 1;
    }

    #ifdef _DEBUG
    std::cout << "Ses yakalama baslatildi!" << std::endl;
    #endif

    // Tray icon oluştur
    TrayIcon trayIcon;
    if (!trayIcon.Initialize(hInstance, L"Ses Dengeleyici - Aktif")) {
        #ifdef _DEBUG
        std::cerr << "Tray icon olusturulamadi!" << std::endl;
        #endif
        return 1;
    }

    // Toggle callback
    trayIcon.SetOnToggle([&]() {
        g_isActive = !g_isActive;
        
        if (g_isActive) {
            trayIcon.SetTooltip(L"Ses Dengeleyici - Aktif");
            trayIcon.ShowNotification(L"Ses Dengeleyici", L"Etkinleştirildi");
            #ifdef _DEBUG
            std::cout << "Ses dengeleme AKTIF" << std::endl;
            #endif
        } else {
            trayIcon.SetTooltip(L"Ses Dengeleyici - Devre Dışı");
            trayIcon.ShowNotification(L"Ses Dengeleyici", L"Devre dışı bırakıldı");
            audioCapture.SetVolume(1.0f); // Normal seviyeye dön
            #ifdef _DEBUG
            std::cout << "Ses dengeleme DEVRE DISI" << std::endl;
            #endif
        }
    });

    // Exit callback
    trayIcon.SetOnExit([&]() {
        g_shouldExit = true;
        #ifdef _DEBUG
        std::cout << "Cikis istegi alindi" << std::endl;
        #endif
    });

    // Başlangıç bildirimi
    trayIcon.ShowNotification(
        L"Ses Dengeleyici",
        L"Uygulama başlatıldı ve arka planda çalışıyor.\n"
        L"Çift tıklayarak açıp-kapatabilirsiniz.");

    #ifdef _DEBUG
    std::cout << "Tray icon olusturuldu. Uygulama calisiyor..." << std::endl;
    std::cout << "Kapatmak icin system tray'den cikin." << std::endl;
    #endif

    // Ana döngü - status güncelleme
    std::thread statusThread([&]() {
        while (!g_shouldExit) {
            if (g_isActive) {
                float currentLevel = compressor.GetCurrentLevel();
                float targetVolume = compressor.GetTargetVolume();
                
                std::wstringstream ss;
                ss << L"Ses Dengeleyici - Aktif\n"
                   << L"Seviye: " << std::fixed << std::setprecision(2) << (currentLevel * 100.0f) << L"%\n"
                   << L"Volume: " << std::fixed << std::setprecision(2) << (targetVolume * 100.0f) << L"%";
                
                trayIcon.SetTooltip(ss.str());
            }
            
            Sleep(500); // 500ms'de bir güncelle
        }
    });

    // Message loop
    trayIcon.ProcessMessages();

    // Temizlik
    g_shouldExit = true;
    if (statusThread.joinable()) {
        statusThread.join();
    }

    audioCapture.Stop();

    #ifdef _DEBUG
    std::cout << "Uygulama kapatiliyor..." << std::endl;
    #endif

    #ifdef _DEBUG
    FreeConsole();
    #endif

    return 0;
}
