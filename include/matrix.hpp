#ifndef MATHSTATICMATRIX_HPP
#define MATHSTATICMATRIX_HPP

#include <cmath>

#include "typedefines.hpp"

namespace Math
{

    //TODO: deteminant, norm, normilize e.t.c.
    //TODO: Spec for Matrix<2,2>, Matrix<3,3>, Matrix<4,4>

template<typename T, TSize Rows, TSize Cols>
struct Matrix
{
    using Type = T;
    using Pointer = Type*;
    using ConstPointer = Type const *;
    using Reference = Type &;
    using ConstReference = const Type &;
    using ColumnRef = Type (&)[Cols];
    using ConstColumnRef = const Type (&)[Cols];

    //TODO: Do we need data init in default constructor
    Matrix() noexcept
    {}

    explicit Matrix(T defaultValue) noexcept
    {
        for(TSize i=Rows;i--;)
            for(TSize j=Cols;j--; _data[i][j]=defaultValue);
    }

    explicit Matrix(const Matrix &rhs) noexcept
    {
        for(TSize i=Rows;i--;)
            for(TSize j=Cols;j--; _data[i][j]=rhs._data[i][j]);
    }

    explicit Matrix(Matrix &&rhs) noexcept
    {
        for(TSize i=Rows;i--;)
            for(TSize j=Cols;j--; _data[i][j]=rhs._data[i][j]);
    }

    Matrix &operator=(const Matrix &rhs)
    {
        for(TSize i=Rows;i--;)
            for(TSize j=Cols;j--;_data[i][j]=rhs._data[i][j]);
        return *this;
    }

    Matrix &operator=(const Matrix &&rhs)
    {
        for(TSize i=Rows;i--;)
            for(TSize j=Cols;j--;_data[i][j]=rhs._data[i][j]);
        return *this;
    }

    ColumnRef operator[](const TSize row)
    {
        assert(row < Rows);
        return _data[row];
    }

    ConstColumnRef operator[](const TSize row) const
    {
        assert(row < Rows);
        return _data[row];
    }

    //{Row,Col}
    Reference operator[](std::pair<TSize,TSize> pos)
    {
        assert(pos.first < Rows);
        assert(pos.second < Cols);
        return _data[pos.first][pos.second];
    }

    //{Row,Col}
    ConstReference operator[](std::pair<TSize,TSize> pos) const
    {
        assert(pos.first < Rows);
        assert(pos.second < Cols);
        return _data[pos.first][pos.second];
    }

    constexpr auto rows() const noexcept
    {
        return Rows;
    }

    constexpr auto size() const noexcept
    {
        return Rows*Cols;
    }

    constexpr auto columns() const noexcept
    {
        return Cols;
    }



private:
    T _data[Rows][Cols];
};


template<typename T, TSize Num>
struct Matrix<T,1,Num> //Vector
{
    using Type = T;
    using Pointer = Type*;
    using ConstPointer = Type const *;
    using Reference = Type &;
    using ConstReference = const Type &;
    using ColumnRef = Type &;
    using ConstColumnRef = const Type &;


    Matrix() noexcept
    {}

    explicit Matrix(const T defaultValue) noexcept
    {
        for(TSize i = Num; i--; _data[i]=defaultValue);
    }

    Matrix(const Matrix &matrix)
    {
        for(TSize i=Num; i--; _data[i] = matrix._data[i]);
    }

    Matrix(Matrix &&matrix)
    {
        for(TSize i=Num; i--; _data[i] = matrix._data[i]);
    }

    Matrix(const T (&t)[Num])
    {
        for(TSize i=Num;i--;_data[i] = t[i]);
    }

    Matrix &operator=(const Matrix &rhs)
    {
        for(TSize i=Num;i--;_data[i]=rhs._data[i]);
        return *this;
    }

    Matrix &operator=(Matrix &&rhs)
    {
        for(TSize i=Num;i--;_data[i]=rhs._data[i]);
        return *this;
    }

