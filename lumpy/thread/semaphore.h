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

#include <lumpy/thread/common.h>

namespace lumpy
{

inline namespace thread
{

#if   defined(_LUMPY_OS_UNIX_)
using sem_t = int;
#elif defined(_LUMPY_OS_WINDOWS_)
using sem_t = void;
#endif

class Semaphore final: INocopyable
{
  public:
    explicit Semaphore(uint value=0);
    ~Semaphore();
    Semaphore(Semaphore&&) = delete;

    Semaphore& operator++();
    Semaphore& operator--();
    bool       operator--(int);

  private:
    sem_t*  sem_;
};

}

}
