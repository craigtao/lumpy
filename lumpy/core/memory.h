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

#include <lumpy/core/type.h>

namespace lumpy
{

inline namespace core
{

void*   _mnew   (size_t size);
void*   _mnew   (void* ptr, size_t size);
void    _mdel   (void* ptr);
void    _mcpy   (void* dst, const void* src, size_t size);

bool    _mlock  (void* ptr, size_t size);
bool    _munlock(void* ptr, size_t size);

void*   _mmap   (cstring path, size_t size);
void    _munmap (void*data);
void    _msync  (void*data, size_t size);

template<class T> T*   mnew   (size_t size)                         { return reinterpret_cast<T*>(_mnew(size*sizeof(T)));       }
template<class T> T*   mnew   (T* ptr, size_t size)                 { return reinterpret_cast<T*>(_mnew(ptr, size*sizeof(T)));  }
template<class T> void mdel   (T* ptr)                              { return _mdel(ptr); }
template<class T> void mcpy   (T* dst,  const T* src, size_t size)  { return _mcpy(dst, src, size*sizeof(T)); }

template<class T> bool mlock  (T* ptr, size_t size)                 { return _mlock  (ptr, size * sizeof(T)); }
template<class T> bool munlock(T* ptr, size_t size)                 { return _munlock(ptr, size * sizeof(T)); }

template<class T> T*   mmap   (cstring path, size_t size)           { return reinterpret_cast<T*>(_mmap(path, size*sizeof(T))); }
template<class T> void munmap (T* data)                             { return _munmap(data);                                     }
template<class T> void msync  (T* data, size_t size)                { return _msync(data, size*sizeof(T));                      }


class EBufferOverflow: public IException
{
 public:
  EBufferOverflow(size_t size, size_t capicity, size_t extent);
  void sformat(IStringBuffer& buffer) const override;

 protected:
  size_t  size_;
  size_t  capicity_;
  size_t  extent_;
};


template<class Type>
struct IBuffer: ArrayView<Type>
{
 public:
  using base   = ArrayView<Type>;
  static constexpr auto default_capicity = 4*4096;//std::numeric_limits<uint>::max();

  IBuffer(Type* ptr, size_t capicity): base(ptr, 0), capicity_(capicity) {}
  ~IBuffer()   = default;

  constexpr auto  capicity() const noexcept { return capicity_; }

  void clear() {
    base::size_ = 0;
  }

  Type* grow(size_t size) noexcept {
    auto result = base::data_ + base::size_;
    base::size_ += size;
    return result;
  }

  Type* push(const Type* src, size_t count) {
    if (base::size_+count >= capicity_) throw(EBufferOverflow(base::size_, capicity_, count));

    auto dst = grow(count);
    mcpy(dst, src, count);
    return dst;
  }

  Type* push(const Type& value, size_t count) {
    if (base::size_+count >= capicity_) throw(EBufferOverflow(base::size_, capicity_, count));

    auto dst = grow(count);
    for (size_t i = 0; i < count; ++i) {
      dst[i] = value;
    }
    return dst;
  }

  template<uint N> Type* push(const Type(&data)[N]) {
    return push(data, N);
  }

  Type& push(const Type& value) {
    return *push(value, 1);
  }

 protected:
  std::size_t  capicity_ = 0;
};


template<class Type>
class Buffer: public IBuffer<Type>, INocopyable
{
 public:
  using base = IBuffer<Type>;

  Buffer(size_t size) : base(mnew<Type>(size), size) {}

  ~Buffer() {
    if (base::data_ != nullptr) mdel(base::data_);
  }

  Buffer(Buffer&& rhs) noexcept: base(rhs) {
    rhs.data_       = nullptr;
    rhs.size_       = 0;
    rhs.capicity_   = 0;
  }

  Buffer& operator=(Buffer&& rhs) noexcept {
    base::operator=(rhs);
    rhs.data_       = 0;
    rhs.size_       = 0;
    rhs.capicity_   = 0;
    return *this;
  }
};

}

}
