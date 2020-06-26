#ifndef CTRJ_SCHEMA_HPP
#define CTRJ_SCHEMA_HPP

/* Notes:
 * - nul<nul<...>> is considered invalid.
 */

#include <climits>
#include <cstdint>
#include <cstdlib>

namespace ctrj {

namespace detail {

template <class... FLDS> struct obj {};

template <const char *K, class T> struct fld {};

template <class T> struct nul {};

template <class T> struct arr {};

struct i64 {};

struct u64 {};

struct f64 {};

struct str {};

struct bol {};

template <typename... FLDS> struct fld_list {};

template <const char *K, typename... FLD_LIST> struct fld_list_drop_neq;

template <const char *K, typename T, typename... FLDS>
struct fld_list_drop_neq<K, fld_list<fld<K, T>, FLDS...>> {
  using type = fld_list<fld<K, T>, FLDS...>;
};

template <const char *K, const char *N, typename T, typename... FLDS>
struct fld_list_drop_neq<K, fld_list<fld<N, T>, FLDS...>>
    : public fld_list_drop_neq<K, fld_list<FLDS...>> {};

template <typename FLD_LIST> struct fld_list_len;

template <> struct fld_list_len<fld_list<>> { static const size_t value = 0; };

template <typename FLD, typename... FLDS>
struct fld_list_len<fld_list<FLD, FLDS...>>
    : public fld_list_len<fld_list<FLDS...>> {
  static const size_t value = fld_list_len<fld_list<FLDS...>>::value + 1;
};

} // namespace detail

template <class... FLDS> using obj = detail::obj<FLDS...>;

template <const char *K, class T> using fld = detail::fld<K, T>;

template <class T> using nul = detail::nul<T>;

template <class T> using arr = detail::arr<T>;

using i64 = detail::i64;

using u64 = detail::u64;

using f64 = detail::f64;

using str = detail::str;

using bol = detail::bol;

} // namespace ctrj

#endif // CTRJ_SCHEMA_HPP
