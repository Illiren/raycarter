#pragma once


#include <cmath>

#include "stddefines.hpp"
#include <concepts>

namespace Math
{

template <typename T> concept Arithmetic = std::is_arithmetic_v<T>;
template <typename From, typename To> concept Convertible = std::is_convertible_v<From,To>;
template <typename T> concept Integral = std::is_integral_v<T>;

template <typename T, TSize R, TSize C>
struct Matrix
{
    static constexpr auto Rows = R;
    static constexpr auto Columns = C;
    static constexpr auto Size = R * C;
    using Type = T;
    using Pointer = T *;
    using Reference = T &;
    using ConstReference = const Type &;
    using ColumnRef = Type (&)[Columns];
    using ConstColumnRef = const Type (&)[Columns];
    using TData = Type[Rows][Columns];
    using RefData = Type (&)[Rows][Columns];
    using ConstRefData = const Type (&)[Rows][Columns];

    constexpr Matrix() noexcept {}
    constexpr Matrix(T defaultValue) noexcept
    {
        for (TSize i = Rows; i--;)
            for (TSize j = Columns; j--; _data[i][j] = defaultValue);
    }
    constexpr Matrix(const Matrix &rhs) noexcept {
        for (TSize i = Rows; i--;)
            for (TSize j = Columns; j--; _data[i][j] = rhs._data[i][j]);
    }
    constexpr Matrix(Matrix &&rhs) noexcept {
        for (TSize i = Rows; i--;)
            for (TSize j = Columns; j--; _data[i][j] = rhs._data[i][j]);
    }
    constexpr Matrix(Type (&data)[Rows][Columns]) {
        for(TSize i = Rows; i--;)
            for(TSize j = Columns; j--; _data[i][j] = data[i][j]);
    }
    constexpr Matrix(TInitializerList<T[Columns]> data) {
        assert(data.size() == Rows && "Rows size not match");
        auto it = data.begin();
        const auto end = data.end();
        for(TSize i = 0; it != end; ++it, i++)
            for(TSize j = Columns; j--; _data[i][j] = (*it)[j]);
    }

    Matrix &operator=(const Matrix &rhs) noexcept {
        for (TSize i = Rows; i--;)
            for (TSize j = Columns; j--; _data[i][j] = rhs._data[i][j]);
        return *this;
    }

    Matrix &operator=(Matrix &&rhs) noexcept {
        for (TSize i = Rows; i--;)
            for (TSize j = Columns; j--; _data[i][j] = rhs._data[i][j]);
        return *this;
    }

    ColumnRef operator[](TSize pos) noexcept {
        assert(pos < Rows && "out of row range");
        return _data[pos];
    }
    ConstColumnRef operator[](TSize pos) const noexcept {
        assert(pos < Rows && "out of row range");
        return _data[pos];
    }

    constexpr TSize rows() const noexcept { return Rows; }
    constexpr TSize columns() const noexcept { return Columns; }
    constexpr TSize size() const noexcept { return Size; }

    inline operator RefData() noexcept { return _data; }
    inline operator ConstRefData() const noexcept { return _data; }

    // Math operations
    Matrix operator-() const noexcept requires Arithmetic<T>{
        Matrix newOne;
        for (TSize i = Rows; i--;)
            for (TSize j = Columns; j--; newOne._data[i][j] = -_data[i][j]);
        return newOne;
    }
    Matrix &operator-=(Matrix m) noexcept requires Arithmetic<T> {
        for (TSize i = Rows; i--;)
            for (TSize j = Columns; j--; _data[i][j] -= m._data[i][j]);
        return *this;
    }
    Matrix &operator+=(Matrix m) noexcept requires Arithmetic<T>{
        for (TSize i = Rows; i--;)
            for (TSize j = Columns; j--; _data[i][j] += m._data[i][j]);
        return *this;
    }
    Matrix &operator*=(Matrix m) noexcept requires Arithmetic<T>{
        for (TSize i = Rows; i--;)
            for (TSize j = Columns; j--; _data[i][j] *= m._data[i][j]);
        return *this;
    }
    Matrix &operator/=(Matrix m) requires Arithmetic<T>{
        for (TSize i = Rows; i--;)
            for (TSize j = Columns; j--; _data[i][j] /= m._data[i][j]);
        return *this;
    }
    Matrix &operator%=(Matrix m) requires Integral<T>{
        for (TSize i = Rows; i--;)
            for (TSize j = Columns; j--; _data[i][j] %= m._data[i][j]);
        return *this;
    }


    Matrix &operator-=(T m) noexcept requires Arithmetic<T> {
        for (TSize i = Rows; i--;)
            for (TSize j = Columns; j--; _data[i][j] -= m);
        return *this;
    }
    Matrix &operator+=(T m) noexcept requires Arithmetic<T>{
        for (TSize i = Rows; i--;)
            for (TSize j = Columns; j--; _data[i][j] += m);
        return *this;
    }
    Matrix &operator*=(T m) noexcept requires Arithmetic<T>{
        for (TSize i = Rows; i--;)
            for (TSize j = Columns; j--; _data[i][j] *= m);
        return *this;
    }
    Matrix &operator/=(T m) requires Arithmetic<T>{
        for (TSize i = Rows; i--;)
            for (TSize j = Columns; j--; _data[i][j] /= m);
        return *this;
    }
    Matrix &operator%=(T m) requires Integral<T>{
        for (TSize i = Rows; i--;)
            for (TSize j = Columns; j--; _data[i][j] %= m);
        return *this;
    }

    // bitwise
    Matrix operator~() noexcept requires Integral<T>
    {
        Matrix newOne;
        for (TSize i = Rows; i--;)
            for (TSize j = Columns; j--; newOne._data[i][j] = ~_data[i][j]);
        return newOne;
    }

    Matrix &operator&=(Matrix m) noexcept requires Integral<T>
    {
        for (TSize i = Rows; i--;)
            for (TSize j = Columns; j--; _data[i][j] &= m._data[i][j]);
        return *this;
    }

    Matrix &operator^=(Matrix m) noexcept requires Integral<T>
    {
        for (TSize i = Rows; i--;)
            for (TSize j = Columns; j--; _data[i][j] ^= m._data[i][j]);
        return *this;
    }

    Matrix &operator|=(Matrix m) noexcept requires Integral<T>
    {
        for (TSize i = Rows; i--;)
            for (TSize j = Columns; j--; _data[i][j] |= m._data[i][j]);
        return *this;
    }

