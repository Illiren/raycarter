#pragma once


#include "stddefines.hpp"
#include <unordered_map>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>


template<typename KeyType>
struct NameGenerator
{


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











