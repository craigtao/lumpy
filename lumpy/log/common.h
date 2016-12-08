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
#include <lumpy/core.h>

namespace lumpy
{

namespace log
{

enum Level
{
    none        = 0,
    trace       = 1,    // for unittest
    debug       = 2,
    info        = 3,
    warn        = 4,
    alert       = 5,
    error       = 6,
    fatal       = 7,
};

class Logger;
class IBackend;

}

}


#define log_trace  ::lumpy::log::LogItem({__FILE__, __FUNCTION__, __LINE__}, ::lumpy::log::trace)
#define log_debug  ::lumpy::log::LogItem({__FILE__, __FUNCTION__, __LINE__}, ::lumpy::log::debug)
#define log_info   ::lumpy::log::LogItem({__FILE__, __FUNCTION__, __LINE__}, ::lumpy::log::info )
#define log_warn   ::lumpy::log::LogItem({__FILE__, __FUNCTION__, __LINE__}, ::lumpy::log::warn )
#define log_alert  ::lumpy::log::LogItem({__FILE__, __FUNCTION__, __LINE__}, ::lumpy::log::alert)
#define log_error  ::lumpy::log::LogItem({__FILE__, __FUNCTION__, __LINE__}, ::lumpy::log::error)
#define log_fatal  ::lumpy::log::LogItem({__FILE__, __FUNCTION__, __LINE__}, ::lumpy::log::fatal)
