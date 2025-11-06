#pragma once

#include <vector>
#include <cmath>
#include <algorithm>

class AudioCompressor {
public:
    AudioCompressor();

    float ProcessAudio(float* audioData, size_t frameCount, int channels);

    void SetThreshold(float threshold) { m_threshold = threshold; } 
    void SetRatio(float ratio) { m_ratio = ratio; }
    void SetAttackTime(float ms) { m_attackTime = ms; }
    void SetReleaseTime(float ms) { m_releaseTime = ms; }
    void SetTargetLevel(float level) { m_targetLevel = level; } 

    float GetCurrentLevel() const { return m_currentLevel; }
    float GetTargetVolume() const { return m_targetVolume; }

private:
    float CalculateRMS(float* audioData, size_t frameCount, int channels);
    float LinearToDb(float linear);
    float DbToLinear(float db);
    float m_threshold;      
    float m_ratio;          
    float m_attackTime;     
    float m_releaseTime;    
    float m_targetLevel;    
    float m_currentLevel;   
    float m_targetVolume;   
    float m_envelope;       
    std::vector<float> m_levelHistory;
    size_t m_historySize;
    size_t m_historyIndex;
};


