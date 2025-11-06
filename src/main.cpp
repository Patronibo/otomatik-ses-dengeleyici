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
    (void)hPrevInstance;
    (void)lpCmdLine;
    (void)nCmdShow;
    
    #ifdef _DEBUG
    if (AllocConsole()) {
        FILE* fpStdout = nullptr;
        FILE* fpStderr = nullptr;
        freopen_s(&fpStdout, "CONOUT$", "w", stdout);
        freopen_s(&fpStderr, "CONOUT$", "w", stderr);
        std::cout << "Ses Dengeleyici baslatiliyor..." << std::endl;
    }
    #endif

    AudioCapture audioCapture;
    AudioCompressor compressor;

    compressor.SetThreshold(-20.0f);  
    compressor.SetRatio(3.0f);        
    compressor.SetAttackTime(5.0f);   
    compressor.SetReleaseTime(50.0f); 
    compressor.SetTargetLevel(0.65f); 

    audioCapture.SetCallback([&](float* audioData, size_t frameCount, int channels) {
        if (!g_isActive) {
            return;
        }
    
        float targetVolume = compressor.ProcessAudio(audioData, frameCount, channels);
        audioCapture.SetVolume(targetVolume);
    });

    if (!audioCapture.Initialize()) {
        #ifdef _DEBUG
        cerr << "Audio capture initialize edilemedi!" << endl;
        #endif
        MessageBoxW(nullptr, 
            L"Ses sistemi başlatılamadı!\n"
            L"Lütfen ses ayarlarınızı kontrol edin.",
            L"Hata", MB_OK | MB_ICONERROR);
        return 1;
    }

    if (!audioCapture.Start()) {
        #ifdef _DEBUG
        cerr << "Audio capture baslatilamadi!" << endl;
        #endif
        MessageBoxW(nullptr,
            L"Ses yakalama başlatılamadı!\n"
            L"Başka bir program ses cihazını kullanıyor olabilir.",
            L"Hata", MB_OK | MB_ICONERROR);
        return 1;
    }

    #ifdef _DEBUG
    cout << "Ses yakalama baslatildi!" << endl;
    #endif

    TrayIcon trayIcon;
    if (!trayIcon.Initialize(hInstance, L"Ses Dengeleyici - Aktif")) {
        #ifdef _DEBUG
        cerr << "Tray icon olusturulamadi!" << endl;
        #endif
        return 1;
    }
    
    trayIcon.SetOnToggle([&]() {
        g_isActive = !g_isActive;
        
        if (g_isActive) {
            trayIcon.SetTooltip(L"Ses Dengeleyici - Aktif");
            trayIcon.ShowNotification(L"Ses Dengeleyici", L"Etkinleştirildi");
            #ifdef _DEBUG
            cout << "Ses dengeleme AKTIF" << endl;
            #endif
        } else {
            trayIcon.SetTooltip(L"Ses Dengeleyici - Devre Dışı");
            trayIcon.ShowNotification(L"Ses Dengeleyici", L"Devre dışı bırakıldı");
            audioCapture.SetVolume(1.0f); 
            #ifdef _DEBUG
            cout << "Ses dengeleme DEVRE DISI" << endl;
            #endif
        }
    });

    trayIcon.SetOnExit([&]() {
        g_shouldExit = true;
        #ifdef _DEBUG
        cout << "Cikis istegi alindi" << endl;
        #endif
    });

    trayIcon.ShowNotification(
        L"Ses Dengeleyici",
        L"Uygulama başlatıldı ve arka planda çalışıyor.\n"
        L"Çift tıklayarak açıp-kapatabilirsiniz.");

    #ifdef _DEBUG
    cout << "Tray icon olusturuldu. Uygulama calisiyor..." << endl;
    cout << "Kapatmak icin system tray'den cikin." << endl;
    #endif

    std::thread statusThread([&]() {
        while (!g_shouldExit) {
            if (g_isActive) {
                float currentLevel = compressor.GetCurrentLevel();
                float targetVolume = compressor.GetTargetVolume();
                
                std::wstringstream ss;
                ss << L"Ses Dengeleyici - Aktif\n"
                   << L"Seviye: " << fixed << setprecision(2) << (currentLevel * 100.0f) << L"%\n"
                   << L"Volume: " << fixed << setprecision(2) << (targetVolume * 100.0f) << L"%";
                
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
    cout << "Uygulama kapatiliyor..." << endl;
    #endif

    #ifdef _DEBUG
    FreeConsole();
    #endif

    return 0;
}

