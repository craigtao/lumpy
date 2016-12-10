
#include <lumpy/ptree.h>

using namespace lumpy;
struct MyClass
{
    double  dval    = 33.44;
    int     ints[3] = {1, 2, 3};

    auto ptree() {
        return toPTree($["dval"]=dval,$["ints"]=ints);
    }
};

struct MyClass2
{
    MyClass a;
    MyClass b;

    auto ptree() {
        return toPTree($["a"]=a, $["b"]=b);
    }
};

int main(int argc, char *argv[]) {
    MyClass2 obj;
    auto s = json::serialize(obj);
    writef("json = {}\n", s.value());
    return 0;
}
