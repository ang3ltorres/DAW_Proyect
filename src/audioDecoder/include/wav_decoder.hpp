#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <typeinfo>

class wavDecoder {
    public:
        wavDecoder();
        wavDecoder(const std::string &);
        ~wavDecoder ();
        //Public methods

        //Loads file and extracts file headers
        int loadFile(const std::string &);
        //Information getters
        //Returns channel count
        unsigned short getChannelCount() const;
        //Returns sampleRate in Hz
        unsigned int getSampleRate () const;
        //Returns PCM value, if not one, it has some form of compression
        unsigned short PCM_value () const;
        //Returns a pointer to a dinamycally allocated C type array, that
        //contains the raw audio Data in PCM format.
        //receives the channel and returns nullptr if channel if
        //the channel is larger than the channel count.
        unsigned short getBlockAlign () const;
        unsigned short getBitsPerSample () const;
        unsigned int getByteRate () const;
        int *getChannelData (const size_t &) const;
        unsigned short *getRawData () const;
        unsigned int getRawDataSize () const;
        enum Error {
            NO_ERROR = 0,
            NOT_WAV_FILE,
            NOT_RIFF,
            NOT_WAVE,
            NOT_FMT,
            NOT_DATA,
            FAIL_OPEN,
            FAIL_READ,
            ALLOC_FAILED
        };
    private:
        struct {
            const std::streamsize RIFF_HEADER = 8;
            const std::streamsize CHUNK_ID_SIZE = 4;
            const  std::streamsize CHUNK_SZ_SIZE = 4;
        };

        unsigned short channelCount;
        unsigned short PCM;
        unsigned short bitsPerSample;
        unsigned short blockAlign;
        unsigned int sampleRate;
        unsigned int byteRate;


        unsigned short *rawData;
        unsigned int rawDataSize;
        char *buffer;
        std::string filePath;
        // std::vector<char *> channelData;
        /*
            Private methods
        */
        unsigned int _readRiff (int &, std::ifstream &);
        void _read_fmt (int &,unsigned int &);
        bool _hasSuffix () const;
        static inline bool _isID (const std::string &, const std::string &);
    //     inline unsigned int _getUint (char *);
        template <typename t>
        inline t _getTypeVal(char *);
        inline void _cleanBuffer();
        inline void _cleanRawData();
        void _offsetBuffer(const size_t &,unsigned int &, int &);
        void _offsetCharray(char *&, const size_t &, unsigned int &, int &);
       /*
            Private constants
       */
        const std::string SUFFIX = ".wav";
        const std::string RIFF_ID = "RIFF";
        const std::string WAVE_ID = "WAVE";
        const std::string FMT_ID = "fmt ";
        const std::string DATA_ID = "data";
};