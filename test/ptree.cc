
#include <lumpy/core.h>
#include <lumpy/ptree.h>

using namespace lumpy;
struct MyClass
{
    double  dval;
    int     ints[3];
    auto ptree() { return toPTree($["dval"]=dval,$["ints"]=ints);}
};

struct MyClass2
{
    MyClass a;
    MyClass b;
    auto ptree() { return toPTree($["a"]=a,$["b"]=b);}
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

    MyClass2 obj;

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
