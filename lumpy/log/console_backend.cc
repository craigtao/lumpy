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

#include <lumpy/log/logger.h>
#include <lumpy/thread.h>

namespace lumpy
{

namespace log
{

cstring ConsoleBackend::get_color(Level level) {
    switch (level) {
    case none:
        return "\033[30m";  // none
    case trace:
        return "\033[30m";  // none
    case debug:
        return "\033[32m";  // green
    case info:
        return "\033[1m";     // bold
    case warn:
        return "\033[33m";  // yellow
    case alert:
        return "\033[31m";  // red
    case error:
        return "\033[1;31m";  // red
    case fatal:
        return "\033[41m";  // RED
    default:
        return "\033[31m";
    }
}

ConsoleBackend::ConsoleBackend()
{}

ConsoleBackend::~ConsoleBackend()
{}

void ConsoleBackend::log(const Location &loc, Level level, double seconds, cstring message, size_t msg_size) {
    static Mutex    mutex;
    static const char fmt[]    = "%s[%9.4f]\033[0;0m ";

    char title[64];
    const auto color = get_color(level);
    const auto title_len = snprintf(title, sizeof(title), fmt, color, seconds);

    LockGuard<> lock(mutex);
    fwrite(title,   1, title_len, stdout);  // title
    fwrite(message, 1, msg_size,  stdout);  // msg
    fwrite("\n",    1, 1,         stdout);  // newline
    if (level>log::info) fflush(stdout);
}

}

}
