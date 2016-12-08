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

namespace lumpy
{

inline namespace core
{

/* --- sfinae       --- */
template<bool Value, class T=void>  using If  = typename std::enable_if< Value, T>::type;
template<bool Value, class T=void>  using Not = typename std::enable_if<!Value, T>::type;

/* --- traits       --- */
template<class T> constexpr bool isClass = std::is_class <T>::value;
template<class T> constexpr bool isEnum  = std::is_enum  <T>::value;
template<class T> constexpr bool isUnion = std::is_union <T>::value;
template<class T> constexpr bool isPod   = std::is_pod   <T>::value;

template<class T> constexpr bool isConst = std::is_const <T>::value;

template<class T> constexpr bool isLref  = std::is_lvalue_reference<T>::value;
template<class T> constexpr bool isRref  = std::is_rvalue_reference<T>::value;

template<class T> constexpr bool isInt   = std::is_integral<T>::value;
template<class T> constexpr bool isFloat = std::is_floating_point<T>::value;
template<class T> constexpr bool isReal  = std::is_arithmetic<T>::value;

template<class T> constexpr bool isSInt  = std::is_signed<T>::value;
template<class T> constexpr bool isUInt  = std::is_unsigned<T>::value;

template<class T,    class U>        constexpr auto isSame = std::is_same<T, U>::value;
template<class Base, class Derived > constexpr auto isBase = std::is_base_of<Base, Derived >::value;

template<class T> constexpr T&  asLRef(T&  val) { return val;                   }
template<class T> constexpr T&  asLRef(T&& val) { return static_cast<T&>(val);  }

template<class T> constexpr T&& asRRef(T&  val) { return static_cast<T&&>(val); }
template<class T> constexpr T&& asRRef(T&& val) { return val;                   }

template<class T>   using TRemoveRef    = typename std::remove_reference<T>::type;
template<class T>   using TRemoveConst  = typename std::remove_const<T>::type;

template<class T, uint=0> constexpr T&& take(typename std::remove_reference<T>::type&  value) noexcept { return (static_cast<T&&>(value)); }
template<class T, uint=0> constexpr T&& take(typename std::remove_reference<T>::type&& value) noexcept { return (static_cast<T&&>(value)); }

}
}
