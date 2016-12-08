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
#include <lumpy/core/string.h>

namespace lumpy
{

inline namespace core
{

cstring traceStack();

class ESystem : public IException
{
public:
    using ErrId = int;

    explicit ESystem(ErrId eid) noexcept: eid_(eid) {}

    ErrId   id()    const noexcept { return eid_;               }
    cstring desc()  const noexcept { return errorDesc(eid_);    }

    void sformat(IStringBuffer& buffer) const override {
        buffer.formats("eid = {}, desc = {}", eid_, desc());
    }

protected:
    ErrId   eid_ = 0;

    static cstring errorDesc(ErrId id);
};

class IEOutOfRange: public IException
{
protected:
    virtual ~IEOutOfRange() = default;
};

template<class T>
class EOutOfRange: public IEOutOfRange
{
public:
    using Type = T;
    const Type  value;
    const Type  begin;
    const Type  end;

    EOutOfRange(const Type& value, const Type& begin, const Type& end)
        : value(value), begin(begin), end(end)
    {}

    void sformat(IStringBuffer& buffer) const override {
        sformats(buffer, "value = {}, range = [{} {})", value, begin, end);
    }
};

}

}
