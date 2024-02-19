#include <../include/wav_decoder.hpp>
//Public methods
wavDecoder::wavDecoder() :
    channelCount(0),
    sampleRate(0),
    PCM(0),
    filePath(""),
    rawData16bit(nullptr),
    rawData24bit(nullptr),
    buffer(nullptr),
    blockAlign(0),
    bitsPerSample(0),
    byteRate(0),
    rawDataSize(0)
    {}

int wavDecoder::loadFile(const std::string &filePath)
{
    this -> filePath = filePath;
    unsigned int file_size = 0;

    int rc = _hasSuffix() ? Error::NO_ERROR : Error::NOT_WAV_FILE;

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
            _cleanBuffer();
            rc = Error::FAIL_READ;
        }
        else {
            wavFileStrm.close();
        }
    }

    if (!rc) {
        rc = _isID(WAVE_ID, std::string(buffer, CHUNK_ID_SIZE)) ? Error::NO_ERROR : Error::NOT_WAVE;
    }

    if (!rc) {
        _offsetBuffer(CHUNK_ID_SIZE, file_size, rc);
    }

    if (!rc) {
        _read_fmt(rc, file_size);
    }

    if (!rc) {
        if (_isID(DATA_ID, std::string(buffer, CHUNK_ID_SIZE))) {
            _offsetBuffer(CHUNK_ID_SIZE, file_size, rc);
        } else {
            rc = Error::NOT_DATA;
        }
    }

    if (!rc) {
        rawDataSize = _getTypeVal<decltype(rawDataSize)>(buffer);
        _offsetBuffer(CHUNK_SZ_SIZE, file_size, rc);
    }

    if (!rc) {
        if (bitsPerSample == 16) {
            rawData16bit = (decltype(rawData16bit))buffer;
            buffer = nullptr;
        } else if (bitsPerSample == 24) {
            _get24bitRawData(rc);
        }
    }
    return rc;
}

void wavDecoder::_get24bitRawData(int &rc)
{
    size_t data_sample_size = rawDataSize / (bitsPerSample / 8);
    const size_t byte_data_sample_size = 3;
    rawData24bit = new unsigned int [data_sample_size];
    if (rawData24bit == nullptr) rc = Error::ALLOC_FAILED;

    if (!rc) {
        for (size_t i = 0; i < data_sample_size; i ++) {
            char tmp_bffr[sizeof(int)] = {0};
            std::copy(buffer + (i * byte_data_sample_size),
                      buffer + (i * byte_data_sample_size) + byte_data_sample_size,
                      tmp_bffr + 1);

            rawData24bit[i] = *(unsigned int *)tmp_bffr;
        }

        _cleanBuffer();
    }
}

unsigned short wavDecoder::getChannelCount() const
{
    return channelCount;
}

unsigned short wavDecoder::PCM_value() const
{
    return PCM;
}

unsigned int wavDecoder::getSampleRate() const
{
    return sampleRate;
}

void *wavDecoder::getRawData() const
{
    void *r_ptr = nullptr;
    if (bitsPerSample == 16) {
        r_ptr = rawData16bit;
    } else if (bitsPerSample == 24) {
        r_ptr = rawData24bit;
    }

    return r_ptr;
}

unsigned int wavDecoder::getRawDataSize() const
{
    return rawDataSize;
}

unsigned short wavDecoder::getBlockAlign() const
{
    return blockAlign;
}

unsigned short wavDecoder::getBitsPerSample() const
{
    return bitsPerSample;
}

unsigned int wavDecoder::getByteRate() const
{
    return byteRate;
}

//Private methods
bool wavDecoder::_hasSuffix() const
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
                rc = _isID(RIFF_ID, std::string(buffer, CHUNK_ID_SIZE)) ? Error::NO_ERROR : Error::NOT_RIFF;
            } else {
                _cleanBuffer();
                rc = Error::FAIL_READ;
            }
        }
    }
    if (!rc) {
        file_size =  _getTypeVal<decltype(file_size)>(this -> buffer + CHUNK_ID_SIZE);
        _cleanBuffer();
    }
    return file_size;
}

void wavDecoder::_read_fmt(int &rc, unsigned int &file_size)
{
    unsigned int fmt_size = 0;
    char *fmt_subchunk = nullptr;
    char *aux = nullptr;

    if (!rc) {
        rc = _isID(FMT_ID, std::string(buffer, CHUNK_ID_SIZE)) ? Error::NO_ERROR : Error::NOT_FMT;
    }

    if (!rc) {
        _offsetBuffer(CHUNK_ID_SIZE, file_size, rc);
    }

    if (!rc) {
        fmt_size = _getTypeVal<decltype(fmt_size)>(buffer);
        _offsetBuffer(CHUNK_SZ_SIZE, file_size, rc);
    }

    if (!rc){
        fmt_subchunk = new char[fmt_size];
        if (fmt_subchunk == nullptr) rc = Error::ALLOC_FAILED;
        //Read PCM and channelCount values
        if (!rc) {
            std::copy (buffer, buffer + fmt_size, fmt_subchunk);
            _offsetBuffer(fmt_size, file_size, rc);
        }
        if (!rc) {
            int field_offset = sizeof(channelCount) + sizeof(PCM);

            PCM = _getTypeVal<decltype(PCM)>(fmt_subchunk);
            channelCount = _getTypeVal<decltype(channelCount)>(fmt_subchunk + sizeof(channelCount));

            _offsetCharray(fmt_subchunk, field_offset, fmt_size, rc);
        }
        //Read sampleRate
        if (!rc) {
            int field_offset = sizeof(sampleRate);

            sampleRate = _getTypeVal<decltype(sampleRate)>(fmt_subchunk);

            _offsetCharray(fmt_subchunk, field_offset, fmt_size, rc);
        }

        if (!rc) {
            int field_offset = sizeof(blockAlign) + sizeof(bitsPerSample);

            byteRate = _getTypeVal<decltype(byteRate)>(fmt_subchunk);
            blockAlign = _getTypeVal<decltype(blockAlign)>(fmt_subchunk + sizeof(byteRate));
            bitsPerSample = _getTypeVal<decltype(bitsPerSample)>(fmt_subchunk + sizeof(byteRate) + sizeof(blockAlign));
        }
        delete [] fmt_subchunk;
    }
}

void wavDecoder::_offsetCharray(char *& charray, const size_t &offset, unsigned int &file_size, int &rc)
{
    char *aux = charray;
    file_size -= offset;
    charray = new char[file_size];

    if (charray == nullptr) rc = Error::ALLOC_FAILED;

    if (!rc) {
        std::copy(aux + offset, aux + offset + file_size, charray);

    }
    delete [] aux;
}

void wavDecoder::_offsetBuffer(const size_t &offset,unsigned int &file_size, int &rc)
{
    _offsetCharray(buffer, offset, file_size, rc);
}

inline bool wavDecoder::_isID(const std::string &id, const std::string &str)
{
    return id == str;
}

template <typename t>
inline t wavDecoder::_getTypeVal(char *file)
{
    return *(t *)(file);
}

inline void wavDecoder::_cleanBuffer()
{
    delete [] buffer;
    buffer = nullptr;
}

inline void wavDecoder::_cleanRawData()
{
    delete []rawData16bit;
    rawData16bit = nullptr;

    delete [] rawData24bit;
    rawData24bit = nullptr;
}

wavDecoder::~wavDecoder()
{
    _cleanBuffer();
    _cleanRawData();
}