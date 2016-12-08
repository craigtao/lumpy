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
#include <lumpy/ptree/json.h>

namespace lumpy
{

namespace ptree
{

static bool isspace(char c) {
    return c == ' ' || (c >= '\t' && c <= '\r');
}

static bool isdelim(char c) {
    return c == ',' || c == ':' || c == ']' || c == '}' || isspace(c) || !c;
}

static char peek_char(cstring &s, cstring e) {
    while (s<e && isspace(*s)) { ++s; }

    if (s>=e) throw s;
    return *s;
}

template<uint S> void parse_keyword(const char(&expect)[S], cstring& str, cstring end) {
    constexpr auto N = S-1;

    if (N>0 && expect[0] != str[0]) throw (str+0);
    if (N>1 && expect[1] != str[1]) throw (str+1);
    if (N>2 && expect[2] != str[2]) throw (str+2);
    if (N>3 && expect[3] != str[3]) throw (str+3);
    if (N>4 && expect[4] != str[4]) throw (str+4);

    if (end-str < N)        throw(str+N);
    if (!isdelim(str[N]))   throw(str+N);
    str += N;
}

struct StringView
{
    cstring     str;
    uint16_t    size;
};

static void parse_failed(cstring s) {
    throw s;
}

static StringView parse_string(cstring &s, cstring e) {
    //  "123456789"?
    //  ^----------^
    //  p--------->s
    auto p = s + 1;
    while (++s < e) {
        if (*s == '"')  break;
        if (*s == '\t' || *s == '\n' || *s == '\r') { parse_failed(s); }
        if (*s == '\\') {
            auto t = s[1];
            if(isspace(t))  { parse_failed(s); }
            else            { ++s; }
        }
    }
    ++s;

    return{ p, uint16_t(s - p - 1) };
}

static double parse_number(cstring& s, cstring e) {
    char* p = nullptr;
    auto  r = strtod(s, &p);
    if (p > e || !isdelim(*p)) parse_failed(p);
    s = p;

    return r;
}

static JNode& parse_any(cstring &s, cstring e,JDoc& json, JNode* root, JNode* pleft);

static JNode& parse_array(cstring& s, cstring e, JDoc& json, JNode* proot, JNode* pleft) {
    auto    root = &json.add_item(proot, pleft, JArray{});
    JNode*  left = nullptr;

    //! not required
    //! if (s[0] != '[') throw s;

    ++s;
    if (peek_char(s, e) == ']') { ++s; return *root; }
    if (*s == '\0') { parse_failed(s); }

    // [*,*,*]
    while (true) {
        auto& item = parse_any(s, e, json, root, left);
        left = &item;
        auto  c = peek_char(s, e);
        if (c == ',') { ++s; continue;      }
        if (c == ']') { ++s; return *root;  }
        parse_failed(s);
    }

    throw s;
}

static JNode& parse_key(cstring& s, cstring e, JDoc& json, JNode* proot, JNode* pleft) {
    if (peek_char(s, e) != '"') parse_failed(s);

    auto    key = parse_string(s, e);
    if (pleft != nullptr && pleft->right != 0) {
        parse_failed(s);
    }
    auto&   item = json.add_item(proot, pleft, JKeyVal{ key.str, key.size });
    return  item;
}

static JNode& parse_object(cstring& s, cstring e, JDoc& json, JNode* proot, JNode* pleft) {
    auto    root = &json.add_item(proot, pleft, JObject{});
    JNode*  left = nullptr;
    ++s;

    // {}
    if (peek_char(s, e) == '}') { ++s; return *root; }

    while (*s) {
        auto& key = parse_key(s, e, json, root, left);
        left = &key;
        if (left != nullptr && left->right != 0) parse_failed(s);

        if (peek_char(s, e) != ':') parse_failed(s);
        ++s;

        auto& val = parse_any(s, e, json, nullptr, nullptr); (void)val;
        if (left != nullptr && left->right != 0) {
            parse_failed(s);
        }

        auto c = peek_char(s, e);
        if (c == ',') { s += 1; continue;       }
        if (c == '}') { s += 1; return *root;   }
        parse_failed(s);
    }

    throw s;
}

static JNode& parse_any(cstring &s, cstring e, JDoc& json, JNode* proot, JNode* pleft) {
    if (pleft != nullptr && pleft->right != 0) {
        parse_failed(s);
    }
    // ignore space
    auto c = peek_char(s, e);

    switch (c) {
        case 'n':   { parse_keyword("null",  s, e);    return json.add_item(proot, pleft, JNull{});     }
        case 't':   { parse_keyword("true",  s, e);    return json.add_item(proot, pleft, JTrue{});     }
        case 'f':   { parse_keyword("false", s, e);    return json.add_item(proot, pleft, JFalse{});    }
        case '[':   { return parse_array (s, e, json, proot, pleft);   }
        case '{':   { return parse_object(s, e, json, proot, pleft);   }
        case '"':   { auto v = parse_string(s, e); return json.add_item(proot, pleft, JString{ v.str, v.size }); }
        case '0':   { if (s[1]!='.' && !isdelim(s[1])) parse_failed(s); }
        case '+': case '-': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': {
            auto v = parse_number(s, e);
            return json.add_item(proot, pleft, JNumber{ v });
        }
        default:    { parse_failed(s); return *proot;                 }
    }
}

void JDoc::parse(cstring s, size_t size) {
    auto b  = s;
    auto e  = s+size;

    try {
        auto c = peek_char(s, e);
        if (c!='{' && c != '[') {
            parse_failed(s);
        }
        parse_any(s, e, *this, nullptr, nullptr);
        while(s<e && isspace(*s)) ++s;
        if (s!=e) {
            parse_failed(s);
        }
    }
    catch (const char *p) {
        throw EJParseFailed(b, p-b);
    }
}

void JDoc::staticInit() {
    static_assert(sizeof(JNode)   == sizeof(JNull),   "bad json size");
    static_assert(sizeof(JNode)   == sizeof(JTrue),   "bad json size");
    static_assert(sizeof(JNode)   == sizeof(JFalse),  "bad json size");
    static_assert(sizeof(JNode)   == sizeof(JArray),  "bad json size");
    static_assert(sizeof(JNode)   == sizeof(JObject), "bad json size");
    static_assert(sizeof(JNode)*2 == sizeof(JNumber), "bad json size");
    static_assert(sizeof(JNode)*2 == sizeof(JString), "bad json size");
}

void sindent(IStringBuffer& buffer ,uint level, bool indent) {
    if (indent) {
        buffer.push(' ', level*2);
    }
}

void sformat(IStringBuffer& buffer, const JNode& head, const FormatSpec& spec, uint level, bool indent) {
    sindent(buffer, level, indent);

    switch (head.type) {
        case JType::Null:  sformat(buffer, "null");        break;
        case JType::True:  sformat(buffer, "true");        break;
        case JType::False: sformat(buffer, "false");       break;

        case JType::Number: { auto* obj = reinterpret_cast<const JNumber*>(&head); sformat(buffer, obj->value, spec);           break; }
        case JType::String: { auto* obj = reinterpret_cast<const JString*>(&head); buffer.push(obj->string - 1, obj->size + 2); break; }

        case JType::Array: {

            if (head.size != 0) {
                buffer.push("[\n");
                auto item = &head + 1;
                while (item != nullptr) {
                    auto& val = *item;
                    sformat(buffer, val, spec, level+1, true);
                    item = item->next();

                    if (item != nullptr) buffer.push(",\n");
                    else                 buffer.push("\n");
                }
                sindent(buffer, level, true); buffer.push("]");
            }
            else {
                buffer.push("[]");
            }
            break;
        }
        case JType::Object: {
            if (head.size != 0) {
                buffer.push("{\n");
                auto item = &head + 1;
                while (item != nullptr) {
                    auto& key = *reinterpret_cast<const JKeyVal*>(item);
                    auto& val = key.value();
                    sindent(buffer, level+1, true); buffer.push(key.string-1, key.size+2); buffer.push(": ");
                    sformat(buffer, val, spec, level+1, false);
                    item = item->next();

                    if (item != nullptr) buffer.push(",\n");
                    else                 buffer.push("\n");
                }
                sindent(buffer, level, true); buffer.push("}");
            }
            else {
                buffer.push("{}");
            }
            break;
        }

        default: break;
    }
}

void sformat(IStringBuffer& buffer, const JNode& node, const FormatSpec& spec) {
    sformat(buffer, node, spec, 0, false);
}

}  // json

}   // lumpy
