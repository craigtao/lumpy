
#if defined(__GNUC__) || defined(__clang__)
#   include <cxxabi.h>
#endif

#include <cstdio>
#include <typeinfo>

namespace lumpy
{

namespace cxxabi
{

const char* demangle(const char* raw_name) {
#ifdef _MSC_VER
    if (raw_name[0]=='s' && raw_name[1]=='t' && raw_name[2]=='r' && raw_name[3]=='u' && raw_name[4]=='s' && raw_name[5] == 't' && raw_name[6] == ' ')    return raw_name+6;
    if (raw_name[0]=='c' && raw_name[1]=='l' && raw_name[2]=='a' && raw_name[3]=='s' && raw_name[4]=='s' && raw_name[5] == ' ')                          return raw_name+5;
    return raw_name;
#else
    static char name[256];
    size_t      size    = sizeof(name);
    int         status  = 0;
    __cxxabiv1::__cxa_demangle(raw_name, name, &size, &status);
    if (status==0) return name;
    return "<bad type>";
#endif
}


const char* demangle(const std::type_info& info) {
    return demangle(info.name());
}

}

}
