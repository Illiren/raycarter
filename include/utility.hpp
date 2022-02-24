#pragma once


#include "stddefines.hpp"
#include <unordered_map>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "location.hpp"


//extern constexpr bool ReleaseBuild = (_DEBUGMODE == 0);

/*
template<typename T, typename ExceptionType = std::exception>
inline void validate(T &&assertion, const ExceptionType exception = {})
{
    if constexpr (!ReleaseBuild)
        if(!assertion)
            throw exception;
}*/

template<typename T>
constexpr inline T adjust(T min, T max, T value)
{
    return std::min(max,std::max(min,value));
}


template<typename Key, typename Info>
class FiniteStateMachine
{
public:
    using TEvent = std::function<void ()>;
    using TCond  = std::function<bool ()>;
    using Links  = std::unordered_map<struct State *, TCond>;

    struct StateInfo
    {
        StateInfo(Info i) : info(i) {}
        Info info;
        TEvent onEnter;
        TEvent onUpdate;
        TEvent onExit;
    };


private:
    struct State : public StateInfo
    {
        State(Info i) : StateInfo(i) {}
        std::unordered_map<State *, TCond> links;
    };

    using StateMap = std::unordered_map<Key, State>;

public:
    FiniteStateMachine() : _current(nullptr) {}
    FiniteStateMachine(Key initKey, Info initInfo)
    {
        _states.emplace(initKey,initInfo);
        _current = &_states.at(initKey);
    }

    void setCurrentState(Key key)
    {
        assert(_states.contains(key) && "ERROR");
        _current = &_states.at(key);
    }

    const StateInfo &currentStateValue() const
    {
        assert(_current != nullptr && "ERROR");
        return *_current;
    }

    StateInfo &currentStateValue()
    {
        assert(_current != nullptr && "ERROR");
        return *_current;
    }

    Key currentState() const
    {
        assert(_current != nullptr && "ERROR");
        for(auto &[key, value] : _states)
            if(&value == _current)
                return key;
        throw std::logic_error("");
    }


    const StateInfo &operator[](Key key) const
    {
        assert(_states.contains(key) && "ERROR");
        return _states.at(key);
    }

    StateInfo &operator[](Key key)
    {
        assert(_states.contains(key) && "ERROR");
        return _states.at(key);
    }

    void addState(Key key, Info info)
    {
        _states.emplace(key, info);
    }
    void removeState(Key key)
    {
        removeLinksTo(key);
        _states.erase(key);
    }

    void linkState(Key s1, Key s2, TCond cond)
    {
        assert(_states.contains(s1) && _states.contains(s2) && "ERROR");
        State *p = &_states.at(s2);
        _states.at(s1).links.insert({p,cond});
    }

    void removeLinksTo(Key toRemove)
    {
        for(auto &[key, state] : _states)
            std::erase_if(state.links, [&toRemove] (const auto &item)
                          {
                              const auto& [key, value] = item;
                              return key == toRemove;
                          });
    }

    void removeLinksFrom(Key key)
    {
        assert(_states.contains(key) && "ERROR");
        _states.at(key).links.clear();
    }

    void removeLink(Key s1, Key s2)
    {
        assert(_states.contains(s1) && _states.contains(s2) && "ERROR");
        auto p = &_states.at(s2);
        _states.at(s1).links.erase(p);
    }

    auto stateCount() const {return _states.size();}

    void update()
    {
        assert(_current != nullptr && "ERROR");

        auto &links = _current->links;
        for(const auto &[state, cond] : links)
            if(cond())
            {
                if(_current->onExit) _current->onExit();
                _current = state;
                if(_current->onEnter) _current->onEnter();
                return;
            }
        if(_current->onUpdate) _current->onUpdate();
    }

    //TODO: Allocator for states

private:
    const State*   _current;
    StateMap _states;
};


/*
struct Serializable
{
    template<typename T>    
    static TSize serializableSize(T value) requires std::is_standard_layout<T>
    {
        return sizeof(T);
    }

    template<typename T>
    static char *serialize(char *target, T value) requires std::is_standard_layout<T>
    {
        return std::copy(target, &value, serializableSize(value));
    }

    template<typename T>
    static char *deserialize(const char *source, T &target) requires std::is_standard_layout<T>
    {
        //std::memcpy();
        return nullptr;
    }
};

template<typename KeyType, typename Type>
class ResourceManagerBase
{
    using TSmartPtr = std::shared_ptr<T>;
    using TWeakPtr = std::weak_ptr<T>;
    using TMapData = std::unordered_map<Key, TSmartPtr>;

public:
    ResourceManagerBase() = default;

    TWeakPtr operator[](KeyType key) const noexcept {return _data[key];}
    TSize size() const noexcept {return _data.size();}

protected:
    TMapData _data;
};

*/









