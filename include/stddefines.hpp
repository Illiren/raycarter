#pragma once

//TODO: Rename To stdoverrides or something

#include <limits>
#include <cassert>
#include <vector>
#include <list>
#include <string>
#include <unordered_map> //TODO: Replace by custom hash

#define normal   //for symmetry (short int, normal int, long int)

using TByte = unsigned char;
using TReal = float;
using TUInt8 = unsigned char;
using TSize = std::size_t;
using TUint32 = unsigned int;
using TUint64 = unsigned long int;
using TInt32 = signed int;
using TInt64 = signed long int;

template<typename T, typename A = std::allocator<T>>
using TArray = std::vector<T,A>;

template<typename T, typename A = std::allocator<T>>
using TList = std::list<T,A>;

template<typename T, TSize size>
using TStaticArray = std::array<T,size>;

template<class Key,
         class T,
         class Hash = std::hash<Key>,
         class KeyEqual = std::equal_to<Key>,
         class A = std::allocator<std::pair<const Key, T>>>
using THashMap = std::unordered_map<Key,T,Hash,KeyEqual,A>;

template<typename T, typename U>
using TPair = std::pair<T,U>;

template<typename ...Args>
using TTuple = std::tuple<Args...>;

using TString = std::string;

template<typename T>
using TInitializerList = std::initializer_list<T>;

#ifdef DEBUG
#define __PARALLEL 0
#else
#define __PARALLEL 1
#endif


