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

namespace lumpy
{

inline namespace core
{

/* --- type immutable   --- */
template<class T, T   ...V> struct TImmutable;
template<class T, uint   N> struct Immutable;

template<class T, T  ...V> struct TImmutable
{
    using Element   = T;
    static constexpr auto Size = uint(sizeof...(V));

    static constexpr uint               size()  noexcept { return Size;     }
    static constexpr Immutable<T,Size>  value() noexcept { return {V...};   }

    constexpr auto operator()() const noexcept { return value(); }

};
template<class T, T ...A, T ...B> TImmutable<T, A..., B...> constexpr operator+(TImmutable<T,A...>, TImmutable<T,B...>) { return {}; }

template<uint ...I> using TIdx  = TImmutable<uint, I...>;
template<bool ...I> using TBool = TImmutable<bool, I...>;

template<uint N, uint ...I> struct _TISeq          : _TISeq<N - 1, I..., uint(sizeof...(I))> { };
template<        uint ...I> struct _TISeq<0, I...> { using Type = TIdx<I...>; };
template<uint N> constexpr typename _TISeq<N>::Type operator!(TIdx<N>) { return {}; }

template<uint I, bool ...V> struct _TIdxIf;
template<uint I>            struct _TIdxIf<I>             { using Type = TIdx<>;};
template<uint I, bool ...V> struct _TIdxIf<I, true, V...> { using Type = decltype(TIdx<I>{} + typename _TIdxIf<I + 1, V...>::Type{}); };
template<uint I, bool ...V> struct _TIdxIf<I, false,V...> { using Type = typename _TIdxIf<I+1, V...>::Type;};
template<bool ...V> constexpr typename _TIdxIf<0, V...>::Type operator~(TBool<V...>) { return {}; }

/* --- immutable    --- */
template <class T, uint N> struct Immutable
{
    using Element   = T;

    template<T ...I>      constexpr Immutable(const T(&arr)[N], TIdx<I...>): elements{ arr[I]...}{}
    template<class ..._T> constexpr Immutable(T val0, _T ...val): elements { val0, val...} {}

    constexpr Immutable(const T(&arr)[N]) : Immutable(arr, !TIdx<N>{}) {}

    constexpr static auto   size()                      { return N;             }
    constexpr auto          operator[](uint i) const    { return elements[i];   }

    const T elements[N];
};

template<class T, uint N>            Immutable<T,N> toImmutable(const T(&a)[N])             { return Immutable<T,N>(a); }
template<class T, uint N, uint ...I> Immutable<T,N> toImmutable(const T(&a)[N], TIdx<I...>) { return Immutable<T,N>(a); }

/* --- map/reduce --- */
template<class F, uint ...I, class A>           constexpr auto _map(const F& f, TIdx<I...>, const A& a)             { return Immutable<decltype(f(a[0])),      sizeof...(I)>({ f(a[I]      )... });       }
template<class F, uint ...I, class A, class B>  constexpr auto _map(const F& f, TIdx<I...>, const A& a, const B& b) { return Immutable<decltype(f(a[0],b[0])), sizeof...(I)>({ f(a[I], b[I])... });       }
template<class F, class T, uint N, class ...U>  constexpr auto  map(const F& f, const T(&v)[N], const U(&...u)[N])  { return _map(f, !TIdx<N>{}, v, u...); }

template<class F, uint ...I, class    V>        constexpr auto _index(const F& f, TIdx<I...>, const V& v)           { return !TBool<f(v[I]...)>{}();                }
template<class F, class T, uint N>              constexpr auto  index(const F& f, const T(&v)[N])                   { return _index(f, !TIdx<N>{}, v);              }

template<class F, uint ...I, class    V>        constexpr auto _select(const F& f, TIdx<I...>, const V& v)          { return _map(Fval{}, !TBool<f(v[I]...)>{}, v); }
template<class F, class T, uint N>              constexpr auto  select(const F& f, const T(&v)[N])                  { return _select(f, !TIdx<N>{}, v);             }

template<class F,            class    V>        constexpr auto _reduce(const F& f, TIdx<0>, const V& v)             { return v[0];                                  }
template<class F, uint    I, class    V>        constexpr auto _reduce(const F& f, TIdx<I>, const V& v)             { return f(_reduce(f, TIdx<I-1>{}, v), v[I]);   }
template<class F, class T, uint N>              constexpr auto  reduce(const F& f, const T(&v)[N])                  { return _reduce(f, TIdx<N-1>{}, v);            }

/* ---  vec operators   --- */
template<class T, uint N> constexpr auto  operator+(const Immutable<T,N>& a, const Immutable<T,N> &b) { return map(Fadd{}, a.elements, b.elements); }
template<class T, uint N> constexpr auto  operator-(const Immutable<T,N>& a, const Immutable<T,N> &b) { return map(Fsub{}, a.elements, b.elements); }
template<class T, uint N> constexpr auto  operator*(const Immutable<T,N>& a, const Immutable<T,N> &b) { return map(Fmul{}, a.elements, b.elements); }
template<class T, uint N> constexpr auto  operator/(const Immutable<T,N>& a, const Immutable<T,N> &b) { return map(Fdiv{}, a.elements, b.elements); }

template<class T, uint N> constexpr bool operator==(const Immutable<T,N>& a, const Immutable<T,N> &b) { return reduce(Fand{}, map(Feq{}, a.elements, b.elements)); }
template<class T, uint N> constexpr bool operator!=(const Immutable<T,N>& a, const Immutable<T,N> &b) { return reduce(Fand{}, map(Fne{}, a.elements, b.elements)); }
template<class T, uint N> constexpr bool operator< (const Immutable<T,N>& a, const Immutable<T,N> &b) { return reduce(Fand{}, map(Flt{}, a.elements, b.elements)); }
template<class T, uint N> constexpr bool operator> (const Immutable<T,N>& a, const Immutable<T,N> &b) { return reduce(Fand{}, map(Fgt{}, a.elements, b.elements)); }
template<class T, uint N> constexpr bool operator<=(const Immutable<T,N>& a, const Immutable<T,N> &b) { return reduce(Fand{}, map(Fle{}, a.elements, b.elements)); }
template<class T, uint N> constexpr bool operator>=(const Immutable<T,N>& a, const Immutable<T,N> &b) { return reduce(Fand{}, map(Fge{}, a.elements, b.elements)); }

template<class T, uint N> constexpr auto sum (const T(&v)[N]) { return reduce(Fadd{}, v); }
template<class T, uint N> constexpr auto prod(const T(&v)[N]) { return reduce(Fmul{}, v); }
template<class T, uint N> constexpr auto all (const T(&v)[N]) { return reduce(Fand{}, v); }
template<class T, uint N> constexpr auto any (const T(&v)[N]) { return reduce(For {}, v); }

template<class T, uint N> constexpr auto sum (const Immutable<T,N>& v) { return reduce(Fadd{}, v.elements); }
template<class T, uint N> constexpr auto prod(const Immutable<T,N>& v) { return reduce(Fmul{}, v.elements); }
template<class T, uint N> constexpr auto all (const Immutable<T,N>& v) { return reduce(Fand{}, v.elements); }
template<class T, uint N> constexpr auto any (const Immutable<T,N>& v) { return reduce(For {}, v.elements); }

}

}
