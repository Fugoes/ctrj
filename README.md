# CTRJ

(C)ompile (T)ime (R)apid(J)SON.

Many people out there have written compile time JSON libraries and bloat their own JSON parsers. This is the compile time JSON library which does not bloat yet another one.

## Overview

- Header only.
- Provide reader API & writer API based on RapidJSON's writer API & SAX reader API.
- High performance. See benchmark session below.
- Type safe. Access to JSON object's properties are type-checked at compile time.
- Allocation free parser. When using the reader API, no allocation happens besides memory needed by the JSON object.

## Usage

### Installation

Just copy the `ctrj/` folder to your project.

### Defining Schema

Here is an example from `test/test_citm_catalogs.cpp`:

```C++
#include "ctrj/schema.hpp"

namespace {
const char _areaNames[] = "areaNames";
const char _audienceSubCategoryNames[] = "audienceSubCategoryNames";
// other keys ...
}

using js_schema = ctrj::obj<
    ctrj::fld<_areaNames, ctrj::dyn_obj<ctrj::str>>,
    ctrj::fld<_audienceSubCategoryNames, ctrj::dyn_obj<ctrj::str>>,
    ctrj::fld<_blockNames, ctrj::dyn_obj<ctrj::str>>,
    ctrj::fld<_events, ctrj::dyn_obj<ctrj::obj<
        ctrj::fld<_description, ctrj::nul<ctrj::str>>,
        ctrj::fld<_id, ctrj::u64>,
        ctrj::fld<_subTopicIds, ctrj::arr<ctrj::u64>>,
        ctrj::fld<_subjectCode, ctrj::nul<ctrj::u64>>,
        ctrj::fld<_subtitle, ctrj::nul<ctrj::str>>,
        ctrj::fld<_topicIds, ctrj::arr<ctrj::u64>>
    >>>,
    ctrj::fld<_venueNames, ctrj::dyn_obj<ctrj::str>>
>;
```

All supported schema elements are:

- Primitive types:
  - `ctrj::u64` for 64-bit unsigned integer,
  - `ctrj::i64` for 64-bit signed integer,
  - `ctrj::f64` for double,
  - `ctrj::str` for string,
  - `ctrj::bol` for boolean.
- Compound types (all type parameters could be another compound type):
  - `ctrj::arr<T>` for array of type `T`,
  - `ctrj::nul<T>` for nullable type of type `T`,
  - `ctrj::obj<ctrj::fld<K0, T0>, ctrj::fld<K1, T1>, ..., ctrj::fld<Kn, Tn>>` for object with `K0` property of type `T0`, `K1` property of type `T1`, ..., `Kn` property of type `Tn`,
  - `ctrj::dyn_obj<T>` for object with dynamic keys, and all these keys point to value of type `T`.

### Using JSON Objects

`ctrj::val<T>` represents a JSON object of schema `T`. After you create an instance of `ctrj::val<T>`, your IDE would guide you. JSON types are mapped into STL types.

Here is an example for the above schema:

```C++
#include "ctrj/value.hpp"

// schema definition

int main() {
  ctrj::val<js_schema> js{};

  js.get<_areaNames>().flds["some field"].str = "some value";
  auto &event = js.get<_events>().flds["some event"];
  event.get<_description>().opt.emplace(ctrj::val<ctrj::str>{});
  event.get<_subTopicIds>().vec.push_back({});
  event.get<_subTopicIds>().vec[0].u64 = 0xdeadbeef;
  // ...
}
```

### Reading JSON

Here is an example for the above schema:

```C++
#include "ctrj/reader.hpp"

int main() {
  // ...
  using handler_t = ctrj::reader<js_schema, rapidjson::BaseReaderHandler,
                                 rapidjson::UTF8<>>;
  rapidjson::StringStream ss{content_c_str};
  rapidjson::Reader reader{};
  handler_t handler{js};
  reader.Parse(ss, handler);
  if (reader.HasParseError()) std::cout << "ERROR" << std::endl;
  // ...
}
```

### Writing JSON

Here is an example for the above schema:

```C++
#include "ctrj/writer.hpp"

int main() {
  // ...
  rapidjson::StringBuffer buf{};
  rapidjson::Writer w{buf};
  ctrj::write<js_schema>(js).to(w);
  std::cout << buf.GetString() << std::endl;
  // ...
}
```

## Benchmark

Firstly, build this repo using `cmake`. Use `Release` build type please. To run some benchmark:

```bash
# benchmarking RapidJSON's Document::Parse
./test_bench_rj 1000 path/to/ctrj/data/citm_catalog.json
# benchmarking CTRJ's parse & validate
./test_citm_catalogs 1000 path/to/ctrj/data/citm_catalog.json
```

Usually `CTRJ` achieves about 70%~80% of the throughput of RapidJSON. Please note that, RapidJSON's benchmark does NOT include validation of schema.