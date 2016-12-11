
#include <lumpy/core.h>
#include <lumpy/ptree.h>

using namespace lumpy;
struct MyClass
{
    double  dval;
    int     ints[3];

    auto ptree() {
        return toPTree(
            $["dval"]=dval,
            $["ints"]=ints);
    }
};

struct MyClass2
{
    MyClass a;
    MyClass b;

    auto ptree() { 
        return toPTree(
            $["a"]=a,
            $["b"]=b);
    }
};

int main(int argc, char *argv[]) {
    MyClass2 obj = {
        { 12.34, { 1, 2, 3}},
        { 33.44, { 5, 6, 7}}
    };

    // struct -> json
    auto json = json::serialize(obj);

    // json   -> string
    writef("json = {}\n", json);

    // json   -> struct
    MyClass2 tmp;
    json::deserialize(json, tmp);

    return 0;
}
