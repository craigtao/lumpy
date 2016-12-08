#include <lumpy/test.h>

namespace lumpy
{

inline namespace core
{

lumpy_unit(vec)
{
    lumpy_test(useable)
    {
        int2 a = { 1, 2 };
    }
};

}

}
