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

using SystemClock   = std::chrono::system_clock;
using SystemTime    = std::chrono::time_point<SystemClock>;

struct Timer
{
    using Clock     = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;
    using Duration  = std::chrono::duration<double>;

    Timer()         { reset(); }
    ~Timer()        = default;

    void reset()    {
        tstart_ = Clock::now();
    }

    double duration() const {
        TimePoint   tnow    = Clock::now();
        Duration    duration= tnow - tstart_;
        auto        seconds = std::chrono::duration_cast<std::chrono::duration<double>>(duration);
        return seconds.count();
    }

  private:
    TimePoint  tstart_;
};


}

}
