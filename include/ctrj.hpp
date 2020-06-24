#ifndef CTRJ_HPP
#define CTRJ_HPP

#include <cstdint>
#include <climits>
#include <string>
#include <optional>

#include "rapidjson/reader.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

namespace ctrj::ctrj_detail {

/* Schema definition */
template<const char *K, class V>
struct field {};

template<class ... FIELDS>
struct object {};

template<class T>
struct nullable {};

/* Value type */
template<class SCHEMA>
struct value;

template<>
struct value<uint64_t> {
  uint64_t u64{};
};

static_assert(sizeof(unsigned) < sizeof(uint64_t));

template<>
struct value<unsigned> {
  unsigned u{};
};

template<>
struct value<int64_t> {
  int64_t i64{};
};

static_assert(sizeof(int) < sizeof(int64_t));

template<>
struct value<int> {
  int i{};
};

template<>
struct value<std::string> {
  std::string str{};
};

template<>
struct value<object<>> {
};

template<const char *K, class SCHEMA>
struct partial_type_of;

template<const char *K, class V, class ... FIELDS>
struct partial_type_of<K, object<field<K, V>, FIELDS ...>> {
  typedef object<field<K, V>, FIELDS ...> type;
};

template<const char *K, const char *X, class V, class ... FIELDS>
struct partial_type_of<K, object<field<X, V>, FIELDS ...>> :
    public partial_type_of<K, object<FIELDS ...>> {
};

template<const char *K, class V, class ... FIELDS>
struct value<object<field<K, V>, FIELDS ...>> :
    public value<object<FIELDS ...>> {
  value<V> value_{};

  template<const char *N>
  auto &get() {
    return value<
        typename partial_type_of<N, object<field<K, V>, FIELDS ...>>::type
    >::value_;
  }
};

template<class T>
struct value<nullable<T>> {
  std::optional<value<T>> opt{};
};

/* Handler */
struct handler_base {
  virtual bool Key(std::string_view str, handler_base **h) = 0;
  virtual bool Null(handler_base **h) = 0;
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
  explicit partial_handler(value<object<>> &) {}

  inline bool Key(std::string_view key, handler_base **h) { return false; }
  inline bool Null(handler_base **h) { return false; }
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
  value<uint64_t> &ref_;

  explicit complete_handler(value<uint64_t> &ref) : ref_(ref) {}

  bool Key(std::string_view str, handler_base **h) override {
    return false;
  }
  bool Null(handler_base **h) override {
    return false;
  }
  bool Int(int num, handler_base **h) override {
    return false;
  }
  bool Uint(unsigned num, handler_base **h) override {
    *h = parent_;
    ref_.u64 = num;
    return true;
  }
  bool Int64(int64_t num, handler_base **h) override {
    return false;
  }
  bool Uint64(uint64_t num, handler_base **h) override {
    *h = parent_;
    ref_.u64 = num;
    return true;
  }
  bool String(std::string_view str, handler_base **h) override { return false; }
  bool StartObject(handler_base **h) override { return false; }
  bool EndObject(handler_base **h) override { return false; }
};

template<>
struct complete_handler<unsigned> : public handler_base {
  handler_base *parent_{nullptr};
  value<unsigned> &ref_;

  explicit complete_handler(value<unsigned> &ref) : ref_(ref) {}

  bool Key(std::string_view str, handler_base **h) override {
    return false;
  }
  bool Null(handler_base **h) override {
    return false;
  }
  bool Int(int num, handler_base **h) override {
    return false;
  }
  bool Uint(unsigned num, handler_base **h) override {
    *h = parent_;
    ref_.u = num;
    return true;
  }
  bool Int64(int64_t num, handler_base **h) override {
    return false;
  }
  bool Uint64(uint64_t num, handler_base **h) override {
    return false;
  }
  bool String(std::string_view str, handler_base **h) override { return false; }
  bool StartObject(handler_base **h) override { return false; }
  bool EndObject(handler_base **h) override { return false; }
};

template<>
struct complete_handler<int64_t> : public handler_base {
  handler_base *parent_{nullptr};
  value<int64_t> &ref_;

  explicit complete_handler(value<int64_t> &ref) : ref_(ref) {}

