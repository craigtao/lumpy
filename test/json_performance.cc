
#include <iostream>
#include <lumpy/core.h>
#include <lumpy/io.h>
#include <lumpy/ptree.h>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/ostreamwrapper.h>

using namespace lumpy;

int main(int argc, char *argv[]) {

    File twitter("data/json/twitter.json", File::Read);
    const auto size = twitter.size();
    auto str = new char[size+1];
    twitter.read(str, size);
    str[size] = '\0';


    double lumpyjson_dump = 0;
    double rapidjson_dump = 0;

    // lumpy
    {
        Timer t;
        JTree json(str, size);
        writef("twitter = {}\n", json.value());
        lumpyjson_dump = t.duration();

    }

// rapidjson
    {
        Timer t;
        rapidjson::Document doc;
        doc.Parse(str);
        rapidjson::OStreamWrapper osw(std::cout);
        rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
        doc.Accept(writer);
        rapidjson_dump = t.duration();

    }

    printf("-------------------\n");

    // lumpy
    for (int i = 0; i < 5; ++i) {
        Timer t;
        JTree json(str, size);
        printf("lumpyjson use:  %fms\n", t.duration());
    }

    // rapidjson
    for(int i = 0; i < 5; ++i) {
        Timer t;
        rapidjson::Document doc;
        doc.Parse(str);
        printf("rapidjson use:  %fms\n", t.duration());
    }

    printf("lumpyjson dump: %fms\n", lumpyjson_dump);
    printf("rapidjson dump: %fms\n", rapidjson_dump);
    return 0;
}
