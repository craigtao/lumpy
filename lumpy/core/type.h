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

#include <lumpy/config.h>

namespace lumpy
{

inline namespace core
{

/* --- built-in types   --- */
using   byte      = unsigned char;
using   sbyte     = signed char;
using   ubyte     = unsigned char;

using   ushort    = unsigned short;
using   uint      = unsigned int;
using   ulong     = unsigned long;

using   llong     = long long;
using   ullong    = unsigned long long;
using   cstring   = const char*;
using   string    = std::string;
using   nullptr_t = std::nullptr_t;
using   index_t   = uint;

template<class ...T> struct null_t{};

template<class T> constexpr T* declptr() noexcept { return static_cast<T*>(nullptr);    }
template<class T> constexpr T  declval() noexcept { return *declptr<T>();               }

/* ---  type convert    --- */
template<class To, class From> constexpr To&        as(From& f)         { return *reinterpret_cast<To*>(&f);      };
template<class To, class From> constexpr const To&  as(const From& f)   { return *reinterpret_cast<const To*>(&f);};

template<class T> constexpr T&        asLVal(const T& v) { return const_cast<T&>(v);            }
template<class T> constexpr const T&  asRVal(const T& v) { return v;                            }

template<class T> constexpr auto      toSInt(const T& t) { return std::make_signed_t<T>(t);     }
template<class T> constexpr auto      toUInt(const T& t) { return std::make_unsigned_t<T>(t);   }

/* --- dollar $         --- */
template<class T>
struct Arg
{
    cstring key;
    T       val;
};

struct TArg
{
    cstring str;
    template<class T> constexpr Arg<T&>       operator=(T&       val) const { return {str, val}; };
    template<class T> constexpr Arg<const T&> operator=(const T& val) const { return {str, val}; };
};

struct Dollar {
    constexpr Dollar() = default;

    template<size_t N> constexpr TArg operator[](const char (&key)[N]) const { return {key}; }
};

static const Dollar $ = {};

template<class T>           constexpr T operator-(Dollar dollar, T offset) { return T(-1) - offset;                            }
template<class T, class S>  constexpr S shrink$(T val, S size)             { return toSInt(val)>=0 ? val : toSInt(val)+size;   }

/* --- functtions --- */
struct Fval  { template<class T> constexpr auto operator()(const T& t) const { return  t; }};
struct Fpos  { template<class T> constexpr auto operator()(const T& t) const { return +t; }};
struct Fneg  { template<class T> constexpr auto operator()(const T& t) const { return -t; }};

struct Fadd  { template<class A, class B> constexpr auto  operator()(const A&a, const B&b) const { return a + b;  } };
struct Fsub  { template<class A, class B> constexpr auto  operator()(const A&a, const B&b) const { return a - b;  } };
struct Fmul  { template<class A, class B> constexpr auto  operator()(const A&a, const B&b) const { return a * b;  } };
struct Fdiv  { template<class A, class B> constexpr auto  operator()(const A&a, const B&b) const { return a / b;  } };
struct Fand  { template<class A, class B> constexpr auto  operator()(const A&a, const B&b) const { return a && b; } };
struct For   { template<class A, class B> constexpr auto  operator()(const A&a, const B&b) const { return a || b; } };

struct Fset2 { template<class A, class B> constexpr auto& operator()(      A&a, const B&b) const { return a  =  b;} };
struct Fadd2 { template<class A, class B> constexpr auto& operator()(      A&a, const B&b) const { return a +=  b;} };
struct Fsub2 { template<class A, class B> constexpr auto& operator()(      A&a, const B&b) const { return a -=  b;} };
struct Fmul2 { template<class A, class B> constexpr auto& operator()(      A&a, const B&b) const { return a *=  b;} };
struct Fdiv2 { template<class A, class B> constexpr auto& operator()(      A&a, const B&b) const { return a /=  b;} };

struct Feq   { template<class A, class B> constexpr bool  operator()(const A&a, const B&b) const { return a == b; } };
struct Fne   { template<class A, class B> constexpr bool  operator()(const A&a, const B&b) const { return a != b; } };
struct Flt   { template<class A, class B> constexpr bool  operator()(const A&a, const B&b) const { return a <  b; } };
struct Fgt   { template<class A, class B> constexpr bool  operator()(const A&a, const B&b) const { return a >  b; } };
struct Fle   { template<class A, class B> constexpr bool  operator()(const A&a, const B&b) const { return a <= b; } };
struct Fge   { template<class A, class B> constexpr bool  operator()(const A&a, const B&b) const { return a >= b; } };

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


/* --- tuple        --- */
template<class ...T> struct Tuple;

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
constexpr Tuple<const T&...> makeTuple(const T& ...t) {
    return { t... };
}

/* --- array        --- */
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


/* --- exception    --- */
struct IStringBuffer;
struct FormatSpec;

class IException
{
  public:
    IException()         {}
    virtual ~IException(){}
    virtual void sformat(IStringBuffer& buffer, const FormatSpec& spec) const {}
};


/* --- no assign    --- */
class INoassignable
{
  protected:
    INoassignable() = default;
    ~INoassignable()= default;
    INoassignable& operator=(INoassignable&&)     = default;
    INoassignable& operator=(const INoassignable&)= delete;
};

/* --- no copy      --- */
class INocopyable: protected INoassignable
{
  protected:
    INocopyable()   = default;
    ~INocopyable()  = default;

    INocopyable(INocopyable&&)      = default;
    INocopyable(const INocopyable&) = delete;
};

/* --- singleton    --- */

/*!
 * class Type: Singleton {
 *   private:
 *     friend class Singleton;
 *     Type(...) { ... }
 *     ~Type()   { ... }
 * }; // ends
 *
 * auto& type = Singleton::instance<Type>(...);
 */
class Singleton
{
public:
    template<class T, class ...Args>
    static T& instance(Args&& ...args) {
        static char buffer[sizeof(T)];
        static std::shared_ptr<T> ptr(new (buffer)T(std::forward<Args>(args)...), &destroy<T>);
        return *(ptr.get());
    }

protected:
    Singleton()  = default;
    ~Singleton() = default;

private:
    Singleton(const Singleton&)  = delete;   // disable copy
    Singleton(Singleton&&)       = delete;   // disable copy
    Singleton& operator=(const Singleton&)  = delete;   // disable assign
    Singleton& operator=(Singleton&&)       = delete;   // disable move

    template<class T>
    static void destroy(T* ptr) { ptr->~T(); }
};

}

}
