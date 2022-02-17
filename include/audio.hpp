#pragma once


#include "sound.hpp"
#include "geometry.hpp"
#include "utility.hpp"
#include <AL/al.h>
#include <AL/alc.h>


using PSound = std::weak_ptr<Sound>;

class SoundSystem
{
    struct SoundSource
    {
        enum State
        {
            Paused  = AL_PAUSED,
            Initial = AL_INITIAL,
            Playing = AL_PLAYING,
            Stoped  = AL_STOPPED
        };

        void setVolume(float volume)
        {
            alSourcef(_sourceid, AL_GAIN, adjust(0.f,1.f,volume));
        }

        void setPitch(float pitch)
        {
            alSourcef(_sourceid, AL_PITCH, adjust(0.f,1.f,pitch));
        }

        void setLooping(bool v)
        {
            alSourcei(_sourceid, AL_LOOPING, v);
        }

        void setGainRange(float min, float max)
        {
            alSourcef(_sourceid, AL_MIN_GAIN, min);
            alSourcef(_sourceid, AL_MAX_GAIN, max);
        }

        State state() const
        {
            ALint val;
            alGetSourcei(_sourceid, AL_SOURCE_STATE, &val);
            return static_cast<State>(val);
        }


        bool inUse() const {return _inUse;}

        SoundSource() :
              _inUse(false)
        {
            alGenSources(1,&_sourceid);
        }

        ~SoundSource()
        {
            alDeleteSources(1,&_sourceid);
        }

        ALuint _sourceid;
        bool   _inUse;
    };


    using TEmitterList = TArray<class SoundEmitter *>;
    using TSourceList = TArray<SoundSource>;

    friend class SoundEmitter;


public:
    void update(float msec);
    void setVolume(float value);
    void setListenerPos(Math::Vector3D<float> pos);

private:
    ALCcontext *_context;
    ALCdevice  *_device;
    float _masterVolume;

    TSourceList  _sources;
    TEmitterList _emitters;
    TEmitterList _frameEmitters;

    Math::Vector3D<float> _listenerPosition;

    friend SoundSystem &soundSystem(unsigned int);

    SoundSource *getFreeSource();

    void attachSources(TEmitterList::iterator from,  TEmitterList::iterator to);
    void detachSources(TEmitterList::iterator from,  TEmitterList::iterator to);

    void registerEmitter(SoundEmitter *emitter);
    void unregisterEmitter(SoundEmitter *emitter);

    SoundSystem(unsigned int channels);

    ~SoundSystem();

};


inline SoundSystem &soundSystem(unsigned int channels = 32)
{
    static SoundSystem ss(channels);
    return ss;
}


class SoundEmitter
{
    using TSource = SoundSystem::SoundSource;

public:
    enum Priority
    {
        Low,
        Medium,
        High,
        Always,
        };

    enum State
    {
        Paused = -1,
        Initial = 0,
        Playing = 1,
        Stoped = 2,
    };

public:
    SoundEmitter();
    ~SoundEmitter();

    void setSound(Sound *s);
    void clearSound();
    void update(float msec);
    void play();
    void stop();
    void pause();
    void reset();
    void setVolume(float volume);
    void setLooping(bool isLooping);
    bool isAttached() const;
    friend bool operator > (SoundEmitter &lhs, SoundEmitter &rhs);

private:
    Sound   *_pSound = nullptr;
    TSource *_alSource = nullptr;
    ALuint  _alBuffer;

    Priority _priority;
    float    _timeLeft;
    float    _volume;
    bool     _isLooping;

    Sphere<float> _position;
    State _state;

    friend class SoundSystem;
    friend struct SoundSource;

    void attach(TSource *source);
    void detach();
};















