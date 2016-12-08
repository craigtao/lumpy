
#include <lumpy/math.h>
#include <lumpy/test.h>

namespace lumpy
{

inline namespace math
{

lumpy_unit(array)
{
    lumpy_test(useable)
    {
        Array<int,2> arr({8, 8});
        arr = 0;

        auto src  = reshape(seq(16), {4u, 4u});
        arr.slice({ 2u, $-1u }, { 2u, $-1u }) = src;

        writef("arr = {:>-6.3f} \n", arr);
        arr = arr+sin(arr);
        writef("arr = {:>-6.3f} \n", arr);
    }

#ifdef NMS_TEST_PERFORMANCE
    lumpy_test(performance)
    {
        Array<float, 3>  arr({ 1024, 1024, 100 });
        Timer t;
        arr += Scalar<float>{1.f};

        auto used = t.duration();
        auto size = arr.numel() * sizeof(float);
        log_info("time use {}sec, {}MB/s", t.duration(), size/used/(1024*1024));
    }
#endif
};

}

}
