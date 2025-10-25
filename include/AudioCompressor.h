#pragma once

#include <vector>
#include <cmath>
#include <algorithm>

class AudioCompressor {
public:
    AudioCompressor();

    // Ses verisini işle ve uygun ses seviyesini hesapla
    float ProcessAudio(float* audioData, size_t frameCount, int channels);

    // Parametreler
    void SetThreshold(float threshold) { m_threshold = threshold; } // dB cinsinden
    void SetRatio(float ratio) { m_ratio = ratio; }
    void SetAttackTime(float ms) { m_attackTime = ms; }
    void SetReleaseTime(float ms) { m_releaseTime = ms; }
    void SetTargetLevel(float level) { m_targetLevel = level; } // 0.0 - 1.0 arası

    float GetCurrentLevel() const { return m_currentLevel; }
    float GetTargetVolume() const { return m_targetVolume; }

private:
    float CalculateRMS(float* audioData, size_t frameCount, int channels);
    float LinearToDb(float linear);
    float DbToLinear(float db);

    // Compressor parametreleri
    float m_threshold;      // dB
    float m_ratio;          // compression ratio
    float m_attackTime;     // ms
    float m_releaseTime;    // ms
    float m_targetLevel;    // hedef ses seviyesi

    // Durum değişkenleri
    float m_currentLevel;   // mevcut RMS seviyesi
    float m_targetVolume;   // hedef volume (0.0 - 1.0)
    float m_envelope;       // envelope follower

    // Smoothing için
    std::vector<float> m_levelHistory;
    size_t m_historySize;
    size_t m_historyIndex;
};

