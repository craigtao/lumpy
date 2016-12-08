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
#include <lumpy/math/common.h>

namespace lumpy
{

inline namespace math
{

/* ---- assign --- */
template<class Func, class Dst, class Src>
void seqRun(Dst& dst, const Src& src, TBool<true>) {
    auto size = dst.shape(dst.rank()-1);
    for (decltype(size) i = 0; i < size; ++i) {
        auto vd = dst[i];
        auto vs = src[i];
        seqRun<Func>(vd, vs, TBool<(Dst::rank()>2)>{});
    }
}

template<class Func, class Dst, class Src>
void seqRun(Dst& dst, const Src& src, TBool<false>) {
    static Func func;
    auto size = dst.shape(0);
    for (decltype(size) i = 0; i < size; ++i) {
        auto& vd = dst(i);
        auto  vs = src(i);
        func(vd, vs);
    }
}

template<class Func, class Dst, class Src>
void runExpr(Dst& dst, const Src& src) {
    auto vd = toExpr(dst);
    auto vs = toExpr(src);
    seqRun<Func>(vd, vs, TBool<(Dst::rank()>1)>{});
}

template<class Dst, class Src> If<isLExpr<Dst>, Dst&> operator+=(Dst& dst, const Src& src) { runExpr<Fadd2>(dst, src);  return dst; }
template<class Dst, class Src> If<isLExpr<Dst>, Dst&> operator-=(Dst& dst, const Src& src) { runExpr<Fsub2>(dst, src);  return dst; }
template<class Dst, class Src> If<isLExpr<Dst>, Dst&> operator*=(Dst& dst, const Src& src) { runExpr<Fmul2>(dst, src);  return dst; }
template<class Dst, class Src> If<isLExpr<Dst>, Dst&> operator/=(Dst& dst, const Src& src) { runExpr<Fdiv2>(dst, src);  return dst; }

}  // math


}  // lumpy
