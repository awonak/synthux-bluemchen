#pragma once

#include "daisy_seed.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

class Vox {
   public:
    static constexpr float kOscLowestFreq = 36;

    void Init(float sampleRate, float oscHighestFreq) {
        // OSCILLATOR SETUP
        _osc.Init(sampleRate);
        _osc.SetWaveform(Oscillator::WAVE_SAW);
        _oscHighestFreq = oscHighestFreq;

        // LFO SETUP
        _lfo.Init(sampleRate);
        _lfo.SetFreq(randLFOFreq());
        _lfo.SetWaveform(_lfo.WAVE_SIN);
    }

    /**
     * Update the oscillator pitch and lfo frequency.
     *
     * @param pitch Float between 0 and 1.0.
     * @param freq Float between 0 and 1.0.
     */
    void Update(float pitch, float freq) {
        auto oscFreqOffset = fmap(pitch, 0, 500);
        auto oscFreqMin = kOscLowestFreq + oscFreqOffset;
        auto oscFreqMax = _oscHighestFreq + oscFreqOffset;
        _oscFreq = fmap(freq, oscFreqMin, oscFreqMax);
        auto lfoAmp = fmap(freq, 0.f, 0.005);
        _lfo.SetAmp(lfoAmp);
    }

    float Process() {
        _osc.SetFreq(_oscFreq * (1.f + _lfo.Process()));
        return _osc.Process();
    }

   private:
    Oscillator _osc;
    Oscillator _lfo;
    float _oscFreq;
    float _oscHighestFreq;

    // Returns a random float between 5.0 and 15.0.
    float randLFOFreq() {
        return daisy::Random::GetFloat(5.0, 15.0);
    }
};
