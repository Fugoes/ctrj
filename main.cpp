#include <iostream>

#include "include/ctrj.hpp"

namespace {
const char _id[] = "id";
const char _uid[] = "uid";
const char _iid[] = "iid";
const char _xid[] = "xid";
const char _content[] = "content";
const char _text[] = "text";
const char _next[] = "next";
}

using js_obj_schema = ctrj::object<
    ctrj::field<_id, ctrj::nullable<uint64_t>>,
    ctrj::field<_content, ctrj::nullable<ctrj::object<
        ctrj::field<_id, uint64_t>,
        ctrj::field<_uid, unsigned>,
        ctrj::field<_xid, int64_t>,
        ctrj::field<_iid, int>,
        ctrj::field<_text, std::string>
    >>>
>;

using x_schema =
ctrj::nullable<
    ctrj::object<
        ctrj::field<_id, uint64_t>,
        ctrj::field<
            _next,
            ctrj::nullable<ctrj::object<ctrj::field<_id, uint64_t>>>
        >,
        ctrj::field<_text, std::string>
    >
>;

int main() {
  {
    ctrj::value<x_schema> x{};
    ctrj::handler<x_schema> handler{x};
    rapidjson::Reader reader{};
    rapidjson::StringStream ss{
        R"( { "id": 1, "next": { "id": 2 }, "text": "abc" } )"
    };
    reader.Parse(ss, handler);
    if (reader.HasParseError()) {
      std::cout << "ERROR" << std::endl;
    } else {
      rapidjson::StringBuffer buf{};
      rapidjson::Writer w{buf};
      ctrj::write<x_schema>::to(x, w);
      std::cout << buf.GetString() << std::endl;
    }
  }

  {
    ctrj::value<js_obj_schema> js_obj{};
    ctrj::handler<js_obj_schema> handler{js_obj};

    rapidjson::Reader reader{};
    rapidjson::StringStream ss{
        R"(
          {
            "id": null,
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
    } else {
      rapidjson::StringBuffer buf{};
      rapidjson::Writer w{buf};
      ctrj::write<js_obj_schema>::to(js_obj, w);
      std::cout << buf.GetString() << std::endl;
    }
  }

  return 0;
}
