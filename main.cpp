#include <iostream>

#include "include/ctrj.hpp"

namespace {
const char _id[] = "id";
const char _content[] = "content";
const char _text[] = "text";
}

using js_obj_schema = ctrj::object<
    ctrj::field<_id, uint64_t>,
    ctrj::field<_content, ctrj::object<
        ctrj::field<_id, uint64_t>,
        ctrj::field<_text, std::string>
    >>
>;

int main() {

  {
    ctrj::value<js_obj_schema> js_obj{};
    ctrj::handler<js_obj_schema> handler{js_obj};
    rapidjson::Reader reader{};
    rapidjson::StringStream ss{
        R"( { "id": 1, "content": { "id": 2, "text": "Hello World" } } )"
    };
    reader.Parse<rapidjson::kParseNumbersAsStringsFlag>(ss, handler);
    assert(!reader.HasParseError());
    std::cout << js_obj.get<_id>().uint64 << std::endl;
    std::cout << js_obj.get<_content>().get<_id>().uint64 << std::endl;
    std::cout << js_obj.get<_content>().get<_text>().string << std::endl;
  }

  {
    ctrj::value<js_obj_schema> js_obj{};
    ctrj::handler<js_obj_schema> handler{js_obj};
    rapidjson::Reader reader{};
    rapidjson::StringStream ss{
        R"( { "id": 1, "content": { "id": 2, "text": 2 } } )"
    };
    reader.Parse<rapidjson::kParseNumbersAsStringsFlag>(ss, handler);
    assert(reader.HasParseError());
  }

  return 0;
}
