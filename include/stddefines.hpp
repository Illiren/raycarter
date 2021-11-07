#pragma once

//TODO: Rename To stdoverrides or something

#include <limits>
#include <cassert>
#include <vector>
#include <string>
#include <unordered_map> //TODO: Replace by custom hash

#define normal   //for symmetry (short int, normal int, long int)

using TReal = float;
using TUInt8 = unsigned char;
using TSize = std::size_t;
using TUint32 = unsigned int;
using TUint64 = unsigned long int;
using TInt32 = signed int;
using TInt64 = signed long int;

template<typename T, typename A = std::allocator<T>>
using TArray = std::vector<T,A>;

template<typename T, TSize size>
using TStaticArray = std::array<T,size>;

template<typename T, typename A = std::allocator<T>>
using THashMap = std::unordered_map<T,A>;

template<typename T, typename U>
using TPair = std::pair<T,U>;

template<typename ...Args>
using TTuple = std::tuple<Args...>;

using TString = std::string;

