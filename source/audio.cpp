#include "audio.hpp"
#include <fstream>
#include <iostream>
#include <AL/al.h>
#include <AL/alc.h>
#include <bit>
#include <cstring>


bool checkALErrors(const TString &filename, const std::uint_fast32_t line)
{
    ALenum error;
    error = alGetError();

    if(error != AL_NO_ERROR)
    {
        std::cout << "Error " << filename << ": " << line << "\n";
        switch (error)
        {
        case AL_INVALID_NAME:
            std::cout << "Invalid name: a bad name (ID) was passed to an OpenAL function";
            break;
        case AL_INVALID_ENUM:
            std::cout << "Invalid enum: an invalid enum value was passed to an OpenAL function";
            break;
        case AL_INVALID_VALUE:
            std::cout << "Invalid enum: an invalid enum value was passed to an OpenAL function";
            break;
        case AL_INVALID_OPERATION:
            std::cout << "Invalid operation: the requested operation is not valid";
            break;
        case AL_OUT_OF_MEMORY:
            std::cout << "Out of memory: the requested operation resulted in OpenAL running out of memory";
            break;
        default:
            std::cout << "Unknown AL Error: " << error;
            break;
        }
        std::cout << std::endl;
        return false;
    }
    return true;
}

bool checkALCErrors(const TString &filename, const std::uint_fast32_t line, ALCdevice* device)
{
    ALCenum error;
    error = alcGetError(device);

    if(error != ALC_NO_ERROR)
    {
        std::cout << "Error " << filename << ": " << line << "\n";
        switch (error)
        {
        case ALC_INVALID_DEVICE:
            std::cout << "Invalid device: a bad device was passed to an OpenAL function";
            break;
        case ALC_INVALID_ENUM:
            std::cout << "Invalid enum: an invalid enum value was passed to an OpenAL function";
            break;
        case ALC_INVALID_CONTEXT:
            std::cout << "Invalid context: a bad context was passed to an OpenAL function";
            break;
        case ALC_INVALID_VALUE:
            std::cout << "Invalid enum: an invalid enum value was passed to an OpenAL function";
            break;
        case ALC_OUT_OF_MEMORY:
            std::cout << "Out of memory: the requested operation resulted in OpenAL running out of memory";
            break;
        default:
            std::cout << "Unknown ALC Error: " << error;
            break;
        }
        std::cout << std::endl;
        return false;
    }
    return true;
}

template<typename alcFunction, typename... Params>
auto alcCallImpl(const char *filename,
                 const std::uint_fast32_t line,
                 alcFunction function,
                 ALCdevice *device,
                 Params... params)
    -> typename std::enable_if_t<std::is_same_v<void, decltype(function(params...))>, bool>
{
    function(std::forward<Params>(params)...);
    return checkALCErrors(filename, line, device);
}

template<typename alcFunction, typename ReturnType, typename... Params>
auto alcCallImpl(const char *filename,
                 const std::uint_fast32_t line,
                 alcFunction function,
                 ReturnType &returnValue,
                 ALCdevice *device,
                 Params... params)
    -> typename std::enable_if_t<!std::is_same_v<void, decltype(function(params...))>, bool>
{
    returnValue = function(std::forward<Params>(params)...);
    return checkALCErrors(filename, line, device);
}

template<typename alcFunction, typename... Params>
auto alCallImpl(const char *filename,
                const std::uint_fast32_t line,
                alcFunction function,
                Params... params)
    -> typename std::enable_if_t<std::is_same_v<void, decltype(function(params...))>, bool>
{
    function(std::forward<Params>(params)...);
    return checkALErrors(filename, line);
}

template<typename alcFunction, typename ReturnType, typename... Params>
auto alCallImpl(const char *filename,
                 const std::uint_fast32_t line,
                 alcFunction function,
                 ReturnType &returnValue,
                 Params... params)
    -> typename std::enable_if_t<!std::is_same_v<void, decltype(function(params...))>, bool>
{
    returnValue = function(std::forward<Params>(params)...);
    return checkALErrors(filename, line);
}

#define alCall(function, ...) alCallImpl(__FILE__, __LINE__, function, __VA_ARGS__)
#define alcCall(function, ...) alcCallImpl(__FILE__, __LINE__, function, __VA_ARGS__)

std::int32_t convertToInt(char *buffer, std::size_t len)
{
    std::int32_t a = 0;
    if(std::endian::native == std::endian::little)
        std::memcpy(&a, buffer, len);
    else
        for(std::size_t i = 0; i < len; ++i)
            reinterpret_cast<char*>(&a)[3-i]=buffer[i];
    return a;
}

