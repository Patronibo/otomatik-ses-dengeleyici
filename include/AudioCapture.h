#pragma once

#include <windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <audiopolicy.h>
#include <endpointvolume.h>
#include <thread>
#include <atomic>
#include <functional>
#include <vector>
using namespace std;

class AudioCapture {
public:
    AudioCapture();
    ~AudioCapture();

    bool Initialize();
    bool Start();
    void Stop();
    bool IsRunning() const { return m_isRunning; }

    using AudioCallback = function<void(float* audioData, size_t frameCount, int channels)>;
    void SetCallback(AudioCallback callback) { m_callback = callback; }

    void SetVolume(float volume);
    float GetVolume() const;

private:
    void CaptureThread();
    void Cleanup();

    IMMDeviceEnumerator* m_pEnumerator;
    IMMDevice* m_pDevice;
    IAudioClient* m_pAudioClient;
    IAudioCaptureClient* m_pCaptureClient;
    IAudioSessionManager* m_pSessionManager;
    ISimpleAudioVolume* m_pVolumeControl;

    WAVEFORMATEX* m_pwfx;
    thread m_captureThread;
    atomic<bool> m_isRunning;
    atomic<bool> m_shouldStop;

    AudioCallback m_callback;
};


