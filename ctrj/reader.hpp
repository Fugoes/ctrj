#ifndef CTRJ_READER_HPP
#define CTRJ_READER_HPP

#include <string_view>

#include "value.hpp"

namespace ctrj {

namespace detail {

enum struct reader_event {
  Key,
  Null,
  Bool,
  I64,
  U64,
  F64,
  String,
  StartObject,
  EndObject,
  StartArray,
  EndArray
};

union reader_data {
  std::string_view key_;
  bool bol_;
  int64_t i64_;
  uint64_t u64_;
  double f64_;
  std::string_view str_;
};

struct reader_state;

typedef bool (*reader_handler_t)(reader_state *, reader_event);

struct reader_stk_frame {
  reader_handler_t handler_{nullptr};
  void *ref_{nullptr};
};

struct reader_state {
  reader_data data_;
  reader_stk_frame *stk_top_;
  uint8_t *bit_stk_top_;

  template <typename T> inline void stk_push(val<T> *ref);

  template <typename T> inline void stk_replace(val<T> *ref);

  template <typename T> inline val<T> *stk_peek_ref();

  inline reader_handler_t stk_peek_handle();

  inline void pop_frame();
};

template <typename T> struct reader_handler;

template <typename T> inline void reader_state::stk_push(val<T> *ref) {
  auto handler = reader_handler<T>::handler;
  stk_top_++;
  stk_top_->handler_ = handler;
  stk_top_->ref_ = static_cast<void *>(ref);
  reader_handler<T>::init(this);
}

template <typename T> inline void reader_state::stk_replace(val<T> *ref) {
  auto handler = reader_handler<T>::handler;
  stk_top_->handler_ = handler;
  stk_top_->ref_ = static_cast<void *>(ref);
  reader_handler<T>::init(this);
}

inline void reader_state::pop_frame() { stk_top_--; }

template <typename T> inline val<T> *reader_state::stk_peek_ref() {
  return static_cast<val<T> *>(stk_top_->ref_);
}

inline reader_handler_t reader_state::stk_peek_handle() {
  return stk_top_->handler_;
}

template <> struct reader_handler<u64> {
  const static size_t stk_size = 1;
  const static size_t bit_stk_size = 0;

  inline static void init(reader_state *p) {}

  static bool handler(reader_state *p, reader_event event);
};

template <> struct reader_handler<i64> {
  const static size_t stk_size = 1;
  const static size_t bit_stk_size = 0;

  inline static void init(reader_state *p) {}

  static bool handler(reader_state *p, reader_event event);
};

template <> struct reader_handler<f64> {
  const static size_t stk_size = 1;
  const static size_t bit_stk_size = 0;

  inline static void init(reader_state *p) {}

  static bool handler(reader_state *p, reader_event event);
};

template <> struct reader_handler<str> {
  const static size_t stk_size = 1;
  const static size_t bit_stk_size = 0;

  inline static void init(reader_state *p) {}

  static bool handler(reader_state *p, reader_event event);
};

template <> struct reader_handler<bol> {
  const static size_t stk_size = 1;
  const static size_t bit_stk_size = 0;

  inline static void init(reader_state *p) {}

  static bool handler(reader_state *p, reader_event event);
};

template <typename T> struct reader_handler<nul<T>> {
  const static size_t stk_size = reader_handler<T>::stk_size;
  const static size_t bit_stk_size = 0;

  inline static void init(reader_state *p) {}

  static bool handler(reader_state *p, reader_event event) {
    auto x = p->stk_peek_ref<nul<T>>();
    if (event == reader_event::Null) {
      x->opt.reset();
      p->pop_frame();
      return true;
    } else {
      x->opt.emplace(val<T>{});
      auto ref = static_cast<val<T> *>(&x->opt.value());
      p->stk_replace(ref);
      return p->stk_top_->handler_(p, event);
    }
  }
};

template <typename R, typename FLD_LIST> struct partial_reader_handler;

template <typename R> struct partial_reader_handler<R, fld_list<>> {
  const static size_t stk_size = 0;
  const static size_t bit_stk_size = 0;

  template <size_t D> inline static bool handle_key(reader_state *p) {
    return false;
  }
};

template <typename R, const char *K, typename T, typename... FLDS>
struct partial_reader_handler<R, fld_list<fld<K, T>, FLDS...>>
    : public partial_reader_handler<R, fld_list<FLDS...>> {
  const static size_t stk_size =
      std::max(reader_handler<T>::stk_size,
               partial_reader_handler<R, fld_list<FLDS...>>::stk_size);
  const static size_t bit_stk_size =
      std::max(reader_handler<T>::bit_stk_size,
               partial_reader_handler<R, fld_list<FLDS...>>::bit_stk_size);

  using RFLDS = partial_reader_handler<R, fld_list<FLDS...>>;

  template <size_t D> inline static bool handle_key(reader_state *p) {
    if (*(p->bit_stk_top_ - D) == 1 || p->data_.key_ != K) {
      return RFLDS::template handle_key<D + 1>(p);
    } else {
      auto x = p->stk_peek_ref<R>();
      *(p->bit_stk_top_ - D) = 1;
      auto ref = static_cast<val<T> *>(&x->template get<K>());
      p->stk_push(ref);
      return true;
    }
  }
};

template <typename... FLDS> struct reader_handler<obj<FLDS...>> {
  using R = obj<FLDS...>;
  using N = fld_list_len<fld_list<FLDS...>>;

  const static size_t stk_size =
      partial_reader_handler<R, fld_list<FLDS...>>::stk_size + 1;
  const static size_t bit_stk_size =
      partial_reader_handler<R, fld_list<FLDS...>>::bit_stk_size + N::value;

  inline static void init(reader_state *p) {
    for (size_t i = 0; i < N::value; i++) {
      p->bit_stk_top_++;
      *(p->bit_stk_top_) = 0;
    }
  }

  static bool handler(reader_state *p, reader_event event) {
    if (event == reader_event::Key) {
      return partial_reader_handler<R, fld_list<FLDS...>>::template handle_key<
          0>(p);
    } else if (event == reader_event::StartObject) {
      return true;
    } else if (event == reader_event::EndObject) {
      for (size_t i = 0; i < N::value; i++) {
        if (*(p->bit_stk_top_) == 0)
          return false;
        p->bit_stk_top_--;
      }
      p->pop_frame();
      return true;
    } else {
      return false;
    }
  }
};

template <typename T> struct reader_handler<arr<T>> {
  const static size_t stk_size = reader_handler<T>::stk_size + 1;
  const static size_t bit_stk_size = reader_handler<T>::bit_stk_size + 1;

  inline static void init(reader_state *p) {
    p->bit_stk_top_++;
    *(p->bit_stk_top_) = 0;
  }

  static bool handler(reader_state *p, reader_event event) {
    auto x = p->stk_peek_ref<arr<T>>();
    if (*(p->bit_stk_top_) == 0) {
      *(p->bit_stk_top_) = 1;
      if (event == reader_event::StartArray) {
        x->vec.clear();
        return true;
      } else {
        return false;
      }
    } else {
      if (event == reader_event::EndArray) {
        p->bit_stk_top_--;
        p->pop_frame();
        return true;
      } else {
        x->vec.emplace_back(val<T>{});
        val<T> &y = x->vec[x->vec.size() - 1];
        p->stk_push(&y);
        return p->stk_peek_handle()(p, event);
      }
    }
  }
};

template <typename SCHEMA, template <typename...> typename B, typename... ARGS>
class reader : public B<ARGS..., reader<SCHEMA, B, ARGS...>> {
  reader_state state_{};
  reader_stk_frame stk_[reader_handler<SCHEMA>::stk_size];
  uint8_t bit_stk_[reader_handler<SCHEMA>::bit_stk_size];

