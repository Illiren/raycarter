#pragma once

#include "stddefines.hpp"
#include <AL/al.h>
#include <AL/alc.h>

class Audio
{
public:
    Audio();
    ~Audio();

    bool play();
    void load(TString filename);
    static TArray<TString> getAvailableDevices(ALCdevice *device);

private:
    uint8_t         channel;
    int32_t         sampleRate;
    uint8_t         bitPerSample;
    ALsizei         size;
    TArray<char>    data;

    ALCdevice *openALDevice;
    ALCcontext *openALContext;
    ALCboolean contextMadeCurrent;


};
