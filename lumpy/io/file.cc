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
#include <lumpy/io/file.h>

namespace lumpy
{

inline namespace io
{

File::File(const string& path, Mode m) {
    const auto mode_str
            = m == Mode::Read      ? "rb"
            : m == Mode::Write     ? "wb"
            : m == Mode::ReadWrite ? "rb+"
            : nullptr;

    if (mode_str == nullptr) throw EFile(EFile::BadMode, path, "bad open mode");

    auto eid = 0;
#ifdef _WIN32
    eid = fopen_s(&impl_, path.c_str(), mode_str);
#else
    impl_ = fopen(path.c_str(), mode_str);
    eid = errno;
#endif

    if (impl_ == nullptr) {
        if (eid == ENOENT)    throw EFile(EFile::NotExist,    path, "no such file");
        if (eid == EACCES)    throw EFile(EFile::AccessDenied,path, "access denied");
        else                  throw EFile(EFile::UnknowError, path, "unknow error");
    }
}

File::~File() {
    fclose(impl_);
}


size_t File::size() const {
    auto pos = ftell(impl_);
    fseek(impl_, 0, SEEK_END);
    auto len = ftell(impl_);
    fseek(impl_, pos, SEEK_SET);

    return len;
}

void File::readRaw(void* data, size_t size, size_t count) {
    fread(data, size, count, impl_);
}

void File::writeRaw(const void* data, size_t size, size_t count) {
    fwrite(data, size, count, impl_);
}


}

}
