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

#include <lumpy/core.h>

namespace lumpy
{

inline namespace io
{

class EFile: public IException
{
  public:
    enum Eid
    {
        Success,
        BadMode,
        NotExist,
        AccessDenied,
        UnknowError
    };

    EFile(Eid eid, const string& path, const string& message)
            : eid_(eid), path_(path), message_(message)
    {}

    void sformat(IStringBuffer& buffer) const {
        buffer.formats("path='{}', message='{}'", path_, message_);
    }
  protected:
    Eid     eid_;
    string  path_;
    string  message_;
};

class File final: INocopyable
{
  public:
    enum Mode
    {
        Read        = 0x1,  // r
        Write       = 0x2,  // w
        ReadWrite   = 0x3   // r+
    };

    explicit File(const string& path, Mode mode);
    ~File();

    size_t size() const;

    template<class T> void read (T*       data, size_t count) { readRaw (data, sizeof(T), count);  }
    template<class T> void write(const T* data, size_t count) { writeRaw(data, sizeof(T), count);  }

    template<class T> T    read ()                            { T data; read(&data, 1); return data;}
    template<class T> void write(const T& data)               { write(&data, 1);                    }

    template<class T, size_t N> void read (      T(&data)[N]) { read (data, N); }
    template<class T, size_t N> void write(const T(&data)[N]) { write(data, N); }

  protected:
    FILE* impl_ = nullptr;
    void readRaw (void*        data, size_t size, size_t count);
    void writeRaw(const void*  data, size_t size, size_t count);
};

}

}