    Matrix &operator<<=(Matrix m) noexcept requires Integral<T>
    {
        for (TSize i = Rows; i--;)
            for (TSize j = Columns; j--; _data[i][j] <<= m._data[i][j]);
        return *this;
    }

    Matrix &operator>>=(Matrix m) noexcept requires Integral<T>
    {
        for (TSize i = Rows; i--;)
            for (TSize j = Columns; j--; _data[i][j] >>= m._data[i][j]);
        return *this;
    }

    //value bitwise
    Matrix &operator&=(T m) noexcept requires Integral<T>
    {
        for (TSize i = Rows; i--;)
            for (TSize j = Columns; j--; _data[i][j] &= m);
        return *this;
    }

    Matrix &operator^=(T m) noexcept requires Integral<T>
    {
        for (TSize i = Rows; i--;)
            for (TSize j = Columns; j--; _data[i][j] ^= m);
        return *this;
    }

    Matrix &operator|=(T m) noexcept requires Integral<T>
    {
        for (TSize i = Rows; i--;)
            for (TSize j = Columns; j--; _data[i][j] |= m);
        return *this;
    }

    Matrix &operator<<=(T m) noexcept requires Integral<T>
    {
        for (TSize i = Rows; i--;)
            for (TSize j = Columns; j--; _data[i][j] <<= m);
        return *this;
    }

    Matrix &operator>>=(T m) noexcept requires Integral<T>
    {
        for (TSize i = Rows; i--;)
            for (TSize j = Columns; j--; _data[i][j] >>= m);
        return *this;
    }


    //Convertible
    template<typename U>
    operator Matrix<U,R,C>() const noexcept requires Convertible<T,U>
    {
        Matrix<U,R,C> m;
        for(TSize i=R;i--;)
            for(TSize j=C;j--;m[i][j] = static_cast<U>(_data[i][j]));
        return m;
    }


protected:
    TData _data;
};

template <typename T, TSize S>
struct Matrix<T, 1, S>
{
    static constexpr auto Rows = 1;
    static constexpr auto Columns = S;
    static constexpr auto Size = S;
    using Type = T;
    using Pointer = T *;
    using Reference = T &;
    using ConstReference = const Type &;
    using ColumnRef = Reference;
    using ConstColumnRef = ConstReference;
    using TData = Type[Columns];
    using RefData = Type (&)[Columns];
    using ConstRefData = const Type (&)[Columns];

    constexpr Matrix() noexcept {}
    constexpr Matrix(TInitializerList<T> initList) noexcept
    {
        assert(initList.size() == Size);
        TSize i = 0;
        for (auto it : initList)
            _data[i++] = it;
    }
    constexpr Matrix(const T (&d)[Size]) noexcept {
        for (TSize i = Size; i--; _data[i] = d[i]);
    }

    constexpr Matrix(const Matrix &v) noexcept
    {
        for (TSize i = Size; i--; _data[i] = v._data[i]);
    }
    constexpr Matrix(Matrix &&v) noexcept
    {
        for (TSize i = Size; i--; _data[i] = v._data[i]);
    }

    Matrix &operator=(const Matrix &rhs) noexcept
    {
        for (TSize i = Size; i--; _data[i] = rhs._data[i]);
        return *this;
    }

    Matrix &operator=(Matrix &&rhs) noexcept
    {
        for (TSize i = Size; i--; _data[i] = rhs._data[i]);
        return *this;
    }

    Reference &operator[](TSize pos) noexcept
    {
        assert(pos < Size);
        return _data[pos];
    }
    ConstReference &operator[](TSize pos) const noexcept
    {
        assert(pos < Size);
        return _data[pos];
    }
    constexpr TSize rows() const noexcept { return Rows; }
    constexpr TSize columns() const noexcept { return Columns; }
    constexpr auto size() const noexcept { return Size; }

    operator RefData() { return _data; }
    operator ConstRefData() const { return _data; }

    // Math operations
    Matrix operator-() const requires Arithmetic<T>
    {
        Matrix newOne;
        for (TSize j = Columns; j--; _data[j] = -_data[j]);
        return newOne;
    }
    Matrix &operator-=(Matrix m) requires Arithmetic<T>
    {
        for (TSize j = Columns; j--; _data[j] -= m._data[j]);
        return *this;
    }
    Matrix &operator+=(Matrix m) requires Arithmetic<T>
    {
        for (TSize j = Columns; j--; _data[j] += m._data[j]);
        return *this;
    }
    Matrix &operator*=(Matrix m) requires Arithmetic<T>
    {
        for (TSize j = Columns; j--; _data[j] *= m._data[j]);
        return *this;
    }
    Matrix &operator/=(Matrix m) requires Arithmetic<T>
    {
        for (TSize j = Columns; j--; _data[j] /= m._data[j]);
        return *this;
    }
    Matrix &operator%=(Matrix m) requires Integral<T>
    {
        for (TSize j = Columns; j--; _data[j] %= m._data[j]);
        return *this;
    }

    Matrix &operator-=(T m) requires Arithmetic<T>
    {
        for (TSize j = Columns; j--; _data[j] -= m);
        return *this;
    }
    Matrix &operator+=(T m) requires Arithmetic<T>
    {
        for (TSize j = Columns; j--; _data[j] += m);
        return *this;
    }
    Matrix &operator*=(T m) requires Arithmetic<T>
    {
        for (TSize j = Columns; j--; _data[j] *= m);
        return *this;
    }
    Matrix &operator/=(T m) requires Arithmetic<T>
    {
        for (TSize j = Columns; j--; _data[j] /= m);
        return *this;
    }
    Matrix &operator%=(T m) requires Integral<T>
    {
        for (TSize j = Columns; j--; _data[j] %= m);
        return *this;
    }

    // bitwise
    Matrix operator~() noexcept requires Integral<T>
    {
        Matrix newOne;
        for (TSize j = Columns; j--; newOne._data[j] = ~_data[j]);
        return newOne;
    }

    Matrix &operator&=(Matrix m) noexcept requires Integral<T>
    {
        for (TSize j = Columns; j--; _data[j] &= m._data[j]);
        return *this;
    }

    Matrix &operator^=(Matrix m) noexcept requires Integral<T>
    {
        for (TSize j = Columns; j--; _data[j] ^= m._data[j]);
        return *this;
    }

    Matrix &operator|=(Matrix m) noexcept requires Integral<T>
    {
        for (TSize j = Columns; j--; _data[j] |= m._data[j]);
        return *this;
    }

    Matrix &operator<<=(Matrix m) noexcept requires Integral<T>
    {
        for (TSize j = Columns; j--; _data[j] <<= m._data[j]);
        return *this;
    }

