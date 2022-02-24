#include "audio.hpp"
#include <fstream>
#include <iostream>
#include <AL/al.h>
#include <AL/alc.h>
#include <bit>
#include <cstring>
#include "utility.hpp"


void SoundSystem::setVolume(float value)
{
    _masterVolume = adjust(0.f,1.f,value);
    alListenerf(AL_GAIN,_masterVolume);
}

void SoundSystem::setListenerPos(Math::Vector3D<float> pos)
{
    _listenerPosition = pos;
    alListener3f(AL_POSITION, _listenerPosition.x(),_listenerPosition.y(),_listenerPosition.z());
}

void SoundSystem::setListenerDir(Math::Vector3D<float> dir)
{
    _listenerRotation = dir;
    alListener3f(AL_DIRECTION, _listenerRotation.x(),_listenerRotation.y(),_listenerPosition.z());
}

SoundSystem::SoundSystem(unsigned int channels)
{
    using namespace std;

    cout << "Sound system init" << endl;
    cout << "Found sound devices: " << alcGetString(nullptr,ALC_DEVICE_SPECIFIER) << endl;

    _device = alcOpenDevice(nullptr);
    if(!_device)
    {
        cout << "Sound system creation has failed! No Valid Device!" << endl;
        return;
    }

    cout << "Sound System created with device: "
         << alcGetString(_device,ALC_DEVICE_SPECIFIER) << endl;

    _context = alcCreateContext(_device,nullptr);
    alcMakeContextCurrent(_context);
    alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);

    _sources.resize(channels);

    cout << "Sound system has " << _sources.size()
         << " channels available!" << endl;
}

SoundSystem::SoundSource *SoundSystem::getFreeSource()
{
    for(auto &source : _sources)
        if(!source._inUse) { source._inUse = true; return &source;}

    return nullptr;
}

void SoundSystem::update(float msec)
{
    (void)msec;

    TEmitterList attachList,
                 detachList;


    std::sort(_emitters.begin(), _emitters.end(),
              [](SoundEmitter *lhs, SoundEmitter *rhs) -> bool {return (*lhs) > (*rhs); });


    for(TSize i=0; auto &emitter : _emitters)
        if((emitter->_isGlobal || intersect(_listenerPosition,emitter->_position)) && (i++ < _sources.size()))
            attachList.push_back(emitter);
        else
            detachList.push_back(emitter);

    if(!attachList.empty())
        attachSources(attachList.begin(),attachList.end());
    if(!detachList.empty())
        detachSources(detachList.begin(),detachList.end());

    for(auto &source : _sources)
        if(source.inUse())
        {
            ALint state;
            alGetSourcei(source._sourceid,AL_SOURCE_STATE, &state);
            if(state == AL_INITIAL)
                alSourcePlay(source._sourceid);
        }
}

void SoundSystem::attachSources(TEmitterList::iterator from, TEmitterList::iterator to)
{
    for(auto &source : _sources)
    {
        if(!source.inUse())
        {
            while((*from)->isAttached()) if(++from == to) return;

            (*from)->attach(&source);
        }
    }
}

void SoundSystem::detachSources(TEmitterList::iterator from, TEmitterList::iterator to)
{
    for(;from != to; ++from)
        (*from)->detach();
}

void SoundSystem::registerEmitter(SoundEmitter *emitter)
{
    _emitters.push_back(emitter);
}

void SoundSystem::unregisterEmitter(SoundEmitter *emitter)
{    
    _emitters.erase(std::remove(_emitters.begin(),_emitters.end(), emitter), _emitters.end());
}

SoundSystem::~SoundSystem()
{
    for(auto &emitter : _emitters)
        emitter->detach();

    alcMakeContextCurrent(nullptr);
    for(auto &[source,inUse] : _sources)
        alDeleteSources(1,&source);
    alcDestroyContext(_context);
    alcCloseDevice(_device);
}

SoundEmitter::SoundEmitter()
{
    reset();
}

SoundEmitter::~SoundEmitter()
{
    auto &s = soundSystem();
    s.unregisterEmitter(this);
    detach();
}

void SoundEmitter::setSound(Sound *s)
{
    _pSound = s;
    if(!s) return;

    {
        ALenum format;
        if(_pSound->channels == 1 && _pSound->bps == 8)
            format = AL_FORMAT_MONO8;
        else if(_pSound->channels == 1 && _pSound->bps == 16)
            format = AL_FORMAT_MONO16;
        else if(_pSound->channels == 2 && _pSound->bps == 8)
            format = AL_FORMAT_STEREO8;
        else if(_pSound->channels == 2 && _pSound->bps == 16)
            format = AL_FORMAT_STEREO16;
        else
        {
            std::cout << "unknown format" << std::endl;
            return;
        }

        _timeLeft = s->length;
        alGenBuffers(1,&_alBuffer);
        alBufferData(_alBuffer,format,s->buffer.data(),s->buffer.size(),(ALsizei)s->sampleRate);
    }
}

