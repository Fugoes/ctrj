#ifndef CTRJ_HPP
#define CTRJ_HPP

#include <cstdint>
#include <string>
#include <optional>

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

namespace ctrj::ctrj_detail {

template<const char *K, class V>
struct field {};

template<class ... KVS>
struct object {};

template<class OBJECT>
struct value_type_of;

template<>
struct value_type_of<uint64_t> {
  uint64_t uint64{};
};

template<>
struct value_type_of<object<>> {
};

template<const char *K, class V, class ... KVS>
struct value_type_of<object<field<K, V>, KVS ...>> :
    public value_type_of<object<KVS ...>> {
  value_type_of<V> value_{};
};

template<const char *K, class OBJECT>
struct value_type_of_field;

template<const char *K, class V, class ... KVS>
struct value_type_of_field<K, object<field<K, V>, KVS ...>> {
  typedef value_type_of<object<field<K, V>, KVS ...>> type;
  typedef value_type_of<V> rtype;
};

template<const char *K, const char *X, class V, class ... KVS>
struct value_type_of_field<K, object<field<X, V>, KVS ...>> :
    public value_type_of_field<K, object<KVS ...>> {
};

template<class OBJECT>
struct value;

template<class KV_LIST>
struct lift_value_type;

template<>
struct lift_value_type<value_type_of<uint64_t>> {
  typedef value_type_of<uint64_t> type;
};

template<class ... KVS>
struct lift_value_type<value_type_of<object<KVS ...>>> {
  typedef value<object<KVS ...>> type;
};

template<class ... KVS>
struct value<object<KVS ...>> : public value_type_of<object<KVS ...>> {
  template<const char *K>
  auto &get() {
    using field_type = value_type_of_field<K, object<KVS ...>>;
    using rtype = typename lift_value_type<typename field_type::rtype>::type;
    auto &r = static_cast<typename field_type::type &>(*this).value_;
    return static_cast<rtype &>(r);
  }
};

}

#endif //CTRJ_HPP
