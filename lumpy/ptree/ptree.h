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

namespace lumpy
{

namespace ptree
{

template<class T, class Enabled=void>
class Value
{
public:
    explicit Value(T& value) : value_(&value) {}

private:
    T* value_;
};

template<class T> Value<T> makeSerializer(T& value) {
    return {value};
}

template<class T>
class Array
{
public:
    explicit Array(T* value, uint size) : value_(value), size_(size) {}
    auto operator[](uint idx) { return makeSerializer(value_[idx]); }

private:
    T*      value_;
    uint    size_;
};

}
}
