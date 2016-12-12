#include <lumpy/io.h>
#include <lumpy/ptree/json.h>
#include <lumpy/test.h>

namespace lumpy
{

namespace ptree
{

struct Inter
{
    double  dval;
    int     ints[3];
    string  strs[2];
    auto ptree() { return ($["dval"]=dval, $["ints"]=ints, $["strs"]=strs); }
};

struct Outer
{
    string  s;
    Inter   a;
    Inter   b;
    auto ptree() { return ($["s"]=s, $["a"]=a, $["b"]=b); }
};

lumpy_unit(ptree) {
    lumpy_test(test) {
        const char str[] = R"({
        "s": "~=>\taa\n~=>\tbb",
        "a": {
            "dval": 12.34,
            "ints": [ 1, 2, 3],
            "strs": ["hello", "world"]
        },
        "b": {
            "dval": 33.44,
            "ints": [ 5, 6, 7]
        }
        })";

        Outer obj;
        JTree json(str, sizeof(str)-1);
        deserialize(json, obj);
        auto dom = json::serialize(obj);
        writef("json = {}\n", dom);
    }
};

}

}
