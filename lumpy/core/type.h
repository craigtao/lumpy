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
