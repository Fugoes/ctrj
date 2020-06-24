#ifndef CTRJ_HPP
#define CTRJ_HPP

#include <cstdint>
#include <climits>
#include <string>

#include "rapidjson/reader.h"
#include "rapidjson/stringbuffer.h"

namespace ctrj::ctrj_detail {

/* Schema definition */
template<const char *K, class V>
struct field {};

template<class ... FIELDS>
struct object {};

/* Value type */
template<class SCHEMA>
struct value_type_of;

template<>
struct value_type_of<uint64_t> {
  uint64_t u64{};
};

static_assert(sizeof(unsigned) < sizeof(uint64_t));

template<>
struct value_type_of<unsigned> {
  unsigned u{};
};

template<>
struct value_type_of<int64_t> {
  int64_t i64{};
};

static_assert(sizeof(int) < sizeof(int64_t));

template<>
struct value_type_of<int> {
  int i{};
};

template<>
struct value_type_of<std::string> {
  std::string str{};
};

template<>
struct value_type_of<object<>> {
};

template<const char *K, class V, class ... FIELDS>
struct value_type_of<object<field<K, V>, FIELDS ...>> :
    public value_type_of<object<FIELDS ...>> {
  value_type_of<V> value_{};
};

template<const char *K, class SCHEMA>
struct value_type_of_field;

template<const char *K, class V, class ... FIELDS>
struct value_type_of_field<K, object<field<K, V>, FIELDS ...>> {
  typedef value_type_of<object<field<K, V>, FIELDS ...>> type;
  typedef value_type_of<V> rtype;
};

template<const char *K, const char *X, class V, class ... FIELDS>
struct value_type_of_field<K, object<field<X, V>, FIELDS ...>> :
    public value_type_of_field<K, object<FIELDS ...>> {
};

template<class SCHEMA>
struct value;

template<class SCHEMA>
struct lift_value_type;

template<>
struct lift_value_type<value_type_of<uint64_t>> {
  typedef value_type_of<uint64_t> type;
};

template<>
struct lift_value_type<value_type_of<unsigned>> {
  typedef value_type_of<unsigned> type;
};

template<>
struct lift_value_type<value_type_of<int64_t>> {
  typedef value_type_of<int64_t> type;
};

template<>
struct lift_value_type<value_type_of<int>> {
  typedef value_type_of<int> type;
};

template<>
struct lift_value_type<value_type_of<std::string>> {
  typedef value_type_of<std::string> type;
};

template<class ... FIELDS>
struct lift_value_type<value_type_of<object<FIELDS ...>>> {
  typedef value<object<FIELDS ...>> type;
};

template<class SCHEMA>
struct value : public value_type_of<SCHEMA> {
  template<const char *K>
  auto &get() {
    using field_type = value_type_of_field<K, SCHEMA>;
    using rtype = typename lift_value_type<typename field_type::rtype>::type;
    auto &r = static_cast<typename field_type::type &>(*this).value_;
    return static_cast<rtype &>(r);
  }
};

/* Handler */
struct handler_base {
  virtual bool Key(std::string_view str, handler_base **h) = 0;
  virtual bool Int(int num, handler_base **h) = 0;
  virtual bool Uint(unsigned num, handler_base **h) = 0;
  virtual bool Int64(int64_t num, handler_base **h) = 0;
  virtual bool Uint64(uint64_t num, handler_base **h) = 0;
  virtual bool String(std::string_view str, handler_base **h) = 0;
  virtual bool StartObject(handler_base **h) = 0;
  virtual bool EndObject(handler_base **h) = 0;
};

template<class ... FIELDS>
struct partial_handler;

template<class SCHEMA>
struct complete_handler;

template<>
struct partial_handler<> {
  explicit partial_handler(value_type_of<object<>> &) {}

  inline bool Key(std::string_view key, handler_base **h) { return false; }
  inline bool Int(int num, handler_base **h) { return false; }
  inline bool Uint(unsigned num, handler_base **h) { return false; }
  inline bool Int64(int64_t num, handler_base **h) { return false; }
  inline bool Uint64(uint64_t num, handler_base **h) { return false; }
  inline bool String(std::string_view str, handler_base **h) { return false; }
  inline bool EndObject(handler_base **h) { return true; }
};

template<>
struct complete_handler<uint64_t> : public handler_base {
  handler_base *parent_{nullptr};
  value_type_of<uint64_t> &ref_;

  explicit complete_handler(value_type_of<uint64_t> &ref) : ref_(ref) {}

