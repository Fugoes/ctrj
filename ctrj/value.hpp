#ifndef CTRJ_VALUE_HPP
#define CTRJ_VALUE_HPP

#include <map>
#include <optional>
#include <string>
#include <vector>

#include "schema.hpp"

namespace ctrj {

namespace detail {

template <typename SCHEMA> struct val;

template <> struct val<i64> { int64_t i64{}; };

template <> struct val<u64> { uint64_t u64{}; };

template <> struct val<f64> { double f64{}; };

template <> struct val<str> { std::string str{}; };

template <> struct val<bol> { bool bol{}; };

template <typename T> struct val<nul<T>> { std::optional<val<T>> opt{}; };

template <typename T> struct val<arr<T>> { std::vector<val<T>> vec{}; };

template <typename FLD_LIST> struct partial_val;

template <> struct partial_val<fld_list<>> {};

template <const char *K, typename T, typename... FLDS>
struct partial_val<fld_list<fld<K, T>, FLDS...>>
    : public partial_val<fld_list<FLDS...>> {
  val<T> _val{};
};

template <typename... FLDS> struct val<obj<FLDS...>> {
  partial_val<fld_list<FLDS...>> _inner{};

  template <const char *K> inline auto &get() {
    using rs = typename fld_list_drop_neq<K, fld_list<FLDS...>>::type;
    return static_cast<partial_val<rs> &>(_inner)._val;
  }
};

template <typename T> struct val<dyn_obj<T>> {
  std::map<std::string, val<T>> flds{};
};

} // namespace detail

template <typename T> using val = detail::val<T>;

} // namespace ctrj

#endif // CTRJ_VALUE_HPP
