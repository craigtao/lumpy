#include <lumpy/test.h>

void setup_env() {
#ifdef _WIN32
    system("chcp 65001");           // set code page utf8
    setlocale(LC_ALL, "C.UTF-8");   // set locale to c.utf-8
#endif
}

int main(int argc, char* argv[]) {
    setup_env();
    try {
        auto unit = argc<1 ? "" : argv[1];
        auto test = argc<2 ? "" : argv[2];
        lumpy::test::invokeUnittest(unit, test);
    }
    catch(lumpy::IException& e)
    {
        lumpy::writef("catch {},{}\n", typeid(e).name(), e);
    }
    catch(...) {
        lumpy::writef("catch unknow exception");
    }
    return 0;
}
