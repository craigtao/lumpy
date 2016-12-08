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

#include <lumpy/log.h>

namespace lumpy
{
namespace log
{

Logger& _default_logger() {
    static Logger           logger;
    static ConsoleBackend   console_backend;

#ifdef _DEBUG
    logger.level(debug);
#else
    logger.level(info);
#endif

    logger += console_backend;
    return logger;
}


Logger* setDefaultLogger(Logger* value = nullptr) {
    static auto logger = &_default_logger();
    if (value != nullptr) { logger = value; }
    return logger;
}

Logger& setDefaultLogger(Logger& value) {
    return *(setDefaultLogger(&value));
}

Logger& getDefaultLogger() {
    return *(setDefaultLogger(nullptr));
}


static thread_local char gsBuffer[4*1024];

LogItem::LogItem(const Location& location, Level level): location_(location), level_(level), buffer_(gsBuffer, sizeof(gsBuffer))
{}

LogItem::~LogItem() {
    const auto msg  = buffer_.data();
    const auto size = buffer_.size();
    getDefaultLogger().log(location_, level_, msg, size);
}


}

}
