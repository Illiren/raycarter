#pragma once

#include "matrix.hpp"
#if __SSE__ == 1
#include <xmmintrin.h>

namespace Math
{
/*
 * 4D Vector float specilization
 */
template <>
struct Matrix<float, 1, 4>
{
    static constexpr auto Rows = 1;
    static constexpr auto Columns = 4;
    static constexpr auto Size = 4;
    using Type = float;
    using Pointer = Type *;
    using Reference = Type &;
    using ConstReference = const Type &;
    using TColumn  = Type;
    using RColumn  = TColumn &;
    using CRColumn = const TColumn &;
    using TData = Type [Size];
    using RData = Type (&)[Size];
    using CRData = const Type (&)[Size];

    constexpr Matrix() noexcept {}
    constexpr Matrix(Type defaultValue) noexcept :
          _data{defaultValue, defaultValue, defaultValue, defaultValue}
    {}
    constexpr Matrix(Type x, Type y, Type z, Type w) noexcept
        : _data{x, y, z, w}
    {}
    constexpr Matrix(const Type (&d)[Size]) noexcept
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
        auto k = Type{1} / length();
        *this *= k;
        return *this;
    }

    operator RData() { return _data; }
    operator CRData() const { return _data; }

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
        *this = _mm_sub_ps(*this,m);
        return *this;
    }
    Matrix &operator+=(Matrix m)
    {
        *this = _mm_add_ps(*this,m);
        return *this;
    }
    Matrix &operator*=(Matrix m)
    {
        *this = _mm_mul_ps(*this,m);
        return *this;
    }
    Matrix &operator/=(Matrix m)
    {
        *this = _mm_div_ps(*this,m);
        return *this;
    }
    Matrix &operator-=(Type m) {
        __m128 v = _mm_set1_ps(m);
        *this = _mm_sub_ps(*this,v);
        return *this;
    }
    Matrix &operator+=(Type m) {
        __m128 v = _mm_set1_ps(m);
        *this = _mm_add_ps(*this,v);
        return *this;
    }
    Matrix &operator*=(Type m) {
        __m128 v = _mm_set1_ps(m);
        *this = _mm_mul_ps(*this,v);
        return *this;
    }
    Matrix &operator/=(Type m) {
        __m128 v = _mm_set1_ps(m);
        *this = _mm_div_ps(*this,v);
        return *this;
    }

    template<typename U>
    operator Matrix<U,Rows,Columns>() const noexcept requires std::is_convertible_v<Type,U>
    {
        Matrix<U,Rows,Columns> m;
        m.x() = static_cast<U>(_data[0]);
        m.y() = static_cast<U>(_data[1]);
        m.z() = static_cast<U>(_data[2]);
        m.w() = static_cast<U>(_data[3]);
        return m;
    }

    Matrix(__m128 data) noexcept
    {
        _mm_store_ps(_data,data);
    }

    Matrix &operator=(__m128 data) noexcept
    {
        _mm_store_ps(_data,data);
        return *this;
    }

    operator __m128() const noexcept
    {
        return _mm_load_ps(_data);
    }

protected:
    TData _data;
};


Matrix<float,1,4>::Type dot(const Matrix<float,1,4> &lhs, const Matrix<float,1,4> &rhs)
{
    return lhs.x() * rhs.x() +
           lhs.y() * rhs.y() +
           lhs.z() * rhs.z() +
           lhs.w() * rhs.w();
}

#endif

#ifdef __AVX__
#include <avx512fintrin.h>

template <>
struct Matrix<float, 4, 4>
{
    static constexpr auto Rows = 4;
    static constexpr auto Columns = 4;
    static constexpr auto Size = Rows * Columns;
    using Type = float;
    using Pointer = Type *;
    using Reference = Type &;
    using ConstReference = const Type &;
    using TColumn  = Matrix<float, 1, Columns>;
    using RColumn  = TColumn &;
    using CRColumn = const TColumn &;
    using TData = Matrix<float, 1, Columns> [Rows];
    using RData = Matrix<float, 1, Columns> (&)[Rows];
    using CRData = const Matrix<float, 1, Columns> (&)[Rows];

    constexpr Matrix() noexcept {}
    constexpr Matrix(Type defaultValue) noexcept :
          _data{{defaultValue,defaultValue,defaultValue,defaultValue},
                {defaultValue,defaultValue,defaultValue,defaultValue},
                {defaultValue,defaultValue,defaultValue,defaultValue},
                {defaultValue,defaultValue,defaultValue,defaultValue}}
    {}
    constexpr Matrix(TColumn x, TColumn y, TColumn z, TColumn w) noexcept
        : _data{x, y, z, w}
    {}
    constexpr Matrix(Type (&data)[Rows][Columns]) :
          _data{data[0],data[1],data[2],data[3]}
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

    RColumn operator[](TSize pos) noexcept {
        assert(pos < Size);
        return _data[pos];
    }
    CRColumn operator[](TSize pos) const noexcept {
        assert(pos < Size);
        return _data[pos];
    }
    constexpr auto size() const noexcept { return Size; }
    constexpr TSize rows() const noexcept { return Rows; }
    constexpr TSize columns() const noexcept { return Columns; }

    operator RData() { return _data; }
    operator CRData() const { return _data; }

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
    /*
    Matrix &operator-=(Matrix m) {
        *this = _mm_sub_ps(*this,m);
        return *this;
    }
    Matrix &operator+=(Matrix m)
    {
        *this = _mm_add_ps(*this,m);
        return *this;
    }
    Matrix &operator*=(Matrix m)
    {
        *this = _mm_mul_ps(*this,m);
        return *this;
    }
    Matrix &operator/=(Matrix m)
    {
        *this = _mm_div_ps(*this,m);
        return *this;
    }
    Matrix &operator-=(Type m) {
        __m128 v = _mm_set1_ps(m);
        *this = _mm_sub_ps(*this,v);
        return *this;
    }
    Matrix &operator+=(Type m) {
        __m128 v = _mm_set1_ps(m);
        *this = _mm_add_ps(*this,v);
        return *this;
    }
    Matrix &operator*=(Type m) {
        __m128 v = _mm_set1_ps(m);
        *this = _mm_mul_ps(*this,v);
        return *this;
    }
    Matrix &operator/=(Type m) {
        __m128 v = _mm_set1_ps(m);
        *this = _mm_div_ps(*this,v);
        return *this;
    }*/

    template<typename U>
    operator Matrix<U,Rows,Columns>() const noexcept requires std::is_convertible_v<Type,U>
    {
        Matrix<U,Rows,Columns> m;
        m.x() = static_cast<U>(_data[0]);
        m.y() = static_cast<U>(_data[1]);
        m.z() = static_cast<U>(_data[2]);
        m.w() = static_cast<U>(_data[3]);
        return m;
    }

//TODO: AVX intrinsix

protected:
    TData _data;
};


#endif

//TODO
// https://developer.arm.com/documentation/dui0472/k/Using-NEON-Support/NEON-intrinsics-for-operations-with-a-scalar-value

}
