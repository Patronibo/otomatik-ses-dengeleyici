#include "AudioCapture.h"
#include <algorithm>
#include <vector>

#ifdef _DEBUG
#include <iostream>
#endif

#undef min
#undef max

// COM için gerekli header'lar
#include <functiondiscoverykeys_devpkey.h>

#pragma comment(lib, "ole32.lib")

AudioCapture::AudioCapture()
    : m_pEnumerator(nullptr)
    , m_pDevice(nullptr)
    , m_pAudioClient(nullptr)
    , m_pCaptureClient(nullptr)
    , m_pSessionManager(nullptr)
    , m_pVolumeControl(nullptr)
    , m_pwfx(nullptr)
    , m_isRunning(false)
    , m_shouldStop(false)
{
}

AudioCapture::~AudioCapture() {
    Stop();
    Cleanup();
}

bool AudioCapture::Initialize() {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) {
        #ifdef _DEBUG
        std::cerr << "COM initialize hatasi" << std::endl;
        #endif
        return false;
    }

    // Device enumerator oluştur
    hr = CoCreateInstance(
        __uuidof(MMDeviceEnumerator), nullptr,
        CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
        reinterpret_cast<void**>(&m_pEnumerator));
    
    if (FAILED(hr)) {
        return false;
    }

    // Varsayılan ses cihazını al (loopback için render device)
    hr = m_pEnumerator->GetDefaultAudioEndpoint(
        eRender, eConsole, &m_pDevice);
    
    if (FAILED(hr)) {
        return false;
    }

    // Audio client oluştur
    hr = m_pDevice->Activate(
        __uuidof(IAudioClient), CLSCTX_ALL,
        nullptr, reinterpret_cast<void**>(&m_pAudioClient));
    
    if (FAILED(hr)) {
        return false;
    }

    // Ses formatını al
    hr = m_pAudioClient->GetMixFormat(&m_pwfx);
    if (FAILED(hr)) {
        return false;
    }

    // Audio client'i loopback mode'da başlat
    hr = m_pAudioClient->Initialize(
        AUDCLNT_SHAREMODE_SHARED,
        AUDCLNT_STREAMFLAGS_LOOPBACK,
        10000000, // 1 saniye buffer
        0,
        m_pwfx,
        nullptr);
    
    if (FAILED(hr)) {
        return false;
    }

    // Capture client oluştur
    hr = m_pAudioClient->GetService(
        __uuidof(IAudioCaptureClient),
        reinterpret_cast<void**>(&m_pCaptureClient));
    
    if (FAILED(hr)) {
        return false;
    }

    // Session manager ve volume control al
    hr = m_pDevice->Activate(
        __uuidof(IAudioSessionManager), CLSCTX_ALL,
        nullptr, reinterpret_cast<void**>(&m_pSessionManager));
    
    if (SUCCEEDED(hr)) {
        m_pSessionManager->GetSimpleAudioVolume(
            nullptr, FALSE, &m_pVolumeControl);
    }

    return true;
}

bool AudioCapture::Start() {
    if (m_isRunning) {
        return true;
    }

    HRESULT hr = m_pAudioClient->Start();
    if (FAILED(hr)) {
        return false;
    }

    m_shouldStop = false;
    m_isRunning = true;
    m_captureThread = std::thread(&AudioCapture::CaptureThread, this);

    return true;
}

void AudioCapture::Stop() {
    if (!m_isRunning) {
        return;
    }

    m_shouldStop = true;
    
    if (m_captureThread.joinable()) {
        m_captureThread.join();
    }

    if (m_pAudioClient) {
        m_pAudioClient->Stop();
    }

    m_isRunning = false;
}

void AudioCapture::CaptureThread() {
    while (!m_shouldStop) {
        UINT32 packetLength = 0;
        HRESULT hr = m_pCaptureClient->GetNextPacketSize(&packetLength);
        
        if (FAILED(hr)) {
            break;
        }

        while (packetLength != 0) {
            BYTE* pData = nullptr;
            UINT32 numFramesAvailable = 0;
            DWORD flags = 0;

            hr = m_pCaptureClient->GetBuffer(
                &pData,
                &numFramesAvailable,
                &flags,
                nullptr,
                nullptr);

            if (FAILED(hr)) {
                break;
            }

            // Sessizlik kontrolü
            if (!(flags & AUDCLNT_BUFFERFLAGS_SILENT)) {
                // Ses verisini float'a dönüştür ve callback'i çağır
                if (m_callback && numFramesAvailable > 0) {
                    int channels = static_cast<int>(m_pwfx->nChannels);
                    
                    // Float array oluştur
                    std::vector<float> floatData(static_cast<size_t>(numFramesAvailable) * channels);
                    
                    // 16-bit PCM'den float'a dönüştür
                    if (m_pwfx->wFormatTag == WAVE_FORMAT_PCM || 
                        (m_pwfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE && 
                         m_pwfx->wBitsPerSample == 16)) {
                        
                        short* samples = reinterpret_cast<short*>(pData);
                        for (UINT32 i = 0; i < numFramesAvailable * static_cast<UINT32>(channels); i++) {
                            floatData[i] = static_cast<float>(samples[i]) / 32768.0f;
                        }
                    }
                    // Float format
                    else if (m_pwfx->wBitsPerSample == 32) {
                        float* samples = reinterpret_cast<float*>(pData);
                        for (UINT32 i = 0; i < numFramesAvailable * static_cast<UINT32>(channels); i++) {
                            floatData[i] = samples[i];
                        }
                    }

                    m_callback(floatData.data(), numFramesAvailable, channels);
                }
            }

            hr = m_pCaptureClient->ReleaseBuffer(numFramesAvailable);
            if (FAILED(hr)) {
                break;
            }

            hr = m_pCaptureClient->GetNextPacketSize(&packetLength);
            if (FAILED(hr)) {
                break;
            }
        }

        Sleep(10);
    }
}

void AudioCapture::SetVolume(float volume) {
    if (m_pVolumeControl) {
        volume = std::max(0.0f, std::min(1.0f, volume));
        m_pVolumeControl->SetMasterVolume(volume, nullptr);
    }
}

float AudioCapture::GetVolume() const {
    if (m_pVolumeControl) {
        float volume = 0.0f;
        m_pVolumeControl->GetMasterVolume(&volume);
        return volume;
    }
    return 1.0f;
}

void AudioCapture::Cleanup() {
    if (m_pVolumeControl) {
        m_pVolumeControl->Release();
        m_pVolumeControl = nullptr;
    }

    if (m_pSessionManager) {
        m_pSessionManager->Release();
        m_pSessionManager = nullptr;
    }

    if (m_pCaptureClient) {
        m_pCaptureClient->Release();
        m_pCaptureClient = nullptr;
    }

    if (m_pAudioClient) {
        m_pAudioClient->Release();
        m_pAudioClient = nullptr;
    }

    if (m_pDevice) {
        m_pDevice->Release();
        m_pDevice = nullptr;
    }

    if (m_pEnumerator) {
        m_pEnumerator->Release();
        m_pEnumerator = nullptr;
    }

    if (m_pwfx) {
        CoTaskMemFree(m_pwfx);
        m_pwfx = nullptr;
    }

    CoUninitialize();
}