    T &operator[](const TSize pos)
    {
        assert(pos < Num);
        return _data[pos];
    }

    const T &operator[](const TSize pos) const
    {
        assert(pos < Num);
        return _data[pos];
    }

    T norm() const noexcept
    {
        T result = 0;
        for(TSize i = 0; i<Num; ++i)
            result+=_data[i]*_data[i];
        return std::sqrt(result);
    }

    constexpr auto rows() const noexcept
    {
        return 1;
    }

    constexpr auto size() const noexcept
    {
        return Num;
    }

    constexpr auto columns() const noexcept
    {
        return Num;
    }

    inline explicit operator auto() {return _data;}

private:
    T _data[Num];
};


//Vector4D
template<typename T>
struct Matrix<T,1,4>
{
    using Type = T;
    using Pointer = Type*;
    using ConstPointer = Type const *;
    using Reference = Type &;
    using ConstReference = const Type &;
    using ColumnRef = Type &;
    using ConstColumnRef = const Type &;

    static constexpr TSize Num = 4;

    Matrix() noexcept
    {}

    Matrix(T x, T y, T z, T w) noexcept :
        _data{x,y,z,w}
    {}

    Matrix(const T (&t)[4]) noexcept :
        _data{t[0],t[1],t[2],t[3]}
    {}

    Matrix(const Matrix &m) noexcept :
        _data{m[0],m[1],m[2],m[3]}
    {}

    Matrix(Matrix &&m) noexcept :
        _data{m[0],m[1],m[2],m[3]}
    {}

    Matrix &operator=(const Matrix &rhs)
    {
        _data[0] = rhs._data[0];
        _data[1] = rhs._data[1];
        _data[2] = rhs._data[2];
        _data[3] = rhs._data[3];
        return *this;
    }

    Matrix &operator=(Matrix &&rhs)
    {
        _data[0] = rhs._data[0];
        _data[1] = rhs._data[1];
        _data[2] = rhs._data[2];
        _data[3] = rhs._data[3];
        return *this;
    }

    Reference operator[](const TSize pos)
    {
        assert(pos < Num);
        return _data[pos];
    }

    ConstReference operator[](const TSize pos) const
    {
        assert(pos < Num);
        return _data[pos];
    }

    constexpr auto rows() const noexcept
    {
        return 1;
    }

    constexpr auto size() const noexcept
    {
        return Num;
    }

    constexpr auto columns() const noexcept
    {
        return Num;
    }

    inline ConstReference x() const noexcept
    {
        return _data[0];
    }

    inline ConstReference y() const noexcept
    {
        return _data[1];
    }

    inline ConstReference z() const noexcept
    {
        return _data[2];
    }

    inline ConstReference w() const noexcept
    {
        return _data[3];
    }

    inline Reference x() noexcept
    {
        return _data[0];
    }

    inline Reference y() noexcept
    {
        return _data[1];
    }

    inline Reference z() noexcept
    {
        return _data[2];
    }

    inline Reference w() noexcept
    {
        return _data[3];
    }

    inline explicit operator auto() {return _data;}

    T norm()
    {
        return std::sqrt(_data[0]*_data[0] +
                         _data[1]*_data[1] +
                         _data[2]*_data[2] +
                         _data[3]*_data[3]);
    }

private:
    T _data[Num];
};



//Vector3D
template<typename T>
struct Matrix<T,1,3>
{
    using Type = T;
    using Pointer = Type*;
    using ConstPointer = Type const *;
    using Reference = Type &;
    using ConstReference = const Type &;
    using ColumnRef = Type &;
    using ConstColumnRef = const Type &;

    static constexpr TSize Num = 3;

    Matrix() noexcept
    {}

    Matrix(T x, T y, T z) noexcept :
        _data{x,y,z}
    {}

    Matrix(const T (&t)[3]) noexcept :
        _data{t[0],t[1],t[2]}
    {}

