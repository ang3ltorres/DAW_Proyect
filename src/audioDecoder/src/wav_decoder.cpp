#include <../include/wav_decoder.hpp>

wavDecoder::wavDecoder() :
    channelCount(0),
    sampleRate(0),
    PCM(0),
    filePath(""),
    rawData(nullptr)
    {}

int wavDecoder::loadFile(const std::string &filePath)
{
    this -> filePath = filePath;
    // TODO create and call the header decoder
    return 0;
}

unsigned short wavDecoder::getChannelCount() const
{
    return 0;
}

bool wavDecoder::isPCM () const
{
    return false;
}

int *wavDecoder::getChannelData (const size_t &channel) const
{
    return nullptr;
}
