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
#include <lumpy/core/type.h>
#include <lumpy/core/traits.h>
#include <lumpy/core/format.h>

namespace lumpy
{

inline namespace core
{

/* --- count digits --- */
uint _countUIntDigits10(uint32_t n) {
    static const uint32_t powers[] = {
        0,
        10,
        100,
        1000,                   // 1k
        10000,
        100000,
        1000000,                // 1M
        10000000,
        100000000,
        1000000000,             // 1G
    };

    uint t = (32 - __builtin_clz(n | 1)) * 1233 >> 12;
    return uint(t) - (n < powers[t]) + 1;
}

uint _countUIntDigits10(uint64_t n) {
    static const uint64_t powers[] = {
        0ul,                    // 0
        10ul,
        100ul,
        1000ul,                 // 1k
        10000ul,
        100000ul,
        1000000ul,              // 1M
        10000000ul,
        100000000ul,
        1000000000ul,           // 1G
        10000000000ul,
        100000000000ul,
        1000000000000ul,        // 1KG
        10000000000000ul,
        100000000000000ul,
        1000000000000000ul,     // 1MG
        10000000000000000ul,
        100000000000000000ul,
        1000000000000000000ul,  // 1GG
    };

    uint t = (64 - __builtin_clzll(n | 1)) * 1233 >> 12;
    return uint(t) - (n < powers[t]) + 1;
}

template<class UInt>
uint _countUIntDigits(UInt value, char type) {
    static_assert(isUInt<UInt>, "shold be uint");
    uint num_digits = 0;
    switch(type) {
        case 'b': case 'B': { do { ++num_digits; } while ((value >>= 1) != 0); return num_digits; }
        case 'o': case 'O': { do { ++num_digits; } while ((value >>= 3) != 0); return num_digits; }
        case 'x': case 'X': { do { ++num_digits; } while ((value >>= 3) != 0); return num_digits; }
        case 'd': case 'n':
        default: {
            if (value<     10) return 1;
            if (value<    100) return 2;
            if (value<   1000) return 3;
            if (value<  10000) return 4;
            if (value< 100000) return 5;
            if (value<1000000) return 6;
            return _countUIntDigits10(toUInt(value));
        }
    }
}

/* --- format --- */
static const char digits[] =
        "00010203040506070809"
        "10111213141516171819"
        "20212223242526272829"
        "30313233343536373839"
        "40414243444546474849"
        "50515253545556575859"
        "60616263646566676869"
        "70717273747576777879"
        "80818283848586878889"
        "90919293949596979899";

template<class UInt>
static void _uintToRStr(char* p, UInt val, char type) {
    static_assert(isUInt<UInt>, "value not unsigned");

    static const auto digits16 = type == 'x' ? "0123456789abcdef": "0123456789ABCDEF";
    switch (type) {
        case 'b': case 'B': { do { *p-- = char('0' + (val & 1)); } while ((val >>= 1) != 0); break; }
        case 'o': case 'O': { do { *p-- = char('0' + (val & 7)); } while ((val >>= 3) != 0); break; }
        case 'x': case 'X': { do { *p-- = digits16[val & 15];    } while ((val >>= 4) != 0); break; }
        case 'd': case 'n':
        default: {
            if (val< 10) {  *--p = '0'+val;                                 return; }
            if (val<100) {  *--p = digits[val*2+1]; *--p = digits[val*2+0]; return; }

            while (val >= 100) {
                auto idx = val % 100; val /= 100;
                *--p = digits[idx*2+1];
                *--p = digits[idx*2+0];
            }

            if (val>=10) {
                *--p = digits[val*2+1];
                *--p = digits[val*2+0];
            }
            else if (val>0) {
                *--p = char('0' + val);
            }
        }
    }
}

template<class T>
static bool _isNeg(const T& value) {
    if (isUInt<T>) return false;
    return value<0;
}

template<class Int>
static void _formatIntSign(IStringBuffer& buffer, Int value, const FormatSpec& spec) {
    if (_isNeg(value)) {
        buffer.push('-');
    }
    else if (spec.sign!='\0') {
        buffer.push(spec.sign == '+' ? '+' : ' ');
    }
}

template<class UInt>
static void _formatIntBody(IStringBuffer& buffer, UInt value, uint size, const FormatSpec& spec) {
    auto s = buffer.grow(size);
    auto p = s + size;
    _uintToRStr(p, value, spec.type);
}

template<class T>
static void _formatInt(IStringBuffer& buffer, T value, FormatSpec spec) {
    const auto has_sign   = _isNeg(value) || spec.sign!='\0';
    const auto uint_val   = toUInt(_isNeg(value) ? 0-value : value);
    const auto num_digits = _countUIntDigits(uint_val, spec.type);
    const auto num_chars  = num_digits + (has_sign? 1:0);

    if (num_chars >= spec.width) {
        _formatIntSign(buffer, value, spec);
        _formatIntBody(buffer, uint_val, num_digits, spec);
    }
    else {
        switch(spec.align) {
            case '<':
                _formatIntSign(buffer, value, spec);
                _formatIntBody(buffer, uint_val, num_digits, spec);
                buffer.push(spec.fill, spec.width-num_chars);
                break;

            case '>':
                buffer.push(spec.fill, spec.width-num_chars);
                _formatIntSign(buffer, value, spec);
                _formatIntBody(buffer, uint_val, num_digits, spec);
                break;

            case '^':
                _formatIntSign(buffer, value, spec);
                buffer.push(spec.fill, spec.width-num_chars);
                _formatIntBody(buffer, uint_val, num_digits, spec);
                break;

            case '=':
                buffer.push(spec.fill, (spec.width-num_chars +0)/2);
                _formatIntSign(buffer, value, spec);
                _formatIntBody(buffer, uint_val, num_digits, spec);
                buffer.push(spec.fill, (spec.width-num_chars +1)/2);
                break;
        }
    }
}

static void _formatFloat(IStringBuffer& buffer, double   value, const FormatSpec& spec) {
    auto head_size = 0;
    if (value<0) {
        buffer.push('-');
        value = 0 - value;
        ++head_size;
    }
    else if (spec.sign) {
        buffer.push(spec.sign=='+' ? '+' : ' ');
        ++head_size;
    }

    auto width  = spec.width < head_size ? 0 : spec.width - head_size;
    auto prec   = spec.prec==0 ? 6 : spec.prec;
    auto ptr    = buffer.data() + buffer.size();
    auto ret    = snprintf(ptr, 64, "%*.*f", width, prec, value);
    buffer.grow(ret);
}

void formatInt   (IStringBuffer& buffer, int32_t  value, const FormatSpec& spec) { _formatInt(buffer, value, spec);  }
void formatInt   (IStringBuffer& buffer, int64_t  value, const FormatSpec& spec) { _formatInt(buffer, value, spec);  }
void formatInt   (IStringBuffer& buffer, uint32_t value, const FormatSpec& spec) { _formatInt(buffer, value, spec);  }
void formatInt   (IStringBuffer& buffer, uint64_t value, const FormatSpec& spec) { _formatInt(buffer, value, spec);  }
void formatFloat (IStringBuffer& buffer, double   value, const FormatSpec& spec) { _formatFloat(buffer,value, spec); }
void formatString(IStringBuffer& buffer, cstring value, uint length, const FormatSpec& spec) {
    const auto prec = length;
    const auto width= spec.width>prec? spec.width: prec;

    if (width==length) {
        buffer.push(value, width);
    }
    else {
        switch(spec.align) {
            case '<':
                buffer.push(value, prec);
                buffer.push(spec.fill, width - prec);
                break;
            case '>': case '\0':
                buffer.push(spec.fill, width - prec);
                buffer.push(value, prec);
                break;
            case '=': case '^':
                buffer.push(spec.fill, (width - prec) / 2);
                buffer.push(value, prec);
                buffer.push(spec.fill, (width - prec + 1) / 2);
                break;
            default: break;
        }
    }
}

/* --- format parser --- */
static bool _isDigit(char c) { return '0' <= c && c <= '9'; }
inline bool _isAlign(char c) { return c == '<' || c == '>' || c == '=' || c == '^'; }

static ubyte  _parseUInt(cstring& p) {
    ubyte   val = (*p++ - '0');
    char    c   = 0;

    while(_isDigit(c = *p++)) {
        val = val*10+(c-'0');
    }

    return val;
}

void _parseFormatSpec(cstring b, cstring e, FormatSpec& spec) {
    // {....}
    // ^    ^
    // b    e

    // match: {}
    if (b + 1 == e)             return;

    auto p = b + 1;

    // 1. match [0-9]{1,2}:
    if (_isDigit(*p))           { spec.width = _parseUInt(p);                   }

    if (*p++!=':')              return;

    // 2 parse fill/align
    if      (_isAlign(p[1]))    { spec.fill = p[0]; spec.align = p[1]; p += 2;  } // match: [fill][align]
    else if (_isAlign(p[0]))    { spec.fill = ' ' ; spec.align = p[0]; p += 1;  } // match: [align]
    else                        { spec.fill = ' ' ; spec.align = '>';           } // match:

    // 3 parse sign
    if (*p=='+' || *p=='-')     { spec.sign = p[0]; p += 1;     }  // match: [+-]
    else                        { spec.sign = '\0';             }  // match:

    // 4 parse width
    if (_isDigit(*p))           { spec.width = _parseUInt(p);   }  // match: [0-9]*
    else                        { spec.width = 0;               }  // match:

    // 5 parse prec
    if (p[0] == '.')            { spec.prec  = _parseUInt(++p); }

    // 6 parse type
    if (p[0] != '}')            { spec.type = p[0];             }
}


cstring _parseFormatSpec(IStringBuffer& buffer, cstring s, FormatSpec& spec) {
    if (s==nullptr) return nullptr;

    auto id = spec.id;
    spec = {};
    spec.id = id;

    // "...{....."
    //  ^  ^
    //  s  b
    auto b = s;
    while (true) {
        if      (b[0] == '\0')  { break;   }  // match eof
        if      (b[0] != '{')   { b += 1;  }  // match [^}]
        else if (b[1] == '{')   { b += 2;  }  // match {{
        else                    { break;   }  // match {
    }
    if (b    !=  s)     buffer.push(s, b - s);
    if (b[0] != '{')    return nullptr;

    // "...{...}..."
    //  ^  ^   ^
    //  s  b   e
    auto e = b + 1;
    while(true) {
        if (e[0] == '\0') { break;            }  // match eof
        if (e[0] != '}' ) { e += 1; continue; }  // math  [^}]
        if (e[1] == '}' ) { e += 2; continue; }  // match }}
        else              {         break;    }  // match }
    }

    // parse ok?
    if (e[0] == '}') {
        _parseFormatSpec(b, e, spec);
        return e+1;
    }

    // bad-format
    return nullptr;
}

IStringBuffer _gWritefBuffer() {
    static constexpr    auto    size    = 4*1024*1024;
    static thread_local auto    buffer  = mnew<char>(size);

    IStringBuffer result(buffer, size);
    return result;
}

}

}
