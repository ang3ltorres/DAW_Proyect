#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

class wavDecoder {
    public:
        wavDecoder();
        wavDecoder(const std::string &);
        //Public methods

        //Loads file and extracts file headers
        int loadFile(const std::string &);
        //Information getters
        //Returns channel count
        unsigned short getChannelCount() const;
        //Returns sampleRate in Hz
        size_t getSampleRate () const;
        //Returns if is PCM, else, it may have a form of compression that needs
        //it's way to handle it in the player
        unsigned int PCM_value () const;
        //Returns a pointer to a dinamycally allocated C type array, that
        //contains the raw audio Data in PCM format.
        //receives the channel and returns nullptr if channel if
        //the channel is larger than the channel count.
        int *getChannelData (const size_t &) const;
        int *getRawData () const;
    private:
        unsigned short channelCount;
        size_t sampleRate;
        unsigned int PCM;
        std::string filePath;
        const std::string _suffix = ".wav";
        //Im thinking in leave this with a C array, because the pointer
        //May be to difficult to handle with an std::array
        std::vector<int *> channelData;
        int *rawData;
        /*
            Private methods
        */
       bool _hasSuffix () const;
       int _readHeader ();
};