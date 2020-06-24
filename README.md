CTRJ
====

Still under construction. Features are incomplete now.

(C)ompile (T)ime (R)apid(J)SON.

Many people out there have written compile time JSON libraries and bloat their own JSON parsers. This is the compile time JSON library which does not bloat yet another one. It is based on the RapidJSON.

## Defining Schema

```C++
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
```

Yes, it supports nesting JSON objects.

## Parsing

```C++
#include "ctrj.hpp"

// js_obj_schema definition

int main() {
    // ...

    ctrj::value<js_obj_schema> js_obj{};
    ctrj::handler<js_obj_schema> handler{js_obj};
    rapidjson::Reader reader{};
    rapidjson::StringStream ss{
        R"( { "id": 1, "content": { "id": 2, "text": "Hello World" } } )"
    };
    reader.Parse(ss, handler);
    assert(!reader.HasParseError());
    std::cout << js_obj.get<_id>().uint64 << std::endl;
    std::cout << js_obj.get<_content>().get<_id>().uint64 << std::endl;
    std::cout << js_obj.get<_content>().get<_text>().string << std::endl;

    // ...
}
```

