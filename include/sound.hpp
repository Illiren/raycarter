#pragma once

#include <fstream>
#include <iostream>
#include "stddefines.hpp"

//Sound data.
struct Sound
{
    enum Type
    {
        OGG,
        WAV
    };

    Sound() = default;
    Sound(TString filename, Type type = WAV)
    {
        switch(type)
        {
        case WAV:
            loadWav(filename);
            break;
        default:
            std::cerr << "Unsupported type" << std::endl;
        }
    }
    Sound(const Sound &) = default;
    Sound(Sound &&) = default;
    ~Sound() = default;

    Sound &operator = (const Sound &) = default;
    Sound &operator = (Sound &&) = default;

    inline explicit operator bool() const noexcept
    {
        return !buffer.empty();
    }

    uint16_t bps;    //beat per second
    uint16_t format; //format
    TUint32 sampleRate;
    TUint32 bitRate;
    TUint32 channels;
    TReal length;
    TArray<char> buffer;


private:
    static bool readChunkHeader(std::ifstream &file, std::string &name, uint32_t &size)
    {
        char chunk[4];
        if(file.eof()) return false;
        file.read(chunk,4);
        if(file.eof()) return false;
        file.read((char*)&size,4);
        name = TString(chunk,4);
        return true;
    }


    void loadWav(TString filename)
    {
        using namespace std;

        struct SoundChunk
        {
            uint16_t format;
            uint16_t channelsNum;
            uint32_t sampleRate;
            uint32_t byteRate;
            uint16_t blockAlign;
            uint16_t bps;
        };

        ifstream file(filename,ios::in | ios::binary);
        if(!file)
        {
            cerr << "Sound::loadWav: Can't open file" << endl;
            return ;
        }

        string chunkName;
        unsigned int chunkSize;
        int size = 0;

        while(readChunkHeader(file,chunkName,chunkSize))
        {
            if(chunkName == "RIFF")
            {
                char type[4];
                file.read(type,4);
            }
            else if(chunkName == "fmt ")
            {
                SoundChunk fmt;

                file.read((char*)&fmt, sizeof(SoundChunk));
                bitRate = fmt.byteRate;
                sampleRate = (float)fmt.sampleRate;
                channels = fmt.channelsNum;
                format = fmt.format;
                bps = fmt.bps;
            }
            else if(chunkName == "data")
            {
                size = chunkSize;
                buffer.resize(size);
                file.read(buffer.data(), chunkSize);
            }
            else
            {
                file.seekg(chunkSize, ios_base::cur);
            }
            if(file.tellg() < 0)
            {
                cout << "Unexpected eof " << endl;
                return;
            }
        }

        file.close();
        length = (float)size/(channels*(float)sampleRate*(bitRate/8.f))*1000.f;
    }
};
