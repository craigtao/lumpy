#include <lumpy/array_list.h>
#include <lumpy/test.h>

namespace lumpy
{

lumpy_unit(array_list)
{

    lumpy_test(useable)
    {
        const uint  size = 1024 * 1024;

        ArrayList<uint> alist;

        for (uint i = 0; i < size; ++i) {
            alist.push_back(i);
        }
    }

#ifdef NMS_TEST_PERFORMANCE
    lumpy_test(performance)
    {
        const uint size = 400 * 1024 * 1024;    // 400M

        ArrayList<uint> alist;

        Timer timer;
        for (uint i = 0; i < size; ++i) {
            alist.push_back(i);
        }
        auto time_used = timer.duration();
        log_info("time used: {:.3}s, {:.3} MB/s", time_used, double(alist.size()*sizeof(uint))/(1024*1024)/time_used);
    }
#endif

};

}
