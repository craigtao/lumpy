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

#include <vector>
#include <queue>

#include <thread>
#include <future>
#include <mutex>

#include <lumpy/core.h>

namespace lumpy
{
inline namespace thread
{

using Thread    = std::thread;
using Mutex     = std::mutex;
using CondVar   = std::condition_variable;

template<class T=Mutex> using LockGuard     = std::lock_guard<T>;
template<class T=Mutex> using UniqueLock    = std::unique_lock<T>;

inline void sleep(double seconds) {
    std::this_thread::sleep_for(std::chrono::microseconds(llong(seconds*1e6)));
}

}
}