  bool Key(std::string_view str, handler_base **h) override {
    return false;
  }
  bool Null(handler_base **h) override {
    return false;
  }
  bool Int(int num, handler_base **h) override {
    *h = parent_;
    ref_.i64 = num;
    return true;
  }
  bool Uint(unsigned num, handler_base **h) override {
    *h = parent_;
    ref_.i64 = num;
    return true;
  }
  bool Int64(int64_t num, handler_base **h) override {
    *h = parent_;
    ref_.i64 = num;
    return true;
  }
  bool Uint64(uint64_t num, handler_base **h) override {
    if (num <= INT64_MAX) {
      *h = parent_;
      ref_.i64 = num;
      return true;
    } else {
      return false;
    }
  }
  bool String(std::string_view str, handler_base **h) override { return false; }
  bool StartObject(handler_base **h) override { return false; }
  bool EndObject(handler_base **h) override { return false; }
};

template<>
struct complete_handler<int> : public handler_base {
  handler_base *parent_{nullptr};
  value<int> &ref_;

  explicit complete_handler(value<int> &ref) : ref_(ref) {}

  bool Key(std::string_view str, handler_base **h) override {
    return false;
  }
  bool Null(handler_base **h) override {
    return false;
  }
  bool Int(int num, handler_base **h) override {
    *h = parent_;
    ref_.i = num;
    return true;
  }
  bool Uint(unsigned num, handler_base **h) override {
    if (num <= INT_MAX) {
      *h = parent_;
      ref_.i = (int) num;
      return true;
    } else {
      return false;
    }
  }
  bool Int64(int64_t num, handler_base **h) override {
    return false;
  }
  bool Uint64(uint64_t num, handler_base **h) override {
    return false;
  }
  bool String(std::string_view str, handler_base **h) override { return false; }
  bool StartObject(handler_base **h) override { return false; }
  bool EndObject(handler_base **h) override { return false; }
};

template<>
struct complete_handler<std::string> : public handler_base {
  handler_base *parent_{nullptr};
  value<std::string> &ref_;

  explicit complete_handler(value<std::string> &ref) : ref_(ref) {}