bool loadWAV(std::ifstream &file,
             std::uint8_t &channels,
             std::int32_t &sampleRate,
             std::uint8_t &bitPerSample,
             ALsizei &size)
{
    using namespace std;

    if(!file.is_open())
    {
        cout << "Could'n open the file" << endl;
        return false;
    }

    char buffer[4];

    if(!file.read(buffer,4))
    {
        cout << "Could'n read RIFF" << endl;
        return false;
    }

    if(std::strncmp(buffer,"RIFF",4) != 0)
    {
        cout << "RIFF is not RIFF" << endl;
        return false;
    }

    if(!file.read(buffer,4))
    {
        cout << "Could'n read size of file" << endl;
        return false;
    }

    if(!file.read(buffer,4))
    {
        //Error could not read wave
        cout << "Couldn't read WAVE" << endl;
        return false;
    }

    if(std::strncmp(buffer, "WAVE",4) != 0)
    {
        cout << "WAVE is not WAVE" << endl;
        return false;
    }

    if(!file.read(buffer,4)) //read fmt/0
    {
        cout << "Couldn't read fmt" << endl;
        return false;
    }

    if(!file.read(buffer,4)) // this is always 16,
    {
        cout << "Couldn't read the 16" << endl;
        return false;
    }

    if(!file.read(buffer,2))
    {
        //PCM
        cout << "Couldn't read the PCM" << endl;
        return false;
    }

    if(!file.read(buffer,2)) // channels count
    {
        cout << "Couldn't read channels count" << endl;
        return false;
    }

    channels = convertToInt(buffer,2);

    //sample rate
    if(!file.read(buffer, 4))
    {
        cout << "Couldn't read sample rate" << endl;
        return false;
    }

    sampleRate = convertToInt(buffer,4);

    if(!file.read(buffer,4))
    {
        cout << "couldn't read something" << endl;
        return false;
    }

    if(!file.read(buffer,2))
    {
        cout << "couldn't read dafaq" << endl;
        return false;
    }

    if(!file.read(buffer,2))
    {
        cout << "Couldn't read bits per sample" << endl;
        return false;
    }

    bitPerSample = convertToInt(buffer,2);

    //data chunk header "data"
    if(!file.read(buffer,4))
    {
        cout << "Couldn't read data header" << endl;
        return false;
    }

    file.read(buffer,4); // LIST
    file.read(buffer,4); // ....
    file.read(buffer,4); // INFO
    file.read(buffer,4); // ISFT
    file.read(buffer,4); // ....
    file.read(buffer,4); // Lavf
    file.read(buffer,4); // 58.2
    file.read(buffer,4); // 9.10
    file.read(buffer,2); // 0.

    if(std::strncmp(buffer,"tada",4) != 0)
    {
        cout << "Data is not data" << endl;
        return false;
    }

    //size of data
    if(!file.read(buffer,4))
    {
        cout << "Couldn't size of data" << endl;
        return false;
    }

    size = convertToInt(buffer,4);

    if(file.eof())
    {
        cout << "Unexpected eof" << endl;
        return false;
    }

    if(file.fail())
    {
        cout << "fail state set on the file" << endl;
        return false;
    }

    return true;
}

Wav::Wav(TString filename)
{
    std::ifstream in(filename, std::ios::binary);

    if(!loadWAV(in,channel,sampleRate,bitPerSample,size))
    {
        std::cout << "Could not load wav header of " << filename << std::endl;
        return;
    }

    data.resize(size);
    in.read(data.data(),size);
}

Wav::operator bool() const noexcept
{
    return !data.empty();
}

AudioDB::AudioDB()
{}

bool AudioDB::load(TString filename, TString nameBase)
{
    TString name = getName(nameBase);
    Wav *w = new Wav(filename);
    if(!w || !(*w))
    {
        std::cout << "Couldn't load wav" << std::endl;
        delete w;
        return false;
    }

    _db.insert({name, TShared(w)});
    return true;
}

PSound AudioDB::operator[](TString id) const
{
    return _db.at(id);
}

TString AudioDB::getName(TString baseName) const
{
    unsigned int counter = 0;
    TString newName = baseName;

    while(1) //THE BAD THING
    {
        auto it = _db.find(newName);

        if(it == _db.end())
            return newName;
        else
        {
            if(counter == std::numeric_limits<decltype(counter)>::max())
                break;
            newName = baseName+std::to_string(counter++);
        }
    }

    return "FUCK"; //Signals an error
}


