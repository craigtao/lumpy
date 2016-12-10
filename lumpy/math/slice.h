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
#include <lumpy/math/run.h>

namespace lumpy
{
inline namespace math
{

template<class Array, uint...Rank> decltype(auto) slice  (Array&& arr, const size_t(&...sections)[Rank])  { return std::forward<Array>(arr).slice(sections...); }
template<class Array, uint   Rank> decltype(auto) permute(Array&& arr, const size_t(&order)[Rank])        { return std::forward<Array>(arr).permute(order);     }
template<class Array, uint   Rank> decltype(auto) reshape(Array&& arr, const size_t(&shape)[Rank])        { return std::forward<Array>(arr).reshape(shape);     }

template<class View, uint Rank>  Slice<View, Rank> makeSlice(const View& view, const size_t(&shape)[Rank], const size_t(&stride)[Rank]) { return { view, shape, stride }; }
template<class View, uint Rank>  Slice<View, Rank> makeSlice(const View& view, const size_t(&shape)[Rank])                              { return { view, shape         }; }

template<class View, uint Rank> struct Slice: protected View
{
    using base    = View;
    using Element = typename base::Element;

    constexpr Slice(const base& view, const size_t(&shape)[Rank], const size_t(&stride)[Rank]): Slice(view, shape, stride, !TIdx<Rank>{}) {}
    constexpr Slice(const base& view, const size_t(&shape)[Rank])                             : Slice(view, shape,         !TIdx<Rank>{}) {}

    constexpr static size_t rank() { return Rank; }

    constexpr size_t shape (uint i) const { return i<rank() ? shape_[i] : 1;  }
    constexpr size_t stride(uint i) const { return i<rank() ? stride_[i]: 0;  }
    constexpr size_t numel()        const { return prod(shape_);              }

    template<class...I> constexpr auto  operator()(const I& ...idx) const   noexcept  { return base::operator[](offsetOf({idx...})); }
    template<class...I> decltype(auto)  operator()(const I& ...idx)         noexcept  { return base::operator[](offsetOf({idx...})); }

    constexpr decltype(auto)            operator[](size_t idx)        const   noexcept  { return _at(idx, TBool<Rank==1>{} ); }
    decltype(auto)                      operator[](size_t idx)                noexcept  { return _at(idx, TBool<Rank==1>{} ); }

    template<uint ...N> constexpr auto  slice(const size_t(&...sections)[N])  const  noexcept { return _slice({Section(sections)...}, TIdx<N...>{}, !TIdx<Rank>{}); }
    template<uint ...N> auto            slice(const size_t(&...sections)[N])         noexcept { return _slice({Section(sections)...}, TIdx<N...>{}, !TIdx<Rank>{}); }

    template<class   T> auto& operator =(const T& value) { runExpr<Fset2>(*this, value);  return *this; }
protected:
    size_t shape_[Rank];
    size_t stride_[Rank];

    constexpr index_t offsetOf(const index_t(&indexs)[Rank]) const {
        return sum(toImmutable(indexs)*toImmutable(stride_));
    }

  private:
    template<uint ...I> Slice(const base& view, const size_t(&shape)[Rank], const size_t(&stride)[Rank], TIdx<I...>): base(view), shape_{ shape[I]...}, stride_{ stride[I]...}                 {}
    template<uint ...I> Slice(const base& view, const size_t(&shape)[Rank],                            TIdx<I...>): base(view), shape_{ shape[I]...}, stride_{ toStride(shape, TIdx<I>{})...}{}

    template<uint ...N, uint ...I> constexpr auto _slice(const Section(&sections)[Rank], TIdx<I...>)             const noexcept { return makeSlice (base::operator()(offsetOf({ (sections[I].begin())... }), offsetOf({ (sections[I].end())... })), { sections[I].length()... }, { stride_[I]... }); }
    template<uint ...N, uint ...I>           auto _slice(const Section(&sections)[Rank], TIdx<I...>)                   noexcept { return makeSlice (base::operator()(offsetOf({ (sections[I].begin())... }), offsetOf({ (sections[I].end())... })), { sections[I].length()... }, { stride_[I]... }); }

    template<uint ...N, uint ...I> constexpr auto _slice(const Section(&sections)[Rank], TIdx<N...>, TIdx<I...>) const noexcept { return _slice({ (sections[I]%shape_[I])...}, ~TBool<(N>1)...>{}); }
    template<uint ...N, uint ...I> auto           _slice(const Section(&sections)[Rank], TIdx<N...>, TIdx<I...>)       noexcept { return _slice({ (sections[I]%shape_[I])...}, ~TBool<(N>1)...>{}); }

    constexpr auto  _at(size_t index, TBool<false>)  const noexcept { return _at(index, !TIdx<Rank-1>{});       }
    auto            _at(size_t index, TBool<false>)        noexcept { return _at(index, !TIdx<Rank-1>{});       }
    constexpr auto& _at(size_t index, TBool<true>)   const noexcept { return base::operator[](index*stride_[0]);}
    auto&           _at(size_t index, TBool<true>)         noexcept { return base::operator[](index*stride_[0]);}

private:
    template<uint ...I> constexpr auto _at(size_t index, TIdx<I...>) const noexcept { return makeSlice(base::operator() ((index+0)*stride_[Rank-1], (index+1)*stride_[Rank-1]), { shape_[I]...}, { stride_[I]...}); }
    template<uint ...I>           auto _at(size_t index, TIdx<I...>)       noexcept { return makeSlice(base::operator() ((index+0)*stride_[Rank-1], (index+1)*stride_[Rank-1]), { shape_[I]...}, { stride_[I]...}); }
};


template<class Data, uint Rank>
void sformat(IStringBuffer& buffer, const Slice<Data, Rank>& obj, FormatSpec spec=FormatSpec{})
{
    const auto size = obj.shape(Rank - 1);
    if (Rank==1) buffer.push("[");
    for (size_t i = 0; i < size; ++i) {
        if (Rank != 1) buffer.push("\n\t");
        sformat(buffer, obj[i], spec);
        if (Rank==1 && i != size-1) buffer.push(",");
    }
    if (Rank==1) buffer.push("]");
}

}

}
