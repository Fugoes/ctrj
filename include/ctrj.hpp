#ifndef CTRJ_HPP
#define CTRJ_HPP

#include <cstdint>
#include <string>
#include <optional>

#include "rapidjson/writer.h"
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
  uint64_t uint64{};
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
  virtual bool Number(std::string_view str, handler_base **h) = 0;
  virtual bool StartObject(handler_base **h) = 0;
  virtual bool EndObject(handler_base **h) = 0;
};

template<class ... FIELDS>
struct partial_handler;

template<class SCHEMA>
struct complete_handler;

template<>
struct partial_handler<> {
  inline bool Key(std::string_view key, handler_base **h) { return false; }
  inline bool Number(std::string_view num, handler_base **h) { return false; }
  inline bool EndObject(handler_base **h) { return true; }
};

template<>
struct complete_handler<uint64_t> : public handler_base {
  handler_base *parent_{nullptr};

  bool Key(std::string_view str, handler_base **h) override { return false; }
  bool Number(std::string_view str, handler_base **h) override {
    *h = parent_;
    return true;
  }
  bool StartObject(handler_base **h) override { return false; }
  bool EndObject(handler_base **h) override { return false; }
};

template<class ... FIELDS>
struct complete_handler<object<FIELDS ...>> : public handler_base {
  handler_base *parent_{nullptr};
  uint8_t state_{0};
  partial_handler<FIELDS ...> inner_{};

  bool Key(std::string_view str, handler_base **h) override {
    return inner_.Key(str, h);
  }
  bool Number(std::string_view str, handler_base **h) override {
    return inner_.Number(str, h);
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
  complete_handler<V> sub_handler_{};

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
  inline bool Number(std::string_view num, handler_base **h) { return false; }
  inline bool EndObject(handler_base **h) {
    return state_ == 1 && partial_handler<FIELDS ...>::EndObject(h);
  }
};

}

#endif //CTRJ_HPP
