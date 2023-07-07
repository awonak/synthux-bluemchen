#include <algorithm>

#include "daisysp.h"
#include "kxmx_bluemchen.h"

#include "looper.h"

using namespace kxmx;
using namespace daisy;
using namespace daisysp;

Bluemchen bluemchen;

Parameter knob1;
Parameter knob2;
Parameter cv1;
Parameter cv2;


// Allocate buffer in SDRAM
static const uint32_t kBufferLengthSec = 5;
static const uint32_t kSampleRate = 48000;
static const size_t kBufferLenghtSamples = kBufferLengthSec * kSampleRate;
static float DSY_SDRAM_BSS buffer[kBufferLenghtSamples];

static synthux::Looper looper;
static PitchShifter pitch_shifter;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
    for (size_t i = 0; i < size; i++) {
        float looper_out = looper.Process(in[1][i]);
        out[0][i] = out[1][i] = pitch_shifter.Process(looper_out);
    }
}

int main(void) {
    bluemchen.Init();
    bluemchen.StartAdc();

    knob1.Init(bluemchen.controls[bluemchen.CTRL_1], 0.0f, 1.0f, Parameter::LINEAR);
    knob2.Init(bluemchen.controls[bluemchen.CTRL_2], 0.0f, 1.0f, Parameter::LINEAR);

    cv1.Init(bluemchen.controls[bluemchen.CTRL_3], 0.0f, 1.0f, Parameter::LINEAR);
    cv2.Init(bluemchen.controls[bluemchen.CTRL_4], 0.0f, 1.0f, Parameter::LINEAR);

    bluemchen.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
    float samplerate = bluemchen.AudioSampleRate();

    // Setup looper
    looper.Init(buffer, kBufferLenghtSamples);

    // Setup pitch shifter
    pitch_shifter.Init(samplerate);

    bluemchen.StartAudio(AudioCallback);

    for (;;) {
        bluemchen.ProcessAllControls();
        
        // Set loop parameters
        float loop_start = knob1.Value();
        float loop_length = knob2.Value();
        looper.SetLoop(loop_start, loop_length);

        // Toggle record
        looper.SetRecording(bluemchen.encoder.RisingEdge());

        // Set pitch - clamp value within a ±12 semitone range.
        int read = bluemchen.encoder.Increment();
        float pitch_val = std::clamp(read, -12, 12);
        pitch_shifter.SetTransposition(pitch_val / 12);
    }
}