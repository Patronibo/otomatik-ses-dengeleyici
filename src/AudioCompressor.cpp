#include "AudioCompressor.h"
#include <numeric>
#include <cmath>
#include <algorithm>

AudioCompressor::AudioCompressor()
    : m_threshold(-20.0f)    // -20 dB threshold
    , m_ratio(4.0f)          // 4:1 compression
    , m_attackTime(10.0f)    // 10 ms attack
    , m_releaseTime(100.0f)  // 100 ms release
    , m_targetLevel(0.7f)    // %70 hedef seviye
    , m_currentLevel(0.0f)
    , m_targetVolume(1.0f)
    , m_envelope(0.0f)
    , m_historySize(10)
    , m_historyIndex(0)
{
    m_levelHistory.resize(m_historySize, 0.0f);
}

float AudioCompressor::ProcessAudio(float* audioData, size_t frameCount, int channels) {
    if (!audioData || frameCount == 0) {
        return 1.0f;
    }

    // RMS seviyesini hesapla
    float rms = CalculateRMS(audioData, frameCount, channels);
    m_currentLevel = rms;

    // Geçmişe ekle (smoothing için)
    m_levelHistory[m_historyIndex] = rms;
    m_historyIndex = (m_historyIndex + 1) % m_historySize;

    // Ortalama seviye
    float avgLevel = std::accumulate(m_levelHistory.begin(), m_levelHistory.end(), 0.0f) / static_cast<float>(m_historySize);

    // dB'ye çevir
    float levelDb = LinearToDb(avgLevel);

    // Compression hesapla
    float gainReduction = 0.0f;
    if (levelDb > m_threshold) {
        // Threshold'u aşan kısım
        float excess = levelDb - m_threshold;
        // Compression ratio uygula
        gainReduction = excess * (1.0f - 1.0f / m_ratio);
    }

    // Hedef gain'i hesapla
    float targetGain = DbToLinear(-gainReduction);

    // Attack/Release envelope
    float attackCoef = std::exp(-1000.0f / (m_attackTime * 48000.0f));
    float releaseCoef = std::exp(-1000.0f / (m_releaseTime * 48000.0f));

    if (targetGain < m_envelope) {
        // Attack (ses yükselirken)
        m_envelope = attackCoef * m_envelope + (1.0f - attackCoef) * targetGain;
    } else {
        // Release (ses düşerken)
        m_envelope = releaseCoef * m_envelope + (1.0f - releaseCoef) * targetGain;
    }

    // Hedef volume'u hesapla
    // Eğer ses çok düşükse yükselt, çok yüksekse düşür
    float volumeAdjust = 1.0f;
    
    if (avgLevel < 0.1f && avgLevel > 0.01f) {
        // Düşük sesleri yükselt
        volumeAdjust = m_targetLevel / (avgLevel + 0.001f);
        volumeAdjust = std::min(volumeAdjust, 3.0f); // Maksimum 3x artış
    } else if (avgLevel > m_targetLevel) {
        // Yüksek sesleri düşür
        volumeAdjust = m_targetLevel / avgLevel;
    }

    // Compression envelope ile birleştir
    m_targetVolume = m_envelope * volumeAdjust;
    
    // Limiter (0.3 - 1.2 arası tut)
    m_targetVolume = std::max(0.3f, std::min(1.2f, m_targetVolume));

    // Smoothing uygula (ani değişimleri önle)
    static float smoothedVolume = 1.0f;
    float smoothingFactor = 0.95f;
    smoothedVolume = smoothingFactor * smoothedVolume + (1.0f - smoothingFactor) * m_targetVolume;

    return smoothedVolume;
}

float AudioCompressor::CalculateRMS(float* audioData, size_t frameCount, int channels) {
    if (frameCount == 0) {
        return 0.0f;
    }

    float sum = 0.0f;
    size_t totalSamples = frameCount * static_cast<size_t>(channels);

    for (size_t i = 0; i < totalSamples; i++) {
        float sample = audioData[i];
        sum += sample * sample;
    }

    return std::sqrt(sum / static_cast<float>(totalSamples));
}

float AudioCompressor::LinearToDb(float linear) {
    if (linear <= 0.0f) {
        return -100.0f; // Minimum dB
    }
    return 20.0f * std::log10(linear);
}

float AudioCompressor::DbToLinear(float db) {
    return std::pow(10.0f, db / 20.0f);
}
