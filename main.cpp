#include <iostream>

#include "include/ctrj.hpp"

namespace {
const char _id[] = "id";
const char _uid[] = "uid";
const char _iid[] = "iid";
const char _xid[] = "xid";
const char _content[] = "content";
const char _text[] = "text";
}

using js_obj_schema = ctrj::object<
    ctrj::field<_id, uint64_t>,
    ctrj::field<_content, ctrj::object<
        ctrj::field<_id, uint64_t>,
        ctrj::field<_uid, unsigned>,
        ctrj::field<_xid, int64_t>,
        ctrj::field<_iid, int>,
        ctrj::field<_text, std::string>
    >>
>;

int main() {

  {
    ctrj::value<js_obj_schema> js_obj{};
    ctrj::handler<js_obj_schema> handler{js_obj};
    rapidjson::Reader reader{};
    rapidjson::StringStream ss{
        R"(
          {
            "id": 1,
            "content": {
              "id": 12,
              "text": "Hello World",
              "uid": 10,
              "xid": 9223372036854775807,
              "iid": 10
            }
          }
        )"
    };
    reader.Parse(ss, handler);
    if (reader.HasParseError()) {
      std::cout << "ERROR" << std::endl;
    }
    std::cout << reader.GetParseErrorCode() << std::endl;
    std::cout << reader.HasParseError() << std::endl;
    std::cout << js_obj.get<_id>().u64 << std::endl;
    std::cout << js_obj.get<_content>().get<_id>().u64 << std::endl;
    std::cout << js_obj.get<_content>().get<_uid>().u << std::endl;
    std::cout << js_obj.get<_content>().get<_text>().str << std::endl;
    std::cout << js_obj.get<_content>().get<_xid>().i64 << std::endl;
    std::cout << js_obj.get<_content>().get<_iid>().i << std::endl;
  }

  return 0;
}
