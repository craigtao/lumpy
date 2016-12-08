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
#include <lumpy/core.h>

namespace lumpy
{

inline namespace math
{

using size_t    = index_t;

/* --- stride --- */
template<class Size, uint Rank, uint I> constexpr auto _toStride(const Size(&shape)[Rank], TIdx<I>, TBool<false>) { return Size(1);}
template<class Size, uint Rank, uint I> constexpr auto _toStride(const Size(&shape)[Rank], TIdx<I>, TBool<true> ) { return shape[I]*_toStride(shape, TIdx<I-1>{}, TBool<(I>1)>{}); }
template<class Size, uint Rank, uint I> constexpr auto  toStride(const Size(&shape)[Rank], TIdx<I>              ) { return _toStride(shape, TIdx<I>  {}, TBool<(I>0)>{}); }

/* --- function --- */
struct Fsin { template<class T> constexpr auto operator()(const T& t) const { return std::sin(t); }};
struct Fcos { template<class T> constexpr auto operator()(const T& t) const { return std::cos(t); }};
struct Ftan { template<class T> constexpr auto operator()(const T& t) const { return std::tan(t); }};

struct Fsqrt{ template<class T> constexpr auto operator()(const T& t) const { return std::sqrt(t);}};
struct Fexp { template<class T> constexpr auto operator()(const T& t) const { return std::exp(t); }};

struct Fpow { template<class A, class B> constexpr auto operator()(const A& a, const B& b) const { return std::pow(a, b); }};


/* --- scalar --- */
template<class T> struct Scalar
{
    using Type = T;

    Scalar(const Type& value): value_(value) {}

    constexpr operator const Type&() const { return value_; }

    template<class ...I> constexpr const T& operator() (const I& ...indexs  ) const { return value_; }
    template<class    I> constexpr Scalar   operator[] (const I&     index  ) const { return *this;  }
    template<class ...I> constexpr Scalar   slice      (const I& ...sections) const { return *this;  }

  protected:
    Type value_;
};

template<class T> constexpr Scalar<T> makeScalar(const T& value) {
    return{ value };
}


/* --- types --- */
template<class Type, uint  Rank>    struct Slice;
template<class Type, uint  Rank>    class  Array;
template<class Type>                struct SeqArray;
template<class Func, class ...T>    struct Expr;


/* --- is expr --- */
TBool<false> _isLExpr(...);

template<class T, uint N>
TBool<true>  _isLExpr(Slice<T,N>*);

template <class T> constexpr bool isLExpr = isSame<TBool<true>, decltype(_isLExpr(declptr<T>()))>;

/* --- to expr --- */
inline namespace detail {
template<class F, class ...T>   constexpr auto _toExpr1(const Expr<F, T...>& val) { return val;      }
template<class T, uint     N>   constexpr auto _toExpr1(const Slice<T,N>&    val) { return val;      }
template<class T>               constexpr auto _toExpr1(const Scalar<T>&     val) { return val;      }
inline                          constexpr auto _toExpr1(...                     ) { return nullptr;  }

template<class T, class E>      constexpr auto _toExpr2(const T& val, const E& expr)  { return expr;            }
template<class T>               constexpr auto _toExpr2(const T& val, nullptr_t    )  { return makeScalar(val); }

template<class F, class ...T>   constexpr auto _makeExpr(const T& ...t) { return Expr<F,T...>{ t...};           }
}

template<class T>               constexpr auto  toExpr(const T& val )   { return _toExpr2(val, _toExpr1(val));  }
template<class F, class ...T>   constexpr auto  makeExpr(const T& ...t) { return _makeExpr<F>(toExpr(t)...);    }
}

}