    Matrix &operator>>=(Matrix m) noexcept requires Integral<T>
    {
        for (TSize j = Columns; j--; _data[j] >>= m._data[j]);
        return *this;
    }

    //value bitwise
    Matrix &operator&=(T m) noexcept requires Integral<T>
    {
        for (TSize j = Columns; j--; _data[j] &= m);
        return *this;
    }

    Matrix &operator^=(T m) noexcept requires Integral<T>
    {
        for (TSize j = Columns; j--; _data[j] ^= m);
        return *this;
    }

    Matrix &operator|=(T m) noexcept requires Integral<T>
    {
        for (TSize j = Columns; j--; _data[j] |= m);
        return *this;
    }

    Matrix &operator<<=(T m) noexcept requires Integral<T>
    {
        for (TSize j = Columns; j--; _data[j] <<= m);
        return *this;
    }

    Matrix &operator>>=(T m) noexcept requires Integral<T>
    {
        for (TSize j = Columns; j--; _data[j] >>= m);
        return *this;
    }

    auto length() const noexcept requires Arithmetic<T>
    {
        Type accum = 0;
        for (TSize i = Size; i++; accum += _data[i] * _data[i]);
        return static_cast<Type>(sqrt(static_cast<float>(accum)));
    }

    Matrix &unit() noexcept requires Arithmetic<T>
    {
        auto k = T(1) / length();
        for (TSize i = Size; i++; _data[i] *= k);
        return *this;
    }

    template<typename U>
    operator Matrix<U,1,S>() const noexcept requires Convertible<T,U>
    {
        Matrix<U,1,S> m;
        for(TSize i=S;i--; m[i] = static_cast<U>(_data[i]));
        return m;
    }

protected:
    TData _data;
};

template <typename T>
struct Matrix<T, 1, 2>
{
    static constexpr auto Rows = 1;
    static constexpr auto Columns = 2;
    static constexpr auto Size = 2;
    using Type = T;
    using Pointer = T *;
    using Reference = T &;
    using ConstReference = const Type &;
    using ColumnRef = Reference;
    using ConstColumnRef = ConstReference;
    using TData = Type[Columns];
    using RefData = Type (&)[Columns];
    using ConstRefData = const Type (&)[Columns];

    constexpr Matrix() noexcept {}
    constexpr Matrix(Type x, Type y) noexcept : _data{x, y} {}
    constexpr Matrix(const T (&d)[Size]) noexcept : _data{d[0], d[1]} {}
    constexpr Matrix(const Matrix &v) noexcept : _data{v._data[0], v._data[1]} {}
    constexpr Matrix(Matrix &&v) noexcept : _data{v._data[0], v._data[1]} {}

    Matrix &operator=(const Matrix &rhs) noexcept {
        _data[0] = rhs._data[0];
        _data[1] = rhs._data[1];
        return *this;
    }

    Matrix &operator=(Matrix &&rhs) noexcept {
        _data[0] = rhs._data[0];
        _data[1] = rhs._data[1];
        return *this;
    }

    Reference &operator[](TSize pos) noexcept {
        assert(pos < Size);
        return _data[pos];
    }
    ConstReference &operator[](TSize pos) const noexcept {
        assert(pos < Size);
        return _data[pos];
    }
    constexpr auto size() const noexcept { return Size; }
    constexpr TSize rows() const noexcept { return Rows; }
    constexpr TSize columns() const noexcept { return Columns; }

    operator RefData() { return _data; }
    operator ConstRefData() const { return _data; }

    inline Reference x() noexcept { return _data[0]; }
    inline Reference y() noexcept { return _data[1]; }
    inline ConstReference x() const noexcept { return _data[0]; }
    inline ConstReference y() const noexcept { return _data[1]; }

    // Math operations
    Matrix operator-() const
    {
        Matrix newOne;
        newOne[0] = -_data[0];
        newOne[1] = -_data[1];
        return newOne;
    }
    Matrix &operator-=(Matrix m)
    {
        _data[0] -= m._data[0];
        _data[1] -= m._data[1];
        return *this;
    }
    Matrix &operator+=(Matrix m)
    {
        _data[0] += m._data[0];
        _data[1] += m._data[1];
        return *this;
    }
    Matrix &operator*=(Matrix m)
    {
        _data[0] *= m._data[0];
        _data[1] *= m._data[1];
        return *this;
    }
    Matrix &operator/=(Matrix m)
    {
        _data[0] /= m._data[0];
        _data[1] /= m._data[1];
        return *this;
    }

    Matrix &operator%=(Matrix m)
    {
        _data[0] %= m._data[0];
        _data[1] %= m._data[1];
        return *this;
    }

    Matrix &operator-=(T m)
    {
        _data[0] -= m;
        _data[1] -= m;
        return *this;
    }
    Matrix &operator+=(T m)
    {
        _data[0] += m;
        _data[1] += m;
        return *this;
    }
    Matrix &operator*=(T m)
    {
        _data[0] *= m;
        _data[1] *= m;
        return *this;
    }
    Matrix &operator/=(T m)
    {
        _data[0] /= m;
        _data[1] /= m;
        return *this;
    }

    Matrix &operator%=(T m)
    {
        _data[0] %= m;
        _data[1] %= m;
        return *this;
    }

    // bitwise
    Matrix operator~() noexcept requires Integral<T>
    {
        Matrix newOne;
        newOne._data[0] = ~_data[0];
        newOne._data[1] = ~_data[1];
        return newOne;
    }

    Matrix &operator&=(Matrix m) noexcept requires Integral<T>
    {
        _data[0] &= m._data[0];
        _data[1] &= m._data[1];
        return *this;
    }

    Matrix &operator^=(Matrix m) noexcept requires Integral<T>
    {
        _data[0] ^= m._data[0];
        _data[1] ^= m._data[1];
        return *this;
    }

    Matrix &operator|=(Matrix m) noexcept requires Integral<T>
    {
        _data[0] |= m._data[0];
        _data[1] |= m._data[1];
        return *this;
    }

    Matrix &operator<<=(Matrix m) noexcept requires Integral<T>
    {
        _data[0] <<= m._data[0];
        _data[1] <<= m._data[1];
        return *this;
    }

    Matrix &operator>>=(Matrix m) noexcept requires Integral<T>
    {
        _data[0] >>= m._data[0];
        _data[1] >>= m._data[1];
        return *this;
    }

    //value bitwise
    Matrix &operator&=(T m) noexcept requires Integral<T>
    {
        _data[0] &= m;
        _data[1] &= m;
        return *this;
    }