  inline reader_handler_t get_handler() { return state_.stk_peek_handle(); }

public:
  explicit reader(val<SCHEMA> &ref) {
    state_.stk_top_ = stk_ - 1;
    state_.bit_stk_top_ = bit_stk_ - 1;
    state_.stk_push(&ref);
  }

  inline bool RawNumber(const char *, size_t, bool) { return false; }

  inline bool Null() { return get_handler()(&state_, reader_event::Null); }
  inline bool Bool(bool b) {
    state_.data_.bol_ = b;
    return get_handler()(&state_, reader_event::Bool);
  }
  inline bool Int(int i) {
    state_.data_.i64_ = i;
    return get_handler()(&state_, reader_event::I64);
  }
  inline bool Int64(int64_t i) {
    state_.data_.i64_ = i;
    return get_handler()(&state_, reader_event::I64);
  }
  inline bool Uint(unsigned i) {
    state_.data_.u64_ = i;
    return get_handler()(&state_, reader_event::U64);
  }
  inline bool Uint64(uint64_t i) {
    state_.data_.u64_ = i;
    return get_handler()(&state_, reader_event::U64);
  }
  inline bool Double(double d) {
    state_.data_.f64_ = d;
    return get_handler()(&state_, reader_event::F64);
  }
  inline bool String(const char *str, size_t length, bool) {
    state_.data_.str_ = std::string_view{str, length};
    return get_handler()(&state_, reader_event::String);
  }
  inline bool StartObject() {
    return get_handler()(&state_, reader_event::StartObject);
  }
  inline bool Key(const char *str, size_t length, bool) {
    state_.data_.key_ = std::string_view{str, length};
    return get_handler()(&state_, reader_event::Key);
  }
  inline bool EndObject(size_t) {
    return get_handler()(&state_, reader_event::EndObject);
  }
  inline bool StartArray() {
    return get_handler()(&state_, reader_event::StartArray);
  }
  inline bool EndArray(size_t) {
    return get_handler()(&state_, reader_event::EndArray);
  }
};

} // namespace detail

template <typename SCHEMA, template <typename...> typename B, typename... ARGS>
using reader = detail::reader<SCHEMA, B, ARGS...>;

} // namespace ctrj

#ifdef CTRJ_HEADER_ONLY
#include "reader.cpp"
#endif

#endif // CTRJ_READER_HPP
