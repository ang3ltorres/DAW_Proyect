#include <../include/wav_decoder.hpp>
//Public methods
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
    int rc;
    _hasSuffix() ? rc = 0 : rc = 1;

    if (!rc) {
        rc = _readHeader();
    }

    return rc;
}

unsigned short wavDecoder::getChannelCount() const
{
    return 0;
}

unsigned int wavDecoder::PCM_value () const
{
    return false;
}

size_t wavDecoder::getSampleRate() const
{
    return 0;
}

int *wavDecoder::getChannelData (const size_t &channel) const
{
    return nullptr;
}

int *wavDecoder::getRawData () const
{
    return nullptr;
}

//Private methods
bool wavDecoder::_hasSuffix () const
{
    if (_suffix.size() > filePath.size()) return false;
    return std::equal(_suffix.rbegin(), _suffix.rend(), filePath.rbegin());
}

int wavDecoder::_readHeader() {
    //TODO: implement this method ASAP
    return 0;
}