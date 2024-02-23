#pragma once
#include <portaudio.h>
#include <audioDecoder/include/wav_decoder.hpp>

typedef struct {
    unsigned short *pcmData;
    int rawDataSize;
} paTestData;

typedef struct {
    unsigned int *pcmData;
    int rawDataSize;
} paTestData24;

template <typename t>
struct PaData {
    int dataSize;
    t *pcmData;
};

volatile int playbackFinished = 0;

template <typename t>
static int wPlayCallback  (const void *inputBuffer, void *outputBuffer,
                              unsigned long framesPerBuffer,
                              const PaStreamCallbackTimeInfo *timeInfo,
                              PaStreamCallbackFlags statusFlags,
                              void *userData)
{
    t *out = (t *)outputBuffer;
    PaData<t> *test_data = (PaData<t> *)userData;
    static t *pcmData = test_data->pcmData;
    PaStreamCallbackResult result = paContinue;

    for (size_t i = 0; i < framesPerBuffer && (test_data->dataSize > 0); i ++) {
        *out++ = *pcmData++;
        *out++ = *pcmData++;
    }

    test_data->dataSize = test_data->dataSize - framesPerBuffer;

    if (test_data->dataSize <= 0) {
        result = paComplete;
    }

    return result;
}

void streamFinishedCallback(void* userData) {
    (void)userData;
    playbackFinished = 1; // Set playback finished flag
}