    Matrix &operator^=(T m) noexcept requires Integral<T>
    {
        _data[0] ^= m;
        _data[1] ^= m;
        return *this;
    }

    Matrix &operator|=(T m) noexcept requires Integral<T>
    {
        _data[0] |= m;
        _data[1] |= m;
        return *this;
    }

    Matrix &operator<<=(T m) noexcept requires Integral<T>
    {
        _data[0] <<= m;
        _data[1] <<= m;
        return *this;
    }

    Matrix &operator>>=(T m) noexcept requires Integral<T>
    {
        _data[0] >>= m;
        _data[1] >>= m;
        return *this;
    }

    auto length() const noexcept
    {
        return sqrt(_data[0] * _data[0] + _data[1] * _data[1]);
    }

    Matrix &unit() noexcept
    {
        auto k = T(1) / length();
        _data[0] *= k;
        _data[1] *= k;
        return *this;
    }

    template<typename U>
    operator Matrix<U,1,2>() const noexcept requires Convertible<T,U>
    {
        Matrix<U,1,2> m;
        m.x() = static_cast<U>(_data[0]);
        m.y() = static_cast<U>(_data[1]);
        return m;
    }

protected:
    TData _data;
};

template <typename T>
struct Matrix<T, 1, 3>
{
    static constexpr auto Rows = 1;
    static constexpr auto Columns = 3;
    static constexpr auto Size = 3;
    using Type = T;
    using Pointer = T *;
    using Reference = T &;
    using ConstReference = const Type &;
    using ColumnRef = Reference;
    using ConstColumnRef = ConstReference;
    using TData = Type[Columns];
    using RefData = Type (&)[Columns];
    using ConstRefData = const Type (&)[Columns];

    constexpr Matrix() noexcept {}
    constexpr Matrix(Type x, Type y, Type z) noexcept : _data{x, y, z} {}
    constexpr Matrix(const T (&d)[Size]) noexcept : _data{d[0], d[1], d[2]} {}
    constexpr Matrix(const Matrix &v) noexcept : _data{v._data[0], v._data[1], v._data[2]} {}
    constexpr Matrix(Matrix &&v) noexcept : _data{v._data[0], v._data[1], v._data[2]} {}

    Matrix &operator=(const Matrix &rhs) noexcept
    {
        _data[0] = rhs[0];
        _data[1] = rhs[1];
        _data[2] = rhs[2];
        return *this;
    }

    Matrix &operator=(Matrix &&rhs) noexcept
    {
        _data[0] = rhs[0];
        _data[1] = rhs[1];
        _data[2] = rhs[2];
        return *this;
    }

    Reference &operator[](TSize pos) noexcept
    {
        assert(pos < Size);
        return _data[pos];
    }
    ConstReference &operator[](TSize pos) const noexcept
    {
        assert(pos < Size);
        return _data[pos];
    }
    constexpr auto size() const noexcept { return Size; }
    constexpr TSize rows() const noexcept { return Rows; }
    constexpr TSize columns() const noexcept { return Columns; }

    auto length() const noexcept
    {
        return sqrt(_data[0]*_data[0] + _data[1]*_data[1] + _data[2]*_data[2]);
    }

    Matrix &unit() noexcept
    {
        auto k = T(1) / length();
        _data[0] *= k;
        _data[1] *= k;
        _data[2] *= k;
        return *this;
    }

    operator RefData() { return _data; }
    operator ConstRefData() const { return _data; }

    inline Reference x() noexcept { return _data[0]; }
    inline Reference y() noexcept { return _data[1]; }
    inline Reference z() noexcept { return _data[2]; }
    inline ConstReference x() const noexcept { return _data[0]; }
    inline ConstReference y() const noexcept { return _data[1]; }
    inline ConstReference z() const noexcept { return _data[2]; }

    // Math operations
    Matrix operator-() const
    {
        Matrix newOne;
        newOne[0] = -_data[0];
        newOne[1] = -_data[1];
        newOne[2] = -_data[2];
        return newOne;
    }
    Matrix &operator-=(Matrix m)
    {
        _data[0] -= m._data[0];
        _data[1] -= m._data[1];
        _data[2] -= m._data[2];
        return *this;
    }
    Matrix &operator+=(Matrix m)
    {
        _data[0] += m._data[0];
        _data[1] += m._data[1];
        _data[2] += m._data[2];
        return *this;
    }
    Matrix &operator*=(Matrix m)
    {
        _data[0] *= m._data[0];
        _data[1] *= m._data[1];
        _data[2] *= m._data[2];
        return *this;
    }
    Matrix &operator/=(Matrix m)
    {
        _data[0] /= m._data[0];
        _data[1] /= m._data[1];
        _data[2] /= m._data[2];
        return *this;
    }
    Matrix &operator%=(Matrix m) noexcept requires Integral<T>
    {
        _data[0] %= m._data[0];
        _data[1] %= m._data[1];
        _data[2] %= m._data[2];
        return *this;
    }

    Matrix &operator-=(T m)
    {
        _data[0] -= m;
        _data[1] -= m;
        _data[2] -= m;
        return *this;
    }
    Matrix &operator+=(T m)
    {
        _data[0] += m;
        _data[1] += m;
        _data[2] += m;
        return *this;
    }
    Matrix &operator*=(T m)
    {
        _data[0] *= m;
        _data[1] *= m;
        _data[2] *= m;
        return *this;
    }
    Matrix &operator/=(T m)
    {
        _data[0] /= m;
        _data[1] /= m;
        _data[2] /= m;
        return *this;
    }
    Matrix &operator%=(T m)
    {
        _data[0] %= m;
        _data[1] %= m;
        _data[2] %= m;
        return *this;
    }

    // bitwise
    Matrix operator~() noexcept requires Integral<T>
    {
        Matrix newOne;
        newOne._data[0] = ~_data[0];
        newOne._data[1] = ~_data[1];
        newOne._data[2] = ~_data[2];
        return newOne;
    }

    Matrix &operator&=(Matrix m) noexcept requires Integral<T>
    {
        _data[0] &= m._data[0];
        _data[1] &= m._data[1];
        _data[2] &= m._data[2];
        return *this;
    }

    Matrix &operator^=(Matrix m) noexcept requires Integral<T>
    {
        _data[0] ^= m._data[0];
        _data[1] ^= m._data[1];
        _data[2] ^= m._data[2];
        return *this;
    }

    Matrix &operator|=(Matrix m) noexcept requires Integral<T>
    {
        _data[0] |= m._data[0];
        _data[1] |= m._data[1];
        _data[2] |= m._data[2];
        return *this;
    }

