
#include "xobj_list.h"

namespace xobj {

Tuple *Tuple::New(uint16_t n) {
    auto size = sizeof(Value) * n + offsetof(Tuple, data);
    auto t = (Tuple *)(new char[size]);
    t->Tuple::Tuple(n);
    return t;
}

void Tuple::Init(Value *l, size_t len) {
    for (auto p = l, e = l + len; p < e; p++)
        p->Value::Value();
}

void Tuple::UnRef(Value *l, size_t len) {
    for (auto p = l, e = l + len; p < e; p++)
        p->Value::~Value();
}

}
