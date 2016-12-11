
#include <lumpy/core.h>
#include <lumpy/ptree.h>

using namespace lumpy;

struct Inter
{
    double  dval;
    int     ints[3];
    auto ptree() { return ($["dval"]=dval, $["ints"]=ints); }
};

struct Outer
{
    Inter a;
    Inter b;
    auto ptree() { return ($["a"]=a, $["b"]=b); }
};

int main(int argc, char *argv[]) {
    const char str[] = R"({
        "a": {
            "dval": 12.34,
            "ints": [ 1, 2, 3]
        },
        "b": {
            "dval": 33.44,
            "ints": [ 5, 6, 7]
        }
    })";

    Outer obj;

    // str -> json
    JTree json(str, sizeof(str)-1);

    // json -> struct
    deserialize(json, obj);

    // struct -> json
    auto dom = json::serialize(obj);

    // json   -> string
    writef("json = {}\n", dom);

    return 0;
}
