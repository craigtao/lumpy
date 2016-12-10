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

#ifdef _LUMPY_OS_WINDOWS_
#include <Windows.h>
#endif

#ifdef _LUMPY_OS_UNIX_
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#endif

#include <sys/timeb.h>
#include <sys/types.h>


/* === libc === */
#include <cstdlib>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <string>
#include <cmath>

/* === stdc++ === */
#include <string>
#include <type_traits>
#include <algorithm>
#include <functional>

#include <vector>
#include <map>
