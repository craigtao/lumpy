/*!
 * lumpy is a common c++1z library, mayby the missing library for you.
 * Copyright (C) 2016 lumpy.zhu@gmail.com
 *
 * This program is free software:
 *   you can redistribute it and/or modify it under GPLv3 (or any later version).
 *
 * This program is destributed in the hope that is will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You shold have received a copy of the GNU General Public License alone with the program.
 * If not, see <http://www.gnu.org/licenses/>
 */
#pragma once

#include <lumpy/core/immutable.h>

namespace lumpy
{
inline namespace core
{

template<class T, uint N> struct Vec {
    using Type      = T;

    template<class ..._T> constexpr Vec(const _T& ...val): data { val...} {}
    constexpr Vec(const T(&arr)[N]) : Vec(arr, !TIdx<N>{}) {}

    constexpr static uint size() { return N; }
    constexpr const Type&   operator[](uint index) const noexcept { return data[index]; }
    Type&                   operator[](uint index)       noexcept { return data[index]; }

    Type  data[size()];

    typedef Type ArrType[N];
    constexpr operator const ArrType& () const noexcept { return data; }
    operator                 ArrType& ()       noexcept { return data; }

  private:
    template<uint ...I> constexpr Vec(const T(&arr)[N], TIdx<I...>): data{ arr[I]...} {}
};

template<class T>
struct Vec<T, 1>
{
    using Type  = T;

    constexpr Vec(T ax)             : x(ax)     {}
    constexpr Vec(const T(&v)[1])   : Vec(v[0]) {}

    static constexpr uint size() { return 1; }
    constexpr const Type&   operator[](uint index) const noexcept { return data[index]; }
    Type&                   operator[](uint index)       noexcept { return data[index]; }

    union { Type  data[1]; struct { Type x; }; };

    typedef Type ArrType[1];
    constexpr operator const ArrType& () const noexcept { return data; }
    operator ArrType& ()                       noexcept { return data; }
};

template<class T>
struct Vec<T,2>
{
    using Type  = T;

    constexpr Vec(T ax, T ay)       : x(ax)   , y(ay) {}
    constexpr Vec(const T(&v)[2])   : Vec(v[0], v[1]) {}

    static constexpr uint size() { return 2; }
    constexpr const Type&   operator[](uint index) const noexcept { return data[index]; }
    Type&                   operator[](uint index)       noexcept { return data[index]; }

    union { Type  data[2]; struct { Type x, y; }; };

    typedef Type ArrType[2];
    constexpr operator const ArrType& () const noexcept { return data; }
    operator                 ArrType& ()       noexcept { return data; }
};

template<class T>
struct Vec<T,3>
{
    using Type  = T;

    constexpr Vec(T ax, T ay, T az) : x(ax)   , y(ay), z(az) {}
    constexpr Vec(const T(&v)[3])   : Vec(v[0], v[1],  v[2]) {}

    static constexpr uint size() { return 3; }
    constexpr const Type&   operator[](uint index) const noexcept { return data[index]; }
    Type&                   operator[](uint index)       noexcept { return data[index]; }

    union { Type  data[3]; struct { Type x, y, z; }; };

    typedef Type ArrType[3];
    constexpr operator const ArrType& () const noexcept { return data; }
    operator                 ArrType& ()       noexcept { return data; }
};

template<class T>
struct Vec<T,4>
{
    using Type  = T;

    constexpr Vec(T ax, T ay, T az, T aw) : x(ax)   , y(ay), z(az), w(aw) {}
    constexpr Vec(const T(&v)[4])         : Vec(v[0], v[1],  v[2],  v[3]) {}

    static constexpr uint size() { return 4; }
    constexpr const Type&   operator[](uint index) const noexcept { return data[index]; }
    Type&                   operator[](uint index)       noexcept { return data[index]; }

    union { Type  data[4]; struct { Type x, y, z, w; }; };

    typedef Type ArrType[4];
    constexpr operator const ArrType& () const noexcept { return data; }
    operator                 ArrType& ()       noexcept { return data; }
};

using   int2    = Vec<int,   2>;
using   int3    = Vec<int,   3>;
using   int4    = Vec<int,   4>;

using   uint2   = Vec<uint,  2>;
using   uint3   = Vec<uint,  3>;
using   uint4   = Vec<uint,  4>;

using   float2  = Vec<float, 2>;
using   float3  = Vec<float, 3>;
using   float4  = Vec<float, 4>;

using   double2 = Vec<double,2>;
using   double3 = Vec<double,3>;
using   double4 = Vec<double,4>;

}

}
