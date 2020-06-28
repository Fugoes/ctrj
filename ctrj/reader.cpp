#include "reader.hpp"

namespace ctrj::detail {

bool reader_handler<u64>::handler(reader_state *p, reader_event event) {
  auto x = p->stk_peek_ref<u64>();
  if (event == reader_event::U64) {
    x->u64 = p->data_.u64_;
    p->stk_pop();
    return true;
  } else {
    return false;
  }
}

bool reader_handler<i64>::handler(reader_state *p, reader_event event) {
  auto x = p->stk_peek_ref<i64>();
  if (event == reader_event::U64) {
    if (p->data_.u64_ > INT64_MAX)
      return false;
    x->i64 = p->data_.u64_;
    p->stk_pop();
    return true;
  } else if (event == reader_event::I64) {
    x->i64 = p->data_.i64_;
    p->stk_pop();
    return true;
  } else {
    return false;
  }
}

bool reader_handler<f64>::handler(reader_state *p, reader_event event) {
  auto x = p->stk_peek_ref<f64>();
  if (event == reader_event::F64) {
    x->f64 = p->data_.f64_;
    p->stk_pop();
    return true;
  } else {
    return false;
  }
}

bool reader_handler<str>::handler(reader_state *p, reader_event event) {
  auto x = p->stk_peek_ref<str>();
  if (event == reader_event::String) {
    x->str = p->data_.str_;
    p->stk_pop();
    return true;
  } else {
    return false;
  }
}

bool reader_handler<bol>::handler(reader_state *p, reader_event event) {
  auto x = p->stk_peek_ref<bol>();
  if (event == reader_event::Bool) {
    x->bol = p->data_.bol_;
    p->stk_pop();
    return true;
  } else {
    return false;
  }
}

} // namespace ctrj::detail
