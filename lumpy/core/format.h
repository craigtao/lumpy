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

#include <lumpy/core/tuple.h>
#include <lumpy/core/string.h>

namespace lumpy
{

inline namespace core
{

/* built-in formats */
void formatInt   (IStringBuffer& buffer, int32_t  value,              const FormatSpec& spec);
void formatInt   (IStringBuffer& buffer, int64_t  value,              const FormatSpec& spec);
void formatInt   (IStringBuffer& buffer, uint32_t value,              const FormatSpec& spec);
void formatInt   (IStringBuffer& buffer, uint64_t value,              const FormatSpec& spec);
void formatFloat (IStringBuffer& buffer, double   value,              const FormatSpec& spec);
void formatString(IStringBuffer& buffer, cstring  value, uint length, const FormatSpec& spec);


/* user-define formats */
template<uint Size>
void sformat(IStringBuffer& buffer, const char(&str)[Size], const FormatSpec& spec={}) {
    sformat(buffer, str, Size-1, spec);
}

inline void sformat(IStringBuffer& buffer, const string&  str, const FormatSpec& spec={}) {
    formatString(buffer, str.c_str(),   str.size(),  spec);
}

inline void sformat(IStringBuffer& buffer, cstring  value, const FormatSpec& spec={}) { formatString(buffer, value, strlen(value), spec);   }
inline void sformat(IStringBuffer& buffer, null_t<> value, const FormatSpec& spec={}) { sformat(buffer, "null", spec);                      }
inline void sformat(IStringBuffer& buffer, bool     value, const FormatSpec& spec={}) { value ? sformat(buffer, "true", spec) : sformat(buffer, "false", spec); }

inline void sformat(IStringBuffer& buffer, int      value, const FormatSpec& spec={}) { formatInt(buffer,   int32_t(value), spec);      }
inline void sformat(IStringBuffer& buffer, uint     value, const FormatSpec& spec={}) { formatInt(buffer,   uint32_t(value), spec);     }
inline void sformat(IStringBuffer& buffer, llong    value, const FormatSpec& spec={}) { formatInt(buffer,   int64_t(value), spec);      }
inline void sformat(IStringBuffer& buffer, ullong   value, const FormatSpec& spec={}) { formatInt(buffer,   uint64_t(value), spec);     }

inline void sformat(IStringBuffer& buffer, long     value, const FormatSpec& spec={}) {
    sizeof(long)==sizeof(int32_t)
            ? formatInt(buffer, int32_t(value), spec)
            : formatInt(buffer, int64_t(value), spec);
}

inline void sformat(IStringBuffer& buffer, ulong     value, const FormatSpec& spec={}) {
    sizeof(ulong)==sizeof(uint32_t)
            ? formatInt(buffer, uint32_t(value), spec)
            : formatInt(buffer, uint64_t(value), spec);
}

inline void sformat(IStringBuffer& buffer, float    value, const FormatSpec& spec={}) { formatFloat(buffer, value, spec);               }
inline void sformat(IStringBuffer& buffer, double   value, const FormatSpec& spec={}) { formatFloat(buffer, value, spec);               }

 
inline void sformat(IStringBuffer& buffer, const IException& value, const FormatSpec& spec=FormatSpec{}) { value.sformat(buffer, spec);}

/* --- parse fmt-spec --- */
cstring _parseFormatSpec(IStringBuffer& buffer, cstring str, FormatSpec& spec);

template<uint I, class ...T> auto& _getFormatArgs(const Tuple<T...>& argv, TBool<true>)  { return *(argv.template at<I>());             }
template<uint I, class ...T> auto& _getFormatArgs(const Tuple<T...>& argv, TBool<false >){ static null_t<> result; return result;       }
template<uint I, class ...T> auto& _getFormatArgs(const Tuple<T...>& argv)  { return _getFormatArgs<I>(argv, TBool<(I<sizeof...(T))>{});}

/* --- string format --- */
template<class T>
void IStringBuffer::format(const T& value, const FormatSpec& spec) {
    sformat(*this, value, spec);
}

template<class ...T>
void IStringBuffer::formats(const string& fmt, const T&... args) {
    auto    argv    = makeTuple(&args...);
    cstring fmtstr  = fmt.c_str();

    for(FormatSpec spec={}; (fmtstr=_parseFormatSpec(*this, fmtstr, spec))!=nullptr; ++spec.id) {
        switch(spec.id) {
            case  0: format(_getFormatArgs< 0>(argv), spec); break;
            case  1: format(_getFormatArgs< 1>(argv), spec); break;
            case  2: format(_getFormatArgs< 2>(argv), spec); break;
            case  3: format(_getFormatArgs< 3>(argv), spec); break;
            case  4: format(_getFormatArgs< 4>(argv), spec); break;
            case  5: format(_getFormatArgs< 5>(argv), spec); break;
            case  6: format(_getFormatArgs< 6>(argv), spec); break;
            case  7: format(_getFormatArgs< 7>(argv), spec); break;
            case  8: format(_getFormatArgs< 8>(argv), spec); break;
            case  9: format(_getFormatArgs< 9>(argv), spec); break;
            case 10: format(_getFormatArgs<10>(argv), spec); break;
            case 11: format(_getFormatArgs<11>(argv), spec); break;
            case 12: format(_getFormatArgs<12>(argv), spec); break;
            case 13: format(_getFormatArgs<13>(argv), spec); break;
            case 14: format(_getFormatArgs<14>(argv), spec); break;
            case 15: format(_getFormatArgs<15>(argv), spec); break;
            case 16: format(_getFormatArgs<16>(argv), spec); break;
            case 17: format(_getFormatArgs<17>(argv), spec); break;
            case 18: format(_getFormatArgs<18>(argv), spec); break;
            case 19: format(_getFormatArgs<19>(argv), spec); break;
            default: throw(IException());
        }
    }
    this->end();
}

/* --- common format --- */
template<class ...Ts>
StringBuffer format(const string& fmt, const Ts&...ts) {
    StringBuffer buffer;
    buffer.formats(fmt, ts...);
    return buffer;
}

IStringBuffer _gWritefBuffer();

template<class ...Ts>
void writef (const string& fmt, const Ts&...ts) {
    auto buffer = _gWritefBuffer();
    buffer.formats(fmt, ts...);
    ::fwrite(buffer.data(), 1, buffer.size(), stdout);
}

}

}
