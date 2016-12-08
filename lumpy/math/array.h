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
#include <lumpy/math/slice.h>

namespace lumpy
{

inline namespace math
{

template<class T>
struct SeqArray
{
    using Element   = T;

    explicit constexpr  SeqArray(Element size)                                          : begin_(0),     end_(size), step_(1)   {}
    constexpr           SeqArray(Element first, Element last, Element step=Element(1))  : begin_(first), end_(last), step_(step){}

    constexpr size_t   size()  const noexcept { return size_t((end_ - begin_)/step_); }

    constexpr Element  operator()(index_t i)                  const noexcept { return begin_ + T(i)*step_;    }
    constexpr Element  operator[](index_t i)                  const noexcept { return begin_ + T(i)*step_;    }
    constexpr SeqArray operator()(index_t first, index_t last)  const noexcept {
        return {
            begin_ + Element(shrink$(first, size()))*step_,
            begin_ + Element(shrink$(last,  size()))*step_
        };
    }

    template<uint N> constexpr Slice<SeqArray, N> reshape(const size_t(&shape)[N]) const { return {*this, shape}; }
protected:
    Element begin_;
    Element end_;
    Element step_;
};

template<class T> constexpr SeqArray<T> seq(T size)                        { return{ 0,     size, 1    }; }
template<class T> constexpr SeqArray<T> seq(T begin, T end, T step=T(1))   { return{ begin, end,  step }; }

template<class Type, uint Rank=1>
class Array: public Slice<ArrayView<Type>, Rank>
{
public:
    using View = ArrayView<Type>;
    using base = Slice<View,  Rank>;

    explicit Array(const size_t(&shape)[Rank])    : base{ makeView(shape), shape } {            }
    Array(Array&& src) noexcept                 : base(src) { src->data_ = nullptr;             }
    ~Array()                                    { if (this->data_!=nullptr) mdel(this->data_);  }

public:     /* propertyeis      */
    Array dup () const { return *this; }

    template<class _Src>
    Array& operator=(const _Src& src) {
        runExpr<Fset2>(*this, src);
        return *this;
    }

protected:  /* constructors _   */
    Array(const Array& src): Array(src.shape()) {
        mcpy(base::data(), src.data_.data_, this->size());
    }

    static View makeView(const size_t(&shape)[Rank]) {
        const auto size = prod(shape);
        return{ mnew<Type>(size), size };
    }
};

template<class T>
StringBuffer& sformat(StringBuffer& buffer, const ArrayView<T>& view, FormatSpec spec=FormatSpec{})
{
    auto size = view.size();
    sformat(buffer, "[");
    for (decltype(size) i = 0; i < size; ++i) {
        sformat(buffer, view[i], spec);
        if ( i!= size-1) sformat(buffer, ",");
    }
    sformat(buffer, "]");
    return buffer;
}

}

}
