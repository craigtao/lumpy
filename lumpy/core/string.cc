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
#include <lumpy/core/string.h>

namespace lumpy
{

inline namespace core
{

template<class T> bool _isNeg(T value, TBool<true> )   { return false;                                 }
template<class T> bool _isNeg(T value, TBool<false>)   { return value < 0;                             }
template<class T> bool  isNeg(T value)                 { return _isNeg(value, TBool<(isUInt<T>)>{});  }


/* --- count digits --- */
template<class Int>
static uint _countDigits10(Int n, If<sizeof(Int)==sizeof(uint32_t)>* =nullptr) {
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

template<class Int>
static uint _countDigits10(Int n, If<sizeof(Int)==sizeof(uint64_t)>* = nullptr) {
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


template<class Int>
static uint countDigits(Int value, char base) {
    uint num_digits = 0;
    if (base=='b' || base=='B') { do { ++num_digits; } while ((value >>= 1) != 0); return num_digits; }
    if (base=='o' || base=='O') { do { ++num_digits; } while ((value >>= 3) != 0); return num_digits; }
    if (base=='x' || base=='X') { do { ++num_digits; } while ((value >>= 3) != 0); return num_digits; }
    return _countDigits10(toUInt(value));
}


static const char digits[] =
        "0001020304050607080910111213141516171819"
        "2021222324252627282930313233343536373839"
        "4041424344454647484950515253545556575859"
        "6061626364656667686970717273747576777879"
        "8081828384858687888990919293949596979899";


template<class UInt>
static void _format_int_body(char* buff, UInt value, uint num_digits, const FormatSpec& spec) {
    auto p = buff + num_digits;

    static const char digits_x[] = "0123456789abcdef";
    static const char digits_X[] = "0123456789ABCDEF";

    switch (spec.type) {
        case 'b': case 'B': { do { *p-- = char('0' + (value & 1)); } while ((value >>= 1) != 0); break; }
        case 'o': case 'O': { do { *p-- = char('0' + (value & 7)); } while ((value >>= 3) != 0); break; }
        case 'x':           { do { *p-- = digits_x[value & 15];    } while ((value >>= 4) != 0); break; }
        case 'X':           { do { *p-- = digits_X[value & 15];    } while ((value >>= 4) != 0); break; }

        default:
            while (value >= 100) {
                auto idx = (value % 100) * 2;
                value /= 100;
                *--p = digits[idx + 1];
                *--p = digits[idx + 0];
            }

            if (value < 10) {
                *--p = char('0' + value);
                return;
            }

            auto idx = value * 2;
            *--p = digits[idx + 1];
            *--p = digits[idx + 0];
            break;
    }
}

template<class Int>
static void _format_int_sign(IStringBuffer& buffer, Int value, const FormatSpec& spec) {
    if (isNeg(value)) {
        buffer.push('-');
    }
    else if (spec.sign!='\0') {
        buffer.push(spec.sign == '+' ? '+' : ' ');
    }
}

template<class T>
static void _format_int(IStringBuffer& buffer, T value, FormatSpec spec) {
    const auto has_sign   = isNeg(value) || spec.sign=='+' || spec.sign == '-';
    const auto uint_val   = toUInt(isNeg(value) ? 0-value : value);
    const auto num_digits = countDigits(uint_val, spec.type);
    const auto num_chars  = num_digits + (has_sign? 1:0);

    if (num_chars >= spec.width) {
        _format_int_sign(buffer, value, spec);
        _format_int_body(buffer.grow(num_digits), uint_val, num_digits, spec);
    }
    else {
        switch(spec.align) {
            case '<':
                _format_int_sign(buffer, value, spec);
                _format_int_body(buffer.grow(num_digits), uint_val, num_digits, spec);
                buffer.push(spec.fill, spec.width-num_chars);
                break;

            case '>':
                buffer.push(spec.fill, spec.width-num_chars);
                _format_int_sign(buffer, value, spec);
                _format_int_body(buffer.grow(num_digits), uint_val, num_digits, spec);
                break;

            case '^':
                _format_int_sign(buffer, value, spec);
                buffer.push(spec.fill, spec.width-num_chars);
                _format_int_body(buffer.grow(num_digits), uint_val, num_digits, spec);
                break;

            case '=':
                buffer.push(spec.fill, (spec.width-num_chars +0)/2);
                _format_int_sign(buffer, value, spec);
                _format_int_body(buffer.grow(num_digits), uint_val, num_digits, spec);
                buffer.push(spec.fill, (spec.width-num_chars +1)/2);
                break;
        }
    }

}

void sformat(IStringBuffer& buffer, null_t<> value, const FormatSpec& spec) { sformat(buffer, "null", spec); }
void sformat(IStringBuffer& buffer, bool     value, const FormatSpec& spec) { value ? sformat(buffer, "true", spec) : sformat(buffer, "false", spec); }

void sformat(IStringBuffer& buffer, int      value, const FormatSpec& spec) { _format_int(buffer, value, spec); }
void sformat(IStringBuffer& buffer, uint     value, const FormatSpec& spec) { _format_int(buffer, value, spec); }
void sformat(IStringBuffer& buffer, long     value, const FormatSpec& spec) { _format_int(buffer, value, spec); }
void sformat(IStringBuffer& buffer, ulong    value, const FormatSpec& spec) { _format_int(buffer, value, spec); }
void sformat(IStringBuffer& buffer, llong    value, const FormatSpec& spec) { _format_int(buffer, value, spec); }
void sformat(IStringBuffer& buffer, ullong   value, const FormatSpec& spec) { _format_int(buffer, value, spec); }

void sformat(IStringBuffer& buffer, float    value, const FormatSpec& spec) {
    sformat(buffer, double(value), spec);
}
void sformat(IStringBuffer& buffer, double   value, const FormatSpec& spec) {
    auto head_size = 0;
    if (isNeg(value)) {
        buffer.push('-');
        value = 0 - value;
        ++head_size;
    }
    else if (spec.sign) {
        buffer.push(spec.sign=='+' ? '+' : ' ');
        ++head_size;
    }

    auto width  = spec.width < head_size ? 0 : spec.width - head_size;
    auto prec   = spec.prec==-1 ? 6 : spec.prec;
    auto ptr    = buffer.data() + buffer.size();
    auto ret    = snprintf(ptr, 64, "%*.*f", width, prec, value);
    buffer.grow(ret);
}

void sformat(IStringBuffer& buffer, cstring value, size_t length, const FormatSpec& spec) {
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
            case '>':
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
inline bool _isDigit(char c) { return '0' <= c && c <= '9'; }
inline bool _isAlign(char c) { return c == '<' || c == '>' || c == '=' || c == '^'; }
inline bool _isSign (char c) { return c == '+' || c == '-'; }

inline int  _parseInt(const char* &p) {
    auto value = 0;
    while (_isDigit(*p)) value = value * 10 + (*p++ - '0');
    return value;
};

FormatSpec FormatParser::next() {
    FormatSpec eof;
    eof.id = -2;

    // find begin
    auto b = ptr_;
    while (true) {
        if      (b[0] == '\0')  { break;  }
        if      (b[0] != '{')   { b += 1; } // match a-z0-9
        else if (b[1] == '{')   { b += 2; } // match {{
        else                    { break;  }
    }
    buffer_.push(ptr_, b - ptr_);   ptr_ = b;
    if (b[0] != '{') return eof;

    // find end
    auto e = b + 1;
    while(true) {
        if (e[0] != '}') { e += 1; continue; }
        if (e[1] == '}') { e += 2; continue; }  // match }}
        break;
    }
    if (e[0] != '}') {
        buffer_.push(ptr_, e - ptr_);
        ptr_ = e;
        return eof;
    }

    ptr_ = e + 1;
    return parse(b, e);
}

FormatSpec FormatParser::parse(const char* b, const char* e) {
    FormatSpec spec;
    // {}
    if (b + 1 == e)             return spec;

    auto p = b + 1;
    // 1 parse index [0-9]*:
    if (_isDigit(*p))           { spec.id = _parseInt(p);   }
    else                        { spec.id = -1;             }

    if (*p++ != ':')            return spec;

    // 2 parse fill/align
    if      (_isAlign(p[1]))    { spec.fill = p[0]; spec.align = p[1]; p += 2; }
    else if (_isAlign(p[0]))    { spec.fill = ' ' ; spec.align = p[0]; p += 1; }
    else                        { spec.fill = ' ' ; spec.align = '<';          }

    // 3 parse sign
    if (_isSign(p[0]))          { spec.sign = p[0]; p += 1;     }

    // 4 parse width
    if (_isDigit(p[0]))         { spec.width =  _parseInt(p);   }

    // 5 parse prec
    if (p[0] == '.')            { spec.prec  = _parseInt(++p);  }

    // 6 parse type
    if (p[0] != '}')            { spec.type = p[0];             }

    return spec;
}

IStringBuffer& _gBufferForWritef() {
    static const auto   capicity = 64*1024*1024;  // 64M
    thread_local static StringBuffer buffer(capicity);
    buffer.clear();
    return buffer;
}

}

}
