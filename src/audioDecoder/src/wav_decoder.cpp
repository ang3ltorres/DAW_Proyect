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
    int rc = _hasSuffix() ? Error::NO_ERROR : Error::NOT_WAV_FILE;
    if (!rc) {
        _readHeader(rc);
    }
    if (!rc) {
        _read_fmt(rc);
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
    bool hasIt;
    if (_suffix.size() >= filePath.size()) hasIt = false;
    if (hasIt) hasIt = std::equal(_suffix.rbegin(), _suffix.rend(), filePath.rbegin());

    return hasIt;
}

void wavDecoder::_readHeader(int &rc)
{
    std::ifstream wavFileStrm(this -> filePath, std::ios::binary);
    struct stat sb;
    if (!wavFileStrm.is_open()) {
        rc = Error::FAIL_OPEN;
    } else {
        this -> buffer = new char[RIFF_HEADER];
        if (wavFileStrm.read(buffer, RIFF_HEADER)) {
            _isRIFF(std::string(buffer, CHUNK_ID_SIZE)) ? rc = Error::NO_ERROR : Error::NOT_RIFF;
        } else {
            delete [] this -> buffer;
            rc = Error::FAIL_READ;
        }
    }
    if (!rc) {
        unsigned int file_size =  _getFileSize(this -> buffer + CHUNK_ID_SIZE);
        delete[] this -> buffer;

        this -> buffer = new char[file_size];
        if (wavFileStrm.read(buffer, file_size)) {
            delete [] this -> buffer;
            rc = Error::FAIL_READ;
        }
    }
}



inline bool wavDecoder::_isRIFF(const std::string &riff_str) const
{
    return RIFF_ID == riff_str;
}

inline unsigned int wavDecoder::_getFileSize(char *file_size)
{
    return *(unsigned int *)(file_size);
}