    Matrix(const Matrix &m) noexcept :
        _data{m[0],m[1],m[2]}
    {}

    Matrix(Matrix &&m) noexcept :
        _data{m[0],m[1],m[2]}
    {}

    Matrix &operator=(const Matrix &rhs)
    {
        _data[0] = rhs._data[0];
        _data[1] = rhs._data[1];
        _data[2] = rhs._data[2];
        return *this;
    }

    Matrix &operator=(Matrix &&rhs)
    {
        _data[0] = rhs._data[0];
        _data[1] = rhs._data[1];
        _data[2] = rhs._data[2];
        return *this;
    }

    Reference operator[](const TSize pos)
    {
        assert(pos < Num);
        return _data[pos];
    }

    ConstReference operator[](const TSize pos) const
    {
        assert(pos < Num);
        return _data[pos];
    }

    constexpr auto rows() const noexcept
    {
        return 1;
    }

    constexpr auto size() const noexcept
    {
        return Num;
    }

    constexpr auto columns() const noexcept
    {
        return Num;
    }

    inline ConstReference x() const noexcept
    {
        return _data[0];
    }

    inline ConstReference y() const noexcept
    {
        return _data[1];
    }

    inline ConstReference z() const noexcept
    {
        return _data[2];
    }

    inline Reference x() noexcept
    {
        return _data[0];
    }

    inline Reference y() noexcept
    {
        return _data[1];
    }

    inline Reference z() noexcept
    {
        return _data[2];
    }

    inline explicit operator auto() {return _data;}

    T norm()
    {
        return std::sqrt(_data[0]*_data[0] + _data[1]*_data[1] + _data[2]*_data[2]);
    }

private:
    T _data[Num];
};


//Vector2D
template<typename T>
struct Matrix<T,1,2>
{
    using Type = T;
    using Pointer = Type*;
    using ConstPointer = Type const *;
    using Reference = Type &;
    using ConstReference = const Type &;
    using ColumnRef = Type &;
    using ConstColumnRef = const Type &;

    static constexpr TSize Num = 2;

    Matrix() noexcept
    {}

    Matrix(T x, T y) noexcept :
        _data{x,y}
    {}

    Matrix(const T (&t)[2]) noexcept :
        _data{t[0],t[1]}
    {}

    Matrix(const Matrix &m) noexcept :
        _data{m[0],m[1]}
    {}

    Matrix(Matrix &&m) noexcept :
        _data{m[0],m[1]}
    {}

    Matrix &operator=(const Matrix &rhs)
    {
        _data[0] = rhs._data[0];
        _data[1] = rhs._data[1];
        return *this;
    }

    Matrix &operator=(Matrix &&rhs)
    {
        _data[0] = rhs._data[0];
        _data[1] = rhs._data[1];
        return *this;
    }

    Reference operator[](const TSize pos)
    {
        assert(pos < Num);
        return _data[pos];
    }

    ConstReference operator[](TSize pos) const
    {
        assert(pos < Num);
        return _data[pos];
    }

    constexpr auto rows() const noexcept
    {
        return 1;
    }

    constexpr auto size() const noexcept
    {
        return Num;
    }

    constexpr auto columns() const noexcept
    {
        return Num;
    }

    inline ConstReference x() const noexcept
    {
        return _data[0];
    }

    inline ConstReference y() const noexcept
    {
        return _data[1];
    }

    inline Reference x() noexcept
    {
        return _data[0];
    }

    inline Reference y() noexcept
    {
        return _data[1];
    }

    T norm()
    {
        return std::sqrt(_data[0]*_data[0] + _data[1]*_data[1]);
    }

    inline explicit operator auto() {return _data;}

private:
    T _data[Num];
};

template<typename T>
struct Matrix<T,1,1> //LoL
{
    using Type = T;
    using Pointer = Type*;
    using ConstPointer = Type const *;
    using Reference = Type &;
    using ConstReference = const Type &;
    using ColumnRef = Type &;
    using ConstColumnRef = const Type &;

