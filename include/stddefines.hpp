#ifndef STDDEFINES_HPP
#define STDDEFINES_HPP

//TODO: Rename To stdoverrides or something

#include <vector>
#include <string>
#include <unordered_map> //TODO: Replace by custom hash
#include "typedefines.hpp"

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




#endif //STDDEFINES_HPP
