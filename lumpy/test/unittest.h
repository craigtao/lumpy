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

#include <lumpy/core.h>

namespace lumpy
{

namespace test
{

typedef void*(FNew )();
typedef void (FDel )(void*);
typedef void (Ftest)(void*);

class _Unittest
{
public:
    static int invoke (const char* type, const char* name=nullptr);

protected:
    static int install(const std::type_info& type, FNew*  pnew, FDel* pdel);
    static int install(const std::type_info& type, Ftest* func, const char*  name, const char* file, int line);
};


template<class T>
class IUnittest: public _Unittest
{
public:
    using base = _Unittest;

    static int static_init() {
        static int value =  base::install(typeid(T), &new_, &del_);
        return value;
    }

protected:
    static T* cast(void* ptr) {
        return reinterpret_cast<T*>(ptr);
    }

    static auto install(Ftest func, const char* name, const char* file, int line) {
        return base::install(typeid(T), func, name, file, line);
    }

private:
    static void* new_()         {  return new T();                          }
    static void  del_(void* ptr){ return delete reinterpret_cast<T*>(ptr);  }
};

int invokeUnittest(const char* type = nullptr, const char* name = nullptr);

}

}

#define lumpy_unit(name)                                                                      \
    class name##_test;                                                                      \
    static int g_##name##_test_initor = test::IUnittest<name##_test>::static_init();        \
    class name##_test: public test::IUnittest<name##_test>

#define lumpy_test(name)                                                                      \
    static void name##_test(void* obj) { cast(obj)->name();}                                \
    int         name##_test_initor = install(&name##_test, #name, __FILE__, __LINE__);      \
    void name()
