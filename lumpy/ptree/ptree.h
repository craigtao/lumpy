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
#include <lumpy/core/traits.h>

namespace lumpy
{

namespace ptree
{

template<class T>    struct PNode;
template<class T>    struct PArray;
template<class ...T> struct PObject;;

template<class T>
struct PNode
{
    constexpr PNode(T value): value_(value) {}

    template<class V> void serialize(V&& v) const {
        v << value_;
    }

    template<class V> void deserialize(V&& v) const {
        v >> value_;
    }
  protected:
    T   value_;
};


template<class T> constexpr PNode<T&> 			 proxy(T&            value, If<isReal<T>>* = nullptr) { return {value}; }
template<class T> constexpr PNode<T>  			 proxy(const T&      value, If<isReal<T>>* = nullptr) { return {value}; }

inline            constexpr PNode<bool&>         proxy(bool&         value) { return {value}; }
inline            constexpr PNode<bool>          proxy(const bool&   value) { return {value}; }
inline            constexpr PNode<string&>       proxy(string&       value) { return {value}; }
inline            constexpr PNode<const string&> proxy(const string& value) { return {value}; }

template<class ...T> PObject<T...> proxyTuple(const Tuple<T...>& tuple);

template<class T> auto proxy(T&       value) -> decltype(proxyTuple(value.ptree())) { return proxyTuple(value.ptree()); }
template<class T> auto proxy(const T& value) -> decltype(proxyTuple(value.ptree())) { return proxyTuple(value.ptree()); }


template<class T>
struct PArray
{
  public:
    class EBadSize{};

    constexpr PArray(T* array, uint size): array_(array), size_(size) {}

    template<class V> void serialize(V&& v) const {
        auto itr = v.array();
        for(uint i = 0; i < size_; ++i) {
            proxy(array_[i]).serialize(*++itr);
        }
    }

    template<class V> void deserialize(const V& v) const {
        const auto itr = v.array();
        if (size_ != v.size()) throw EBadSize{};
        for(size_t i = 0; i < size_; ++i) {
            proxy(array_[i]).deserialize(*++itr);
        }
    }

  protected:
    T*          array_;
    const uint  size_;
};

template<class T>  PArray<      T> proxy(      T  array[], size_t size) { return { array, size}; }
template<class T>  PArray<const T> proxy(const T  array[], size_t size) { return { array, size}; }

template<class T, size_t size> PArray<      T> proxy(      T (&array)[size]) { return { array, size}; }
template<class T, size_t size> PArray<const T> proxy(const T (&array)[size]) { return { array, size}; }

template<class ...T>
struct PObject
{
  public:
    constexpr static const auto size = sizeof...(T);

    constexpr PObject(const Tuple<T...>& tuple): tuple_(tuple) {}

    template<class V> void serialize(V&& v) const {
        auto itr = v.object();
        serialize_dispatch<0>(itr, TBool<true>{});
    }

    template<class V> void deserialize(const V& v) const {
        const auto itr = v.object();
        deserialize_dispatch<0>(itr, TBool<true>{});
    }
  protected:
    Tuple<T...> tuple_;

    template<uint I, class V> void serialize_dispatch(V& itr, TBool<true>) const {
        auto item = tuple_.template at<I>();
        proxy(item.value).serialize(*itr[item.name]);
        serialize_dispatch<I+1>(itr, TBool<(I+1<size)>{});
    }

    template<uint I, class V> void deserialize_dispatch(const V& itr, TBool<true>) const {
        auto item = tuple_.template at<I>();
        auto u = itr[item.name];
        if (u) {
            proxy(item.value).deserialize(*itr);
            deserialize_dispatch<I+1>(itr, TBool<(I+1<size)>{});
        }
    }

    template<uint I, class V> void serialize_dispatch  (V&& v, TBool<false>) const {}
    template<uint I, class V> void deserialize_dispatch(V&& v, TBool<false>) const {}
};

template<class T>    auto& proxy(      PNode<T>&      node) { return node; }
template<class T>    auto& proxy(const PNode<T>&      node) { return node; }
template<class T>    auto& proxy(      PArray<T>&     arr)  { return arr;  }
template<class T>    auto& proxy(const PArray<T>&     arr)  { return arr;  }
template<class ...T> auto& proxy(      PObject<T...>& obj)  { return obj;  }
template<class ...T> auto& proxy(const PObject<T...>& obj)  { return obj;  }

template<class ...T>
PObject<T...> proxyTuple(const Tuple<T...>& tuple) {
    return {tuple};
}

}

}
