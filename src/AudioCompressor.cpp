#include "AudioCompressor.h"
#include <numeric>
#include <cmath>
#include <algorithm>
using namespace std;

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

    float rms = CalculateRMS(audioData, frameCount, channels);
    m_currentLevel = rms;

    m_levelHistory[m_historyIndex] = rms;
    m_historyIndex = (m_historyIndex + 1) % m_historySize;

    float avgLevel = accumulate(m_levelHistory.begin(), m_levelHistory.end(), 0.0f) / static_cast<float>(m_historySize);

    float levelDb = LinearToDb(avgLevel);

    float gainReduction = 0.0f;
    if (levelDb > m_threshold) {
        float excess = levelDb - m_threshold;
        gainReduction = excess * (1.0f - 1.0f / m_ratio);
    }

    float targetGain = DbToLinear(-gainReduction);
    float attackCoef = exp(-1000.0f / (m_attackTime * 48000.0f));
    float releaseCoef = exp(-1000.0f / (m_releaseTime * 48000.0f));

    if (targetGain < m_envelope) {
        m_envelope = attackCoef * m_envelope + (1.0f - attackCoef) * targetGain;
    } else {
        m_envelope = releaseCoef * m_envelope + (1.0f - releaseCoef) * targetGain;
    }

    float volumeAdjust = 1.0f;
    
    if (avgLevel < 0.1f && avgLevel > 0.01f) {
        volumeAdjust = m_targetLevel / (avgLevel + 0.001f);
        volumeAdjust = min(volumeAdjust, 3.0f); 
    } else if (avgLevel > m_targetLevel) {
        volumeAdjust = m_targetLevel / avgLevel;
    }

    m_targetVolume = m_envelope * volumeAdjust;
    
    m_targetVolume = max(0.3f, min(1.2f, m_targetVolume));

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

    return sqrt(sum / static_cast<float>(totalSamples));
}

float AudioCompressor::LinearToDb(float linear) {
    if (linear <= 0.0f) {
        return -100.0f; 
    }
    return 20.0f * log10(linear);
}

float AudioCompressor::DbToLinear(float db) {
    return pow(10.0f, db / 20.0f);
}

