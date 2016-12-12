#include <lumpy/io.h>
#include <lumpy/ptree/json.h>
#include <lumpy/test.h>

using namespace lumpy;

struct Inter
{
    double                      dval;       // value support
    int                         ints[3];    // array support
    std::vector<std::string>    strs;       // std::vector support

    // innner access
    auto toPTree() { return ($["dval"]=dval, $["ints"]=ints, $["strs"]=strs); }
};

struct Outer
{
    std::string  s;                         // std::string support
    Inter   a;                              // user-defined type support
    Inter   b;
};

// outer access
inline auto toPTree(Outer& o) { return ($["s"]=o.s, $["a"]=o.a, $["b"]=o.b); }

const char json_str[] = R"({
    "s": "~=>\taa\n~=>\tbb",
    "a": {
        "dval": 12.349988,
        "ints": [ 12345, 23456, 34567],
        "strs": ["~~~~~~~~~~hello\t~~~~~~~~~~~", "~~~~~~~~~~~~world\t~~~~~~~~~~"]
    },
    "b": {
        "dval": 33.44,
        "ints": [ 5, 6, 7]
    }
})";

namespace lumpy
{

namespace ptree
{

lumpy_unit(ptree) {
    lumpy_test(test) {
        Outer obj;
        JTree json(json_str, sizeof(json_str)-1);
        deserialize(json, obj);
        auto dom = json::serialize(obj);
        writef("json = {}\n", dom);
    }

#ifdef NDEBUG
    lumpy_test(performance) {
        Timer lumpy;
        for(uint i = 0; i < 1000; ++i) {
            Outer obj;
            JTree json(json_str, sizeof(json_str)-1);
            deserialize(json, obj);
        }
        log_info("lumpy use: {}", lumpy.duration());
    }
#endif

};

}

}
