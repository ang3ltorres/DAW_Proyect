#pragma once
#include <portaudio.h>
#include <audioDecoder/include/wav_decoder.hpp>

typedef struct {
    unsigned short *pcmData;
    int rawDataSize;
} paTestData;

volatile int playbackFinished = 0;

static int wav_play_callback (const void *inputBuffer, void *outputBuffer,
                              unsigned long framesPerBuffer,
                              const PaStreamCallbackTimeInfo *timeInfo,
                              PaStreamCallbackFlags statusFlags,
                              void *userData) {

    unsigned short *out = (unsigned short *)outputBuffer;
    paTestData *test_Data = (paTestData *)userData;
    static unsigned short *pcmData = test_Data->pcmData;
    PaStreamCallbackResult result = paContinue;

    for (size_t i = 0; i < framesPerBuffer; i ++) {
        *out++ = *pcmData++;
        *out++ = *pcmData++;
    }

    test_Data->rawDataSize = test_Data->rawDataSize - framesPerBuffer;

    if (test_Data->rawDataSize <= 0) {
        result = paComplete;
    }

    return result;
}

void streamFinishedCallback(void* userData) {
    (void)userData;
    playbackFinished = 1; // Set playback finished flag
}