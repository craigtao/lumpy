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

namespace json
{

static bool isspace(char c) {
    return c == ' ' || (c >= '\t' && c <= '\r');
}

static bool isdelim(char c) {
    return c == ',' || c == ':' || c == ']' || c == '}' || isspace(c) || !c;
}

struct StringView
{
    cstring     str;
    uint16_t    size;
};

static inline void parse_failed(cstring s) {
    throw s;
}

static char peek_char(cstring &s, cstring e) {
    while (s<e && isspace(*s)) { ++s; }
    if (s>=e) parse_failed(s);
    return *s;
}

// true, false, null
template<uint S> void parse_keyword(const char(&k)[S], cstring& s, cstring e) {
    constexpr auto N = S-1;
    if (N==4 && *reinterpret_cast<const uint32_t*>(s+0) != *reinterpret_cast<const uint32_t*>(k+0)) { parse_failed(s);}
    if (N==5 && *reinterpret_cast<const uint32_t*>(s+1) != *reinterpret_cast<const uint32_t*>(k+1)) { parse_failed(s);}
    s += N;
}

static StringView parse_string(cstring &s, cstring e) {
    //  "123456789"?
    //  ^----------^
    //  p--------->s
    auto p = s + 1;
    while (++s < e) {
        auto c = ubyte(*s);
        if (c<=0x7F) {
            if (*s>=' ') {
                if (c=='"')    break;
                if (c=='\\')   {
                    c = *++s;
                    if (c<=' ') parse_failed(s);
                }
            }
            else {
                parse_failed(s);
            }
        }
        else {  // utf-8 support
            if (c== 0b11000000){ s+=1;}
            if (c== 0b11100000){ s+=2;}
            if (c== 0b11110000){ s+=3;}
            if (c== 0b11111000){ s+=4;}
            if (c== 0b11111100){ s+=5;}
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

static JNode& parse_any(cstring &s, cstring e,JTree& json, JNode* root, JNode* pleft);

static JNode& parse_array(cstring& s, cstring e, JTree& json, JNode* proot, JNode* pleft) {
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

    parse_failed(s);
}

static JNode& parse_key(cstring& s, cstring e, JTree& json, JNode* proot, JNode* pleft) {
    if (peek_char(s, e) != '"') parse_failed(s);

    auto    key = parse_string(s, e);
    if (pleft != nullptr && pleft->next() != 0) {
        parse_failed(s);
    }
    auto&   item = json.add_item(proot, pleft, JKeyVal{ key.str, key.size });
    return  item;
}

static JNode& parse_object(cstring& s, cstring e, JTree& json, JNode* proot, JNode* pleft) {
    auto    root = &json.add_item(proot, pleft, JObject{});
    JNode*  left = nullptr;
    ++s;

    // {}
    if (peek_char(s, e) == '}') { ++s; return *root; }

    while (*s) {
        auto& key = parse_key(s, e, json, root, left);
        left = &key;
        if (left != nullptr && left->next() != 0) parse_failed(s);

        if (peek_char(s, e) != ':') parse_failed(s);
        ++s;

        auto& val = parse_any(s, e, json, nullptr, nullptr); (void)val;
        if (left != nullptr && left->next() != 0) {
            parse_failed(s);
        }

        auto c = peek_char(s, e);
        if (c == ',') { s += 1; continue;       }
        if (c == '}') { s += 1; return *root;   }
        parse_failed(s);
    }

    throw s;
}

static JNode& parse_any(cstring &s, cstring e, JTree& json, JNode* proot, JNode* pleft) {
    if (pleft != nullptr && pleft->next() != 0) {
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
        default:    { parse_failed(s); return *proot;   }
    }
}

void JTree::parse(cstring s, size_t size) {
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

void ITree::operator>>(std::string& value) const {
    if (node_->type() != JType::String) { throw EUnexpect(); }
    auto node = reinterpret_cast<JString*>(node_);
    auto src = node->value();
    auto end = src+node->size();

    value.reserve(node->size());
    for(auto c = *src; c=*src, src<end; ++src) {
        if (c=='\\') {
            auto x = *++src;
            switch(x) {
            case 'a': value.push_back('\a'); break;
            case 'b': value.push_back('\b'); break;
            case 'e': value.push_back('\033'); break;
            case 'r': value.push_back('\r'); break;
            case 'n': value.push_back('\n'); break;
            case 't': value.push_back('\t'); break;
            default:  value.push_back(c); value.push_back(x); break;
            }
        }
        else {
            value.push_back(c);
        }
    }
}

void ITree::operator<<(const std::string& value) {
    if (node_->type() == JType::Null) {
        node_->type(JType::String);
        buffer_->grow(1);
    }
    if (node_->type() != JType::String) { throw EUnexpect(); }
    auto node = reinterpret_cast<JString*>(node_);
    node->value(value.c_str(), uint(value.size()));
}

void JTree::staticInit() {
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

    switch (head.type()) {
        case JType::Null:  sformat(buffer, "null");        break;
        case JType::True:  sformat(buffer, "true");        break;
        case JType::False: sformat(buffer, "false");       break;

        case JType::Number: {
            auto* obj = reinterpret_cast<const JNumber*>(&head);
            sformat(buffer, obj->value(), spec);
            break;
        }

        case JType::String: {
            auto* obj = reinterpret_cast<const JString*>(&head);
            static thread_local char jbuff[4096];
            auto src = obj->value();
            auto end = src+obj->size();
            auto dst = jbuff;

            *dst++ = '"';
            for(uint i =0; i < uint(sizeof(jbuff)-1) && src<end; ++i, ++src) {
                auto c = *src;
                switch(c) {
                case '\a':  *dst++ = '\\'; *dst++ = 'a'; break;
                case '\b':  *dst++ = '\\'; *dst++ = 'b'; break;
                case '\033':*dst++ = '\\'; *dst++ = 'e'; break;
                case '\r':  *dst++ = '\\'; *dst++ = 'r'; break;
                case '\n':  *dst++ = '\\'; *dst++ = 'n'; break;
                case '\t':  *dst++ = '\\'; *dst++ = 't'; break;
                default:    *dst++ = c;    break;
                }
            }
            *dst++= '"';
            buffer.push(jbuff, dst-jbuff);
            break;
        }

        case JType::Array: {
            if (head.size() != 0) {
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
            auto size = head.size();
            if (size != 0) {
                buffer.push("{\n");
                auto item = &head + 1;
                for(uint i = 0; i < size; ++i) {
                    if (item->type()!=JType::KeyVal) {
                        throw EUnexpect{};
                    }

                    auto& key = *reinterpret_cast<const JKeyVal*>(item);
                    auto& val = key.value();
                    sindent(buffer, level+1, true);

                    buffer.push('"');
                    buffer.push(key.key(), key.size());
                    buffer.push("\": ");

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

}  // lumpy
