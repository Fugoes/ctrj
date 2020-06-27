#include <iostream>

#include "../ctrj/value.hpp"
#include "../ctrj/reader.hpp"
#include "../ctrj/writer.hpp"

#include "rapidjson/reader.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

static const char _x[] = "x";
static const char _y[] = "y";
static const char _z[] = "z";

using js_schema = ctrj::obj<
    ctrj::fld<_x, ctrj::nul<ctrj::u64>>,
    ctrj::fld<_y, ctrj::obj<ctrj::fld<_x, ctrj::u64>>>,
    ctrj::fld<_z, ctrj::obj<
        ctrj::fld<_y, ctrj::u64>,
        ctrj::fld<_x, ctrj::arr<ctrj::u64>>
    >>
>;

int main() {
  ctrj::val<js_schema> js{};

  rapidjson::StringStream
      ss{R"({"x":null,"y":{"x":12},"z":{"x":[1,2,3,{"y":1}],"y":11}}})"};
  rapidjson::Reader reader{};
  ctrj::reader<js_schema, rapidjson::BaseReaderHandler, rapidjson::UTF8<>>
      handler{js};
  reader.Parse(ss, handler);
  if (reader.HasParseError())
    std::cout << "ERROR" << std::endl;

  std::cout << ctrj::detail::reader_handler<js_schema>::stk_size << std::endl;

  rapidjson::StringBuffer buf{};
  rapidjson::Writer w{buf};
  ctrj::write<js_schema>(js).to(w);

  std::cout << buf.GetString() << std::endl;
  return 0;
}