    Matrix &operator<<=(Matrix m) noexcept requires Integral<T>
    {
        _data[0] <<= m._data[0];
        _data[1] <<= m._data[1];
        _data[2] <<= m._data[2];
        return *this;
    }

    Matrix &operator>>=(Matrix m) noexcept requires Integral<T>
    {
        _data[0] >>= m._data[0];
        _data[1] >>= m._data[1];
        _data[2] >>= m._data[2];
        return *this;
    }

    //value bitwise
    Matrix &operator&=(T m) noexcept requires Integral<T>
    {
        _data[0] &= m;
        _data[1] &= m;
        _data[2] &= m;
        return *this;
    }

    Matrix &operator^=(T m) noexcept requires Integral<T>
    {
        _data[0] ^= m;
        _data[1] ^= m;
        _data[2] ^= m;
        return *this;
    }

    Matrix &operator|=(T m) noexcept requires Integral<T>
    {
        _data[0] |= m;
        _data[1] |= m;
        _data[2] |= m;
        return *this;
    }

    Matrix &operator<<=(T m) noexcept requires Integral<T>
    {
        _data[0] <<= m;
        _data[1] <<= m;
        _data[2] <<= m;
        return *this;
    }

    Matrix &operator>>=(T m) noexcept requires Integral<T>
    {
        _data[0] >>= m;
        _data[1] >>= m;
        _data[2] >>= m;
        return *this;
    }


    template<typename U>
    operator Matrix<U,1,3>() const noexcept requires Convertible<T,U>
    {
        Matrix<U,1,3> m;
        m.x() = static_cast<U>(_data[0]);
        m.y() = static_cast<U>(_data[1]);
        m.z() = static_cast<U>(_data[2]);
        return m;
    }

protected:
    TData _data;
};

template <typename T>
struct Matrix<T, 1, 4>
{
    static constexpr auto Rows = 1;
    static constexpr auto Columns = 4;
    static constexpr auto Size = 4;
    using Type = T;
    using Pointer = T *;
    using Reference = T &;
    using ConstReference = const Type &;
    using ColumnRef = Reference;
    using ConstColumnRef = ConstReference;
    using TData = Type[Columns];
    using RefData = Type (&)[Columns];
    using ConstRefData = const Type (&)[Columns];

    constexpr Matrix() noexcept {}
    constexpr Matrix(Type x, Type y, Type z, Type w) noexcept
        : _data{x, y, z, w}
    {}
    constexpr Matrix(const T (&d)[Size]) noexcept
        : _data{d[0], d[1], d[2], d[3]}
    {}
    constexpr Matrix(const Matrix &v) noexcept :
          _data{v._data[0], v._data[1], v._data[2], v._data[3]}
    {}
    constexpr Matrix(Matrix &&v) noexcept :
          _data{v._data[0], v._data[1], v._data[2], v._data[3]}
    {}

    Matrix &operator=(const Matrix &rhs) noexcept {
        _data[0] = rhs._data[0];
        _data[1] = rhs._data[1];
        _data[2] = rhs._data[2];
        _data[3] = rhs._data[3];
        return *this;
    }

    Matrix &operator=(Matrix &&rhs) noexcept {
        _data[0] = rhs._data[0];
        _data[1] = rhs._data[1];
        _data[2] = rhs._data[2];
        _data[3] = rhs._data[3];
        return *this;
    }

    Reference &operator[](TSize pos) noexcept {
        assert(pos < Size);
        return _data[pos];
    }
    ConstReference &operator[](TSize pos) const noexcept {
        assert(pos < Size);
        return _data[pos];
    }
    constexpr auto size() const noexcept { return Size; }
    constexpr TSize rows() const noexcept { return Rows; }
    constexpr TSize columns() const noexcept { return Columns; }

    auto length() const noexcept
    {
        return sqrt(_data[0] * _data[0] + _data[1] * _data[1] +
                    _data[2] * _data[2] + _data[3] * _data[3]);
    }

    Matrix &unit() noexcept
    {
        auto k = T(1) / length();
        _data[0] *= k;
        _data[1] *= k;
        _data[2] *= k;
        _data[3] *= k;
        return *this;
    }

    operator RefData() { return _data; }
    operator ConstRefData() const { return _data; }

    inline Reference x() noexcept { return _data[0]; }
    inline Reference y() noexcept { return _data[1]; }
    inline Reference z() noexcept { return _data[2]; }
    inline Reference w() noexcept { return _data[3]; }
    inline ConstReference x() const noexcept { return _data[0]; }
    inline ConstReference y() const noexcept { return _data[1]; }
    inline ConstReference z() const noexcept { return _data[2]; }
    inline ConstReference w() const noexcept { return _data[3]; }

    // Math operations
    Matrix operator-() const {
        Matrix newOne;
        newOne[0] = -_data[0];
        newOne[1] = -_data[1];
        newOne[2] = -_data[2];
        newOne[3] = -_data[3];
        return newOne;
    }
    Matrix &operator-=(Matrix m) {
        _data[0] -= m._data[0];
        _data[1] -= m._data[1];
        _data[2] -= m._data[2];
        _data[3] -= m._data[3];
        return *this;
    }
    Matrix &operator+=(Matrix m) {
        _data[0] += m._data[0];
        _data[1] += m._data[1];
        _data[2] += m._data[2];
        _data[3] += m._data[3];
        return *this;
    }
    Matrix &operator*=(Matrix m) {
        _data[0] *= m._data[0];
        _data[1] *= m._data[1];
        _data[2] *= m._data[2];
        _data[3] *= m._data[3];
        return *this;
    }
    Matrix &operator/=(Matrix m)
    {
        _data[0] /= m._data[0];
        _data[1] /= m._data[1];
        _data[2] /= m._data[2];
        _data[3] /= m._data[3];
        return *this;
    }
    Matrix &operator%=(Matrix m) noexcept requires Integral<T>
    {
        _data[0] %= m._data[0];
        _data[1] %= m._data[1];
        _data[2] %= m._data[2];
        _data[3] %= m._data[3];
        return *this;
    }
    Matrix &operator-=(T m) {
        _data[0] -= m;
        _data[1] -= m;
        _data[2] -= m;
        _data[3] -= m;
        return *this;
    }
    Matrix &operator+=(T m) {
        _data[0] += m;
        _data[1] += m;
        _data[2] += m;
        _data[3] += m;
        return *this;
    }
    Matrix &operator*=(T m) {
        _data[0] *= m;
        _data[1] *= m;
        _data[2] *= m;
        _data[3] *= m;
        return *this;
    }
    Matrix &operator/=(T m) {
        _data[0] /= m;
        _data[1] /= m;
        _data[2] /= m;
        _data[3] /= m;
        return *this;
    }
    Matrix &operator/=(T m) noexcept requires Integral<T>
    {
        _data[0] %= m;
        _data[1] %= m;
        _data[2] %= m;
        _data[3] %= m;
        return *this;
    }