  bool Key(std::string_view str, handler_base **h) override {
    return false;
  }
  bool Null(handler_base **h) override {
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
struct complete_handler<nullable<object<FIELDS ...>>> : public handler_base {
  handler_base *parent_{nullptr};
  value<nullable<object<FIELDS ...>>> &ref_;
  std::optional<partial_handler<FIELDS ...>> inner_{};

  explicit complete_handler(value<nullable<object<FIELDS ...>>> &ref)
      : ref_(ref) {}

  bool Key(std::string_view str, handler_base **h) override {
    return inner_.value().Key(str, h);
  }
  bool Null(handler_base **h) override {
    if (!inner_.has_value()) {
      *h = parent_;
      ref_.opt.reset();
      return true;
    } else {
      return false;
    }
  }
  bool Int(int num, handler_base **h) override {
    return inner_.value().Int(num, h);
  }
  bool Uint(unsigned num, handler_base **h) override {
    return inner_.value().Uint(num, h);
  }
  bool Int64(int64_t num, handler_base **h) override {
    return inner_.value().Int64(num, h);
  }
  bool Uint64(uint64_t num, handler_base **h) override {
    return inner_.value().Uint64(num, h);
  }
  bool String(std::string_view str, handler_base **h) override {
    return inner_.value().String(str, h);
  }
  bool StartObject(handler_base **h) override {
    if (!inner_.has_value()) {
      ref_.opt.emplace(value<object<FIELDS ...>>{});
      inner_.emplace(partial_handler<FIELDS ...>{ref_.opt.value()});
      return true;
    } else {
      return false;
    }
  }
  bool EndObject(handler_base **h) override {
    if (inner_.has_value()) {
      if (inner_.value().EndObject(h)) {
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

template<class ... FIELDS>
struct complete_handler<object<FIELDS ...>> : public handler_base {
  handler_base *parent_{nullptr};
  uint8_t state_{0};
  partial_handler<FIELDS ...> inner_;

  explicit complete_handler(value<object<FIELDS ...>> &ref)
      : inner_(ref) {}

  bool Key(std::string_view str, handler_base **h) override {
    return inner_.Key(str, h);
  }
  bool Null(handler_base **h) override {
    return false;
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

  partial_handler(value<object<field<K, V>, FIELDS ...>> &ref)
      : partial_handler<FIELDS ...>
            (static_cast<value<object<FIELDS ...>> &>(ref)),
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
  inline bool Null(handler_base **h) { return false; }
  inline bool Int(int num, handler_base **h) { return false; }
  inline bool Uint(unsigned num, handler_base **h) { return false; }
  inline bool Int64(int64_t num, handler_base **h) { return false; }
  inline bool Uint64(uint64_t num, handler_base **h) { return false; }
  inline bool String(std::string_view str, handler_base **h) { return false; }
  inline bool EndObject(handler_base **h) {
    return state_ == 1 && partial_handler<FIELDS ...>::EndObject(h);
  }
};

template<>
struct complete_handler<nullable<uint64_t>> : public handler_base {
  handler_base *parent_{nullptr};
  value<nullable<uint64_t>> &ref_;

  explicit complete_handler(value<nullable<uint64_t>> &ref) : ref_(ref) {}

  bool Key(std::string_view str, handler_base **h) override {
    return false;
  }
  bool Null(handler_base **h) override {
    *h = parent_;
    ref_.opt.reset();
    return true;
  }
  bool Int(int num, handler_base **h) override {
    return false;
  }
  bool Uint(unsigned num, handler_base **h) override {
    *h = parent_;
    ref_.opt.emplace(value<uint64_t>{});
    ref_.opt.value().u64 = num;
    return true;
  }
  bool Int64(int64_t num, handler_base **h) override {
    return false;
  }
  bool Uint64(uint64_t num, handler_base **h) override {
    *h = parent_;
    ref_.opt.emplace(value<uint64_t>{});
    ref_.opt.value().u64 = num;
    return true;
  }
  bool String(std::string_view str, handler_base **h) override { return false; }
  bool StartObject(handler_base **h) override { return false; }
  bool EndObject(handler_base **h) override { return false; }
};

template<>
struct complete_handler<nullable<unsigned>> : public handler_base {
  handler_base *parent_{nullptr};
  value<nullable<unsigned>> &ref_;

  explicit complete_handler(value<nullable<unsigned>> &ref) : ref_(ref) {}

  bool Key(std::string_view str, handler_base **h) override {
    return false;
  }
  bool Null(handler_base **h) override {
    *h = parent_;
    ref_.opt.reset();
    return true;
  }
  bool Int(int num, handler_base **h) override {
    return false;
  }
  bool Uint(unsigned num, handler_base **h) override {
    *h = parent_;
    ref_.opt.emplace(value<unsigned>{});
    ref_.opt.value().u = num;
    return true;
  }
  bool Int64(int64_t num, handler_base **h) override {
    return false;
  }
  bool Uint64(uint64_t num, handler_base **h) override {
    return false;
  }
  bool String(std::string_view str, handler_base **h) override { return false; }
  bool StartObject(handler_base **h) override { return false; }
  bool EndObject(handler_base **h) override { return false; }
};

template<>
struct complete_handler<nullable<int64_t>> : public handler_base {
  handler_base *parent_{nullptr};
  value<nullable<int64_t>> &ref_;

  explicit complete_handler(value<nullable<int64_t>> &ref) : ref_(ref) {}

  bool Key(std::string_view str, handler_base **h) override {
    return false;
  }
  bool Null(handler_base **h) override {
    *h = parent_;
    return true;
  }
  bool Int(int num, handler_base **h) override {
    *h = parent_;
    ref_.opt.emplace(value<int64_t>{});
    ref_.opt.value().i64 = num;
    return true;
  }
  bool Uint(unsigned num, handler_base **h) override {
    *h = parent_;
    ref_.opt.emplace(value<int64_t>{});
    ref_.opt.value().i64 = num;
    return true;
  }
  bool Int64(int64_t num, handler_base **h) override {
    *h = parent_;
    ref_.opt.emplace(value<int64_t>{});
    ref_.opt.value().i64 = num;
    return true;
  }
  bool Uint64(uint64_t num, handler_base **h) override {
    if (num <= INT64_MAX) {
      *h = parent_;
      ref_.opt.emplace(value<int64_t>{});
      ref_.opt.value().i64 = num;
      return true;
    } else {
      return false;
    }
  }
  bool String(std::string_view str, handler_base **h) override { return false; }
  bool StartObject(handler_base **h) override { return false; }
  bool EndObject(handler_base **h) override { return false; }
};

template<>
struct complete_handler<nullable<int>> : public handler_base {
  handler_base *parent_{nullptr};
  value<nullable<int>> &ref_;

  explicit complete_handler(value<nullable<int>> &ref) : ref_(ref) {}

  bool Key(std::string_view str, handler_base **h) override {
    return false;
  }
  bool Null(handler_base **h) override {
    *h = parent_;
    return true;
  }
  bool Int(int num, handler_base **h) override {
    *h = parent_;
    ref_.opt.emplace(value<int>{});
    ref_.opt.value().i = num;
    return true;
  }
  bool Uint(unsigned num, handler_base **h) override {
    if (num <= INT_MAX) {
      *h = parent_;
      ref_.opt.emplace(value<int>{});
      ref_.opt.value().i = (int) num;
      return true;
    } else {
      return false;
    }
  }
  bool Int64(int64_t num, handler_base **h) override {
    return false;
  }
  bool Uint64(uint64_t num, handler_base **h) override {
    return false;
  }
  bool String(std::string_view str, handler_base **h) override { return false; }
  bool StartObject(handler_base **h) override { return false; }
  bool EndObject(handler_base **h) override { return false; }
};

template<>
struct complete_handler<nullable<std::string>> : public handler_base {
  handler_base *parent_{nullptr};
  value<nullable<std::string>> &ref_;

  explicit complete_handler(value<nullable<std::string>> &ref) : ref_(ref) {}

  bool Key(std::string_view str, handler_base **h) override {
    return false;
  }
  bool Null(handler_base **h) override {
    *h = parent_;
    return true;
  }
  bool Int(int num, handler_base **h) override { return false; }
  bool Uint(unsigned num, handler_base **h) override { return false; }
  bool Int64(int64_t num, handler_base **h) override { return false; }
  bool Uint64(uint64_t num, handler_base **h) override { return false; }
  bool String(std::string_view str, handler_base **h) override {
    *h = parent_;
    ref_.opt.emplace(value<std::string>{});
    ref_.opt.value().str = str;
    return true;
  }
  bool StartObject(handler_base **h) override { return false; }
  bool EndObject(handler_base **h) override { return false; }
};

template<class SCHEMA>
struct handler :
    public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, handler<SCHEMA>> {
  handler_base *h_{nullptr};
  complete_handler<SCHEMA> inner_;

  explicit handler(value<SCHEMA> &v) :
      inner_(static_cast<value<SCHEMA> &>(v)) {
    h_ = &inner_;
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
  bool Null() {
    return h_->Null(&h_);
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

template<class SCHEMA>
struct write;

template<class ... FIELDS>
struct partial_write {};

template<>
struct write<uint64_t> {
  template<class OutputStream,
      class SourceEncoding = rapidjson::UTF8<>,
      class TargetEncoding = rapidjson::UTF8<>,
      class StackAllocator = rapidjson::CrtAllocator,
      unsigned writeFlags = rapidjson::kWriteDefaultFlags>
  inline static void to(
      value<uint64_t> &v,
      rapidjson::Writer<OutputStream, SourceEncoding, TargetEncoding,
                        StackAllocator, writeFlags> &w) {
    w.Uint64(v.u64);
  }
};

template<>
struct write<int64_t> {
  template<class OutputStream,
      class SourceEncoding = rapidjson::UTF8<>,
      class TargetEncoding = rapidjson::UTF8<>,
      class StackAllocator = rapidjson::CrtAllocator,
      unsigned writeFlags = rapidjson::kWriteDefaultFlags>
  inline static void to(
      value<int64_t> &v,
      rapidjson::Writer<OutputStream, SourceEncoding, TargetEncoding,
                        StackAllocator, writeFlags> &w) {
    w.Int64(v.i64);
  }
};

template<>
struct write<unsigned> {
  template<class OutputStream,
      class SourceEncoding = rapidjson::UTF8<>,
      class TargetEncoding = rapidjson::UTF8<>,
      class StackAllocator = rapidjson::CrtAllocator,
      unsigned writeFlags = rapidjson::kWriteDefaultFlags>
  inline static void to(
      value<unsigned> &v,
      rapidjson::Writer<OutputStream, SourceEncoding, TargetEncoding,
                        StackAllocator, writeFlags> &w) {
    w.Uint(v.u);
  }
};

template<>
struct write<int> {
  template<class OutputStream,
      class SourceEncoding = rapidjson::UTF8<>,
      class TargetEncoding = rapidjson::UTF8<>,
      class StackAllocator = rapidjson::CrtAllocator,
      unsigned writeFlags = rapidjson::kWriteDefaultFlags>
  inline static void to(
      value<int> &v,
      rapidjson::Writer<OutputStream, SourceEncoding, TargetEncoding,
                        StackAllocator, writeFlags> &w) {
    w.Uint(v.i);
  }
};

template<>
struct write<std::string> {
  template<class OutputStream,
      class SourceEncoding = rapidjson::UTF8<>,
      class TargetEncoding = rapidjson::UTF8<>,
      class StackAllocator = rapidjson::CrtAllocator,
      unsigned writeFlags = rapidjson::kWriteDefaultFlags>
  inline static void to(
      value<std::string> &v,
      rapidjson::Writer<OutputStream, SourceEncoding, TargetEncoding,
                        StackAllocator, writeFlags> &w) {
    w.String(v.str.data(), v.str.length());
  }
};

template<>
struct write<partial_write<>> {
  template<class OutputStream,
      class SourceEncoding = rapidjson::UTF8<>,
      class TargetEncoding = rapidjson::UTF8<>,
      class StackAllocator = rapidjson::CrtAllocator,
      unsigned writeFlags = rapidjson::kWriteDefaultFlags>
  inline static void to(
      value<object<>> &v,
      rapidjson::Writer<OutputStream, SourceEncoding, TargetEncoding,
                        StackAllocator, writeFlags> &w) {
  }
};

template<const char *K, class V, class ... FIELDS>
struct write<partial_write<field<K, V>, FIELDS ...>> {
  template<class OutputStream,
      class SourceEncoding = rapidjson::UTF8<>,
      class TargetEncoding = rapidjson::UTF8<>,
      class StackAllocator = rapidjson::CrtAllocator,
      unsigned writeFlags = rapidjson::kWriteDefaultFlags>
  inline static void to(
      value<object<field<K, V>, FIELDS ...>> &v,
      rapidjson::Writer<OutputStream, SourceEncoding, TargetEncoding,
                        StackAllocator, writeFlags> &w) {
    w.Key(K);
    write<V>::template to<OutputStream>(v.template get<K>(), w);
    write<partial_write<FIELDS ...>>::to(
        static_cast<value<object<FIELDS ...>> &>(v), w);
  }
};

template<class ... FIELDS>
struct write<object<FIELDS ...>> {
  template<class OutputStream,
      class SourceEncoding = rapidjson::UTF8<>,
      class TargetEncoding = rapidjson::UTF8<>,
      class StackAllocator = rapidjson::CrtAllocator,
      unsigned writeFlags = rapidjson::kWriteDefaultFlags>
  inline static void to(
      value<object<FIELDS ...>> &v,
      rapidjson::Writer<OutputStream, SourceEncoding, TargetEncoding,
                        StackAllocator, writeFlags> &w) {
    w.StartObject();
    write<partial_write<FIELDS ...>>::to(v, w);
    w.EndObject();
  }
};

template<class T>
struct write<nullable<T>> {
  template<class OutputStream,
      class SourceEncoding = rapidjson::UTF8<>,
      class TargetEncoding = rapidjson::UTF8<>,
      class StackAllocator = rapidjson::CrtAllocator,
      unsigned writeFlags = rapidjson::kWriteDefaultFlags>
  inline static void to(
      value<nullable<T>> &v,
      rapidjson::Writer<OutputStream, SourceEncoding, TargetEncoding,
                        StackAllocator, writeFlags> &w) {
    if (!v.opt.has_value()) {
      w.Null();
    } else {
      write<T>::to(v.opt.value(), w);
    }
  }
};

}

namespace ctrj {

template<const char *K, class V>
using field = ctrj_detail::field<K, V>;

template<class ... FIELDS>
using object = ctrj_detail::object<FIELDS ...>;

template<class T>
using nullable = ctrj_detail::nullable<T>;

template<class SCHEMA>
using handler = ctrj_detail::handler<SCHEMA>;

template<class SCHEMA>
using value = ctrj_detail::value<SCHEMA>;

template<class T>
using write = ctrj_detail::write<T>;

}

#endif //CTRJ_HPP
