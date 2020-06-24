#include <iostream>

#include "include/ctrj.hpp"

namespace {
const char _id[] = "id";
const char _text[] = "text";
}

using js_obj_schema = ctrj::object<
    ctrj::field<_id, uint64_t>,
    ctrj::field<_text, ctrj::object<
        ctrj::field<_id, uint64_t>,
        ctrj::field<_text, uint64_t>
    >>
>;

int main() {

  {
    ctrj::value<js_obj_schema> js_obj{};
    ctrj::handler<js_obj_schema> handler{js_obj};
    rapidjson::Reader reader{};
    rapidjson::StringStream ss{
        R"( { "id": 1, "text": { "id": 2, "text": 3} } )"
    };
    reader.Parse<rapidjson::kParseNumbersAsStringsFlag>(ss, handler);
    assert(!reader.HasParseError());
    std::cout << js_obj.get<_id>().uint64 << std::endl;
    std::cout << js_obj.get<_text>().get<_id>().uint64 << std::endl;
    std::cout << js_obj.get<_text>().get<_text>().uint64 << std::endl;
  }

  {
    ctrj::value<js_obj_schema> js_obj{};
    ctrj::handler<js_obj_schema> handler{js_obj};
    rapidjson::Reader reader{};
    rapidjson::StringStream ss{
        R"( { "id": 1, "txt": { "id": 2, "text": 3} } )"
    };
    reader.Parse<rapidjson::kParseNumbersAsStringsFlag>(ss, handler);
    assert(reader.HasParseError());
  }

  return 0;
}
