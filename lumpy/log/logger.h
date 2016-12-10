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
#include <lumpy/log/common.h>

namespace lumpy
{

namespace log
{

struct Location
{
    cstring file;
    cstring func;
    int     line;
};

class IBackend: INocopyable
{
    friend class Logger;

  public:
    IBackend()          = default;
    virtual ~IBackend() = default;
    virtual void log(const Location& loc, Level level, double seconds, cstring message, size_t size) = 0;
};

class Logger final: INocopyable
{
  public:
    Logger()    {}
    ~Logger()   {}

    Level level()           const noexcept { return level_; }
    void  level(Level value)      noexcept { level_ = value;}

    Logger& operator+=(IBackend& be) {
        for (auto& backend: backends_) {
            if (backend == &be) return *this;
        }
        backends_.push_back(&be);
        return *this;
    }

    void log(const Location& location, Level level, cstring message, size_t size) {
        if (level < level_) return;

        auto proc_time = timer_.duration();
        for (auto& backend: backends_) {
            backend->log(location, level, proc_time, message, size);
        }
    }

  protected:
    std::vector<IBackend*>  backends_;
    Level                   level_  = info;
    Timer                   timer_;
};

Logger& setDefaultLogger(Logger& logger);
Logger& getDefaultLogger();


class ConsoleBackend final : public IBackend
{
  public:
    ConsoleBackend();
    ~ConsoleBackend();
    void log(const Location& loc, Level level, double seconds, cstring message, size_t size) override;

  protected:
    static const char*  get_color(Level level);

};

class LogItem: public INocopyable
{
  public:
    LogItem(const Location& location, Level level);
    ~LogItem();

    template <class Fmt, typename... Args>
    auto& operator()(const Fmt& fmt, const Args& ...args) {
        if (level_ >= getDefaultLogger().level())  {
            buffer_.formats(fmt, args...);
        }

        return *this;
    }

    template<class Value>
    auto& operator<<(const Value& value) {
        if (level_ >= getDefaultLogger().level())  {
            sformat(buffer_, value);
        }
        return *this;
    }

  protected:
    Location        location_;
    Level           level_;
    IStringBuffer   buffer_;
};

}

}
