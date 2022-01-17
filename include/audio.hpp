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

struct SoundSequence
{
    SoundSequence() = default;
    SoundSequence(PSound s);

    void play();
    bool isPlayed() const;
    PSound sound;

private:
    std::weak_ptr<bool> playstate;
    friend class Audio;
};


class Audio
{
    using Thread = std::thread;
    using TMutex = std::mutex;
    using Task = std::function<void ()>;
    using TCondVar = std::condition_variable;
    template<typename T>
    using TQueue = std::queue<T>;

public:
    friend Audio &GetAudio();

    std::weak_ptr<bool> play(PSound wav, bool repeatable = false);
    std::weak_ptr<bool> playTest(PSound wav, bool repeatable = false);


    static TArray<TString> getAvailableDevices(ALCdevice *device);

    ~Audio();
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

    Audio();
};

Audio &GetAudio();



class AudioTest
{
    static constexpr TSize NumBuffers = 4;
    static constexpr ALsizei BufferSize = 65536;
public:
    AudioTest();
    ~AudioTest();

    void play(const Wav &wav);


private:
    ALCdevice  *openALDevice;
    ALCcontext *openALContext;
    ALCboolean contextMadeCurrent;

};
