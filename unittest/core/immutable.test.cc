#include <lumpy/test.h>

namespace lumpy
{

inline namespace core
{

lumpy_unit(immutable)
{
    lumpy_test(create) {
        auto a = (!TIdx<4>{})();

        test::expect(0) == a[0];
        test::expect(1) == a[1];
        test::expect(2) == a[2];
        test::expect(3) == a[3];
    }

    lumpy_test(binary_op) {
        auto a = (!TIdx<4>{})();
        auto b = (!TIdx<4>{})();

        auto add_val = a+b;     test::expect(add_val[2]) == 2+2;
        auto mul_val = a*b;     test::expect(mul_val[2]) == 2*2;
    }

    lumpy_test(reduce) {
        auto a = (!TIdx<4>{})();

        auto sum_val    = sum(a);
        test::expect(sum_val) == 6;
    }
};

}

}
