#include <portaudio.h>
#include <audioDecoder/include/wav_decoder.hpp>


static int audioCallback(const void *inputBuffer, void *outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo *timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void *userData)
{
    wavDecoder decodedInfo = *(wavDecoder *)userData;
    static int currentPosition = 0;
    unsigned short *pcmData = (unsigned short *)decodedInfo.getRawData();
    unsigned short *out = (unsigned short *)outputBuffer;
    int i;
    (void)inputBuffer; // Prevent unused variable warning

    for (i = 0; i < framesPerBuffer; i++)
    {
        for (int channel = 0; channel < decodedInfo.getChannelCount(); channel++)
        {
            if (currentPosition < decodedInfo.getRawDataSize() / sizeof(unsigned short))
            {
                // Copy PCM data to output buffer
                *out++ = pcmData[currentPosition]; // Left channel
                *out++ = pcmData[currentPosition]; // Right channel
                currentPosition++;
            }
            else
            {
                // If we reach the end of PCM data, stop playback
                return paComplete;
            }
        }
    }

    return paContinue;
}