    // bitwise
    Matrix operator~() noexcept requires Integral<T>
    {
        Matrix newOne;
        newOne._data[0] = ~_data[0];
        newOne._data[1] = ~_data[1];
        newOne._data[2] = ~_data[2];
        newOne._data[3] = ~_data[3];
        return newOne;
    }

    Matrix &operator&=(Matrix m) noexcept requires Integral<T>
    {
        _data[0] &= m._data[0];
        _data[1] &= m._data[1];
        _data[2] &= m._data[2];
        _data[3] &= m._data[3];
        return *this;
    }

    Matrix &operator^=(Matrix m) noexcept requires Integral<T>
    {
        _data[0] ^= m._data[0];
        _data[1] ^= m._data[1];
        _data[2] ^= m._data[2];
        _data[3] ^= m._data[3];
        return *this;
    }

    Matrix &operator|=(Matrix m) noexcept requires Integral<T>
    {
        _data[0] |= m._data[0];
        _data[1] |= m._data[1];
        _data[2] |= m._data[2];
        _data[3] |= m._data[3];
        return *this;
    }

    Matrix &operator<<=(Matrix m) noexcept requires Integral<T>
    {
        _data[0] <<= m._data[0];
        _data[1] <<= m._data[1];
        _data[2] <<= m._data[2];
        _data[3] <<= m._data[3];
        return *this;
    }

    Matrix &operator>>=(Matrix m) noexcept requires Integral<T>
    {
        _data[0] >>= m[0];
        _data[1] >>= m[1];
        _data[2] >>= m[2];
        _data[3] >>= m[3];
        return *this;
    }

    //value bitwise
    Matrix &operator&=(T m) noexcept requires Integral<T>
    {
        _data[0] &= m;
        _data[1] &= m;
        _data[2] &= m;
        _data[3] &= m;
        return *this;
    }
    Matrix &operator^=(T m) noexcept requires Integral<T>
    {
        _data[0] ^= m;
        _data[1] ^= m;
        _data[2] ^= m;
        _data[3] ^= m;
        return *this;
    }
    Matrix &operator|=(T m) noexcept requires Integral<T>
    {
        _data[0] |= m;
        _data[1] |= m;
        _data[2] |= m;
        _data[3] |= m;
        return *this;
    }
    Matrix &operator<<=(T m) noexcept requires Integral<T>
    {
        _data[0] <<= m;
        _data[1] <<= m;
        _data[2] <<= m;
        _data[3] <<= m;
        return *this;
    }
    Matrix &operator>>=(T m) noexcept requires Integral<T>
    {
        _data[0] >>= m;
        _data[1] >>= m;
        _data[2] >>= m;
        _data[3] >>= m;
        return *this;
    }

    template<typename U>
    operator Matrix<U,1,4>() const noexcept requires Convertible<T,U>
    {
        Matrix<U,1,4> m;
        m.x() = static_cast<U>(_data[0]);
        m.y() = static_cast<U>(_data[1]);
        m.z() = static_cast<U>(_data[2]);
        m.w() = static_cast<U>(_data[3]);
        return m;
    }


protected:
    TData _data;
};

template <typename T>
struct Matrix<T, 1, 1>
{
    static constexpr auto Rows = 1;
    static constexpr auto Columns = 1;
    static constexpr auto Size = 1;
    using Type = T;
    using Pointer = T *;
    using Reference = T &;
    using ConstReference = const Type &;
    using ColumnRef = Reference;
    using ConstColumnRef = ConstReference;
    using TData = Type;
    using RefData = Type &;
    using ConstRefData = const Type &;

    constexpr Matrix() noexcept {}
    constexpr Matrix(Type a) noexcept : _data{a} {}
    constexpr Matrix(const Matrix &v) noexcept : _data{v._data} {}
    constexpr Matrix(Matrix &&v) noexcept : _data{v._data} {}

    Matrix &operator=(const Matrix &rhs) noexcept {
        _data = rhs._data;
        return *this;
    }

    Matrix &operator=(Matrix &&rhs) noexcept {
        _data = rhs._data;
        return *this;
    }

    constexpr auto size() const noexcept { return Size; }
    constexpr TSize rows() const noexcept { return Rows; }
    constexpr TSize columns() const noexcept { return Columns; }

    auto length() const noexcept { return _data; }
    void unit() const noexcept
    {
        auto k = T(1) / length();
        _data *= k;
    }

    operator RefData() { return _data; }
    operator ConstRefData() const { return _data; }

    // Math operations
    Matrix operator-() const {
        Matrix newOne;
        newOne._data = -_data;
        return newOne;
    }
    Matrix &operator-=(Matrix m) {
        _data -= m._data;
        return *this;
    }
    Matrix &operator+=(Matrix m) {
        _data += m._data;
        return *this;
    }
    Matrix &operator*=(Matrix m) {
        _data *= m._data;
        return *this;
    }
    Matrix &operator/=(Matrix m) {
        _data /= m._data;
        return *this;
    }

