#pragma once


#include "stddefines.hpp"
#include <unordered_map>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>

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











