#include <iostream>
#include "include/ctrj.hpp"

namespace x = ctrj::ctrj_detail;

namespace {
const char _id[] = "id";
const char _text[] = "text";
}

using js_obj_schema = x::object<
    x::field<_id, uint64_t>,
    x::field<
        _text,
        x::object<
            x::field<_id, uint64_t>,
            x::field<_text, uint64_t>
        >
    >
>;

int main() {
  x::value<js_obj_schema> js_obj{};
  js_obj.get<_id>().uint64 = 2;
  std::cout << js_obj.get<_text>().get<_id>().uint64 << std::endl;
  return 0;
}
