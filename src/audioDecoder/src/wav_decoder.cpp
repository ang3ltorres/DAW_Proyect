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
    int file_size = 0;
    std::ifstream wavFileStrm;
    if (!rc) {
        wavFileStrm = std::ifstream(this -> filePath, std::ios::binary);
        file_size = _readRiff(rc, wavFileStrm);
    }

    if (!rc) {
        buffer = new char[file_size];
        if (buffer == nullptr) rc = Error::ALLOC_FAILED;
    }

    if (!rc) {
        if (!wavFileStrm.read(buffer, file_size)) {
            delete [] this -> buffer;
            rc = Error::FAIL_READ;
        }
        else {
            wavFileStrm.close();
        }
    }

    if (!rc) {
        rc = _isID(WAVE_ID, std::string(buffer, CHUNK_ID_SIZE)) ? Error::NO_ERROR : Error::NOT_WAVE;
        file_size -= CHUNK_ID_SIZE;
    }

    if (!rc) {
        _read_fmt(rc, file_size);
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
    if (SUFFIX.size() >= filePath.size()) hasIt = false;
    if (hasIt) hasIt = std::equal(SUFFIX.rbegin(), SUFFIX.rend(), filePath.rbegin());

    return hasIt;
}

unsigned int wavDecoder::_readRiff(int &rc, std::ifstream &wavFileStrm)
{

    unsigned int file_size = 0;
    if (!wavFileStrm.is_open()) {
        rc = Error::FAIL_OPEN;
    } else {
        buffer = new char[RIFF_HEADER];
        if (buffer == nullptr) rc = Error::ALLOC_FAILED;

        if (!rc) {
            if (wavFileStrm.read(buffer, RIFF_HEADER)) {
                _isID(RIFF_ID, std::string(buffer, CHUNK_ID_SIZE)) ? rc = Error::NO_ERROR : Error::NOT_RIFF;
            } else {
                _cleanBuffer();
                rc = Error::FAIL_READ;
            }
        }
    }
    if (!rc) {
        file_size =  _getFileSize(this -> buffer + CHUNK_ID_SIZE);
        _cleanBuffer();
    }
    return file_size;
}

void wavDecoder::_read_fmt (int &rc, int &file_size)
{
    char *tmp_buffer = buffer;
    buffer = new char[file_size];
    char *str_begin = tmp_buffer + CHUNK_ID_SIZE;
    char *str_end = str_begin + file_size;

    std::copy (str_begin, str_end, buffer);
    delete [] tmp_buffer;


}

inline bool wavDecoder::_isID (const std::string &id, const std::string &str)
{
    return id == str;
}

inline unsigned int wavDecoder::_getFileSize(char *file_size)
{
    return *(unsigned int *)(file_size);
}

inline void wavDecoder::_cleanBuffer()
{
    delete [] buffer;
    buffer = nullptr;
}

wavDecoder::~wavDecoder()
{
    _cleanBuffer();
}