  bool Key(std::string_view str, handler_base **h) override {
    return false;
  }
  bool Int(int num, handler_base **h) {
    return false;
  }
  bool Uint(unsigned num, handler_base **h) {
    *h = parent_;
    ref_.u64 = num;
    return true;
  }
  bool Int64(int64_t num, handler_base **h) {
    return false;
  }
  bool Uint64(uint64_t num, handler_base **h) {
    *h = parent_;
    ref_.u64 = num;
    return true;
  }
  bool String(std::string_view str, handler_base **h) { return false; }
  bool StartObject(handler_base **h) override { return false; }
  bool EndObject(handler_base **h) override { return false; }
};

template<>
struct complete_handler<unsigned> : public handler_base {
  handler_base *parent_{nullptr};
  value_type_of<unsigned> &ref_;

  explicit complete_handler(value_type_of<unsigned> &ref) : ref_(ref) {}

  bool Key(std::string_view str, handler_base **h) override {
    return false;
  }
  bool Int(int num, handler_base **h) {
    return false;
  }
  bool Uint(unsigned num, handler_base **h) {
    *h = parent_;
    ref_.u = num;
    return true;
  }
  bool Int64(int64_t num, handler_base **h) {
    return false;
  }
  bool Uint64(uint64_t num, handler_base **h) {
    return false;
  }
  bool String(std::string_view str, handler_base **h) { return false; }
  bool StartObject(handler_base **h) override { return false; }
  bool EndObject(handler_base **h) override { return false; }
};

template<>
struct complete_handler<int64_t> : public handler_base {
  handler_base *parent_{nullptr};
  value_type_of<int64_t> &ref_;

  explicit complete_handler(value_type_of<int64_t> &ref) : ref_(ref) {}

  bool Key(std::string_view str, handler_base **h) override {
    return false;
  }
  bool Int(int num, handler_base **h) {
    *h = parent_;
    ref_.i64 = num;
    return true;
  }
  bool Uint(unsigned num, handler_base **h) {
    *h = parent_;
    ref_.i64 = num;
    return true;
  }
  bool Int64(int64_t num, handler_base **h) {
    *h = parent_;
    ref_.i64 = num;
    return true;
  }
  bool Uint64(uint64_t num, handler_base **h) {
    if (num <= INT64_MAX) {
      *h = parent_;
      ref_.i64 = num;
      return true;
    } else {
      return false;
    }
  }
  bool String(std::string_view str, handler_base **h) { return false; }
  bool StartObject(handler_base **h) override { return false; }
  bool EndObject(handler_base **h) override { return false; }
};

template<>
struct complete_handler<int> : public handler_base {
  handler_base *parent_{nullptr};
  value_type_of<int> &ref_;

  explicit complete_handler(value_type_of<int> &ref) : ref_(ref) {}

  bool Key(std::string_view str, handler_base **h) override {
    return false;
  }
  bool Int(int num, handler_base **h) {
    *h = parent_;
    ref_.i = num;
    return true;
  }
  bool Uint(unsigned num, handler_base **h) {
    if (num <= INT_MAX) {
      *h = parent_;
      ref_.i = num;
      return true;
    } else {
      return false;
    }
  }
  bool Int64(int64_t num, handler_base **h) {
    return false;
  }
  bool Uint64(uint64_t num, handler_base **h) {
    return false;
  }
  bool String(std::string_view str, handler_base **h) { return false; }
  bool StartObject(handler_base **h) override { return false; }
  bool EndObject(handler_base **h) override { return false; }
};

template<>
struct complete_handler<std::string> : public handler_base {
  handler_base *parent_{nullptr};
  value_type_of<std::string> &ref_;

  explicit complete_handler(value_type_of<std::string> &ref) : ref_(ref) {}

  bool Key(std::string_view str, handler_base **h) override {
    return false;
  }
  bool Int(int num, handler_base **h) override { return false; }
  bool Uint(unsigned num, handler_base **h) override { return false; }
  bool Int64(int64_t num, handler_base **h) override { return false; }
  bool Uint64(uint64_t num, handler_base **h) override { return false; }
  bool String(std::string_view str, handler_base **h) override {
    *h = parent_;
    ref_.str = str;
    return true;
  }
  bool StartObject(handler_base **h) override { return false; }
  bool EndObject(handler_base **h) override { return false; }
};

template<class ... FIELDS>
struct complete_handler<object<FIELDS ...>> : public handler_base {
  handler_base *parent_{nullptr};
  uint8_t state_{0};
  partial_handler<FIELDS ...> inner_;

