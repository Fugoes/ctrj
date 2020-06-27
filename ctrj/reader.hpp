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
  std::vector<reader_stk_frame> stk_{};
  std::vector<bool> bitset_{};

  template <typename T> inline void push_frame(val<T> *ref);

  template <typename T> inline void replace_frame(val<T> *ref);

  inline void pop_frame();
};

template <typename T> struct reader_handler;

template <typename T> void reader_state::push_frame(val<T> *ref) {
  auto handler = reader_handler<T>::handler;
  stk_.push_back(reader_stk_frame{handler, static_cast<void *>(ref)});
  reader_handler<T>::init(this);
}

template <typename T> void reader_state::replace_frame(val<T> *ref) {
  auto handler = reader_handler<T>::handler;
  stk_[stk_.size() - 1].handler_ = handler;
  stk_[stk_.size() - 1].ref_ = static_cast<void *>(ref);
  reader_handler<T>::init(this);
}

void reader_state::pop_frame() { stk_.pop_back(); }

template <> struct reader_handler<u64> {
  inline static void init(reader_state *p) {}

  static bool handler(reader_state *p, reader_event event);
};

template <> struct reader_handler<i64> {
  inline static void init(reader_state *p) {}

  static bool handler(reader_state *p, reader_event event);
};

template <> struct reader_handler<f64> {
  inline static void init(reader_state *p) {}

  static bool handler(reader_state *p, reader_event event);
};

template <> struct reader_handler<str> {
  inline static void init(reader_state *p) {}

  static bool handler(reader_state *p, reader_event event);
};

template <> struct reader_handler<bol> {
  inline static void init(reader_state *p) {}

  static bool handler(reader_state *p, reader_event event);
};

template <typename T> struct reader_handler<nul<T>> {
  inline static void init(reader_state *p) {}

  static bool handler(reader_state *p, reader_event event) {
    auto x = static_cast<val<nul<T>> *>(p->stk_[p->stk_.size() - 1].ref_);
    if (event == reader_event::Null) {
      x->opt.reset();
      p->pop_frame();
      return true;
    } else {
      x->opt.emplace(val<T>{});
      auto ref = static_cast<val<T> *>(&x->opt.value());
      p->replace_frame(ref);
      return p->stk_[p->stk_.size() - 1].handler_(p, event);
    }
  }
};

template <typename R, typename FLD_LIST> struct partial_reader_handler;

template <typename R> struct partial_reader_handler<R, fld_list<>> {
  inline static bool handle_key(reader_state *p, size_t n) { return false; }
};

template <typename R, const char *K, typename T, typename... FLDS>
struct partial_reader_handler<R, fld_list<fld<K, T>, FLDS...>>
    : public partial_reader_handler<R, fld_list<FLDS...>> {
  using RFLDS = partial_reader_handler<R, fld_list<FLDS...>>;

  inline static bool handle_key(reader_state *p, size_t n) {
    if (!p->bitset_[p->bitset_.size() - 1 - n]) {
      if (p->data_.key_ == K) {
        p->bitset_[p->bitset_.size() - 1 - n] = true;
        auto x = static_cast<val<R> *>(p->stk_[p->stk_.size() - 1].ref_);
        auto y = static_cast<val<T> *>(&x->template get<K>());
        p->push_frame(y);
        return true;
      } else {
        return RFLDS::handle_key(p, n + 1);
      }
    } else {
      return RFLDS::handle_key(p, n + 1);
    }
  }
};

template <typename... FLDS> struct reader_handler<obj<FLDS...>> {
  using R = obj<FLDS...>;
  using N = fld_list_len<fld_list<FLDS...>>;

  inline static void init(reader_state *p) {
    for (size_t i = 0; i < N::value; i++)
      p->bitset_.push_back(false);
  }

  static bool handler(reader_state *p, reader_event event) {
    if (event == reader_event::Key) {
      return partial_reader_handler<R, fld_list<FLDS...>>::handle_key(p, 0);
    } else if (event == reader_event::StartObject) {
      return true;
    } else if (event == reader_event::EndObject) {
      for (size_t i = 0; i < N::value; i++) {
        bool flag = p->bitset_.back();
        if (!flag)
          return false;
        p->bitset_.pop_back();
      }
      p->pop_frame();
      return true;
    } else {
      return false;
    }
  }
};

template <typename T> struct reader_handler<arr<T>> {
  inline static void init(reader_state *p) { p->bitset_.push_back(false); }

  static bool handler(reader_state *p, reader_event event) {
    if (!p->bitset_[p->bitset_.size() - 1]) {
      p->bitset_[p->bitset_.size() - 1] = true;
      if (event == reader_event::StartArray) {
        auto x = static_cast<val<arr<T>> *>(p->stk_[p->stk_.size() - 1].ref_);
        x->vec.clear();
        return true;
      } else {
        return false;
      }
    } else {
      if (event == reader_event::EndArray) {
        p->bitset_.pop_back();
        p->pop_frame();
        return true;
      } else {
        auto x = static_cast<val<arr<T>> *>(p->stk_[p->stk_.size() - 1].ref_);
        x->vec.emplace_back(val<T>{});
        val<T> &y = x->vec[x->vec.size() - 1];
        p->push_frame(&y);
        return (p->stk_[p->stk_.size() - 1].handler_)(p, event);
      }
    }
  }
};

template <typename SCHEMA, template <typename...> typename B, typename... ARGS>
class reader : public B<ARGS..., reader<SCHEMA, B, ARGS...>> {
  reader_state state_{};

  inline reader_handler_t get_handler() {
    return state_.stk_[state_.stk_.size() - 1].handler_;
  }

public:
  explicit reader(val<SCHEMA> &ref) { state_.push_frame(&ref); }

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