    Matrix &operator-=(T m) {
        _data -= m;
        return *this;
    }
    Matrix &operator+=(T m) {
        _data += m;
        return *this;
    }
    Matrix &operator*=(T m) {
        _data *= m;
        return *this;
    }
    Matrix &operator/=(T m) {
        _data /= m;
        return *this;
    }

protected:
    TData _data;
};

template <typename T, TSize Dim>
using Vector = Matrix<T, 1, Dim>;
template <typename T>
using Vector2D = Matrix<T, 1, 2>;
template <typename T>
using Vector3D = Matrix<T, 1, 3>;
template <typename T>
using Vector4D = Matrix<T, 1, 4>;

template <typename T, TSize Row, TSize Col>
bool operator==(Matrix<T, Row, Col> lhs, Matrix<T, Row, Col> rhs)
{
    for (TSize i = Row; i--;)
        for (TSize j = Col; j--;)
            if (lhs[i][j] != rhs[i][j])
                return false;
    return true;
}

template <typename T, TSize Dim>
bool operator==(Matrix<T, 1, Dim> lhs, Matrix<T, 1, Dim> rhs)
{
    for (TSize i = Dim; i--;)
        if (lhs[i] != rhs[i])
            return false;
    return true;
}

template <typename T, TSize Row, TSize Col>
bool operator!=(const Matrix<T, Row, Col> lhs, const Matrix<T, Row, Col> rhs)
{
    for (TSize i = Row; i--;)
        for (TSize j = Col; j--;)
            if (lhs[i][j] == rhs[i][j])
                return false;
    return true;
}

template <typename T, TSize Dim>
bool operator!=(Matrix<T, 1, Dim> lhs, Matrix<T, 1, Dim> rhs)
{
    for (TSize i = Dim; i--;)
        if (lhs[i] == rhs[i])
            return false;
    return true;
}

template <typename T, TSize Rows, TSize Cols>
Matrix<T, Rows, Cols> norm(Matrix<T, Rows, Cols> matrix, T l = 1.)
{
    matrix = matrix * (l / matrix.length());
    return matrix;
}

template <typename T, TSize Rows, TSize Cols>
Matrix<T, Rows, Cols> operator*(Matrix<T, Rows, Cols> lhs,
                                const Matrix<T, Rows, Cols> rhs)
{
    return lhs *= rhs;
}

template <typename T, TSize Rows, TSize Cols>
Matrix<T, Rows, Cols> operator+(Matrix<T, Rows, Cols> lhs,
                                const Matrix<T, Rows, Cols> rhs)
{
    return lhs += rhs;
}

template <typename T, TSize Rows, TSize Cols>
Matrix<T, Rows, Cols> operator-(Matrix<T, Rows, Cols> lhs,
                                const Matrix<T, Rows, Cols> rhs)
{
    return lhs -= rhs;
}

template <typename T, TSize Rows, TSize Cols>
Matrix<T, Rows, Cols> operator/(Matrix<T, Rows, Cols> lhs,
                                const Matrix<T, Rows, Cols> rhs)
{
    return lhs /= rhs;
}

template <typename T, TSize Rows, TSize Cols>
Matrix<T, Rows, Cols> operator%(Matrix<T, Rows, Cols> lhs,
                                const Matrix<T, Rows, Cols> rhs)
{
    return lhs %= rhs;
}

//bitwise
template <typename T, TSize Rows, TSize Cols>
Matrix<T, Rows, Cols> operator^(Matrix<T, Rows, Cols> lhs,
                                const Matrix<T, Rows, Cols> rhs)
{
    return lhs ^= rhs;
}

template <typename T, TSize Rows, TSize Cols>
Matrix<T, Rows, Cols> operator&(Matrix<T, Rows, Cols> lhs,
                                const Matrix<T, Rows, Cols> rhs)
{
    return lhs &= rhs;
}

template <typename T, TSize Rows, TSize Cols>
Matrix<T, Rows, Cols> operator|(Matrix<T, Rows, Cols> lhs,
                                const Matrix<T, Rows, Cols> rhs)
{
    return lhs |= rhs;
}

template <typename T, TSize Rows, TSize Cols>
Matrix<T, Rows, Cols> operator>>=(Matrix<T, Rows, Cols> lhs,
                                const Matrix<T, Rows, Cols> rhs)
{
    return lhs >>= rhs;
}

template <typename T, TSize Rows, TSize Cols>
Matrix<T, Rows, Cols> operator<<=(Matrix<T, Rows, Cols> lhs,
                                  const Matrix<T, Rows, Cols> rhs)
{
    return lhs <<= rhs;
}


template <typename T, typename U, TSize Rows, TSize Cols>
Matrix<T, Rows, Cols> operator*(Matrix<T, Rows, Cols> lhs, const U rhs)
{
    return lhs *= rhs;
}

template <typename T, typename U, TSize Rows, TSize Cols>
Matrix<T, Rows, Cols> operator+(Matrix<T, Rows, Cols> lhs, const U rhs)
{
    return lhs += rhs;
}

template <typename T, typename U, TSize Rows, TSize Cols>
Matrix<T, Rows, Cols> operator-(Matrix<T, Rows, Cols> lhs, const U rhs)
{
    return lhs -= rhs;
}

template <typename T, typename U, TSize Rows, TSize Cols>
Matrix<T, Rows, Cols> operator/(Matrix<T, Rows, Cols> lhs, const U rhs)
{
    return lhs /= rhs;
}

template <typename T, typename U, TSize Rows, TSize Cols>
Matrix<T, Rows, Cols> operator^(Matrix<T, Rows, Cols> lhs, const U rhs)
{
    return lhs ^= rhs;
}

template <typename T, typename U, TSize Rows, TSize Cols>
Matrix<T, Rows, Cols> operator&(Matrix<T, Rows, Cols> lhs, const U rhs)
{
    return lhs &= rhs;
}

template <typename T, typename U, TSize Rows, TSize Cols>
Matrix<T, Rows, Cols> operator|(Matrix<T, Rows, Cols> lhs, const U rhs)
{
    return lhs |= rhs;
}

template <typename T, typename U, TSize Rows, TSize Cols>
Matrix<T, Rows, Cols> operator>>=(Matrix<T, Rows, Cols> lhs, const U rhs)
{
    return lhs >>= rhs;
}

template <typename T, typename U, TSize Rows, TSize Cols>
Matrix<T, Rows, Cols> operator<<=(Matrix<T, Rows, Cols> lhs, const U rhs)
{
    return lhs <<= rhs;
}

template <typename T, typename U, TSize Rows, TSize Cols>
Matrix<T, Rows, Cols> operator*(const U rhs, Matrix<T, Rows, Cols> lhs)
{
    return lhs *= rhs;
}

template <typename T, typename U, TSize Rows, TSize Cols>
Matrix<T, Rows, Cols> operator+(const U rhs, Matrix<T, Rows, Cols> lhs)
{
    return lhs += rhs;
}

template <typename T, typename U, TSize Rows, TSize Cols>
Matrix<T, Rows, Cols> operator-(const U rhs, Matrix<T, Rows, Cols> lhs)
{
    return lhs -= rhs;
}

template <typename T, typename U, TSize Rows, TSize Cols>
Matrix<T, Rows, Cols> operator/(const U rhs, Matrix<T, Rows, Cols> lhs)
{
    return lhs /= rhs;
}

template <typename T, typename U, TSize Rows, TSize Cols>
Matrix<T, Rows, Cols> operator^(const U rhs, Matrix<T, Rows, Cols> lhs)
{
    return lhs ^= rhs;
}

template <typename T, typename U, TSize Rows, TSize Cols>
Matrix<T, Rows, Cols> operator&(const U rhs, Matrix<T, Rows, Cols> lhs)
{
    return lhs &= rhs;
}

template <typename T, typename U, TSize Rows, TSize Cols>
Matrix<T, Rows, Cols> operator|(const U rhs, Matrix<T, Rows, Cols> lhs)
{
    return lhs |= rhs;
}


template <typename T, TSize Cols>
Vector<T, Cols>unit(const Vector<T, Cols> &vector) noexcept
{
    Vector<T, Cols> result = vector;
    result.unit();
    return result;
}


template <typename T, TSize Cols>
Matrix<T, Cols, 1> transpose(Vector<T, Cols> vector) noexcept
{
    Matrix<T, Cols, 1> result;

    for (TSize i = Cols; i--; result[0][i] = vector[i]);

    return result;
}

template <typename T, TSize Rows, TSize Cols>
Matrix<T, Cols, Rows> transpose(const Matrix<T, Rows, Cols> &matrix) noexcept
{
    Matrix<T, Cols, Rows> result;

    for (TSize i = Rows; i--;)
        for (TSize j = Cols; j--; result[j][i] = matrix[i][j]);

    return result;
}

template <typename T, TSize Dim, TSize LRow, TSize RCol>
Matrix<T, LRow, RCol> dot(const Matrix<T, LRow, Dim> &lhs,
                          const Matrix<T, Dim, RCol> &rhs)
{
    Matrix<T, LRow, RCol> res;

    for (TSize i = LRow; i--;)
        for (TSize j = RCol; j--;)
            for (TSize k = Dim; k--; res[i][j] += lhs[i][k] * rhs[k][j]);

    return res;
}

template <typename T, TSize Dim>
T dot(const Matrix<T, 1, Dim> &lhs, const Matrix<T, Dim, 1> &rhs)
{
    T res = {0};

    for (TSize i = Dim; i--; res += lhs[i] * rhs[i][0]);

    return res;
}

template <typename T, TSize Dim>
Matrix<T, Dim, Dim> dot(const Matrix<T, Dim, 1> &lhs,
                        const Matrix<T, 1, Dim> &rhs)
{
    Matrix<T, Dim, Dim> res;

    for (TSize i = Dim; i--;)
        for (TSize j = Dim; j--;)
            res[i][j] = lhs[i][0] * rhs[j];

    return res;
}

//Just cuz.
template <typename T, TSize Dim>
T dot(const Vector<T,Dim> &lhs, const Vector<T,Dim> &rhs)
{
    T res = {0};

    for (TSize i = Dim; i--; res += lhs[i] * rhs[i]);

    return res;
}

template <typename T>
T cross(const Vector<T, 2> &lhs, const Vector<T, 2> &rhs) //illegal
{
    return lhs.x() * rhs.y() - lhs.y() * rhs.x();
}

template <typename T>
Vector3D<T> cross(const Vector3D<T> &lhs, const Vector3D<T> &rhs)
{
    Vector3D<T> result{lhs.y() * rhs.z() - lhs.z() * rhs.y(),
                       lhs.z() * rhs.x() - lhs.x() * rhs.z(),
                       lhs.x() * rhs.y() - lhs.y() * rhs.x()};
    return result;
}

template <typename T, TSize dim>
auto projection(const Vector<T, dim> &of, const Vector<T, dim> &onto)
{
    return (dot(of, onto) / onto.length()) * onto;
}

template <typename T, TSize R, TSize C>
Matrix<T, R - 1, C - 1> submatrix(const Matrix<T, R, C> &m, TSize subRow,
                                  TSize subCol)
{
    assert(subRow < R && subCol < C && "Out of Range");

    Matrix<T, R - 1, C - 1> result;
    TSize si = R - 1, sj;
    for (TSize i = R; i-- > 0;)
        if (i != subRow) {
            sj = C - 1;
            --si;
            for (TSize j = C; j-- > 0;)
                if (j != subCol)
                    result[si][--sj] = m[i][j];
        }

    return result;
}

template <typename T>
T submatrix(const Matrix<T, 2, 2> &m, TSize subRow, TSize subCol)
{
    assert(subRow < 2 && subCol < 2 && "Out of Range");

    if (subRow == 0)
        if (subCol == 0)
            return m[1][1];
        else
            return m[1][0];
    else if (subCol == 0)
        return m[0][1];
    else
        return m[0][0];
}

template <typename T>
T determinant(const Matrix<T, 1, 1> &m)
{
    return static_cast<const T &>(m);
}

template <typename T> T determinant(const T &m) { return T(m); }

template <typename T>
T determinant(const Matrix<T, 2, 2> &m)
{
    return m[0][0] * m[1][1] - m[0][1] * m[1][0];
}

template <typename T>
T determinant(const Matrix<T, 3, 3> &m)
{
    return m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
           m[0][1] * (m[1][0] * m[2][2] - m[2][0] * m[1][2]) +
           m[0][2] * (m[1][0] * m[2][1] - m[2][0] * m[1][1]);
}

template <typename T, TSize Dim>
T determinant(const Matrix<T, Dim, Dim> &m)
{
    signed char o = 1;
    T result = {0};

    for (TSize i = 0; i < Dim; ++i) {
        result += o * m[0][i] * determinant(submatrix(m, 0, i));
        o *= -1;
    }

    return result;
}

template <typename T, TSize Dim>
Matrix<T, Dim, Dim> minor(const Matrix<T, Dim, Dim> &m)
{
    Matrix<T, Dim, Dim> result;

    for (TSize i = Dim; i--;)
        for (TSize j = Dim; j--; result[i][j] = determinant(submatrix(m, i, j)));

    return result;
}

template <typename T, TSize Dim>
Matrix<T, Dim, Dim> invert(const Matrix<T, Dim, Dim> &d,
                           bool *correctness = nullptr)
{
    constexpr auto sing = [](TSize i, TSize j) -> T
    {
        auto s = i + j;
        if (s % 2 == 0)
            return 1;
        else
            return -1;
    };

    Matrix<T, Dim, Dim> result;
    /// A^-1 = (1/|A|) * transpose(A)
    const auto det = determinant(d);
    if (correctness)
    {
        if (det == 0)
        {
            *correctness = false;
            return result; // NO
        }
        else
            *correctness = true;
    }

    result = minor(d);

    for (TSize i = 0; i < result.rows(); ++i)
        for (TSize j = 0; j < result.columns(); j++)
            result[i][j] *= sing(i, j);

    result = transpose(result);
    result /= det;
    return result;
}




}
