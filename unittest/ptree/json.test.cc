#include <lumpy/io.h>
#include <lumpy/ptree/json.h>
#include <lumpy/test.h>

namespace lumpy
{

namespace ptree
{

lumpy_unit(json) {
    lumpy_test(parse_ok) {

        StringBuffer path;
        for (auto i = 1; i <= 3; ++i) {
            path.clear();
            path.formats("data/json/pass{}.json", i);

            auto text = readAll(path.c_str());

            try {
                JDoc json(text.c_str(), text.size());
                auto& value = json.value();
                log_trace("json{:2s} = {}\n", i, value);
            }
            catch (ptree::EJParseFailed& e) {
                log_error("{:2s} not pass", i);
                log_error << e;
                throw e;
            }
        }
        return;
    }

    lumpy_test(parse_fail) {
        StringBuffer path;

        auto fail_count = 0;
        for (auto i = 1; i <= 33; ++i) {
            path.clear();
            path.formats("data/json/fail{}.json", i);

            try {
                auto text = readAll(path.c_str());
                JDoc json(text.c_str(), text.size());
                log_error("json{:2s} = {}\n", i, json.value());
            }
            catch (ptree::EJParseFailed&) {
                ++fail_count;
                log_debug("{:2s} pase fail", i);
            }
        }
    }

#ifdef NMS_TEST_PERFORMANCE
    lumpy_test(performance) {
        File twitter("data/json/twitter.json", File::Read);
        const auto size = twitter.size();
        auto str = new char[size+1];
        twitter.read(str, size);
        str[size] = '\0';

        for (int i = 0; i < 2; ++i) {
            Timer t;
            JDoc json(str, size);
            printf("lumpycc json use: %f\n", t.duration());
        }
    }
#endif

private:
    static string readAll(string path) {
        string str = "";

        File file(path, File::Read);
        auto size = file.size();
        str.resize(size);
        file.read(&str[0], size);

        return str;
    }
};

}

}
