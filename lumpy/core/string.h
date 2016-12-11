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

#include <lumpy/core/array.h>
#include <lumpy/core/memory.h>

namespace lumpy
{

inline namespace core
{

// id:[[fill]align][sign][width].[prec][type]
struct FormatSpec
{
    ubyte   id;
    char    fill;
    char    align;
    char    sign;
    ubyte   width;
    ubyte   prec;
    char    type;
};


struct IStringBuffer: IBuffer<char>
{
  public:
    using base = IBuffer<char>;

    IStringBuffer(char* buffer, size_t size): base(buffer, size) {}
    ~IStringBuffer() = default;

    void end() {
        base::data_[base::size_] = '\0';
    }

    template<class ...Ts>
    void formats(const string& fmt, const Ts&... args);

    template<class T>
    void format(const T& value, const FormatSpec& spec={});
};

class StringBuffer: public IStringBuffer, INocopyable
{
  public:
    using base = IStringBuffer;

    StringBuffer(size_t capicity=4*1024):  base(mnew<char>(capicity), capicity) {}
    ~StringBuffer() { delete base::data_;}

    StringBuffer(StringBuffer&& rhs) noexcept: base(rhs) {
        rhs.data_       = nullptr;
        rhs.size_       = 0;
        rhs.capicity_   = 0;
    }

    StringBuffer& operator=(StringBuffer&& rhs) noexcept {
        base::operator  = (rhs);
        rhs.data_       = 0;
        rhs.size_       = 0;
        rhs.capicity_   = 0;
        return *this;
    }
};


}

}
