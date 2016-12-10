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

template<class T>
struct PNode
{
    constexpr PNode(T value): value_(value) {}

    template<class V> void visit(V&& v) const {
        v.val(value_);
    }
  protected:
    T   value_;
};

template<class T> auto proxy(T&       value) -> decltype(value.ptree()) { return value.ptree(); }
template<class T> auto proxy(const T& value) -> decltype(value.ptree()) { return value.ptree(); }

template<class T> PNode<T&>       proxy(T&       value, If<isReal<T>>* = nullptr) { return {value}; }
template<class T> PNode<const T&> proxy(const T& value, If<isReal<T>>* = nullptr) { return {value}; }

inline PNode<bool&>         proxy(bool&         value) { return {value}; }
inline PNode<const bool&>   proxy(const bool&   value) { return {value}; }

inline PNode<string&>       proxy(string&       value) { return {value}; }
inline PNode<const string&> proxy(const string& value) { return {value}; }

template<class T>
struct PArray
{
  public:
    constexpr PArray(T* array, uint size): array_(array), size_(size) {}

    template<class V> void visit(V&& v) const {
        v.beginArray();
        for(size_t i = 0; i < size_; ++i) {
            proxy(array_[i]).visit(std::forward<V>(v));
        }
        v.end();
    }
  protected:
    T*      array_;
    size_t  size_;
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

    constexpr PObject(Arg<T> ...arg): keys_{arg.key...}, vals_{arg.val...} {}

    template<class V> void visit(V&& v) const {
        v.beginObject();
        visit_dispatch<0>(std::forward<V>(v), TBool<true>{});
        v.end();
     }
  protected:
    cstring     keys_[sizeof...(T)];
    Tuple<T...> vals_;

    template<uint I, class V> void visit_dispatch(V&& v, TBool<true>) const {
        v.key(keys_[I]);
        proxy(vals_.template at<I>()).visit(v);
        visit_dispatch<I+1>(std::forward<V>(v), TBool<(I+1<size)>{});
    }

    template<uint I, class V> void visit_dispatch(V&& v, TBool<false>) const {
    }
};

template<class T>    auto& proxy(      PNode<T>&      node) { return node; }
template<class T>    auto& proxy(const PNode<T>&      node) { return node; }
template<class T>    auto& proxy(      PArray<T>&     arr)  { return arr;  }
template<class T>    auto& proxy(const PArray<T>&     arr)  { return arr;  }
template<class ...T> auto& proxy(      PObject<T...>& obj)  { return obj;  }
template<class ...T> auto& proxy(const PObject<T...>& obj)  { return obj;  }

template<class T, class ...U>
PObject<T,U...> proxy(Arg<T> arg, Arg<U> ...args) {
    return {arg, args...};
}

template<class ...T>
auto toPTree(T&& ...t) {
    return proxy(std::forward<T>(t)...);
}

}

using ptree::toPTree;

}
