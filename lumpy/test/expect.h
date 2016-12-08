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

namespace lumpy
{

namespace test
{

template<class T>
struct UnExpect: public IException
{
public:
    UnExpect(const T& a, const T& b, const char* op)
        : a_(a), b_(b), op_(op)
    {}

    void sformat(IStringBuffer& buffer) const override {
        buffer.formats("excepct '{} {} {}' failed", a_, op_, b_);
    }
protected:
    T           a_;
    T           b_;
    const char* op_  = "?";
};

template<class T>
struct _ExpectHelper
{
    const T&    a;

    void operator==(const T& b) { ensure(a == b, a, b, "=="); }
    void operator!=(const T& b) { ensure(a != b, a, b, "!="); }
    void operator<=(const T& b) { ensure(a <= b, a, b, "<="); }
    void operator>=(const T& b) { ensure(a >= b, a, b, ">="); }
    void operator< (const T& b) { ensure(a <= b, a, b, "<" ); }
    void operator> (const T& b) { ensure(a >= b, a, b, ">" ); }

private:
    static void ensure(bool condition, const T& a, const T& b, const char* op) {
        if (condition) return;
        throw UnExpect<T>(a, b, op);
    }
};

template<class T> _ExpectHelper<T> expect(const T& value) {
    return{ value };
}
}

}
