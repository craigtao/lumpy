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
#include <lumpy/core/memory.h>

namespace lumpy
{

inline namespace core
{

// id:[[fill]align][sign][width].[prec][type]
struct FormatSpec
{
  int8_t  id     = -1;
  char    fill   = ' ';  // [a-z0-9]
  char    align  = '>';  // [<>=]
  char    sign   = '\0';  // [+-]
  uint8_t width  = +0;    // [0-9]*
  int8_t  prec   = -1;    // [0-9]*
  char    type   = '\0';  // [a-z]
};

template<class ...TArgs>
struct FormatArgs
{
  explicit FormatArgs(const TArgs& ...args): args_{&args ...}{}

  template<uint I> decltype(auto) get() const {
    return _get(TBool<(I<sizeof...(TArgs))>{}, TIdx<I>{});
  }

 protected:
  std::tuple<const TArgs*...> args_;
  template<uint I> auto& _get(TBool<true >, TIdx<I>) const { return *std::get<I>(args_);   }
  template<uint I> auto  _get(TBool<false>, TIdx<I>) const { return null_t<>{};            }
};

class FormatParser
{
 public:
  FormatParser(IBuffer<char>& buffer, const string& fmt)
      : buffer_(buffer), ptr_(fmt.c_str())
  {}

  explicit operator bool() const { return *ptr_ != 0; }
  FormatSpec      next();

 private:
  IBuffer<char>&  buffer_;
  const char*     ptr_;

  static FormatSpec  parse(const char* b, const char* e);
};



struct IStringBuffer: IBuffer<char>
{
 public:
  using base = IBuffer<char>;

  IStringBuffer(char* buffer, size_t size): base(buffer, size) {}
  ~IStringBuffer() = default;

  auto c_str() const noexcept {
    return data();
  }

  void end() {
    base::data_[base::size_] = '\0';
  }

  template<class ...Ts>
  void formats(const string& fmt, const Ts&... args);

  template<class T>
  void format(const T& value, const FormatSpec& spec=FormatSpec{});
};

class StringBuffer: public IStringBuffer, INocopyable
{
 public:
  using base = IStringBuffer;

  StringBuffer(size_t capicity=4*1024):  base(mnew<char>(capicity), capicity) {}
  ~StringBuffer() { delete base::data_;}

  StringBuffer(StringBuffer&& rhs) noexcept: base(rhs) {
    rhs.data_       = nullptr;
    rhs.size_       = 0;
    rhs.capicity_   = 0;
  }

  StringBuffer& operator=(StringBuffer&& rhs) noexcept {
    base::operator=(rhs);
    rhs.data_       = 0;
    rhs.size_       = 0;
    rhs.capicity_   = 0;
    return *this;
  }
};

// built-in types support
void sformat(IStringBuffer& buffer, null_t<> value, const FormatSpec& spec=FormatSpec{});
void sformat(IStringBuffer& buffer, bool     value, const FormatSpec& spec=FormatSpec{});
void sformat(IStringBuffer& buffer, int      value, const FormatSpec& spec=FormatSpec{});
void sformat(IStringBuffer& buffer, uint     value, const FormatSpec& spec=FormatSpec{});
void sformat(IStringBuffer& buffer, long     value, const FormatSpec& spec=FormatSpec{});
void sformat(IStringBuffer& buffer, ulong    value, const FormatSpec& spec=FormatSpec{});
void sformat(IStringBuffer& buffer, llong    value, const FormatSpec& spec=FormatSpec{});
void sformat(IStringBuffer& buffer, ullong   value, const FormatSpec& spec=FormatSpec{});
void sformat(IStringBuffer& buffer, float    value, const FormatSpec& spec=FormatSpec{});
void sformat(IStringBuffer& buffer, double   value, const FormatSpec& spec=FormatSpec{});
void sformat(IStringBuffer& buffer, cstring  value, size_t size, const FormatSpec& spec=FormatSpec{});

inline void sformat(IStringBuffer& buffer, const IException&    obj, const FormatSpec& spec=FormatSpec{}) { obj.sformat(buffer);                             }
inline void sformat(IStringBuffer& buffer, cstring              str, const FormatSpec& spec=FormatSpec{}) { sformat(buffer, str,        ::strlen(str), spec);}
inline void sformat(IStringBuffer& buffer, const string&        str, const FormatSpec& spec=FormatSpec{}) { sformat(buffer, str.c_str(), str.size(),   spec);}
inline void sformat(IStringBuffer& buffer, const IStringBuffer&  str, const FormatSpec& spec=FormatSpec{}) { sformat(buffer, str.data(), str.size(),   spec);}

template<uint Size> void sformat(IStringBuffer& buffer, const char(&str)[Size],   const FormatSpec& spec=FormatSpec{}) {
  return sformat(buffer, str,         Size-1,       spec);
}

template<class ...Ts>
void IStringBuffer::formats(const string& fmt, const Ts&... args) {
  FormatArgs<Ts...>   argv  { args... };
  FormatParser        parser{ *this, fmt };
  int8_t              id = 0;

  while(parser) {
    auto spec = parser.next();

    if (spec.id == -1) spec.id = char(id);
    if (spec.id == -2) break;

    id = spec.id+1;

    switch(spec.id) {
      case  0: sformat(*this, argv.template get< 0>(), spec); break;
      case  1: sformat(*this, argv.template get< 1>(), spec); break;
      case  2: sformat(*this, argv.template get< 2>(), spec); break;
      case  3: sformat(*this, argv.template get< 3>(), spec); break;
      case  4: sformat(*this, argv.template get< 4>(), spec); break;
      case  5: sformat(*this, argv.template get< 5>(), spec); break;
      case  6: sformat(*this, argv.template get< 6>(), spec); break;
      case  7: sformat(*this, argv.template get< 7>(), spec); break;
      case  8: sformat(*this, argv.template get< 8>(), spec); break;
      case  9: sformat(*this, argv.template get< 9>(), spec); break;
      case 10: sformat(*this, argv.template get<10>(), spec); break;
      case 11: sformat(*this, argv.template get<11>(), spec); break;
      case 12: sformat(*this, argv.template get<12>(), spec); break;
      case 13: sformat(*this, argv.template get<13>(), spec); break;
      case 14: sformat(*this, argv.template get<14>(), spec); break;
      case 15: sformat(*this, argv.template get<15>(), spec); break;
      case 16: sformat(*this, argv.template get<16>(), spec); break;
      case 17: sformat(*this, argv.template get<17>(), spec); break;
      case 18: sformat(*this, argv.template get<18>(), spec); break;
      case 19: sformat(*this, argv.template get<19>(), spec); break;
      default: throw(IException());
    }
  }
  end();
}

template<class T>
void IStringBuffer::format(const T& value, const FormatSpec& spec) {
  sformat(*this, value, spec);
}

template<class ...Ts> StringBuffer format(const string& fmt, const Ts&...ts) {
  StringBuffer buffer;
  buffer.formats(fmt, ts...);
  return buffer;
}

IStringBuffer& _gBufferForWritef();
template<class ...Ts> void writef (const string& fmt, const Ts&...ts) {
  auto& buffer = _gBufferForWritef();
  buffer.formats(fmt, ts...);
  ::fwrite(buffer.data(), 1, buffer.size(), stdout);
}

}

}
