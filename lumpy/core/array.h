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

template<class T, class S>
constexpr S shrink$(T val, S size) {
    return toSInt(val)>=0 ? val : toSInt(val)+size;
}

struct Section
{
    using index_t = uint;

    constexpr explicit Section(const index_t(&arr)[1]) : begin_(arr[0]), end_(arr[0]+1),step_(1)     {}
    constexpr explicit Section(const index_t(&arr)[2]) : begin_(arr[0]), end_(arr[1]),  step_(1)     {}
    constexpr explicit Section(const index_t(&arr)[3]) : begin_(arr[0]), end_(arr[2]),   step_(arr[1]){}

    constexpr Section  operator%(index_t& size) const noexcept { return { shrink$(begin_, size), shrink$(end_, size), step_}; }

    constexpr auto  begin()  const noexcept { return begin_;                 }
    constexpr auto  end()    const noexcept { return end_;                   }
    constexpr auto  length() const noexcept { return (end_ - begin_) / step_;}

  protected:
    index_t   begin_;
    index_t   end_;
    index_t   step_;

    constexpr Section(index_t begin, index_t end, index_t step): begin_(begin), end_(end), step_(step) {}
};

template<class T>
struct ArrayIterator : std::iterator< std::random_access_iterator_tag, T>
{
    constexpr explicit ArrayIterator(T* ptr): ptr_(ptr) {}

    constexpr auto operator->() const   noexcept { return  ptr_; }
    auto           operator->()         noexcept { return  ptr_; }

    constexpr auto&operator* () const   noexcept { return *ptr_; }
    auto&          operator* ()         noexcept { return *ptr_; }

    constexpr bool operator==(const ArrayIterator& other) const noexcept { return ptr_ == other.ptr_; }
    constexpr bool operator!=(const ArrayIterator& other) const noexcept { return !(*this == other); }

    auto& operator++()      { ++ptr_;                   return *this;   }
    auto  operator++(int)   { auto tmp = *this; ++ptr_; return tmp;     }

  protected:
    T* ptr_;
};

template<class T> struct       ArrayView;
template<class T> constexpr    ArrayView<T> toArrayView(T* ptr, index_t size) { return {ptr, size}; }

template<class T> struct  ArrayView
{
    using Element       = T;

    constexpr ArrayView()   = default;
    ~ArrayView()            = default;
    constexpr ArrayView(Element* data, size_t size) : data_(data), size_(size) { }

    constexpr size_t        size()  const noexcept { return size_;              }
    constexpr bool          empty() const noexcept { return size() == 0;        }
    constexpr auto          data()  const noexcept { return asRVal(data_);      }
    auto                    data()        noexcept { return data_;              }

    template<class I> constexpr const auto& operator[](I index)        const noexcept { return data_[index]; }
    template<class I> auto&                 operator[](I index)              noexcept { return data_[index]; }
    template<class I> constexpr auto        operator()(I begin, I end) const noexcept { return toArrayView(asRVal(data_) + shrink$(begin, size_), shrink$(end, size_)-shrink$(begin, size_)); }
    template<class I> auto                  operator()(I begin, I end)       noexcept { return toArrayView(data_         + shrink$(begin, size_), shrink$(end, size_)-shrink$(begin, size_)); }
  protected:
    Element*    data_   = nullptr;
    size_t      size_   = 0;
};

}

}
