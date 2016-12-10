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
#if defined(__GNUC__) || defined(__clang__)
#include <cxxabi.h>
#endif

#include <map>
#include <lumpy/test.h>

namespace lumpy
{

namespace test
{

struct ClassInfo
{
    const std::type_info*   type;
    FNew*      pnew;
    FDel*      pdel;

    cstring name() const { return cxxabi::demangle(*type); }
};

struct MethodInfo {
    Ftest*      func;
    cstring     name;
    cstring     file;
    int         line;

    enum Status {
        RUN,
        OK,
        FAILED,
    };

    bool invoke(void* obj, const std::type_info* type) const {
        show(RUN, type);

        try {
            func(obj);
            show(OK, type);
            return true;
        }
        catch (IException& e) {
            static StringBuffer buffer;
            buffer.clear();
            e.sformat(buffer, {});

            ::printf("\033[33m[    throw]\033[0;0m  throw (%s)\n", cxxabi::demangle(typeid(e)));
            ::printf("\033[33m[    !!!!!]\033[0;0m    { %.*s }\n", uint(buffer.size()), buffer.data());
        }
        catch (...) {
            ::printf("\033[33m[     throw]\033[0m  throw (%s)\n", "unknow");
        }
        show(FAILED, type);
        return false;
    }

private:
    void show(Status stat, const std::type_info* type) const {
        static const char* titles[] = {
            "\033[1;36m[RUN      ]\033[0;0m",
            "\033[1;32m[    OK   ]\033[0;0m",
            "\033[1;31m[    FAIL ]\033[0;0m"
        };

        const auto type_name = cxxabi::demangle(*type);
        const auto type_len  = std::strlen(type_name) - sizeof("test");
        const auto func_name = name;
        ::printf("%s %.*s::%s\n", titles[stat], int(type_len), type_name, func_name);;
    }
};

static auto& g_classes() { static std::map<string, ClassInfo> result; return result;}
static auto& g_methods() { static std::map<const std::type_info*, std::vector<MethodInfo>> result; return result;}

int _Unittest::install(const std::type_info& type, FNew* pnew, FDel* pdel) {
    auto class_info = g_classes().find(type.name());
    if (class_info != g_classes().end()) return -1;

    g_classes()[type.name()] = { &type, pnew, pdel };
    return 0;
}

int _Unittest::install(const std::type_info& type, Ftest* func, const char* name, const char* file, int line) {
    auto& class_info = g_methods()[&type];

    class_info.push_back({ func, name, file, line });
    return 0;
}

static bool str_match(const char* expect, const char* str) {
    if (str == nullptr) return true;

    const auto length = ::strlen(str);
    const auto result = ::strncmp(expect, str, str[length - 1] == '*' ? length - 1 : length) == 0;
    return result;
}

int _Unittest::invoke(const char* type, const char* name) {
    auto count = 0;

    for (auto class_itr = g_classes().begin(); class_itr != g_classes().end(); ++class_itr) {
        if (!str_match(class_itr->second.name(), type)) continue;

        auto object = class_itr->second.pnew(); {
            auto& methods = g_methods()[class_itr->second.type];
            for (auto& method : methods) {
                if (!str_match(method.name, name)) continue;
                method.invoke(object, class_itr->second.type);
                ++count;
            }
        }
        class_itr->second.pdel(object);
    }

    return count!=0;
}

int invokeUnittest(const char* type, const char* name) {
    return test::_Unittest::invoke(type, name);
}

}

}
