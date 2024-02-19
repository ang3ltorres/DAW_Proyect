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

static int wav_play_callback_24 (const void *inputBuffer, void *outputBuffer,
                              unsigned long framesPerBuffer,
                              const PaStreamCallbackTimeInfo *timeInfo,
                              PaStreamCallbackFlags statusFlags,
                              void *userData) {

    unsigned int *out = (unsigned int *)outputBuffer;
    paTestData24 *test_Data = (paTestData24 *)userData;
    static unsigned int *pcmData = test_Data->pcmData;
    PaStreamCallbackResult result = paContinue;

    if (test_Data->rawDataSize <= 0) {
        result = paComplete;
    }

    for (size_t i = 0; i < framesPerBuffer; i ++) {
        *out++ = *pcmData++;
        *out++ = *pcmData++;
    }

    test_Data->rawDataSize = test_Data->rawDataSize - framesPerBuffer;

    return result;
}

void streamFinishedCallback(void* userData) {
    (void)userData;
    playbackFinished = 1; // Set playback finished flag
}