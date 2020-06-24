#include <iostream>
#include "include/ctrj.hpp"

namespace x = ctrj::ctrj_detail;

namespace {
const char _id[] = "id";
const char _text[] = "text";
}

using js_obj_schema = x::object<
    x::field<_id, uint64_t>,
    x::field<_text, x::object<
        x::field<_id, uint64_t>,
        x::field<_text, uint64_t>
    >>
>;

int main() {
  x::value<js_obj_schema> js_obj{};
  x::complete_handler<js_obj_schema> handler{};
  std::cout << sizeof(handler) << std::endl;
  x::handler_base *h{&handler};

  bool flag;
  x::handler_base *t{nullptr};

  if (!h->StartObject(&h)) return 0;
  if (!h->Key("id", &h)) return 0;
  if (!h->Number("123", &h)) return 0;
  if (!h->Key("text", &h)) return 0;
  if (!h->StartObject(&h)) return 0;
  if (!h->Key("text", &h)) return 0;
  if (!h->Number("128", &h)) return 0;
  if (!h->Key("id", &h)) return 0;
  if (!h->Number("128", &h)) return 0;
  if (!h->EndObject(&h)) return 0;

  std::cout << "SUCC" << std::endl;

  return 0;
}
