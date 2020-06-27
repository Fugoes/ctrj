#include <chrono>
#include <fstream>
#include <iostream>

#include "rapidjson/document.h"

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
  int N = std::stoi(argv[1]);

  std::ifstream is{std::string(argv[2])};
  std::string content{};
  content.assign(std::istreambuf_iterator<char>(is),
                 std::istreambuf_iterator<char>());
  auto content_c_str = content.c_str();

  auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < N; i++) {
    rapidjson::Document document{};
    document.Parse(content_c_str);
    if (document.HasParseError())
      std::cout << "ERROR" << std::endl;
  }
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end - start;
  std::cout << elapsed.count() / N << " s/OP" << std::endl;
  std::cout << (double) (content.length() * N) / elapsed.count() / 1024 / 1024
            << " MiB/s" << std::endl;

  return 0;
}