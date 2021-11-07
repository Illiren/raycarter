#pragma once

#include "stddefines.hpp"
#include <unordered_map>
#include <memory>
#include <AL/al.h>
#include <AL/alc.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>

struct Wav
{
    Wav(TString filename); //loader

    Wav() = default;
    Wav(const Wav &) = default;
    Wav(Wav &&) = default;
    Wav &operator=(const Wav &) = default;
    Wav &operator=(Wav &&) = default;

    explicit operator bool() const noexcept;

    uint8_t         channel;
    int32_t         sampleRate;
    uint8_t         bitPerSample;
    ALsizei         size = 0;
    TArray<char>    data;
};

using PSound = std::weak_ptr<Wav>;

class AudioDB
{
    using TShared = std::shared_ptr<Wav>;
    using TMapData = std::unordered_map<TString, TShared>;
public:
    AudioDB();

    bool load(TString filename, TString nameBase = "Undefined");
    PSound operator[](TString id) const;

private:
    TMapData _db;
    TString getName(TString baseName) const;
};


class Audio
{
public:
    Audio();
    ~Audio();

    bool play();
    bool play(PSound wav, bool repeatable = false);
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


struct SoundSequence
{
    SoundSequence() = default;
    SoundSequence(PSound s) :
          sound(s)
    {}

    void play();
    bool isPlayed() const;
    PSound sound;

private:
    std::weak_ptr<bool> playstate;
    friend class NAudio;
};


class NAudio
{
    using Thread = std::thread;
    using TMutex = std::mutex;
    using Task = std::function<void ()>;
    using TCondVar = std::condition_variable;
    template<typename T>
    using TQueue = std::queue<T>;

public:
    friend NAudio &GetAudio();

    std::weak_ptr<bool> play(PSound wav, bool repeatable = false);
    static TArray<TString> getAvailableDevices(ALCdevice *device);

    ~NAudio();
private:
    ALCdevice  *openALDevice;
    ALCcontext *openALContext;
    ALCboolean contextMadeCurrent;
    TArray<Thread> _worker;
    TQueue<Task>   _tasks;
    TMutex         _queueMutex;
    TCondVar       _cond;
    bool           _running;


    inline void enqueue(const Task &f);
    inline void enqueue(Task &&f);

    NAudio();
};

NAudio &GetAudio();