void SoundEmitter::clearSound()
{
    alDeleteBuffers(1,&_alBuffer);
    _pSound = nullptr;
}

void SoundEmitter::update(float msec)
{
    if(!_pSound) return;

    if(_state == Playing)
    {
        _timeLeft += msec/1000;

        if(_timeLeft >= _pSound->length)
        {
            if(_isLooping)
                _timeLeft = 0.f;
        }
    }

    if(_alSource)
    {
        alSourcefv(_alSource->_sourceid, AL_POSITION, (float*)_position.origin);
        alSourcefv(_alSource->_sourceid, AL_DIRECTION,(float*)_rotation);
        ALint state;

        alGetSourcei(_alSource->_sourceid,AL_SOURCE_STATE, &state);
        if(state == AL_STOPPED) // AL_LOOPING is a flag that indicates that the source will not be in AL_STOPPED
            requestToStop();
    }
}

void SoundEmitter::setPosition(Math::Vector3D<float> pos)
{
    _position.origin = pos;
}

void SoundEmitter::setDirection(Math::Vector3D<float> dir)
{
    _rotation = dir;
}

void SoundEmitter::setRadius(float r)
{
    _position.radius = r;
    if(!_alSource) return;
    alSourcef(_alSource->_sourceid, AL_MAX_DISTANCE, r);
}

void SoundEmitter::play()
{    
    if(_state == Playing) return;

    auto &s = soundSystem();
    s.registerEmitter(this);
    _state = Playing;
}

void SoundEmitter::stop()
{
    if(_state == Stoped) return;
    _state = Stoped;
    if(!_alSource) return;
    alSourceStop(_alSource->_sourceid);
}

void SoundEmitter::pause()
{
    if(_state == Paused) return;
    _state = Paused;
    if(!_alSource) return;
    alSourceStop(_alSource->_sourceid);
}

void SoundEmitter::reset()
{
    _priority = Low;
    _state = Initial;
    _volume = 1.f;
    _position.radius = 10.f;
    _timeLeft = 0.f;
    _position.origin = {0.f,0.f,0.f};
    _isGlobal = true;
    _isLooping = true;
    detach();
    _pSound = nullptr;
}

void SoundEmitter::setVolume(float volume)
{
    _volume = adjust(0.f,1.f,volume);
    if(!_alSource) return;
    alSourcef(_alSource->_sourceid, AL_GAIN, _volume);
}

void SoundEmitter::setLooping(bool isLooping)
{
    _isLooping = isLooping;
    if(!_alSource) return;
    alSourcei(_alSource->_sourceid, AL_LOOPING, _isLooping);
}

void SoundEmitter::setGlobal(bool isGlobal)
{
    _isGlobal = isGlobal;
    //if(!_alSource) return;
    //alSourcei(_alSource->_sourceid, AL_SOURCE_RELATIVE, !_isGlobal);
}

bool operator>(SoundEmitter &lhs, SoundEmitter &rhs)
{
    return lhs._priority > rhs._priority;
}

void SoundEmitter::attach(TSource *source)
{
    if(!_pSound) return;
    assert(source != nullptr);

    _alSource = source;
    _alSource->_inUse = true;
    _state = Playing;
    alSourcef(_alSource->_sourceid, AL_MAX_DISTANCE, _position.radius);
    alSourcef(_alSource->_sourceid, AL_REFERENCE_DISTANCE, _position.radius);
    alSourcei(_alSource->_sourceid,AL_BUFFER, _alBuffer);
    alSourcef(_alSource->_sourceid,AL_SEC_OFFSET,_timeLeft);
    alSourcei(_alSource->_sourceid, AL_LOOPING, _isLooping);
    //alSourcei(_alSource->_sourceid, AL_SOURCE_RELATIVE, !_isGlobal);
    alSourcef(_alSource->_sourceid, AL_GAIN, _volume);
}

void SoundEmitter::detach()
{
    if(!_alSource) return;
    alSourceStop(_alSource->_sourceid);
    alSourcei(_alSource->_sourceid,AL_BUFFER,0);
    alSourceRewind(_alSource->_sourceid);
    _alSource->_inUse = false;
    _alSource = nullptr;
}

void SoundEmitter::requestToStop()
{
    auto &s = soundSystem();
    s.unregisterEmitter(this);
    detach();
    switch(_state)
    {
    case SoundEmitter::Paused:
        break;
    case SoundEmitter::Stoped:
    case SoundEmitter::Playing:
    {
        _state = Stoped;
        _timeLeft = 0.f;
        break;
    }
    }
}

bool SoundEmitter::isAttached() const
{
    return _alSource != nullptr;
}
