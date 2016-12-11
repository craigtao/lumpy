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

#include <lumpy/core/type.h>
#include <lumpy/core/immutable.h>

namespace lumpy
{

inline namespace core
{

template<uint N>        struct ArgName;
template<class ...T>    struct Tuple;

struct Dollar {
    constexpr Dollar() = default;

    template<size_t N>
    constexpr ArgName<N> operator[](const char (&key)[N]) const {
        return {key};
    }

    template<class T>
    constexpr T operator-(T value) const {
        return T(-1) - value;
    }
};
static const Dollar $ = {};

/* --- tuple        --- */
template<>
struct Tuple<>
{
    constexpr static const uint size = 0;
};

template<class T>
struct Tuple<T>
{
    constexpr static const uint size = 1;

    template<class U> constexpr Tuple(U&& u): value(std::forward<U>(u)) {}

    template<uint I> T&                 at()                 { static_assert(I==0, "out of range"); return value; }
    template<uint I> constexpr const T& at() const noexcept  { static_assert(I==0, "out of range"); return value; }

protected:
    T value;
};

template<class T, class ...Ts>
struct Tuple<T, Ts...>
{
    constexpr static const uint size = uint(1+sizeof...(Ts));

    template<class U, class ...Us> constexpr Tuple(U&& u, Us&& ...us): value(std::forward<U>(u)), values(std::forward<Us>(us)...) {}

    template<uint I> auto&           at()                { return at_dispatch<I>(TBool<I==0>{});  }
    template<uint I> constexpr auto& at() const noexcept { return at_dispatch<I>(TBool<I==0>{});  }


  protected:
    T               value;
    Tuple<Ts...>    values;

  private:
    template<uint I> T&                 at_dispatch(TBool<true>)                 { return value; }
    template<uint I> constexpr const T& at_dispatch(TBool<true>) const noexcept  { return value; }

    template<uint I> auto&              at_dispatch(TBool<false>)                { return values.template at<I-1>(); }
    template<uint I> constexpr auto&    at_dispatch(TBool<false>)const noexcept  { return values.template at<I-1>(); }
};

template<class ...T>
constexpr Tuple<T...> makeTuple(const T& ...t) {
    return { t... };
}

template<class Tuple, class Suffix, uint ...I>
constexpr auto tupleAddSuffix(const Tuple& tuple, const Suffix& suffix, TIdx<I...>) {
    return makeTuple(tuple.template at<I>()..., suffix);
}

template<class ...T, class U>
constexpr Tuple<T...,U> operator,(const Tuple<T...>& tuple, U&& value) {
    return tupleAddSuffix(tuple, value, !TIdx<sizeof...(T)>{});
}

/* --- arg      --- */
template<class T, uint N=0>
struct Arg
{
    cstring name;
    T       value;
    static const auto size = N;
};

template<uint N>
struct ArgName
{
    cstring name;
    template<class T> constexpr Arg<T&,       N> operator=(T&       val) const { return {name, val}; };
    template<class T> constexpr Arg<const T&, N> operator=(const T& val) const { return {name, val}; };
};

template<class TA, uint NA, class TB, uint NB>
constexpr auto operator,(const Arg<TA,NA>&a, const Arg<TB,NB>& b) {
    auto tuple = makeTuple(a, b);
    return tuple;
}

}

}