    Matrix() noexcept
    {}

    Matrix(T x) noexcept :
        _data(x)
    {}

    Matrix &operator=(Matrix rhs)
    {
        _data = rhs._data;

        return *this;
    }

    inline operator T() const
    {
        return _data;
    }

private:
    T _data;
};


template<typename T, TSize Num>
using Vector = Matrix<T,1,Num>;

template<typename T>
using Vector4D = Vector<T,4>;

template<typename T>
using Vector3D = Vector<T,3>;

template<typename T>
using Vector2D = Vector<T,2>;



template<typename T, TSize Row, TSize Col>
constexpr bool isSquare(const Matrix<T,Row,Col> &)
{
    return Col == Row;
}

template<typename T, TSize Row, TSize Col>
constexpr bool isVector(const Matrix<T,Row,Col> &)
{
    return Row == 1;
}

template<typename T, TSize Row, TSize Col>
constexpr bool isSingular(const Matrix<T,Row,Col> &i)
{
    return Col == 0 || Row == 0;
}


//Comparsion

template<typename T, TSize Row, TSize Col>
bool operator==(Matrix<T,Row,Col> lhs, Matrix<T,Row,Col> rhs)
{
    for(TSize i=Row;i--;)
        for(TSize j=Col;j--;)
            if(lhs[i][j] != rhs[i][j])
                return false;
    return true;
}

template<typename T, TSize Num>
bool operator==(Vector<T,Num> lhs, Vector<T,Num> rhs)
{
    for(TSize i=Num;i--;)
        if(lhs[i] != rhs[i])
            return false;
    return true;
}

template<typename T, TSize Row, TSize Col>
bool operator!=(Matrix<T,Row,Col> lhs, Matrix<T,Row,Col> rhs)
{
    for(TSize i=Row;i--;)
        for(TSize j=Col;j--;)
            if(lhs[i][j] == rhs[i][j])
                return false;
    return true;
}

template<typename T, TSize Num>
bool operator!=(Vector<T,Num> lhs, Vector<T,Num> rhs)
{
    for(TSize i=Num;i--;)
        if(lhs[i] == rhs[i])
            return false;
    return true;
}

template<typename T, TSize Row, TSize Col>
bool operator<=(Matrix<T,Row,Col> lhs, Matrix<T,Row,Col> rhs)
{
    for(TSize i=Row;i--;)
        for(TSize j=Col;j--;)
            if(!(lhs[i][j] <= rhs[i][j]))
                return false;
    return true;
}

template<typename T, TSize Num>
bool operator>=(Vector<T,Num> lhs, Vector<T,Num> rhs)
{
    for(TSize i=Num;i--;)
        if(!(lhs[i] >= rhs[i]))
            return false;
    return true;
}

template<typename T, TSize Row, TSize Col>
bool operator<(Matrix<T,Row,Col> lhs, Matrix<T,Row,Col> rhs)
{
    for(TSize i=Row;i--;)
        for(TSize j=Col;j--;)
            if(!(lhs[i][j] < rhs[i][j]))
                return false;
    return true;
}

template<typename T, TSize Num>
bool operator>(Vector<T,Num> lhs, Vector<T,Num> rhs)
{
    for(TSize i=Num;i--;)
        if(!(lhs[i] > rhs[i]))
            return false;
    return true;
}

//Arithmetical methods
//unary op
template<typename T, TSize Rows, TSize Cols>
Matrix<T,Rows,Cols> operator-(const Matrix<T,Rows,Cols> rhs)
{
    Matrix<T,Rows,Cols> r;
    for(TSize i=Rows; i--;)
        for(TSize j=Cols; j--; r[i][j] = -rhs[i][j]);
    return r;
}

template<typename T, TSize Rows, TSize Cols>
Matrix<T,Rows,Cols> &operator*=(Matrix<T,Rows,Cols> &lhs, const Matrix<T,Rows,Cols> rhs)
{
    for(TSize i=Rows; i--;)
        for(TSize j=Cols; j--; lhs[i][j] *= rhs[i][j]);
    return lhs;
}

template<typename T, TSize Rows, TSize Cols>
Matrix<T,Rows,Cols> &operator+=(Matrix<T,Rows,Cols> &lhs, const Matrix<T,Rows,Cols> rhs)
{
    for(TSize i=Rows; i--;)
        for(TSize j=Cols; j--; lhs[i][j] += rhs[i][j]);
    return lhs;
}

template<typename T, TSize Rows, TSize Cols>
Matrix<T,Rows,Cols> &operator-=(Matrix<T,Rows,Cols> &lhs, const Matrix<T,Rows,Cols> rhs)
{
    for(TSize i=Rows; i--;)
        for(TSize j=Cols; j--; lhs[i][j] -= rhs[i][j]);
    return lhs;
}

template<typename T, TSize Rows, TSize Cols>
Matrix<T,Rows,Cols> &operator/=(Matrix<T,Rows,Cols> &lhs, const Matrix<T,Rows,Cols> rhs)
{
    for(TSize i=Rows; i--;)
        for(TSize j=Cols; j--; lhs[i][j] /= rhs[i][j]);
    return lhs;
}

template<typename T, typename U, TSize Rows, TSize Cols>
Matrix<T,Rows,Cols> &operator*=(Matrix<T,Rows,Cols> &lhs, const U rhs)
{
    for(TSize i=Rows; i--;)
        for(TSize j=Cols; j--; lhs[i][j] *= rhs);
    return lhs;
}

template<typename T, typename U, TSize Rows, TSize Cols>
Matrix<T,Rows,Cols> &operator+=(Matrix<T,Rows,Cols> &lhs, const U rhs)
{
    for(TSize i=Rows; i--;)
        for(TSize j=Cols; j--; lhs[i][j] += rhs);
    return lhs;
}

template<typename T, typename U, TSize Rows, TSize Cols>
Matrix<T,Rows,Cols> &operator-=(Matrix<T,Rows,Cols> &lhs, const U rhs)
{
    for(TSize i=Rows; i--;)
        for(TSize j=Cols; j--; lhs[i][j] -= rhs);
    return lhs;
}

template<typename T, typename U, TSize Rows, TSize Cols>
Matrix<T,Rows,Cols> &operator/=(Matrix<T,Rows,Cols> &lhs, const U rhs)
{
    for(TSize i=Rows; i--;)
        for(TSize j=Cols; j--; lhs[i][j] /= rhs);
    return lhs;
}

template<typename T, TSize Rows, TSize Cols>
Matrix<T,Rows,Cols> normalize(Matrix<T,Rows,Cols> matrix)
{
    matrix = (matrix)*(1./matrix.norm());
    return matrix;
}

template<typename T, typename U, TSize Rows, TSize Cols>
Matrix<T,Rows,Cols> normalize(Matrix<T,Rows,Cols> matrix, U l)
{
    matrix = (matrix)*(l/matrix.norm());
    return matrix;
}

//binary op

template<typename T, TSize Rows, TSize Cols>
Matrix<T,Rows,Cols> operator*(Matrix<T,Rows,Cols> lhs, const Matrix<T,Rows,Cols> rhs)
{
    return lhs*=rhs;
}

template<typename T, TSize Rows, TSize Cols>
Matrix<T,Rows,Cols> operator+(Matrix<T,Rows,Cols> lhs, const Matrix<T,Rows,Cols> rhs)
{
    return lhs+=rhs;
}

template<typename T, TSize Rows, TSize Cols>
Matrix<T,Rows,Cols> operator-(Matrix<T,Rows,Cols> lhs, const Matrix<T,Rows,Cols> rhs)
{
    return lhs-=rhs;
}

template<typename T, TSize Rows, TSize Cols>
Matrix<T,Rows,Cols> operator/(Matrix<T,Rows,Cols> lhs, const Matrix<T,Rows,Cols> rhs)
{
    return lhs/=rhs;
}

template<typename T, typename U, TSize Rows, TSize Cols>
Matrix<T,Rows,Cols> operator*(Matrix<T,Rows,Cols> lhs, const U rhs)
{
    return lhs*=rhs;
}

template<typename T, typename U, TSize Rows, TSize Cols>
Matrix<T,Rows,Cols> operator+(Matrix<T,Rows,Cols> lhs, const U rhs)
{
    return lhs+=rhs;
}

template<typename T, typename U, TSize Rows, TSize Cols>
Matrix<T,Rows,Cols> operator-(Matrix<T,Rows,Cols> lhs, const U rhs)
{
    return lhs-=rhs;
}

template<typename T, typename U, TSize Rows, TSize Cols>
Matrix<T,Rows,Cols> operator/(Matrix<T,Rows,Cols> lhs, const U rhs)
{
    return lhs/=rhs;
}

template<typename T, typename U, TSize Rows, TSize Cols>
Matrix<T,Rows,Cols> operator*(const U rhs, Matrix<T,Rows,Cols> lhs)
{
    return lhs*=rhs;
}

template<typename T, typename U, TSize Rows, TSize Cols>
Matrix<T,Rows,Cols> operator+(const U rhs, Matrix<T,Rows,Cols> lhs)
{
    return lhs+=rhs;
}

template<typename T, typename U, TSize Rows, TSize Cols>
Matrix<T,Rows,Cols> operator-(const U rhs, Matrix<T,Rows,Cols> lhs)
{
    return lhs-=rhs;
}

template<typename T, typename U, TSize Rows, TSize Cols>
Matrix<T,Rows,Cols> operator/(const U rhs, Matrix<T,Rows,Cols> lhs)
{
    return lhs/=rhs;
}


template<typename T, TSize Rows, TSize Cols>
Matrix<T,Cols,Rows> transpose(const Matrix<T,Rows,Cols> matrix) noexcept
{
    Matrix<T,Cols,Rows> result;

    for(TSize i=Rows; i--;)
        for(TSize j=Cols; j--; result[j][i] = matrix[i][j]);

    return result;
}


template<typename T, TSize Dim, TSize LRow, TSize RCol>
Matrix<T,LRow,RCol> dotProduct(const Matrix<T,LRow,Dim> lhs, const Matrix<T,Dim,RCol> rhs)
{
    Matrix<T,LRow,RCol> res;

    for(TSize i=LRow;i--;)
        for(TSize j=RCol;j--;)
            for(TSize k=Dim; k--; res[i][j] += lhs[i][k] * rhs[k][j]);

    return res;
}

template<typename T, TSize Num>
Vector<T,Num> operator-(const Vector<T,Num> rhs)
{
    Matrix<T,1,Num> r;
    for(TSize i=Num; i--; r[i] = -(rhs[i]));
    return r;
}

template<typename T, TSize Num>
Vector<T,Num> &operator*=(Vector<T,Num> &lhs, Vector<T,Num> rhs)
{
    for(TSize i=Num; i--; lhs[i] *= rhs[i]);
    return lhs;
}

template<typename T, TSize Num>
Vector<T,Num> &operator+=(Vector<T,Num> &lhs, Vector<T,Num> rhs)
{
    for(TSize i=Num; i--; lhs[i] += rhs[i]);
    return lhs;
}

template<typename T, TSize Num>
Vector<T,Num> &operator-=(Vector<T,Num> &lhs, Vector<T,Num> rhs)
{
    for(TSize i=Num; i--; lhs[i] -= rhs[i]);
    return lhs;
}

template<typename T,  TSize Num>
Vector<T,Num> &operator/=(Vector<T,Num> &lhs, Vector<T,Num> rhs)
{
    for(TSize i=Num; i--; lhs[i] /= rhs[i]);
    return lhs;
}

template<typename T, typename U, TSize Num>
Vector<T,Num> &operator*=(Vector<T,Num> &lhs,  U rhs)
{
    for(TSize i=Num; i--; lhs[i] *= rhs);
    return lhs;
}

template<typename T, typename U, TSize Num>
Vector<T,Num> &operator+=(Vector<T,Num> &lhs, U rhs)
{
    for(TSize i=Num; i--; lhs[i] += rhs);
    return lhs;
}

template<typename T, typename U, TSize Num>
Vector<T,Num> &operator-=(Vector<T,Num> &lhs, U rhs)
{
    for(TSize i=Num; i--; lhs[i] -= rhs);
    return lhs;
}

template<typename T, typename U, TSize Num>
Vector<T,Num> &operator/=(Vector<T,Num> &lhs, U rhs)
{
    for(TSize i=Num; i--; lhs[i] /= rhs);
    return lhs;
}

template<typename T, TSize Dim>
T dotProduct(Vector<T,Dim> lhs, Vector<T,Dim> rhs)
{
    T ret = T();
    for(TSize i=Dim;i--;ret+=lhs[i]*rhs[i]);
    return ret;
}

template<typename T, TSize size>
auto projection(Vector<T,size> &lhs, Vector<T,size> &rhs)
{
    return (lhs*rhs)/(rhs*rhs)*rhs;
}

//TODO: Other Binary Functions


//TODO: Other specializations

template<typename T>
T crossProduct(const Vector<T,2> lhs, const Vector<T,2> rhs)
{
    return lhs.x()*rhs.y()-lhs.y()*rhs.x();
}

template<typename T>
Vector<T,3> crossProduct(const Vector<T,3> lhs, const Vector<T,3> rhs)
{
    Vector<T,3> result
    (
        lhs.y()*rhs.z() - lhs.z()*rhs.y(),
        lhs.z()*rhs.x() - lhs.x()*rhs.z(),
        lhs.x()*rhs.y() - lhs.y()*rhs.x()
    );
    return result;
}


//TODO: ToDo
template<typename T, TSize Row, TSize Col>
Matrix<T,Row,Col> inverse(Matrix<T,Row,Col> m)
{
    //TODO: Invert
}

template<typename T, TSize Row, TSize Col, TSize OldRow, TSize OldCol>
Matrix<T,Row,Col> submatrix(const Matrix<T,OldRow,OldCol> m , TSize ro, TSize co)
{
    static_assert(Row < OldRow && Col < OldCol, "Submatrix dim can't be greater than parent matrix dim");
    assert(Row + ro < OldRow && Col + co < OldCol && "ARRRRRR"); //TODO: Better description

    Matrix<T,Row,Col> result;

    for(TSize i=Row;i--;)
        for(TSize j=Col;j--;result[i][j] = m[i+ro][j+co]);

    return result;
}

template<typename T, TSize Size, TSize OldSize>
Vector<T,Size> submatrix(const Vector<T,OldSize> v, TSize start) //TODO: Better name needed
{
    static_assert(Size < OldSize, "Subvector dim can't be greate than parent dim");
    assert(Size+start < OldSize && "ARRRRRR"); //TODO: Better description
    Vector<T,Size> result;

    for(TSize i=Size;i--;)
        result[i] = v[i+start];
    return result;
}

template<typename T, TSize Size, TSize OldSize>
constexpr auto subvector = &submatrix<T,Size,OldSize>; //function alias

template<typename T, TSize Rows, TSize Cols>
Matrix<T,Rows,Cols> deteminant(const Matrix<T,Rows,Cols> &matrix) noexcept
{
    static_assert(Rows == Cols, "Matrix must be squart");

    Matrix<T,Rows,Cols> m;

}

}





#endif //MATHSTATICMATRIX_HPP
