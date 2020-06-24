CTRJ
====

Still under construction. Features are incomplete now.

(C)ompile (T)ime (R)apid(J)SON.

Many people out there have written compile time JSON libraries and bloat their own JSON parsers. This is the compile time JSON library which does not bloat yet another one. It is based on the RapidJSON.

## Defining Schema

```C++
using js_obj_schema = ctrj::object<
    ctrj::field<_id, uint64_t>,
    ctrj::field<_text, ctrj::object<
        ctrj::field<_id, uint64_t>,
        ctrj::field<_text, uint64_t>
    >>
>;
```

Yes, it supports nesting JSON objects.

## Parsing

```C++
#include "ctrj.hpp"

namespace {
const char _id[] = "id";
const char _text[] = "text";
}

// js_obj_schema definition

int main() {
    // ...

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

    // ...
}
```

