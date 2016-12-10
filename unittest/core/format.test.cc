
#include <lumpy/math.h>
#include <lumpy/test.h>

namespace lumpy
{

inline namespace core
{

lumpy_unit(format)
{
    lumpy_test(format_int)
    {
        writef("short  = [{}]\n", short (-1234));
        writef("ushort = [{}]\n", ushort(+1234));
        writef("int    = [{}]\n", int   (-1234));
        writef("uint   = [{}]\n", uint  (+1234));
        writef("long   = [{}]\n", long  (-1234));
        writef("ulong  = [{}]\n", ulong (+1234));
        writef("llong  = [{}]\n", llong (-1234));
        writef("ullong = [{}]\n", ullong(+1234));
    }

    lumpy_test(format_width) {
        writef("<: [{:<4}]    [{:<8}]\n",  +3, +3);
        writef(">: [{:>4}]    [{:>8}]\n",  -3, -3);
        writef("=: [{:=4}]    [{:=8}]\n",  +3, +3);
        writef("^: [{:^4}]    [{:^8}]\n",  -3, -3);
    }
};

}

}
