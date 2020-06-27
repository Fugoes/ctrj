#include <chrono>
#include <fstream>
#include <iostream>

#include "../ctrj/reader.hpp"
#include "../ctrj/value.hpp"

#include "rapidjson/reader.h"
#include "rapidjson/stringbuffer.h"

namespace {

const char __id[] = "_id";
const char _index[] = "index";
const char _guid[] = "guid";
const char _isActive[] = "isActive";
const char _balance[] = "balance";
const char _picture[] = "picture";
const char _age[] = "age";
const char _eyeColor[] = "eyeColor";
const char _name[] = "name";
const char _gender[] = "gender";
const char _company[] = "company";
const char _email[] = "email";
const char _phone[] = "phone";
const char _address[] = "address";
const char _about[] = "about";
const char _registered[] = "registered";
const char _latitude[] = "latitude";
const char _longitude[] = "longitude";
const char _tags[] = "tags";
const char _friends[] = "friends";
const char _id[] = "id";
const char _greeting[] = "greeting";
const char _favoriteFruit[] = "favoriteFruit";

} // namespace

int main(int argc, char *argv[]) {
  using js_schema = ctrj::arr<ctrj::obj<
      ctrj::fld<__id, ctrj::str>, ctrj::fld<_index, ctrj::u64>,
      ctrj::fld<_guid, ctrj::str>, ctrj::fld<_isActive, ctrj::bol>,
      ctrj::fld<_balance, ctrj::str>, ctrj::fld<_picture, ctrj::str>,
      ctrj::fld<_age, ctrj::u64>, ctrj::fld<_eyeColor, ctrj::str>,
      ctrj::fld<_name, ctrj::str>, ctrj::fld<_gender, ctrj::str>,
      ctrj::fld<_company, ctrj::str>, ctrj::fld<_email, ctrj::str>,
      ctrj::fld<_phone, ctrj::str>, ctrj::fld<_address, ctrj::str>,
      ctrj::fld<_about, ctrj::str>, ctrj::fld<_registered, ctrj::str>,
      ctrj::fld<_latitude, ctrj::f64>, ctrj::fld<_longitude, ctrj::f64>,
      ctrj::fld<_tags, ctrj::arr<ctrj::str>>,
      ctrj::fld<_friends, ctrj::arr<ctrj::obj<ctrj::fld<_id, ctrj::u64>,
                                              ctrj::fld<_name, ctrj::str>>>>,
      ctrj::fld<_greeting, ctrj::str>, ctrj::fld<_favoriteFruit, ctrj::str>>>;

  int N = std::stoi(argv[1]);

  std::ifstream is{std::string(argv[2])};
  std::string content{};
  content.assign(std::istreambuf_iterator<char>(is),
                 std::istreambuf_iterator<char>());
  auto content_c_str = content.c_str();
  ctrj::val<js_schema> js{};

  auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < N; i++) {
    rapidjson::StringStream ss{content_c_str};
    rapidjson::Reader reader{};
    ctrj::reader<js_schema, rapidjson::BaseReaderHandler, rapidjson::UTF8<>>
        handler{js};
    reader.Parse(ss, handler);
    if (reader.HasParseError())
      std::cout << "ERROR" << std::endl;
  }
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end - start;
  std::cout << elapsed.count() / N << " s/OP" << std::endl;
  std::cout << (double) (content.length() * N) / elapsed.count() / 1024 / 1024
            << " MiB/s" << std::endl;

  return 0;
}