  explicit complete_handler(value_type_of<object<FIELDS ...>> &ref)
      : inner_(ref) {}

  bool Key(std::string_view str, handler_base **h) override {
    return inner_.Key(str, h);
  }
  bool Int(int num, handler_base **h) override {
    return inner_.Int(num, h);
  }
  bool Uint(unsigned num, handler_base **h) override {
    return inner_.Uint(num, h);
  }
  bool Int64(int64_t num, handler_base **h) override {
    return inner_.Int64(num, h);
  }
  bool Uint64(uint64_t num, handler_base **h) override {
    return inner_.Uint64(num, h);
  }
  bool String(std::string_view str, handler_base **h) override {
    return inner_.String(str, h);
  }
  bool StartObject(handler_base **h) override {
    if (state_ == 0) {
      state_ = 1;
      return true;
    } else {
      return false;
    }
  }
  bool EndObject(handler_base **h) override {
    if (state_ == 1) {
      if (inner_.EndObject(h)) {
        *h = parent_;
        return true;
      } else {
        return false;
      }
    } else {
      return false;
    }
  }
};

template<const char *K, class V, class ... FIELDS>
struct partial_handler<field<K, V>, FIELDS ...> :
    public partial_handler<FIELDS ...> {
  uint8_t state_{0};
  complete_handler<V> sub_handler_;

  partial_handler(value_type_of<object<field<K, V>, FIELDS ...>> &ref)
      : partial_handler<FIELDS ...>
            (static_cast<value_type_of<object<FIELDS ...>> &>(ref)),
        sub_handler_(ref.value_) {
  }

  inline bool Key(std::string_view key, handler_base **h) {
    if (state_ == 0) {
      if (key == K) {
        state_ = 1;
        sub_handler_.parent_ = *h;
        *h = &sub_handler_;
        return true;
      } else {
        return partial_handler<FIELDS ...>::Key(key, h);
      }
    } else {
      return partial_handler<FIELDS ...>::Key(key, h);
    }
  }
  inline bool Int(int num, handler_base **h) { return false; }
  inline bool Uint(unsigned num, handler_base **h) { return false; }
  inline bool Int64(int64_t num, handler_base **h) { return false; }
  inline bool Uint64(uint64_t num, handler_base **h) { return false; }
  inline bool String(std::string_view str, handler_base **h) { return false; }
  inline bool EndObject(handler_base **h) {
    return state_ == 1 && partial_handler<FIELDS ...>::EndObject(h);
  }
};
template<class SCHEMA>
struct handler :
    public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, handler<SCHEMA>> {
  handler_base *h_{nullptr};
  complete_handler<SCHEMA> inner_;

  explicit handler(value<SCHEMA> &value) :
      inner_(static_cast<value_type_of<SCHEMA> &>(value)) {
    h_ = &inner_;
  }

  bool Null() {
    return false;
  }
  bool Bool(bool b) {
    return false;
  }
  bool Double(double d) {
    return false;
  }
  bool StartArray() {
    return false;
  }
  bool EndArray(rapidjson::SizeType elementCount) {
    return false;
  }
  bool RawNumber(const char *str, rapidjson::SizeType length, bool copy) {
    return false;
  }

  bool Key(const char *str, rapidjson::SizeType length, bool copy) {
    return h_->Key(std::string_view(str, length), &h_);
  }
  bool Int(int i) {
    return h_->Int(i, &h_);
  }
  bool Uint(unsigned i) {
    return h_->Uint(i, &h_);
  }
  bool Int64(int64_t i) {
    return h_->Int64(i, &h_);
  }
  bool Uint64(uint64_t i) {
    return h_->Uint64(i, &h_);
  }
  bool String(const char *str, rapidjson::SizeType length, bool copy) {
    return h_->String(std::string_view(str, length), &h_);
  }
  bool StartObject() {
    return h_->StartObject(&h_);
  }
  bool EndObject(rapidjson::SizeType memberCount) {
    return h_->EndObject(&h_);
  }
};

}

namespace ctrj {

template<const char *K, class V>
using field = ctrj_detail::field<K, V>;

template<class ... FIELDS>
using object = ctrj_detail::object<FIELDS ...>;

template<class SCHEMA>
using handler = ctrj_detail::handler<SCHEMA>;

template<class SCHEMA>
using value = ctrj_detail::value<SCHEMA>;

}

#endif //CTRJ_HPP
