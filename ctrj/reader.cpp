#include "reader.hpp"

namespace ctrj::detail {

bool reader_handler<u64>::handler(reader_state *p, reader_event event) {
  auto x = static_cast<val<u64> *>(p->stk_[p->stk_.size() - 1].ref_);
  if (event == reader_event::U64) {
    x->u64 = p->data_.u64_;
    p->pop_frame();
    return true;
  } else {
    return false;
  }
}

bool reader_handler<i64>::handler(reader_state *p, reader_event event) {
  auto x = static_cast<val<i64> *>(p->stk_[p->stk_.size() - 1].ref_);
  if (event == reader_event::U64) {
    if (p->data_.u64_ > INT64_MAX)
      return false;
    x->i64 = p->data_.u64_;
    p->pop_frame();
    return true;
  } else if (event == reader_event::I64) {
    x->i64 = p->data_.i64_;
    p->pop_frame();
    return true;
  } else {
    return false;
  }
}

bool reader_handler<f64>::handler(reader_state *p, reader_event event) {
  auto x = static_cast<val<f64> *>(p->stk_[p->stk_.size() - 1].ref_);
  if (event == reader_event::F64) {
    x->f64 = p->data_.f64_;
    p->pop_frame();
    return true;
  } else {
    return false;
  }
}

bool reader_handler<str>::handler(reader_state *p, reader_event event) {
  auto x = static_cast<val<str> *>(p->stk_[p->stk_.size() - 1].ref_);
  if (event == reader_event::String) {
    x->str = p->data_.str_;
    p->pop_frame();
    return true;
  } else {
    return false;
  }
}

bool reader_handler<bol>::handler(reader_state *p, reader_event event) {
  auto x = static_cast<val<bol> *>(p->stk_[p->stk_.size() - 1].ref_);
  if (event == reader_event::Bool) {
    x->bol = p->data_.bol_;
    p->pop_frame();
    return true;
  } else {
    return false;
  }
}

} // namespace ctrj::detail
