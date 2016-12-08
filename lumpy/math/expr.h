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
#include <lumpy/math/common.h>

namespace lumpy
{

inline namespace math
{

template<class F, class TA>
struct Expr<F, TA>
{
    constexpr Expr(const TA& a): va(a) {}

    template<class ...I> constexpr auto operator()(const I& ...idx) const { return F{}(va(idx...));             }
    template<class    I> constexpr auto operator[](const I&    idx) const { return makeExpr<F>(va[idx]);        }
    template<class ...S> constexpr auto slice     (const S& ... ss) const { return makeExpr(va.slice(ss...));   }

  protected:
    TA  va;
};


template<class F, class TA, class TB>
struct Expr<F, TA, TB>
{
    constexpr Expr(const TA& a, const TB& b): va(a), vb(b) {}
    template<class ...I> constexpr auto operator()(const I&  ...idx) const { return F{}(va(idx...), vb(idx...));                    }
    template<class    I> constexpr auto operator[](const I&     idx) const { return makeExpr<F>(va[idx], vb[idx]);                  }
    template<class ...S> constexpr auto slice     (const S&  ... ss) const { return makeExpr<F>(va.slice(ss...), vb.slice(ss...));  }

protected:
    TA  va;
    TB  vb;
};


/* --- expr test --- */
template<class ...T>    struct _CanExprHelper;
template<class ...T>    constexpr bool canExpr = _CanExprHelper<T...>::value;

template<class A> struct _CanExprHelper<A>
{
  private:
    template<class F, class ...T> static char test(Expr<F,T...>*);
    template<class T, uint     N> static char test(Slice<T,N>*);
    template<class T>             static char test(Scalar<T>*);
    static                               int  test(...);
  public:
    static constexpr bool value = sizeof(test(declptr<A>())) == sizeof(char);
};

template<class A, class B> struct _CanExprHelper<A, B>
{
    static constexpr bool value = _CanExprHelper<A>::value || _CanExprHelper<B>::value;
};

/* --- functions --- */
template<class TA,           class = If<canExpr<TA>>>     constexpr auto operator+(const TA& a)              { return makeExpr<Fpos >(a);   }
template<class TA,           class = If<canExpr<TA>>>     constexpr auto operator-(const TA& a)              { return makeExpr<Fneg >(a);   }
template<class TA,           class = If<canExpr<TA>>>     constexpr auto sin      (const TA& a)              { return makeExpr<Fsin >(a);   }
template<class TA,           class = If<canExpr<TA>>>     constexpr auto cos      (const TA& a)              { return makeExpr<Fcos >(a);   }
template<class TA,           class = If<canExpr<TA>>>     constexpr auto tan      (const TA& a)              { return makeExpr<Ftan >(a);   }
template<class TA,           class = If<canExpr<TA>>>     constexpr auto sqrt     (const TA& a)              { return makeExpr<Fsqrt>(a);   }
template<class TA,           class = If<canExpr<TA>>>     constexpr auto exp      (const TA& a)              { return makeExpr<Fexp >(a);   }

template<class TA, class TB, class = If<canExpr<TA, TB>>> constexpr auto operator+(const TA& a, const TB& b) { return makeExpr<Fadd >(a,b); }
template<class TA, class TB, class = If<canExpr<TA, TB>>> constexpr auto operator-(const TA& a, const TB& b) { return makeExpr<Fadd >(a,b); }
template<class TA, class TB, class = If<canExpr<TA, TB>>> constexpr auto operator*(const TA& a, const TB& b) { return makeExpr<Fmul >(a,b); }
template<class TA, class TB, class = If<canExpr<TA, TB>>> constexpr auto operator/(const TA& a, const TB& b) { return makeExpr<Fdiv >(a,b); }
template<class TA, class TB, class = If<canExpr<TA, TB>>> constexpr auto pow      (const TA& a, const TB& b) { return makeExpr<Fpow >(a,b); }


}

}
