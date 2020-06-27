#ifndef CTRJ_WRITER_HPP
#define CTRJ_WRITER_HPP

#include "value.hpp"

namespace ctrj {

namespace detail {

template <typename SCHEMA> struct write;

template <> struct write<i64> {
  const val<i64> &ref_;

  inline explicit write(const val<i64> &ref) : ref_(ref) {}

  template <typename WRITER> inline void to(WRITER &w) { w.Int64(ref_.i64); }
};

template <> struct write<u64> {
  const val<u64> &ref_;

  inline explicit write(const val<u64> &ref) : ref_(ref) {}

  template <typename WRITER> inline void to(WRITER &w) { w.Uint64(ref_.u64); }
};

template <> struct write<f64> {
  const val<f64> &ref_;

  inline explicit write(const val<f64> &ref) : ref_(ref) {}

  template <typename WRITER> inline void to(WRITER &w) { w.Double(ref_.f64); }
};

template <> struct write<str> {
  const val<str> &ref_;

  inline explicit write(const val<str> &ref) : ref_(ref) {}

  template <typename WRITER> inline void to(WRITER &w) {
    w.String(ref_.str.data(), ref_.str.length());
  }
};

template <> struct write<bol> {
  const val<bol> &ref_;

  inline explicit write(const val<bol> &ref) : ref_(ref) {}

  template <typename WRITER> inline void to(WRITER &w) { w.Bool(ref_.bol); }
};

template <typename T> struct write<nul<T>> {
  const val<nul<T>> &ref_;

  inline explicit write(const val<nul<T>> &ref) : ref_(ref) {}

  template <typename WRITER> inline void to(WRITER &w) {
    if (ref_.opt.has_value())
      write<T>(ref_.opt.value()).to(w);
    else
      w.Null();
  }
};

template <typename T> struct write<arr<T>> {
  const val<arr<T>> &ref_;

  inline explicit write(const val<arr<T>> &ref) : ref_(ref) {}

  template <typename WRITER> inline void to(WRITER &w) {
    w.StartArray();
    for (const val<T> &x : ref_.vec)
      write<T>(x).to(w);
    w.EndArray();
  }
};

template <typename FLD_LIST> struct partial_write;

template <> struct partial_write<fld_list<>> {
  template <typename WRITER>
  static inline void write_to(const partial_val<fld_list<>> &ref, WRITER &w) {}
};

template <const char *K, typename T, typename... FLDS>
struct partial_write<fld_list<fld<K, T>, FLDS...>> {
  using param_type = const partial_val<fld_list<fld<K, T>, FLDS...>> &;

  template <typename WRITER>
  static inline void write_to(param_type ref, WRITER &w) {
    w.Key(K);
    write<T>(ref._val).to(w);
    auto inner = static_cast<const partial_val<fld_list<FLDS...>> &>(ref);
    partial_write<fld_list<FLDS...>>::write_to(inner, w);
  }
};

template <typename... FLDS> struct write<obj<FLDS...>> {
  const val<obj<FLDS...>> &ref_;

  inline explicit write(const val<obj<FLDS...>> &ref) : ref_(ref) {}

  template <typename WRITER> inline void to(WRITER &w) {
    w.StartObject();
    partial_write<fld_list<FLDS...>>::write_to(ref_._inner, w);
    w.EndObject();
  }
};

template <typename T> struct write<dyn_obj<T>> {
  const val<dyn_obj<T>> &ref_;

  inline explicit write(const val<dyn_obj<T>> &ref) : ref_(ref) {}

  template <typename WRITER> inline void to(WRITER &w) {
    w.StartObject();
    for (auto const &kv : ref_.flds) {
      w.Key(kv.first.data(), kv.first.length());
      write<T>(kv.second).to(w);
    }
    w.EndObject();
  }
};

} // namespace detail

template <typename SCHEMA> using write = detail::write<SCHEMA>;

} // namespace ctrj

#endif // CTRJ_WRITER_HPP