std::weak_ptr<bool> Audio::play(PSound wav, bool repeatable)
{
    std::shared_ptr<bool> sharedBool(new bool);

    enqueue([this, wav, repeatable, sharedBool]()
            {
                if(wav.expired())
                {
                    std::cout << "Error sound is expired " << std::endl;
                    return;
                }
                auto sound = wav.lock();
                const auto channel = sound->channel;
                const auto bitPerSample = sound->bitPerSample;
                const auto data = sound->data.data();
                const auto size = sound->data.size();
                const auto sampleRate = sound->sampleRate;
                ALuint buffer;
                alCall(alGenBuffers,1,&buffer);
                ALenum format;
                if(channel == 1 && bitPerSample == 8)
                    format = AL_FORMAT_MONO8;
                else if(channel == 1 && bitPerSample == 16)
                    format = AL_FORMAT_MONO16;
                else if(channel == 2 && bitPerSample == 8)
                    format = AL_FORMAT_STEREO8;
                else if(channel == 2 && bitPerSample == 16)
                    format = AL_FORMAT_STEREO16;
                else
                {
                    //Error
                    return;
                }
                alCall(alBufferData, buffer, format, data, size, sampleRate);

                ALuint source;
                alCall(alGenSources,1,&source);
                alCall(alSourcef, source, AL_PITCH, 1);
                alCall(alSource3f, source, AL_POSITION, 0,0,0);
                alCall(alSource3f, source, AL_VELOCITY, 0,0,0);
                alCall(alSourcei, source, AL_LOOPING, AL_FALSE);
                alCall(alSourcei, source, AL_BUFFER, buffer);
                alCall(alSourcePlay, source);

                ALint state = AL_PLAYING;
                *sharedBool = true;
                while(state == AL_PLAYING && (*sharedBool == true))
                {
                    alCall(alGetSourcei, source, AL_SOURCE_STATE, &state);
                }

                alCall(alDeleteSources,1,&source);
                alCall(alDeleteBuffers,1,&buffer);
            });
    return sharedBool;
}

void Audio::enqueue(const Task &f)
{
    {
        std::unique_lock<TMutex> lock(_queueMutex);
        if(!_running)
            throw std::runtime_error("Enqueued on stopped thread pool");
        _tasks.emplace(f);
    }
    _cond.notify_one();
}

void Audio::enqueue(Task &&f)
{
    {
        std::unique_lock<TMutex> lock(_queueMutex);
        if(!_running)
            throw std::runtime_error("Enqueued on stopped thread pool");
        _tasks.emplace(std::move(f));
    }
    _cond.notify_one();
}

Audio::Audio()
{
    openALDevice = alcOpenDevice(nullptr);
    if(!openALDevice)
    {
        std::cout << "Audio error: couldn't open device" << std::endl;
        return;
    }

    if(!alcCall(alcCreateContext,openALContext,openALDevice,openALDevice, nullptr) || !openALContext)
    {
        std::cout << "Audio error: couldn't create context" << std::endl;
        return;
    }

    contextMadeCurrent = false;
    if(!alcCall(alcMakeContextCurrent,contextMadeCurrent,openALDevice,openALContext) || contextMadeCurrent != ALC_TRUE)
    {
        std::cout << "Audio error: couldn't make context current" << std::endl;
        return;
    }

    TSize threads = std::max(Thread::hardware_concurrency(), 1u);
    _running = true;
    for(TSize i=0;i<threads;++i)
    {
        _worker.emplace_back(
            [this]
            {
                while(true)
                {
                    Task task;

                    {
                        std::unique_lock<TMutex> lock(this->_queueMutex);
                        this->_cond.wait(lock, [this] {return !this->_running || !this->_tasks.empty();});

                        if(!this->_running && this->_tasks.empty())
                            return;
                        task = std::move(_tasks.front());
                        this->_tasks.pop();
                    }

                    task();
                }
            });
    }
}

Audio::~Audio()
{
    {
        std::unique_lock<TMutex> lock(_queueMutex);
        _running = false;
    }

    _cond.notify_all();

    for(auto &worker : _worker)
        worker.join();

    alcCall(alcMakeContextCurrent, contextMadeCurrent, openALDevice, nullptr);
    alcCall(alcDestroyContext,openALDevice, openALContext);
    ALCboolean closed;
    alcCall(alcCloseDevice, closed, openALDevice, openALDevice);
}

Audio &GetAudio()
{
    static Audio audio;
    return audio;
}

SoundSequence::SoundSequence(PSound s) :
      sound(s)
{}

void SoundSequence::play()
{
    if(sound.expired())
        return; //No sound

    auto &a = GetAudio();

    if(playstate.expired())
    {
        playstate = a.play(sound);
    }
    else
    {
        *(playstate.lock())=false;
        playstate = a.play(sound);
    }
}

bool SoundSequence::isPlayed() const {return !playstate.expired();}









#undef alCall
