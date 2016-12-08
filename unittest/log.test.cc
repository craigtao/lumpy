#include <lumpy/test.h>

namespace lumpy
{

lumpy_unit(log)
{
    lumpy_test(level)
    {
        log_debug   << "debug ";
        log_info    << "info  "  << 2.f;
        log_warn    << "warn  "  << "3";
        log_alert   << "alert "  << string("4");
        log_error   << "errro ";
        log_fatal   << "fatal ";
    }
};

}
