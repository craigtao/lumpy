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
#include <lumpy/core/format.h>

namespace lumpy
{

namespace external
{

#ifdef _WIN32
namespace windows
{
static bool mlock(void* ptr, size_t size)
{
    if (ptr == nullptr || size == 0) return true;

    ulong new_protect = 0x02;    // read only
    ulong old_protect = 0x00;
    auto  stat = VirtualProtect(ptr, size, new_protect, &old_protect);
    return stat != 0;
}

static bool munlock(void* ptr, size_t size)
{
    if (ptr == nullptr || size == 0) return true;

    ulong new_protect = 0x04;    // enabled write
    ulong old_protect = 0x00;
    auto  stat = VirtualProtect(ptr, size, new_protect, &old_protect);
    return stat != 0;
}

static string kernelName(cstring path) {
    string name = path;
    for (auto& c : name) {
        if (c == ':' || c == '\\' || c == '/') c = '|';
    }
    return name;
}

static auto& gMMapTables()
{
    static std::map<void*, void*> map;
    return map;
}

static void* mmap(cstring path, size_t size)
{
    const auto size_low     = reinterpret_cast<const long*>(&size)[0];
    const auto size_high    = reinterpret_cast<const long*>(&size)[1];
    const auto name         = kernelName(path);

    auto obj = OpenFileMappingA(PAGE_READWRITE, 0, name.c_str());
    if (obj == INVALID_HANDLE_VALUE) {
        // if open failed, fid = INVALID_HANDLE_VALUE;
        auto fid    = CreateFileA       (path, GENERIC_WRITE, 0, nullptr, 1, OPEN_EXISTING, nullptr );
        obj         = CreateFileMappingA(fid, nullptr, PAGE_READWRITE, size_high, size_low, name.c_str()   );
        CloseHandle(fid);
    }

    auto data = MapViewOfFile(obj, FILE_MAP_ALL_ACCESS, 0, 0, 0);

    gMMapTables()[data] = obj;
    return data;
}

static void  munmap(void* data)
{
    auto obj = gMMapTables()[data];
    if (obj == INVALID_HANDLE_VALUE) return;

    UnmapViewOfFile(data);
    CloseHandle(obj);

}

static void  msync(void* data, size_t size)
{
    FlushViewOfFile(data, size);
}

}
#endif

#if defined(__APPLE__) || defined(__linux__) || defined(__CYGWIN__)
namespace unix
{
static bool mlock(void* ptr, size_t size) {
    return false;
}

static bool munlock(void* ptr, size_t size) {
    return false;
}

void* mmap(cstring path, size_t size) {
    return nullptr;
}

void munmap(void* ptr) {
    return;
}

void msync(void* data, size_t size) {
    return;
}

}
#endif
}

inline namespace core
{

static constexpr size_t ceilBy(size_t value, size_t block) {
    return (value+block-1)/block*block;
}

static auto realSize(size_t size) {
    constexpr auto sL1  = 4llu*1024;          // L1 cache 4K
    constexpr auto sL2  = 2llu*1024*1024;     // L2 cache 2M
    constexpr auto sL3  = 1llu*1024*1024*1024;// L3 cache 1G
    const auto value= size<sL1 ? size : size<sL2 ? ceilBy(size, sL1) : size<sL3 ? ceilBy(size, sL3) : ceilBy(size, sL3);
    return value;
}

void* _mnew(size_t size) {
    const auto real_size = realSize(size);
    const auto result = ::malloc(real_size);
    return result;
}

void* _mnew(void* ptr, size_t size) {
    const auto real_size = realSize(size);
    const auto result = ::realloc(ptr, real_size);
    return result;
}

void _mdel(void* ptr) {
    ::free(ptr);
}

void _mcpy(void* dst, const void* src, size_t size) {
    ::memcpy(dst, src, size);
}

bool _mlock(void* ptr, size_t size) {
    return external::mlock(ptr, size);
}

bool _munlock(void* ptr, size_t size) {
    return external::munlock(ptr, size);
}

void* _mmap(cstring path, size_t size) {
    return external::mmap(path, size);
}

void  _munmap(void* data) {
    return external::munmap(data);
}

void _msync(void* data, size_t size) {
    return external::msync(data, size);
}

// buffer
EBufferOverflow::EBufferOverflow(size_t size, size_t capicity, size_t extent)
        : size_(size), capicity_(capicity), extent_(extent)
{}

void EBufferOverflow::sformat(IStringBuffer& buffer, const FormatSpec& spec) const {
    buffer.formats("size={}, capicity={}, extent={}", size_, capicity_, extent_);
}

void EBufferOverflow::sformat(IStringBuffer& buffer) const {
    sformat(buffer, {});
}

}

}
